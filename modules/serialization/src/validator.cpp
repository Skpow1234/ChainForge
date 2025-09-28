#include "chainforge/serialization/validator.hpp"
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/core/timestamp.hpp"
#include "chainforge/core/hash.hpp"
#include <chrono>
#include <limits>

namespace chainforge {
namespace serialization {

// Error handling helper
inline core::ErrorInfo make_validation_error(ValidationError code, const std::string& message) {
    return core::ErrorInfo(
        static_cast<int>(code),
        message,
        "validation",
        __FILE__,
        __LINE__
    );
}

DefaultValidator::DefaultValidator() = default;
DefaultValidator::~DefaultValidator() = default;

ValidationResult DefaultValidator::validate_block(const Block& block) {
    // Validate block header
    auto header_result = validate_block_header(block);
    if (!header_result.has_value()) {
        return header_result;
    }

    // Validate transactions
    for (const auto& tx : block.get_transactions()) {
        auto tx_result = validate_transaction(*tx);
        if (!tx_result.has_value()) {
            return tx_result;
        }
    }

    // Validate block hash
    auto hash_result = validate_hash(block.get_hash());
    if (!hash_result.has_value()) {
        return hash_result;
    }

    return core::success();
}

ValidationResult DefaultValidator::validate_transaction(const Transaction& tx) {
    // Validate transaction structure
    auto structure_result = validate_transaction_structure(tx);
    if (!structure_result.has_value()) {
        return structure_result;
    }

    // Validate inputs
    for (const auto& input : tx.get_inputs()) {
        // Validate input addresses, amounts, etc.
        if (!input.get_address().has_value()) {
            return make_validation_error(
                ValidationError::INVALID_ADDRESS,
                "Transaction input missing address"
            );
        }

        auto addr_result = validate_address(input.get_address().value());
        if (!addr_result.has_value()) {
            return addr_result;
        }
    }

    // Validate outputs
    for (const auto& output : tx.get_outputs()) {
        auto addr_result = validate_address(output.get_address());
        if (!addr_result.has_value()) {
            return addr_result;
        }

        auto amount_result = validate_amount(output.get_amount());
        if (!amount_result.has_value()) {
            return amount_result;
        }
    }

    // Validate gas parameters
    if (tx.get_gas_limit() == 0) {
        return make_validation_error(
            ValidationError::INVALID_TRANSACTION,
            "Transaction gas limit must be greater than zero"
        );
    }

    return core::success();
}

ValidationResult DefaultValidator::validate_address(const Address& addr) {
    return validate_address_format(addr);
}

ValidationResult DefaultValidator::validate_amount(const Amount& amount) {
    return validate_amount_range(amount);
}

ValidationResult DefaultValidator::validate_timestamp(const Timestamp& ts) {
    return validate_timestamp_range(ts);
}

ValidationResult DefaultValidator::validate_hash(const Hash& hash) {
    return validate_hash_length(hash);
}

ValidationResult DefaultValidator::validate_serialized_data(
    const std::vector<uint8_t>& data,
    const std::string& type_name) {

    // Basic size validation
    if (data.empty()) {
        return make_validation_error(
            ValidationError::MISSING_REQUIRED_FIELD,
            "Serialized data cannot be empty for type: " + type_name
        );
    }

    // Type-specific validation could be added here
    // For now, just check basic constraints
    constexpr size_t MAX_SERIALIZED_SIZE = 10 * 1024 * 1024; // 10MB limit
    if (data.size() > MAX_SERIALIZED_SIZE) {
        return make_validation_error(
            ValidationError::DATA_TOO_LARGE,
            "Serialized data too large for type: " + type_name
        );
    }

    return core::success();
}

// Private validation methods
ValidationResult DefaultValidator::validate_block_header(const Block& block) {
    // Validate timestamp is reasonable (not too far in future/past)
    auto ts_result = validate_timestamp(block.get_timestamp());
    if (!ts_result.has_value()) {
        return ts_result;
    }

    // Validate difficulty is positive
    if (block.get_difficulty() == 0) {
        return make_validation_error(
            ValidationError::INVALID_BLOCK,
            "Block difficulty must be greater than zero"
        );
    }

    // Validate block height
    if (block.get_height() > std::numeric_limits<uint32_t>::max()) {
        return make_validation_error(
            ValidationError::INVALID_BLOCK,
            "Block height exceeds maximum allowed value"
        );
    }

    return core::success();
}

ValidationResult DefaultValidator::validate_transaction_structure(const Transaction& tx) {
    // Validate transaction has at least one input and one output
    if (tx.get_inputs().empty()) {
        return make_validation_error(
            ValidationError::INVALID_TRANSACTION,
            "Transaction must have at least one input"
        );
    }

    if (tx.get_outputs().empty()) {
        return make_validation_error(
            ValidationError::INVALID_TRANSACTION,
            "Transaction must have at least one output"
        );
    }

    // Validate transaction version
    if (tx.get_version() == 0) {
        return make_validation_error(
            ValidationError::INVALID_TRANSACTION,
            "Transaction version must be greater than zero"
        );
    }

    return core::success();
}

ValidationResult DefaultValidator::validate_address_format(const Address& addr) {
    // TODO: Implement proper address validation
    // For now, just check basic constraints
    (void)addr; // Suppress unused parameter warning
    return core::success();
}

ValidationResult DefaultValidator::validate_amount_range(const Amount& amount) {
    // TODO: Implement proper amount validation
    // Check for negative amounts, overflow, etc.
    (void)amount; // Suppress unused parameter warning
    return core::success();
}

ValidationResult DefaultValidator::validate_timestamp_range(const Timestamp& ts) {
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();

    // Allow timestamps within reasonable range (1 hour in past, 1 hour in future)
    constexpr int64_t ALLOWED_DRIFT_SECONDS = 3600;

    // TODO: Implement proper timestamp validation
    (void)ts; // Suppress unused parameter warning
    (void)now_seconds; // Suppress unused variable warning
    (void)ALLOWED_DRIFT_SECONDS; // Suppress unused variable warning

    return core::success();
}

ValidationResult DefaultValidator::validate_hash_length(const Hash& hash) {
    // TODO: Implement proper hash validation
    // Check hash length is correct for the algorithm
    (void)hash; // Suppress unused parameter warning
    return core::success();
}

// Factory function
std::unique_ptr<Validator> create_validator() {
    return std::make_unique<DefaultValidator>();
}

} // namespace serialization
} // namespace chainforge
