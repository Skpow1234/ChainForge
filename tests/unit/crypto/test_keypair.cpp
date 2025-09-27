#include <gtest/gtest.h>
#include "chainforge/crypto/keypair.hpp"
#include "chainforge/crypto/signature.hpp"
#include <vector>

namespace chainforge::crypto::test {

class KeyPairTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(KeyPairTest, GenerateKeyPair) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    EXPECT_FALSE(keypair.private_key().empty());
    EXPECT_FALSE(keypair.public_key().empty());
}

TEST_F(KeyPairTest, KeyPairFromPrivateKey) {
    // Generate a keypair first
    auto gen_result = KeyPair::generate();
    EXPECT_TRUE(gen_result.has_value());
    
    KeyPair original = gen_result.value();
    std::vector<uint8_t> private_key = original.private_key();
    
    // Create new keypair from private key
    auto result = KeyPair::from_private_key(private_key);
    EXPECT_TRUE(result.has_value());
    
    KeyPair derived = result.value();
    EXPECT_EQ(derived.private_key(), private_key);
    EXPECT_EQ(derived.public_key(), original.public_key());
}

TEST_F(KeyPairTest, InvalidPrivateKey) {
    std::vector<uint8_t> invalid_key = {0x01, 0x02, 0x03}; // Too short
    
    auto result = KeyPair::from_private_key(invalid_key);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::INVALID_KEY);
}

TEST_F(KeyPairTest, SignAndVerify) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    std::vector<uint8_t> message = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    
    // Sign message
    auto sign_result = keypair.sign(message);
    EXPECT_TRUE(sign_result.has_value());
    
    Signature signature = sign_result.value();
    EXPECT_FALSE(signature.empty());
    
    // Verify signature
    auto verify_result = keypair.verify(message, signature);
    EXPECT_TRUE(verify_result.has_value());
    EXPECT_TRUE(verify_result.value());
}

TEST_F(KeyPairTest, SignAndVerifyWithDifferentMessage) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    std::vector<uint8_t> message1 = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    std::vector<uint8_t> message2 = {0x57, 0x6f, 0x72, 0x6c, 0x64}; // "World"
    
    // Sign first message
    auto sign_result = keypair.sign(message1);
    EXPECT_TRUE(sign_result.has_value());
    
    Signature signature = sign_result.value();
    
    // Verify with different message (should fail)
    auto verify_result = keypair.verify(message2, signature);
    EXPECT_TRUE(verify_result.has_value());
    EXPECT_FALSE(verify_result.value());
}

TEST_F(KeyPairTest, SignAndVerifyWithDifferentKey) {
    auto result1 = KeyPair::generate();
    auto result2 = KeyPair::generate();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    KeyPair keypair1 = result1.value();
    KeyPair keypair2 = result2.value();
    std::vector<uint8_t> message = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    
    // Sign with first keypair
    auto sign_result = keypair1.sign(message);
    EXPECT_TRUE(sign_result.has_value());
    
    Signature signature = sign_result.value();
    
    // Verify with different keypair (should fail)
    auto verify_result = keypair2.verify(message, signature);
    EXPECT_TRUE(verify_result.has_value());
    EXPECT_FALSE(verify_result.value());
}

TEST_F(KeyPairTest, PublicKeyDerivation) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    std::vector<uint8_t> public_key = keypair.public_key();
    
    // Public key should be derivable from private key
    auto derived_result = KeyPair::public_key_from_private(keypair.private_key());
    EXPECT_TRUE(derived_result.has_value());
    EXPECT_EQ(derived_result.value(), public_key);
}

TEST_F(KeyPairTest, KeyPairEquality) {
    auto result1 = KeyPair::generate();
    auto result2 = KeyPair::generate();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    KeyPair keypair1 = result1.value();
    KeyPair keypair2 = result2.value();
    
    // Different keypairs should not be equal
    EXPECT_NE(keypair1, keypair2);
    
    // Same keypair should be equal
    EXPECT_EQ(keypair1, keypair1);
}

TEST_F(KeyPairTest, KeyPairFromSamePrivateKey) {
    auto gen_result = KeyPair::generate();
    EXPECT_TRUE(gen_result.has_value());
    
    KeyPair original = gen_result.value();
    std::vector<uint8_t> private_key = original.private_key();
    
    // Create two keypairs from same private key
    auto result1 = KeyPair::from_private_key(private_key);
    auto result2 = KeyPair::from_private_key(private_key);
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    KeyPair keypair1 = result1.value();
    KeyPair keypair2 = result2.value();
    
    EXPECT_EQ(keypair1, keypair2);
    EXPECT_EQ(keypair1, original);
}

TEST_F(KeyPairTest, KeySizeValidation) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    
    // Check key sizes
    EXPECT_EQ(keypair.private_key().size(), 32); // secp256k1 private key size
    EXPECT_EQ(keypair.public_key().size(), 64);  // secp256k1 public key size (uncompressed)
}

