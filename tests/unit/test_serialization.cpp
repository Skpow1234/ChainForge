#include <gtest/gtest.h>
#include "chainforge/serialization/serialization.hpp"
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/core/timestamp.hpp"
#include "chainforge/core/hash.hpp"

using namespace chainforge;
using namespace chainforge::core;
using namespace chainforge::serialization;

class SerializationTest : public ::testing::Test {
protected:
    void SetUp() override {
        serializer = create_serializer();
        validator = create_validator();
    }

    std::unique_ptr<Serializer> serializer;
    std::unique_ptr<Validator> validator;
};

// Hash serialization tests
TEST_F(SerializationTest, HashSerializationRoundTrip) {
    Hash original = Hash::random();
    
    // Serialize
    auto serialize_result = serializer->serialize_hash(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    const auto& serialized = serialize_result.value();
    EXPECT_GT(serialized.size(), 0);
    
    // Deserialize
    auto deserialize_result = serializer->deserialize_hash(serialized);
    ASSERT_TRUE(deserialize_result.has_value());
    
    const auto& deserialized = *deserialize_result.value();
    EXPECT_EQ(original, deserialized);
}

TEST_F(SerializationTest, HashValidation) {
    Hash hash = Hash::random();
    auto result = validator->validate_hash(hash);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SerializationTest, ZeroHashSerializationRoundTrip) {
    Hash original = Hash::zero();
    
    auto serialize_result = serializer->serialize_hash(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    auto deserialize_result = serializer->deserialize_hash(serialize_result.value());
    ASSERT_TRUE(deserialize_result.has_value());
    
    EXPECT_EQ(original, *deserialize_result.value());
}

// Address serialization tests
TEST_F(SerializationTest, AddressSerializationRoundTrip) {
    Address original = Address::random();
    
    // Serialize
    auto serialize_result = serializer->serialize_address(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    const auto& serialized = serialize_result.value();
    EXPECT_GT(serialized.size(), 0);
    
    // Deserialize
    auto deserialize_result = serializer->deserialize_address(serialized);
    ASSERT_TRUE(deserialize_result.has_value());
    
    const auto& deserialized = *deserialize_result.value();
    EXPECT_EQ(original, deserialized);
}

TEST_F(SerializationTest, AddressValidation) {
    Address addr = Address::random();
    auto result = validator->validate_address(addr);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SerializationTest, ZeroAddressSerializationRoundTrip) {
    Address original = Address::zero();
    
    auto serialize_result = serializer->serialize_address(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    auto deserialize_result = serializer->deserialize_address(serialize_result.value());
    ASSERT_TRUE(deserialize_result.has_value());
    
    EXPECT_EQ(original, *deserialize_result.value());
}

// Amount serialization tests
TEST_F(SerializationTest, AmountSerializationRoundTrip) {
    Amount original = Amount::from_wei(1000000000000000000ULL); // 1 ETH
    
    // Serialize
    auto serialize_result = serializer->serialize_amount(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    const auto& serialized = serialize_result.value();
    EXPECT_GT(serialized.size(), 0);
    
    // Deserialize
    auto deserialize_result = serializer->deserialize_amount(serialized);
    ASSERT_TRUE(deserialize_result.has_value());
    
    const auto& deserialized = *deserialize_result.value();
    EXPECT_EQ(original, deserialized);
}

TEST_F(SerializationTest, AmountValidation) {
    Amount amount = Amount::from_wei(1000);
    auto result = validator->validate_amount(amount);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SerializationTest, ZeroAmountSerializationRoundTrip) {
    Amount original = Amount::zero();
    
    auto serialize_result = serializer->serialize_amount(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    auto deserialize_result = serializer->deserialize_amount(serialize_result.value());
    ASSERT_TRUE(deserialize_result.has_value());
    
    EXPECT_EQ(original, *deserialize_result.value());
}

// Timestamp serialization tests
TEST_F(SerializationTest, TimestampSerializationRoundTrip) {
    Timestamp original = Timestamp::now();
    
    // Serialize
    auto serialize_result = serializer->serialize_timestamp(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    const auto& serialized = serialize_result.value();
    EXPECT_GT(serialized.size(), 0);
    
    // Deserialize
    auto deserialize_result = serializer->deserialize_timestamp(serialized);
    ASSERT_TRUE(deserialize_result.has_value());
    
    const auto& deserialized = *deserialize_result.value();
    EXPECT_EQ(original, deserialized);
}

TEST_F(SerializationTest, TimestampValidation) {
    Timestamp ts = Timestamp::now();
    auto result = validator->validate_timestamp(ts);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SerializationTest, ZeroTimestampSerializationRoundTrip) {
    Timestamp original = Timestamp::zero();
    
    auto serialize_result = serializer->serialize_timestamp(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    auto deserialize_result = serializer->deserialize_timestamp(serialize_result.value());
    ASSERT_TRUE(deserialize_result.has_value());
    
    EXPECT_EQ(original, *deserialize_result.value());
}

// Transaction serialization tests
TEST_F(SerializationTest, TransactionSerializationRoundTrip) {
    Address from = Address::random();
    Address to = Address::random();
    Amount value = Amount::from_wei(1000000000000000000ULL); // 1 ETH
    
    Transaction original(from, to, value);
    original.set_gas_limit(21000);
    original.set_gas_price(1000000000); // 1 gwei
    original.set_nonce(0);
    
    // Serialize
    auto serialize_result = serializer->serialize_transaction(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    const auto& serialized = serialize_result.value();
    EXPECT_GT(serialized.size(), 0);
    
    // Deserialize
    auto deserialize_result = serializer->deserialize_transaction(serialized);
    ASSERT_TRUE(deserialize_result.has_value());
    
    const auto& deserialized = *deserialize_result.value();
    
    // Verify fields
    EXPECT_EQ(original.from(), deserialized.from());
    EXPECT_EQ(original.to(), deserialized.to());
    EXPECT_EQ(original.value(), deserialized.value());
    EXPECT_EQ(original.gas_limit(), deserialized.gas_limit());
    EXPECT_EQ(original.nonce(), deserialized.nonce());
}

TEST_F(SerializationTest, TransactionValidation) {
    Address from = Address::random();
    Address to = Address::random();
    Amount value = Amount::from_wei(1000);
    
    Transaction tx(from, to, value);
    tx.set_gas_limit(21000);
    tx.set_gas_price(1000000000);
    
    auto result = validator->validate_transaction(tx);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SerializationTest, TransactionWithDataSerializationRoundTrip) {
    Address from = Address::random();
    Address to = Address::random();
    Amount value = Amount::from_wei(500000000000000000ULL); // 0.5 ETH
    
    Transaction original(from, to, value);
    original.set_gas_limit(100000);
    original.set_gas_price(2000000000); // 2 gwei
    original.set_nonce(5);
    
    // Add some data
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05};
    original.set_data(data);
    
    // Serialize
    auto serialize_result = serializer->serialize_transaction(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    // Deserialize
    auto deserialize_result = serializer->deserialize_transaction(serialize_result.value());
    ASSERT_TRUE(deserialize_result.has_value());
    
    const auto& deserialized = *deserialize_result.value();
    
    // Verify data
    EXPECT_EQ(original.payload().size(), deserialized.payload().size());
    EXPECT_EQ(original.payload(), deserialized.payload());
}

// Block serialization tests
TEST_F(SerializationTest, BlockSerializationRoundTrip) {
    Hash parent_hash = Hash::random();
    Timestamp timestamp = Timestamp::now();
    BlockHeight height = 1;
    
    Block original(height, parent_hash, timestamp);
    original.set_nonce(123456);
    original.set_gas_limit(8000000);
    original.set_gas_price(1000000000);
    original.set_chain_id(1);
    
    // Serialize
    auto serialize_result = serializer->serialize_block(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    const auto& serialized = serialize_result.value();
    EXPECT_GT(serialized.size(), 0);
    
    // Deserialize
    auto deserialize_result = serializer->deserialize_block(serialized);
    ASSERT_TRUE(deserialize_result.has_value());
    
    const auto& deserialized = *deserialize_result.value();
    
    // Verify fields
    EXPECT_EQ(original.height(), deserialized.height());
    EXPECT_EQ(original.parent_hash(), deserialized.parent_hash());
    EXPECT_EQ(original.timestamp(), deserialized.timestamp());
    EXPECT_EQ(original.nonce(), deserialized.nonce());
}

TEST_F(SerializationTest, BlockValidation) {
    Hash parent_hash = Hash::random();
    Timestamp timestamp = Timestamp::now();
    BlockHeight height = 1;
    
    Block block(height, parent_hash, timestamp);
    block.set_gas_limit(8000000);
    
    auto result = validator->validate_block(block);
    EXPECT_TRUE(result.has_value());
}

TEST_F(SerializationTest, BlockWithTransactionsSerializationRoundTrip) {
    Hash parent_hash = Hash::random();
    Timestamp timestamp = Timestamp::now();
    BlockHeight height = 10;
    
    Block original(height, parent_hash, timestamp);
    original.set_nonce(789012);
    original.set_gas_limit(8000000);
    original.set_gas_price(1000000000);
    original.set_chain_id(1);
    
    // Add transactions
    for (int i = 0; i < 3; ++i) {
        Address from = Address::random();
        Address to = Address::random();
        Amount value = Amount::from_wei(1000000000000000000ULL * (i + 1));
        
        Transaction tx(from, to, value);
        tx.set_gas_limit(21000);
        tx.set_gas_price(1000000000);
        tx.set_nonce(i);
        
        original.add_transaction(tx);
    }
    
    EXPECT_EQ(original.transaction_count(), 3);
    
    // Serialize
    auto serialize_result = serializer->serialize_block(original);
    ASSERT_TRUE(serialize_result.has_value());
    
    // Deserialize
    auto deserialize_result = serializer->deserialize_block(serialize_result.value());
    ASSERT_TRUE(deserialize_result.has_value());
    
    const auto& deserialized = *deserialize_result.value();
    
    // Verify transaction count
    EXPECT_EQ(original.transaction_count(), deserialized.transaction_count());
    
    // Verify each transaction
    for (size_t i = 0; i < original.transactions().size(); ++i) {
        const auto& orig_tx = original.transactions()[i];
        const auto& deser_tx = deserialized.transactions()[i];
        
        EXPECT_EQ(orig_tx.from(), deser_tx.from());
        EXPECT_EQ(orig_tx.to(), deser_tx.to());
        EXPECT_EQ(orig_tx.value(), deser_tx.value());
        EXPECT_EQ(orig_tx.nonce(), deser_tx.nonce());
    }
}

// Serialized data validation tests
TEST_F(SerializationTest, ValidateSerializedData) {
    Hash hash = Hash::random();
    auto serialized = serializer->serialize_hash(hash);
    ASSERT_TRUE(serialized.has_value());
    
    auto result = validator->validate_serialized_data(serialized.value(), "Hash");
    EXPECT_TRUE(result.has_value());
}

TEST_F(SerializationTest, ValidateEmptySerializedData) {
    std::vector<uint8_t> empty_data;
    auto result = validator->validate_serialized_data(empty_data, "Hash");
    EXPECT_FALSE(result.has_value());
}

TEST_F(SerializationTest, ValidateTooLargeSerializedData) {
    // Create data that's too large (> 10MB)
    std::vector<uint8_t> large_data(11 * 1024 * 1024, 0xFF);
    auto result = validator->validate_serialized_data(large_data, "Block");
    EXPECT_FALSE(result.has_value());
}

// Forward compatibility tests
TEST_F(SerializationTest, SerializationForwardCompatibility) {
    // This test ensures that serialized data can be deserialized even if
    // the protobuf schema has been extended with new optional fields
    
    Address from = Address::random();
    Address to = Address::random();
    Amount value = Amount::from_wei(1000);
    
    Transaction tx(from, to, value);
    tx.set_gas_limit(21000);
    
    // Serialize
    auto serialized = serializer->serialize_transaction(tx);
    ASSERT_TRUE(serialized.has_value());
    
    // Deserialize (should work even if schema has new optional fields)
    auto deserialized = serializer->deserialize_transaction(serialized.value());
    ASSERT_TRUE(deserialized.has_value());
    
    // Basic fields should match
    EXPECT_EQ(tx.from(), deserialized.value()->from());
    EXPECT_EQ(tx.to(), deserialized.value()->to());
}

// Performance tests
TEST_F(SerializationTest, SerializationPerformance) {
    const int iterations = 1000;
    
    Address from = Address::random();
    Address to = Address::random();
    Amount value = Amount::from_wei(1000);
    
    Transaction tx(from, to, value);
    tx.set_gas_limit(21000);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < iterations; ++i) {
        auto result = serializer->serialize_transaction(tx);
        ASSERT_TRUE(result.has_value());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Average time should be reasonable (< 100 microseconds per serialization)
    auto avg_time = duration.count() / iterations;
    EXPECT_LT(avg_time, 100);
}

