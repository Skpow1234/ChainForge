#pragma once

#include <gmock/gmock.h>
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/serialization/serializer.hpp"
#include "chainforge/serialization/validator.hpp"
#include <memory>
#include <vector>

namespace chainforge {
namespace testing {

/**
 * @brief Mock serializer for testing serialization without actual I/O
 */
class MockSerializer : public serialization::Serializer {
public:
    MOCK_METHOD(
        serialization::SerializationResult<std::vector<uint8_t>>,
        serialize_block,
        (const core::Block& block),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::unique_ptr<core::Block>>,
        deserialize_block,
        (const std::vector<uint8_t>& data),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::vector<uint8_t>>,
        serialize_transaction,
        (const core::Transaction& tx),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::unique_ptr<core::Transaction>>,
        deserialize_transaction,
        (const std::vector<uint8_t>& data),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::vector<uint8_t>>,
        serialize_address,
        (const core::Address& addr),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::unique_ptr<core::Address>>,
        deserialize_address,
        (const std::vector<uint8_t>& data),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::vector<uint8_t>>,
        serialize_amount,
        (const core::Amount& amount),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::unique_ptr<core::Amount>>,
        deserialize_amount,
        (const std::vector<uint8_t>& data),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::vector<uint8_t>>,
        serialize_timestamp,
        (const core::Timestamp& ts),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::unique_ptr<core::Timestamp>>,
        deserialize_timestamp,
        (const std::vector<uint8_t>& data),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::vector<uint8_t>>,
        serialize_hash,
        (const core::Hash& hash),
        (override)
    );

    MOCK_METHOD(
        serialization::SerializationResult<std::unique_ptr<core::Hash>>,
        deserialize_hash,
        (const std::vector<uint8_t>& data),
        (override)
    );
};

/**
 * @brief Mock validator for testing validation logic
 */
class MockValidator : public serialization::Validator {
public:
    MOCK_METHOD(
        serialization::ValidationResult,
        validate_block,
        (const core::Block& block),
        (override)
    );

    MOCK_METHOD(
        serialization::ValidationResult,
        validate_transaction,
        (const core::Transaction& tx),
        (override)
    );

    MOCK_METHOD(
        serialization::ValidationResult,
        validate_address,
        (const core::Address& addr),
        (override)
    );

    MOCK_METHOD(
        serialization::ValidationResult,
        validate_amount,
        (const core::Amount& amount),
        (override)
    );

    MOCK_METHOD(
        serialization::ValidationResult,
        validate_timestamp,
        (const core::Timestamp& ts),
        (override)
    );

    MOCK_METHOD(
        serialization::ValidationResult,
        validate_hash,
        (const core::Hash& hash),
        (override)
    );

    MOCK_METHOD(
        serialization::ValidationResult,
        validate_serialized_data,
        (const std::vector<uint8_t>& data, const std::string& type_name),
        (override)
    );
};

/**
 * @brief Mock actions for common testing scenarios
 */
namespace actions {

// Action to return a successful serialization result
inline auto ReturnSerializedData(const std::vector<uint8_t>& data) {
    return ::testing::Return(serialization::SerializationResult<std::vector<uint8_t>>(data));
}

// Action to return a serialization error
inline auto ReturnSerializationError(
    serialization::SerializationError code,
    const std::string& message
) {
    return ::testing::Return(
        serialization::SerializationResult<std::vector<uint8_t>>(
            core::ErrorInfo(
                static_cast<int>(code),
                message,
                "mock_serializer",
                __FILE__,
                __LINE__
            )
        )
    );
}

// Action to return a successful validation
inline auto ReturnValidationSuccess() {
    return ::testing::Return(core::success());
}

// Action to return a validation error
inline auto ReturnValidationError(
    serialization::ValidationError code,
    const std::string& message
) {
    return ::testing::Return(
        serialization::ValidationResult(
            core::ErrorInfo(
                static_cast<int>(code),
                message,
                "mock_validator",
                __FILE__,
                __LINE__
            )
        )
    );
}

} // namespace actions

/**
 * @brief Test spy for tracking method calls
 */
template<typename T>
class CallTracker {
public:
    void Record(const T& value) {
        calls_.push_back(value);
    }

