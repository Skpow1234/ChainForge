#include "chainforge/core/address.hpp"
#include <iomanip>
#include <sstream>
#include <random>
#include <algorithm>

namespace chainforge::core {

Address::Address(const Address160& data) : data_(data) {}

Address::Address(const std::string& hex_string) {
    if (hex_string.length() != ADDRESS_SIZE * 2) {
        throw std::invalid_argument("Invalid hex string length for address");
    }

    for (size_t i = 0; i < ADDRESS_SIZE; ++i) {
        std::string byte_str = hex_string.substr(i * 2, 2);
        data_[i] = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
    }
}

Address::Address(const std::vector<uint8_t>& data) {
    if (data.size() != ADDRESS_SIZE) {
        throw std::invalid_argument("Invalid data size for address");
    }
    std::copy(data.begin(), data.end(), data_.begin());
}

std::string Address::to_hex() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (uint8_t byte : data_) {
        ss << std::setw(2) << static_cast<int>(byte);
    }

    return ss.str();
}

std::vector<uint8_t> Address::to_bytes() const {
    return std::vector<uint8_t>(data_.begin(), data_.end());
}

bool Address::is_valid() const noexcept {
    return !is_zero();  // For now, just check it's not zero
}

bool Address::is_contract() const noexcept {
    // Contract addresses typically have the last byte >= 0x80
    // This is a simplified check
    return data_[ADDRESS_SIZE - 1] >= 0x80;
}

bool Address::is_zero() const noexcept {
    return std::all_of(data_.begin(), data_.end(), [](uint8_t b) { return b == 0; });
}

bool Address::operator==(const Address& other) const noexcept {
    return data_ == other.data_;
}

bool Address::operator!=(const Address& other) const noexcept {
    return !(*this == other);
}

bool Address::operator<(const Address& other) const noexcept {
    return data_ < other.data_;
}

Address Address::zero() noexcept {
    Address160 zero_data{};
    return Address(zero_data);
}

Address Address::random() {
    Address160 random_data;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);

    for (auto& byte : random_data) {
        byte = dis(gen);
    }
    return Address(random_data);
}

Address Address::from_public_key(const std::vector<uint8_t>& public_key) {
    // TODO: Implement proper address derivation from public key
    // For now, return a placeholder address derived from the key
    Address160 addr_data{};
    auto copy_size = std::min(public_key.size(), ADDRESS_SIZE);
    std::copy(public_key.begin(), public_key.begin() + static_cast<long>(copy_size), addr_data.begin());
    return Address(addr_data);
}

// Free functions
Address address_from_hex(const std::string& hex_string) {
    return Address(hex_string);
}

std::string address_to_hex(const Address& address) {
    return address.to_hex();
}

bool is_valid_address(const std::string& hex_string) {
    try {
        Address addr(hex_string);
        return addr.is_valid();
    } catch (const std::exception&) {
        return false;
    }
}

Address derive_address_from_public_key(const std::vector<uint8_t>& public_key) {
    return Address::from_public_key(public_key);
}

Address derive_contract_address(const Address& sender, uint64_t nonce) {
    // Simple contract address derivation: hash(sender + nonce)
    // TODO: Implement proper contract address derivation
    std::vector<uint8_t> data;
    data.reserve(ADDRESS_SIZE + sizeof(nonce));

    auto sender_bytes = sender.to_bytes();
    data.insert(data.end(), sender_bytes.begin(), sender_bytes.end());

    // Add nonce as bytes (big-endian)
    for (int i = sizeof(nonce) - 1; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((nonce >> (i * 8)) & 0xFF));
    }

    // For now, just take first 20 bytes as contract address
    Address160 contract_addr{};
    auto copy_size = std::min(data.size(), ADDRESS_SIZE);
    std::copy(data.begin(), data.begin() + static_cast<long>(copy_size), contract_addr.begin());
    return Address(contract_addr);
}

} // namespace chainforge::core
