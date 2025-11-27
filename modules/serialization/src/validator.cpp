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
    for (const auto& tx : block.transactions()) {
        auto tx_result = validate_transaction(tx);
        if (!tx_result.has_value()) {
            return tx_result;
        }
    }

    // Validate block hash
    auto hash_result = validate_hash(block.calculate_hash());
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

    // Validate addresses
    auto from_result = validate_address(tx.from());
    if (!from_result.has_value()) {
        return from_result;
    }

    auto to_result = validate_address(tx.to());
    if (!to_result.has_value()) {
        return to_result;
    }

    // Validate amount
    auto amount_result = validate_amount(tx.value());
    if (!amount_result.has_value()) {
        return amount_result;
    }

    // Validate gas parameters
    if (tx.gas_limit() == 0) {
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
    auto ts_result = validate_timestamp(block.timestamp());
    if (!ts_result.has_value()) {
        return ts_result;
    }

    // Validate gas limit is positive
    if (block.gas_limit() == 0) {
        return make_validation_error(
            ValidationError::INVALID_BLOCK,
            "Block gas limit must be greater than zero"
        );
    }

    // Validate block height
    if (block.height() > std::numeric_limits<uint32_t>::max()) {
        return make_validation_error(
            ValidationError::INVALID_BLOCK,
            "Block height exceeds maximum allowed value"
        );
    }

    return core::success();
}

ValidationResult DefaultValidator::validate_transaction_structure(const Transaction& tx) {
    // Validate transaction has valid addresses
    if (tx.from().is_zero()) {
        return make_validation_error(
            ValidationError::INVALID_TRANSACTION,
            "Transaction from address cannot be zero"
        );
    }

    // Note: to address can be zero for contract creation
    
    // Validate amount is valid
    if (!tx.value().is_valid()) {
        return make_validation_error(
            ValidationError::INVALID_AMOUNT,
            "Transaction value is invalid"
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
