#include <gtest/gtest.h>
#include "chainforge/serialization/serialization.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/core/timestamp.hpp"
#include "chainforge/core/hash.hpp"
#include <vector>
#include <memory>

namespace {

class SerializationTest : public ::testing::Test {
protected:
    void SetUp() override {
        serializer_ = chainforge::serialization::create_serializer();
        validator_ = chainforge::serialization::create_validator();
    }

    std::unique_ptr<chainforge::serialization::Serializer> serializer_;
    std::unique_ptr<chainforge::serialization::Validator> validator_;
};

// Test serialization and deserialization of basic types
TEST_F(SerializationTest, AddressSerialization) {
    // Create a test address
    chainforge::Address original_addr; // Default constructed

    // Serialize
    auto serialize_result = serializer_->serialize_address(original_addr);
    ASSERT_TRUE(serialize_result.has_value()) << "Serialization failed: " << serialize_result.error().to_string();

    auto serialized_data = serialize_result.value();
    ASSERT_FALSE(serialized_data.empty()) << "Serialized data should not be empty";

    // Deserialize
    auto deserialize_result = serializer_->deserialize_address(serialized_data);
    ASSERT_TRUE(deserialize_result.has_value()) << "Deserialization failed: " << deserialize_result.error().to_string();

    auto deserialized_addr = std::move(deserialize_result.value());
    ASSERT_TRUE(deserialized_addr != nullptr) << "Deserialized address should not be null";

    // Validate
    auto validation_result = validator_->validate_address(*deserialized_addr);
    ASSERT_TRUE(validation_result.has_value()) << "Validation failed: " << validation_result.error().to_string();
}

TEST_F(SerializationTest, AmountSerialization) {
    // Create a test amount
    chainforge::Amount original_amount; // Default constructed

    // Serialize
    auto serialize_result = serializer_->serialize_amount(original_amount);
    ASSERT_TRUE(serialize_result.has_value()) << "Serialization failed: " << serialize_result.error().to_string();

    auto serialized_data = serialize_result.value();
    ASSERT_FALSE(serialized_data.empty()) << "Serialized data should not be empty";

    // Deserialize
    auto deserialize_result = serializer_->deserialize_amount(serialized_data);
    ASSERT_TRUE(deserialize_result.has_value()) << "Deserialization failed: " << deserialize_result.error().to_string();

    auto deserialized_amount = std::move(deserialize_result.value());
    ASSERT_TRUE(deserialized_amount != nullptr) << "Deserialized amount should not be null";

    // Validate
    auto validation_result = validator_->validate_amount(*deserialized_amount);
    ASSERT_TRUE(validation_result.has_value()) << "Validation failed: " << validation_result.error().to_string();
}

TEST_F(SerializationTest, TimestampSerialization) {
    // Create a test timestamp
    chainforge::Timestamp original_ts; // Default constructed

    // Serialize
    auto serialize_result = serializer_->serialize_timestamp(original_ts);
    ASSERT_TRUE(serialize_result.has_value()) << "Serialization failed: " << serialize_result.error().to_string();

    auto serialized_data = serialize_result.value();
    ASSERT_FALSE(serialized_data.empty()) << "Serialized data should not be empty";

    // Deserialize
    auto deserialize_result = serializer_->deserialize_timestamp(serialized_data);
    ASSERT_TRUE(deserialize_result.has_value()) << "Deserialization failed: " << deserialize_result.error().to_string();

    auto deserialized_ts = std::move(deserialize_result.value());
    ASSERT_TRUE(deserialized_ts != nullptr) << "Deserialized timestamp should not be null";

    // Validate
    auto validation_result = validator_->validate_timestamp(*deserialized_ts);
    ASSERT_TRUE(validation_result.has_value()) << "Validation failed: " << validation_result.error().to_string();
}

TEST_F(SerializationTest, HashSerialization) {
    // Create a test hash
    chainforge::Hash original_hash; // Default constructed

    // Serialize
    auto serialize_result = serializer_->serialize_hash(original_hash);
    ASSERT_TRUE(serialize_result.has_value()) << "Serialization failed: " << serialize_result.error().to_string();

    auto serialized_data = serialize_result.value();
    ASSERT_FALSE(serialized_data.empty()) << "Serialized data should not be empty";

    // Deserialize
    auto deserialize_result = serializer_->deserialize_hash(serialized_data);
    ASSERT_TRUE(deserialize_result.has_value()) << "Deserialization failed: " << deserialize_result.error().to_string();

    auto deserialized_hash = std::move(deserialize_result.value());
    ASSERT_TRUE(deserialized_hash != nullptr) << "Deserialized hash should not be null";

    // Validate
    auto validation_result = validator_->validate_hash(*deserialized_hash);
    ASSERT_TRUE(validation_result.has_value()) << "Validation failed: " << validation_result.error().to_string();
}

// Test validation of serialized data
TEST_F(SerializationTest, ValidateSerializedData) {
    // Test with empty data
    std::vector<uint8_t> empty_data;
    auto empty_result = validator_->validate_serialized_data(empty_data, "test");
    ASSERT_FALSE(empty_result.has_value()) << "Empty data should fail validation";

    // Test with valid serialized data
    chainforge::Address addr;
    auto serialize_result = serializer_->serialize_address(addr);
    ASSERT_TRUE(serialize_result.has_value());

    auto valid_result = validator_->validate_serialized_data(serialize_result.value(), "Address");
    ASSERT_TRUE(valid_result.has_value()) << "Valid serialized data should pass validation";
}

// Test error handling
TEST_F(SerializationTest, InvalidDataHandling) {
    // Test deserialization with invalid data
    std::vector<uint8_t> invalid_data = {0xFF, 0xFF, 0xFF, 0xFF};

    auto result = serializer_->deserialize_address(invalid_data);
    // This might succeed or fail depending on implementation - both are acceptable
    // The important thing is that it doesn't crash
}

// Test round-trip consistency
TEST_F(SerializationTest, RoundTripConsistency) {
    // Test that serialize -> deserialize -> serialize produces the same result
    chainforge::Address original_addr;

    // First round
    auto first_serialize = serializer_->serialize_address(original_addr);
    ASSERT_TRUE(first_serialize.has_value());

    auto first_deserialize = serializer_->deserialize_address(first_serialize.value());
    ASSERT_TRUE(first_deserialize.has_value());

    // Second round
    auto second_serialize = serializer_->serialize_address(*first_deserialize.value());
    ASSERT_TRUE(second_serialize.has_value());

    // Results should be identical
    ASSERT_EQ(first_serialize.value(), second_serialize.value())
        << "Round-trip serialization should be consistent";
}

} // namespace