    void Clear() {
        calls_.clear();
    }

    size_t CallCount() const {
        return calls_.size();
    }

    const std::vector<T>& Calls() const {
        return calls_;
    }

    bool WasCalledWith(const T& value) const {
        return std::find(calls_.begin(), calls_.end(), value) != calls_.end();
    }

private:
    std::vector<T> calls_;
};

/**
 * @brief Stub implementations for testing without real dependencies
 */
namespace stubs {

/**
 * @brief Stub serializer that always succeeds with fake data
 */
class StubSerializer : public serialization::Serializer {
public:
    serialization::SerializationResult<std::vector<uint8_t>> 
    serialize_block(const core::Block&) override {
        return std::vector<uint8_t>{0x01, 0x02, 0x03};
    }

    serialization::SerializationResult<std::unique_ptr<core::Block>>
    deserialize_block(const std::vector<uint8_t>&) override {
        auto block = std::make_unique<core::Block>(
            1,
            core::Hash::random(),
            core::Timestamp::now()
        );
        block->set_gas_limit(8000000);
        return block;
    }

    serialization::SerializationResult<std::vector<uint8_t>>
    serialize_transaction(const core::Transaction&) override {
        return std::vector<uint8_t>{0x04, 0x05, 0x06};
    }

    serialization::SerializationResult<std::unique_ptr<core::Transaction>>
    deserialize_transaction(const std::vector<uint8_t>&) override {
        return std::make_unique<core::Transaction>(
            core::Address::random(),
            core::Address::random(),
            core::Amount::from_wei(1000)
        );
    }

    serialization::SerializationResult<std::vector<uint8_t>>
    serialize_address(const core::Address&) override {
        return std::vector<uint8_t>{0x07, 0x08};
    }

    serialization::SerializationResult<std::unique_ptr<core::Address>>
    deserialize_address(const std::vector<uint8_t>&) override {
        return std::make_unique<core::Address>(core::Address::random());
    }

    serialization::SerializationResult<std::vector<uint8_t>>
    serialize_amount(const core::Amount&) override {
        return std::vector<uint8_t>{0x09, 0x0A};
    }

    serialization::SerializationResult<std::unique_ptr<core::Amount>>
    deserialize_amount(const std::vector<uint8_t>&) override {
        return std::make_unique<core::Amount>(core::Amount::from_wei(1000));
    }

    serialization::SerializationResult<std::vector<uint8_t>>
    serialize_timestamp(const core::Timestamp&) override {
        return std::vector<uint8_t>{0x0B, 0x0C};
    }

    serialization::SerializationResult<std::unique_ptr<core::Timestamp>>
    deserialize_timestamp(const std::vector<uint8_t>&) override {
        return std::make_unique<core::Timestamp>(core::Timestamp::now());
    }

    serialization::SerializationResult<std::vector<uint8_t>>
    serialize_hash(const core::Hash&) override {
        return std::vector<uint8_t>{0x0D, 0x0E};
    }

    serialization::SerializationResult<std::unique_ptr<core::Hash>>
    deserialize_hash(const std::vector<uint8_t>&) override {
        return std::make_unique<core::Hash>(core::Hash::random());
    }
};

/**
 * @brief Stub validator that always validates successfully
 */
class StubValidator : public serialization::Validator {
public:
    serialization::ValidationResult validate_block(const core::Block&) override {
        return core::success();
    }

    serialization::ValidationResult validate_transaction(const core::Transaction&) override {
        return core::success();
    }

    serialization::ValidationResult validate_address(const core::Address&) override {
        return core::success();
    }

    serialization::ValidationResult validate_amount(const core::Amount&) override {
        return core::success();
    }

    serialization::ValidationResult validate_timestamp(const core::Timestamp&) override {
        return core::success();
    }

    serialization::ValidationResult validate_hash(const core::Hash&) override {
        return core::success();
    }

    serialization::ValidationResult validate_serialized_data(
        const std::vector<uint8_t>&,
        const std::string&
    ) override {
        return core::success();
    }
};

} // namespace stubs

} // namespace testing
} // namespace chainforge

