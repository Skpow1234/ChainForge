#include <gtest/gtest.h>
#include "chainforge/crypto/signature.hpp"
#include "chainforge/crypto/keypair.hpp"
#include <vector>
#include <random>

namespace chainforge::crypto::test {

class SignatureTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Generate test keypairs
        auto keypair1_result = KeyPair::generate();
        auto keypair2_result = KeyPair::generate();
        
        EXPECT_TRUE(keypair1_result.has_value());
        EXPECT_TRUE(keypair2_result.has_value());
        
        keypair1 = keypair1_result.value();
        keypair2 = keypair2_result.value();
        
        // Initialize random number generator
        rng.seed(std::random_device{}());
    }
    
    void TearDown() override {}
    
    KeyPair keypair1, keypair2;
    std::mt19937 rng;
};

TEST_F(SignatureTest, DefaultConstructor) {
    Signature sig;
    EXPECT_TRUE(sig.empty());
    EXPECT_EQ(sig.size(), 0);
}

TEST_F(SignatureTest, SignatureFromBytes) {
    std::vector<uint8_t> sig_bytes(64, 0x42);
    Signature sig(sig_bytes);
    
    EXPECT_FALSE(sig.empty());
    EXPECT_EQ(sig.size(), 64);
    EXPECT_EQ(sig.to_bytes(), sig_bytes);
}

TEST_F(SignatureTest, SignatureFromHex) {
    std::string hex_string = "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef";
    auto result = Signature::from_hex(hex_string);
    
    EXPECT_TRUE(result.has_value());
    Signature sig = result.value();
    EXPECT_EQ(sig.to_hex(), hex_string);
}

TEST_F(SignatureTest, InvalidHexSignature) {
    std::string invalid_hex = "invalid_hex";
    auto result = Signature::from_hex(invalid_hex);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::INVALID_SIGNATURE);
}

TEST_F(SignatureTest, ShortHexSignature) {
    std::string short_hex = "1234567890abcdef";
    auto result = Signature::from_hex(short_hex);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::INVALID_SIGNATURE);
}

TEST_F(SignatureTest, SignatureEquality) {
    std::vector<uint8_t> sig_bytes1(64, 0x42);
    std::vector<uint8_t> sig_bytes2(64, 0x42);
    std::vector<uint8_t> sig_bytes3(64, 0x43);
    
    Signature sig1(sig_bytes1);
    Signature sig2(sig_bytes2);
    Signature sig3(sig_bytes3);
    
    EXPECT_EQ(sig1, sig2);
    EXPECT_NE(sig1, sig3);
}

TEST_F(SignatureTest, SignatureInequality) {
    std::vector<uint8_t> sig_bytes1(64, 0x42);
    std::vector<uint8_t> sig_bytes2(64, 0x43);
    
    Signature sig1(sig_bytes1);
    Signature sig2(sig_bytes2);
    
    EXPECT_NE(sig1, sig2);
    EXPECT_FALSE(sig1 == sig2);
}

TEST_F(SignatureTest, SignatureHash) {
    std::vector<uint8_t> sig_bytes1(64, 0x42);
    std::vector<uint8_t> sig_bytes2(64, 0x42);
    std::vector<uint8_t> sig_bytes3(64, 0x43);
    
    Signature sig1(sig_bytes1);
    Signature sig2(sig_bytes2);
    Signature sig3(sig_bytes3);
    
    EXPECT_EQ(std::hash<Signature>{}(sig1), std::hash<Signature>{}(sig2));
    EXPECT_NE(std::hash<Signature>{}(sig1), std::hash<Signature>{}(sig3));
}

TEST_F(SignatureTest, SignatureSerialization) {
    std::vector<uint8_t> original_bytes(64);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (size_t i = 0; i < 64; ++i) {
        original_bytes[i] = byte_dist(rng);
    }
    
    Signature original_sig(original_bytes);
    
    // Serialize
    std::vector<uint8_t> serialized = original_sig.serialize();
    EXPECT_FALSE(serialized.empty());
    
    // Deserialize
    auto deserialized_result = Signature::deserialize(serialized);
    EXPECT_TRUE(deserialized_result.has_value());
    
    Signature deserialized_sig = deserialized_result.value();
    EXPECT_EQ(deserialized_sig, original_sig);
}

TEST_F(SignatureTest, DeserializeInvalidData) {
    std::vector<uint8_t> invalid_data = {0x01, 0x02, 0x03};
    
    auto result = Signature::deserialize(invalid_data);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::DESERIALIZATION_ERROR);
}