TEST_F(KeyPairTest, DeterministicKeyGeneration) {
    std::vector<uint8_t> seed = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    auto result1 = KeyPair::from_seed(seed);
    auto result2 = KeyPair::from_seed(seed);
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    KeyPair keypair1 = result1.value();
    KeyPair keypair2 = result2.value();
    
    // Same seed should generate same keypair
    EXPECT_EQ(keypair1, keypair2);
}

TEST_F(KeyPairTest, DifferentSeedsGenerateDifferentKeys) {
    std::vector<uint8_t> seed1 = {0x01, 0x02, 0x03, 0x04, 0x05};
    std::vector<uint8_t> seed2 = {0x01, 0x02, 0x03, 0x04, 0x06};
    
    auto result1 = KeyPair::from_seed(seed1);
    auto result2 = KeyPair::from_seed(seed2);
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    KeyPair keypair1 = result1.value();
    KeyPair keypair2 = result2.value();
    
    // Different seeds should generate different keypairs
    EXPECT_NE(keypair1, keypair2);
}

TEST_F(KeyPairTest, SignEmptyMessage) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    std::vector<uint8_t> empty_message;
    
    // Should be able to sign empty message
    auto sign_result = keypair.sign(empty_message);
    EXPECT_TRUE(sign_result.has_value());
    
    Signature signature = sign_result.value();
    
    // Should be able to verify empty message
    auto verify_result = keypair.verify(empty_message, signature);
    EXPECT_TRUE(verify_result.has_value());
    EXPECT_TRUE(verify_result.value());
}

TEST_F(KeyPairTest, SignLargeMessage) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    std::vector<uint8_t> large_message(1024 * 1024, 0x42); // 1MB message
    
    // Should be able to sign large message
    auto sign_result = keypair.sign(large_message);
    EXPECT_TRUE(sign_result.has_value());
    
    Signature signature = sign_result.value();
    
    // Should be able to verify large message
    auto verify_result = keypair.verify(large_message, signature);
    EXPECT_TRUE(verify_result.has_value());
    EXPECT_TRUE(verify_result.value());
}

TEST_F(KeyPairTest, CopyAndMove) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair original = result.value();
    KeyPair copy = original;
    KeyPair moved = std::move(original);
    
    EXPECT_EQ(copy.private_key(), moved.private_key());
    EXPECT_EQ(copy.public_key(), moved.public_key());
    EXPECT_TRUE(original.private_key().empty()); // Moved-from state
}

TEST_F(KeyPairTest, Hash) {
    auto result1 = KeyPair::generate();
    auto result2 = KeyPair::generate();
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    KeyPair keypair1 = result1.value();
    KeyPair keypair2 = result2.value();
    
    // Different keypairs should have different hashes
    EXPECT_NE(std::hash<KeyPair>{}(keypair1), std::hash<KeyPair>{}(keypair2));
    
    // Same keypair should have same hash
    EXPECT_EQ(std::hash<KeyPair>{}(keypair1), std::hash<KeyPair>{}(keypair1));
}

TEST_F(KeyPairTest, KeyPairSerialization) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair original = result.value();
    
    // Serialize keypair
    std::vector<uint8_t> serialized = original.serialize();
    EXPECT_FALSE(serialized.empty());
    
    // Deserialize keypair
    auto deserialized_result = KeyPair::deserialize(serialized);
    EXPECT_TRUE(deserialized_result.has_value());
    
    KeyPair deserialized = deserialized_result.value();
    EXPECT_EQ(deserialized.private_key(), original.private_key());
    EXPECT_EQ(deserialized.public_key(), original.public_key());
}

TEST_F(KeyPairTest, DeserializeInvalidData) {
    std::vector<uint8_t> invalid_data = {0x01, 0x02, 0x03};
    
    auto result = KeyPair::deserialize(invalid_data);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::DESERIALIZATION_ERROR);
}

TEST_F(KeyPairTest, KeyPairValidation) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    
    // Valid keypair should pass validation
    auto validation_result = keypair.validate();
    EXPECT_TRUE(validation_result.has_value());
    EXPECT_TRUE(validation_result.value());
}

TEST_F(KeyPairTest, KeyPairWithCorruptedPrivateKey) {
    auto result = KeyPair::generate();
    EXPECT_TRUE(result.has_value());
    
    KeyPair keypair = result.value();
    
    // Corrupt private key
    std::vector<uint8_t> corrupted_private = keypair.private_key();
    corrupted_private[0] ^= 0xFF; // Flip bits
    
    KeyPair corrupted_keypair = KeyPair::from_private_key(corrupted_private).value();
    
    // Validation should fail
    auto validation_result = corrupted_keypair.validate();
    EXPECT_TRUE(validation_result.has_value());
    EXPECT_FALSE(validation_result.value());
}

} // namespace chainforge::crypto::test
