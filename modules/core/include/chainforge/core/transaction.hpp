#pragma once

#include "types.hpp"
#include "hash.hpp"
#include "address.hpp"
#include "amount.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace chainforge::core {

/**
 * Transaction class representing a blockchain transaction
 * Contains transaction data and validation logic
 */
class Transaction {
public:
    // Constructors
    Transaction() = default;
    Transaction(const Address& from, const Address& to, const Amount& value);
    Transaction(const TransactionData& data);
    
    // Copy and move
    Transaction(const Transaction&) = default;
    Transaction(Transaction&&) = default;
    Transaction& operator=(const Transaction&) = default;
    Transaction& operator=(Transaction&&) = default;
    
    // Destructor
    ~Transaction() = default;
    
    // Accessors
    const TransactionData& data() const noexcept { return data_; }
    TransactionData& data() noexcept { return data_; }
    
    // Field accessors
    const Address& from() const noexcept { return data_.from; }
    const Address& to() const noexcept { return data_.to; }
    const Amount& value() const noexcept { return data_.value; }
    GasLimit gas_limit() const noexcept { return data_.gas_limit; }
    GasPrice gas_price() const noexcept { return data_.gas_price; }
    const std::vector<uint8_t>& data() const noexcept { return data_.data; }
    uint64_t nonce() const noexcept { return data_.nonce; }
    
    // Setters
    void set_from(const Address& from);
    void set_to(const Address& to);
    void set_value(const Amount& value);
    void set_gas_limit(GasLimit gas_limit);
    void set_gas_price(GasPrice gas_price);
    void set_data(const std::vector<uint8_t>& data);
    void set_nonce(uint64_t nonce);
    
    // Transaction operations
    Hash calculate_hash() const;
    Amount calculate_fee() const;
    bool is_contract_creation() const noexcept;
    bool is_contract_call() const noexcept;
    bool is_transfer() const noexcept;
    
    // Validation
    bool is_valid() const;
    bool validate_signature() const;
    bool validate_gas() const;
    bool validate_amount() const;
    
    // Size and capacity
    size_t size() const;
    bool is_too_large() const;
    
    // Utility methods
    std::string to_string() const;
    std::string to_json() const;
    std::string to_hex() const;

private:
    TransactionData data_;
    mutable std::optional<Hash> cached_hash_;
    
    // Helper methods
    void invalidate_cache();
    bool validate_addresses() const;
    bool validate_nonce() const;
};

// Free functions
std::ostream& operator<<(std::ostream& os, const Transaction& transaction);
bool operator==(const Transaction& lhs, const Transaction& rhs);
bool operator!=(const Transaction& lhs, const Transaction& rhs);

// Transaction creation helpers
Transaction create_transfer_transaction(const Address& from, const Address& to, const Amount& value);
Transaction create_contract_transaction(const Address& from, const std::vector<uint8_t>& contract_data);
Transaction create_contract_call_transaction(const Address& from, const Address& contract, const std::vector<uint8_t>& call_data);

// Transaction validation
bool is_valid_transaction(const Transaction& transaction);
bool is_valid_transaction_data(const TransactionData& data);

// Gas calculation
GasLimit estimate_gas(const Transaction& transaction);
Amount calculate_transaction_fee(const Transaction& transaction);

} // namespace chainforge::core