TEST_F(SignatureTest, SignatureValidation) {
    std::vector<uint8_t> valid_sig_bytes(64, 0x42);
    Signature valid_sig(valid_sig_bytes);
    
    auto validation_result = valid_sig.validate();
    EXPECT_TRUE(validation_result.has_value());
    EXPECT_TRUE(validation_result.value());
}

TEST_F(SignatureTest, EmptySignatureValidation) {
    Signature empty_sig;
    
    auto validation_result = empty_sig.validate();
    EXPECT_TRUE(validation_result.has_value());
    EXPECT_FALSE(validation_result.value());
}

TEST_F(SignatureTest, InvalidSignatureValidation) {
    std::vector<uint8_t> invalid_sig_bytes(32, 0x42); // Wrong size
    Signature invalid_sig(invalid_sig_bytes);
    
    auto validation_result = invalid_sig.validate();
    EXPECT_TRUE(validation_result.has_value());
    EXPECT_FALSE(validation_result.value());
}

TEST_F(SignatureTest, SignatureRecovery) {
    std::vector<uint8_t> message = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    
    // Sign message
    auto sign_result = keypair1.sign(message);
    EXPECT_TRUE(sign_result.has_value());
    
    Signature signature = sign_result.value();
    
    // Recover public key from signature
    auto recovery_result = Signature::recover_public_key(message, signature);
    EXPECT_TRUE(recovery_result.has_value());
    
    std::vector<uint8_t> recovered_public_key = recovery_result.value();
    EXPECT_EQ(recovered_public_key, keypair1.public_key());
}

TEST_F(SignatureTest, SignatureRecoveryWithWrongMessage) {
    std::vector<uint8_t> message1 = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    std::vector<uint8_t> message2 = {0x57, 0x6f, 0x72, 0x6c, 0x64}; // "World"
    
    // Sign first message
    auto sign_result = keypair1.sign(message1);
    EXPECT_TRUE(sign_result.has_value());
    
    Signature signature = sign_result.value();
    
    // Try to recover with wrong message
    auto recovery_result = Signature::recover_public_key(message2, signature);
    EXPECT_FALSE(recovery_result.has_value());
    EXPECT_EQ(recovery_result.error().code, ErrorCode::SIGNATURE_RECOVERY_FAILED);
}

TEST_F(SignatureTest, SignatureRecoveryWithCorruptedSignature) {
    std::vector<uint8_t> message = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    
    // Create corrupted signature
    std::vector<uint8_t> corrupted_sig_bytes(64, 0x00);
    Signature corrupted_sig(corrupted_sig_bytes);
    
    // Try to recover with corrupted signature
    auto recovery_result = Signature::recover_public_key(message, corrupted_sig);
    EXPECT_FALSE(recovery_result.has_value());
    EXPECT_EQ(recovery_result.error().code, ErrorCode::SIGNATURE_RECOVERY_FAILED);
}

TEST_F(SignatureTest, SignatureDerivation) {
    std::vector<uint8_t> message = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    
    // Sign message
    auto sign_result = keypair1.sign(message);
    EXPECT_TRUE(sign_result.has_value());
    
    Signature signature = sign_result.value();
    
    // Derive signature components
    auto components_result = signature.derive_components();
    EXPECT_TRUE(components_result.has_value());
    
    auto [r, s, v] = components_result.value();
    
    // Components should be valid
    EXPECT_FALSE(r.empty());
    EXPECT_FALSE(s.empty());
    EXPECT_TRUE(v == 0 || v == 1);
}

TEST_F(SignatureTest, SignatureFromComponents) {
    std::vector<uint8_t> r(32, 0x12);
    std::vector<uint8_t> s(32, 0x34);
    uint8_t v = 1;
    
    auto result = Signature::from_components(r, s, v);
    EXPECT_TRUE(result.has_value());
    
    Signature sig = result.value();
    EXPECT_FALSE(sig.empty());
    
    // Verify components can be derived back
    auto components_result = sig.derive_components();
    EXPECT_TRUE(components_result.has_value());
    
    auto [derived_r, derived_s, derived_v] = components_result.value();
    EXPECT_EQ(derived_r, r);
    EXPECT_EQ(derived_s, s);
    EXPECT_EQ(derived_v, v);
}

TEST_F(SignatureTest, InvalidSignatureComponents) {
    std::vector<uint8_t> invalid_r(16, 0x12); // Wrong size
    std::vector<uint8_t> s(32, 0x34);
    uint8_t v = 1;
    
    auto result = Signature::from_components(invalid_r, s, v);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::INVALID_SIGNATURE);
}

