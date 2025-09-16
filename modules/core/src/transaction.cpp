#include "chainforge/core/transaction.hpp"
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace chainforge::core {

Transaction::Transaction(const Address& from, const Address& to, const Amount& value)
    : data_{from, to, value, 21000, 1, {}, 0} {}

Transaction::Transaction(const TransactionData& data) : data_(data) {}

void Transaction::set_from(const Address& from) {
    data_.from = from;
    invalidate_cache();
}

void Transaction::set_to(const Address& to) {
    data_.to = to;
    invalidate_cache();
}

void Transaction::set_value(const Amount& value) {
    data_.value = value;
    invalidate_cache();
}

void Transaction::set_gas_limit(GasLimit gas_limit) {
    data_.gas_limit = gas_limit;
    invalidate_cache();
}

void Transaction::set_gas_price(GasPrice gas_price) {
    data_.gas_price = gas_price;
    invalidate_cache();
}

void Transaction::set_data(const std::vector<uint8_t>& data) {
    data_.data = data;
    invalidate_cache();
}

void Transaction::set_nonce(uint64_t nonce) {
    data_.nonce = nonce;
    invalidate_cache();
}

Hash Transaction::calculate_hash() const {
    if (cached_hash_.has_value()) {
        return cached_hash_.value();
    }

    // Create a simple hash from transaction data
    // TODO: Implement proper transaction hashing with RLP encoding
    std::vector<uint8_t> hash_data;

    // Add addresses
    auto from_bytes = data_.from.to_bytes();
    hash_data.insert(hash_data.end(), from_bytes.begin(), from_bytes.end());

    auto to_bytes = data_.to.to_bytes();
    hash_data.insert(hash_data.end(), to_bytes.begin(), to_bytes.end());

    // Add value as bytes (big-endian)
    auto value_wei = data_.value.wei();
    for (int i = sizeof(value_wei) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((value_wei >> (i * 8)) & 0xFF));
    }

    // Add gas limit and price
    for (int i = sizeof(data_.gas_limit) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((data_.gas_limit >> (i * 8)) & 0xFF));
    }

    for (int i = sizeof(data_.gas_price) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((data_.gas_price >> (i * 8)) & 0xFF));
    }

    // Add nonce
    for (int i = sizeof(data_.nonce) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((data_.nonce >> (i * 8)) & 0xFF));
    }

    // Add data
    hash_data.insert(hash_data.end(), data_.data.begin(), data_.data.end());

    cached_hash_ = hash_sha256(hash_data);
    return cached_hash_.value();
}

Amount Transaction::calculate_fee() const {
    return Amount(data_.gas_limit * data_.gas_price);
}

bool Transaction::is_contract_creation() const noexcept {
    return data_.to.is_zero() && !data_.data.empty();
}

bool Transaction::is_contract_call() const noexcept {
    return !data_.to.is_zero() && !data_.data.empty();
}

bool Transaction::is_transfer() const noexcept {
    return !data_.to.is_zero() && data_.data.empty();
}

bool Transaction::is_valid() const {
    return validate_addresses() && validate_gas() && validate_amount() && validate_nonce();
}

bool Transaction::validate_signature() const {
    // TODO: Implement signature validation
    return true;
}

bool Transaction::validate_gas() const {
    return data_.gas_limit >= 21000 && data_.gas_price > 0;
}

bool Transaction::validate_amount() const {
    return !data_.value.is_zero() || is_contract_creation();
}

size_t Transaction::size() const {
    size_t size = sizeof(TransactionData);
    size += data_.data.size();
    return size;
}

bool Transaction::is_too_large() const {
    return size() > MAX_TRANSACTION_SIZE;
}

std::string Transaction::to_string() const {
    std::stringstream ss;
    ss << "Transaction{"
       << "from: " << data_.from.to_hex()
       << ", to: " << data_.to.to_hex()
       << ", value: " << data_.value.to_string()
       << ", gas_limit: " << data_.gas_limit
       << ", gas_price: " << data_.gas_price
       << ", nonce: " << data_.nonce
       << ", data_size: " << data_.data.size()
       << "}";
    return ss.str();
}

std::string Transaction::to_json() const {
    nlohmann::json j;
    j["from"] = data_.from.to_hex();
    j["to"] = data_.to.to_hex();
    j["value"] = data_.value.to_string();
    j["gasLimit"] = data_.gas_limit;
    j["gasPrice"] = data_.gas_price;
    j["nonce"] = data_.nonce;
    j["data"] = "0x" + hash_from_hex("00").to_hex(); // Placeholder
    j["hash"] = calculate_hash().to_hex();

    return j.dump(2);
}

std::string Transaction::to_hex() const {
    // TODO: Implement proper RLP encoding
    return calculate_hash().to_hex();
}

void Transaction::invalidate_cache() {
    cached_hash_.reset();
}

bool Transaction::validate_addresses() const {
    return data_.from.is_valid() && (data_.to.is_valid() || is_contract_creation());
}

bool Transaction::validate_nonce() const {
    return true; // For now, always valid
}

// Free functions
std::ostream& operator<<(std::ostream& os, const Transaction& transaction) {
    os << transaction.to_string();
    return os;
}

bool operator==(const Transaction& lhs, const Transaction& rhs) {
    return lhs.calculate_hash() == rhs.calculate_hash();
}

bool operator!=(const Transaction& lhs, const Transaction& rhs) {
    return !(lhs == rhs);
}

Transaction create_transfer_transaction(const Address& from, const Address& to, const Amount& value) {
    return Transaction(from, to, value);
}

Transaction create_contract_transaction(const Address& from, const std::vector<uint8_t>& contract_data) {
    Transaction tx(from, Address::zero(), Amount::zero());
    tx.set_data(contract_data);
    tx.set_gas_limit(53000); // Higher gas limit for contract creation
    return tx;
}

Transaction create_contract_call_transaction(const Address& from, const Address& contract, const std::vector<uint8_t>& call_data) {
    Transaction tx(from, contract, Amount::zero());
    tx.set_data(call_data);
    return tx;
}

bool is_valid_transaction(const Transaction& transaction) {
    return transaction.is_valid();
}

bool is_valid_transaction_data(const TransactionData& data) {
    Transaction tx(data);
    return tx.is_valid();
}

GasLimit estimate_gas(const Transaction& transaction) {
    if (transaction.is_contract_creation()) {
        return 53000 + transaction.payload().size() * 200; // Rough estimate
    } else if (transaction.is_contract_call()) {
        return 21000 + transaction.payload().size() * 68; // Rough estimate
    } else {
        return 21000; // Standard transfer
    }
}

Amount calculate_transaction_fee(const Transaction& transaction) {
    return transaction.calculate_fee();
}

} // namespace chainforge::core
