#include "chainforge/core/transaction.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace chainforge::core {

Transaction::Transaction(const Address& from, const Address& to, const Amount& value)
    : data_{from.data(), to.data(), value.wei(), 21000, 1, {}, 0, {}} {}

Transaction::Transaction(const TransactionData& data) : data_(data) {}

void Transaction::set_from(const Address& from) {
    data_.from = from.data();
    invalidate_cache();
}

void Transaction::set_to(const Address& to) {
    data_.to = to.data();
    invalidate_cache();
}

void Transaction::set_value(const Amount& value) {
    data_.value = value.wei();
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

    // Canonical preimage (RLP comes later): keccak256 of field concatenation
    std::vector<uint8_t> hash_data;
    hash_data.insert(hash_data.end(), data_.from.begin(), data_.from.end());
    hash_data.insert(hash_data.end(), data_.to.begin(), data_.to.end());

    auto append_be = [&](uint64_t value, size_t width) {
        for (int i = static_cast<int>(width) - 1; i >= 0; --i) {
            hash_data.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
        }
    };

    append_be(data_.value, 8);
    append_be(data_.gas_limit, 8);
    append_be(data_.gas_price, 8);
    append_be(data_.nonce, 8);
    hash_data.insert(hash_data.end(), data_.data.begin(), data_.data.end());

    cached_hash_ = hash_keccak256(hash_data);
    return cached_hash_.value();
}

Amount Transaction::calculate_fee() const {
    return Amount::from_wei(data_.gas_limit * data_.gas_price);
}

bool Transaction::is_contract_creation() const noexcept {
    Address to_addr(data_.to);
    return to_addr.is_zero() && !data_.data.empty();
}

bool Transaction::is_contract_call() const noexcept {
    Address to_addr(data_.to);
    return !to_addr.is_zero() && !data_.data.empty();
}

bool Transaction::is_transfer() const noexcept {
    Address to_addr(data_.to);
    return !to_addr.is_zero() && data_.data.empty();
}

bool Transaction::is_valid() const {
    return validate_addresses() && validate_gas() && validate_amount() && validate_nonce();
}

bool Transaction::validate_signature() const {
    // Unsigned txs are allowed in local/dev pools; when present, require 65-byte r||s||v.
    if (data_.signature.empty()) {
        return true;
    }
    return data_.signature.size() == 65;
}

bool Transaction::validate_gas() const {
    return data_.gas_limit >= 21000 && data_.gas_price > 0;
}

bool Transaction::validate_amount() const {
    Amount value(data_.value);
    return !value.is_zero() || is_contract_creation();
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
    Address from_addr(data_.from);
    Address to_addr(data_.to);
    Amount value(data_.value);
    ss << "Transaction{"
       << "from: " << from_addr.to_hex()
       << ", to: " << to_addr.to_hex()
       << ", value: " << value.to_string()
       << ", gas_limit: " << data_.gas_limit
       << ", gas_price: " << data_.gas_price
       << ", nonce: " << data_.nonce
       << ", data_size: " << data_.data.size()
       << "}";
    return ss.str();
}

std::string Transaction::to_json() const {
    nlohmann::json j;
    Address from_addr(data_.from);
    Address to_addr(data_.to);
    Amount value(data_.value);
    j["from"] = from_addr.to_hex();
    j["to"] = to_addr.to_hex();
    j["value"] = value.to_string();
    j["gasLimit"] = data_.gas_limit;
    j["gasPrice"] = data_.gas_price;
    j["nonce"] = data_.nonce;
    {
        std::ostringstream oss;
        oss << "0x" << std::hex << std::setfill('0');
        for (uint8_t b : data_.data) {
            oss << std::setw(2) << static_cast<int>(b);
        }
        j["data"] = oss.str();
    }
    j["hash"] = "0x" + calculate_hash().to_hex();

    return j.dump(2);
}

std::string Transaction::to_hex() const {
    // Hex of the hash preimage fields (not RLP yet)
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (uint8_t b : data_.from) oss << std::setw(2) << static_cast<int>(b);
    for (uint8_t b : data_.to) oss << std::setw(2) << static_cast<int>(b);
    auto append_be = [&](uint64_t value) {
        for (int i = 7; i >= 0; --i) {
            oss << std::setw(2) << static_cast<int>((value >> (i * 8)) & 0xFF);
        }
    };
    append_be(data_.value);
    append_be(data_.gas_limit);
    append_be(data_.gas_price);
    append_be(data_.nonce);
    for (uint8_t b : data_.data) oss << std::setw(2) << static_cast<int>(b);
    return oss.str();
}

void Transaction::invalidate_cache() {
    cached_hash_.reset();
}

bool Transaction::validate_addresses() const {
    Address from_addr(data_.from);
    Address to_addr(data_.to);
    return from_addr.is_valid() && (to_addr.is_valid() || is_contract_creation());
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