TEST_F(SignatureTest, SignatureCopyAndMove) {
    std::vector<uint8_t> sig_bytes(64, 0x42);
    Signature original_sig(sig_bytes);
    
    // Test copy constructor
    Signature copy_sig = original_sig;
    EXPECT_EQ(copy_sig, original_sig);
    
    // Test move constructor
    Signature moved_sig = std::move(original_sig);
    EXPECT_EQ(moved_sig, copy_sig);
    EXPECT_TRUE(original_sig.empty()); // Moved-from state
    
    // Test copy assignment
    Signature assigned_sig;
    assigned_sig = copy_sig;
    EXPECT_EQ(assigned_sig, copy_sig);
    
    // Test move assignment
    Signature move_assigned_sig;
    move_assigned_sig = std::move(copy_sig);
    EXPECT_EQ(move_assigned_sig, assigned_sig);
    EXPECT_TRUE(copy_sig.empty()); // Moved-from state
}

TEST_F(SignatureTest, SignatureComparison) {
    std::vector<uint8_t> sig_bytes1(64, 0x42);
    std::vector<uint8_t> sig_bytes2(64, 0x43);
    
    Signature sig1(sig_bytes1);
    Signature sig2(sig_bytes2);
    
    // Test ordering
    EXPECT_TRUE(sig1 < sig2 || sig2 < sig1); // One should be less than the other
    EXPECT_FALSE(sig1 < sig1); // Should not be less than itself
    EXPECT_TRUE(sig1 <= sig1); // Should be less than or equal to itself
    EXPECT_TRUE(sig1 >= sig1); // Should be greater than or equal to itself
}

TEST_F(SignatureTest, SignaturePerformance) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random signature data
        std::vector<uint8_t> sig_bytes(64);
        for (size_t j = 0; j < 64; ++j) {
            sig_bytes[j] = byte_dist(rng);
        }
        
        Signature sig(sig_bytes);
        
        // Perform various operations
        std::string hex = sig.to_hex();
        std::vector<uint8_t> bytes = sig.to_bytes();
        bool empty = sig.empty();
        size_t hash_value = std::hash<Signature>{}(sig);
        
        // Use variables to prevent optimization
        (void)hex;
        (void)bytes;
        (void)empty;
        (void)hash_value;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Performance should be reasonable (less than 1 second for 1k iterations)
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(SignatureTest, SignatureEdgeCases) {
    // Test zero signature
    std::vector<uint8_t> zero_sig_bytes(64, 0x00);
    Signature zero_sig(zero_sig_bytes);
    EXPECT_FALSE(zero_sig.empty());
    EXPECT_EQ(zero_sig.to_hex(), std::string(128, '0'));
    
    // Test all ones signature
    std::vector<uint8_t> ones_sig_bytes(64, 0xFF);
    Signature ones_sig(ones_sig_bytes);
    EXPECT_FALSE(ones_sig.empty());
    EXPECT_EQ(ones_sig.to_hex(), std::string(128, 'f'));
    
    // Test alternating pattern
    std::vector<uint8_t> alternating_sig_bytes(64);
    for (size_t i = 0; i < 64; ++i) {
        alternating_sig_bytes[i] = (i % 2) ? 0xAA : 0x55;
    }
    Signature alternating_sig(alternating_sig_bytes);
    EXPECT_FALSE(alternating_sig.empty());
}

TEST_F(SignatureTest, SignatureWithDifferentMessages) {
    std::vector<std::vector<uint8_t>> messages = {
        {}, // Empty message
        {0x48, 0x65, 0x6c, 0x6c, 0x6f}, // "Hello"
        std::vector<uint8_t>(1024, 0x42), // Large message
        {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09} // Binary data
    };
    
    for (const auto& message : messages) {
        // Sign message
        auto sign_result = keypair1.sign(message);
        EXPECT_TRUE(sign_result.has_value());
        
        Signature signature = sign_result.value();
        
        // Verify signature
        auto verify_result = keypair1.verify(message, signature);
        EXPECT_TRUE(verify_result.has_value());
        EXPECT_TRUE(verify_result.value());
        
        // Try to verify with different keypair
        auto wrong_verify_result = keypair2.verify(message, signature);
        EXPECT_TRUE(wrong_verify_result.has_value());
        EXPECT_FALSE(wrong_verify_result.value());
    }
}

} // namespace chainforge::crypto::test
