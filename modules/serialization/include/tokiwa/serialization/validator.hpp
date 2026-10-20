#pragma once

#include <memory>
#include <string>
#include <vector>
#include "tokiwa/core/expected.hpp"
#include "tokiwa/core/error.hpp"
#include "tokiwa/core/block.hpp"
#include "tokiwa/core/transaction.hpp"
#include "tokiwa/core/address.hpp"
#include "tokiwa/core/amount.hpp"
#include "tokiwa/core/timestamp.hpp"
#include "tokiwa/core/hash.hpp"

namespace tokiwa {
namespace serialization {

/**
 * @brief Validation error codes
 */
enum class ValidationError {
    INVALID_HASH = 1,
    INVALID_SIGNATURE = 2,
    INVALID_AMOUNT = 3,
    INVALID_ADDRESS = 4,
    INVALID_TIMESTAMP = 5,
    INVALID_BLOCK = 6,
    INVALID_TRANSACTION = 7,
    DATA_TOO_LARGE = 8,
    MISSING_REQUIRED_FIELD = 9
};

/**
 * @brief Result type for validation operations
 */
using ValidationResult = core::Result<void>;

/**
 * @brief Interface for data validation
 */
class Validator {
public:
    virtual ~Validator() = default;

    /**
     * @brief Validate a block
     */
    virtual ValidationResult validate_block(const core::Block& block) = 0;

    /**
     * @brief Validate a transaction
     */
    virtual ValidationResult validate_transaction(const core::Transaction& tx) = 0;

    /**
     * @brief Validate an address
     */
    virtual ValidationResult validate_address(const core::Address& addr) = 0;

    /**
     * @brief Validate an amount
     */
    virtual ValidationResult validate_amount(const core::Amount& amount) = 0;

    /**
     * @brief Validate a timestamp
     */
    virtual ValidationResult validate_timestamp(const core::Timestamp& ts) = 0;

    /**
     * @brief Validate a hash
     */
    virtual ValidationResult validate_hash(const core::Hash& hash) = 0;

    /**
     * @brief Validate serialized data without deserializing
     */
    virtual ValidationResult validate_serialized_data(
        const std::vector<uint8_t>& data,
        const std::string& type_name) = 0;
};

/**
 * @brief Default validator implementation
 */
class DefaultValidator : public Validator {
public:
    DefaultValidator();
    ~DefaultValidator() override;

    ValidationResult validate_block(const core::Block& block) override;
    ValidationResult validate_transaction(const core::Transaction& tx) override;
    ValidationResult validate_address(const core::Address& addr) override;
    ValidationResult validate_amount(const core::Amount& amount) override;
    ValidationResult validate_timestamp(const core::Timestamp& ts) override;
    ValidationResult validate_hash(const core::Hash& hash) override;
    ValidationResult validate_serialized_data(
        const std::vector<uint8_t>& data,
        const std::string& type_name) override;

private:
    // Validation helper methods
    ValidationResult validate_block_header(const core::Block& block);
    ValidationResult validate_transaction_structure(const core::Transaction& tx);
    ValidationResult validate_address_format(const core::Address& addr);
    ValidationResult validate_amount_range(const core::Amount& amount);
    ValidationResult validate_timestamp_range(const core::Timestamp& ts);
    ValidationResult validate_hash_length(const core::Hash& hash);
};

/**
 * @brief Create a default validator instance
 */
std::unique_ptr<Validator> create_validator();

} // namespace serialization
} // namespace tokiwa
