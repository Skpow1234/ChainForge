#include "tokiwa/core/address.hpp"
#include "tokiwa/core/hash.hpp"
#include "tokiwa/crypto/hash.hpp"
#include "tokiwa/crypto/random.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace tokiwa::core {
namespace {

std::string strip_0x(std::string hex) {
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex.erase(0, 2);
    }
    return hex;
}

std::vector<uint8_t> normalize_pubkey(const std::vector<uint8_t>& public_key) {
    // Accept 64-byte uncompressed (no 0x04) or 65-byte with 0x04 prefix.
    if (public_key.size() == 65 && public_key[0] == 0x04) {
        return std::vector<uint8_t>(public_key.begin() + 1, public_key.end());
    }
    if (public_key.size() == 64) {
        return public_key;
    }
    // Compressed 33-byte keys are not expanded here (needs curve decompress).
    throw std::invalid_argument("Public key must be 64 bytes (uncompressed) or 65 bytes with 0x04 prefix");
}

} // namespace

Address::Address(const Address160& data) : data_(data) {}

Address::Address(const std::string& hex_string) {
    const std::string hex = strip_0x(hex_string);
    if (hex.length() != ADDRESS_SIZE * 2) {
        throw std::invalid_argument("Invalid hex string length for address");
    }

    for (size_t i = 0; i < ADDRESS_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
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
    // EIP-55 checksummed hex without 0x prefix
    std::string lower;
    lower.reserve(ADDRESS_SIZE * 2);
    {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (uint8_t byte : data_) {
            ss << std::setw(2) << static_cast<int>(byte);
        }
        lower = ss.str();
    }

    auto hash_result = tokiwa::crypto::Hash::keccak256(lower);
    if (!hash_result.success()) {
        return lower;
    }
    const auto hash_hex = tokiwa::crypto::Hash::to_hex(hash_result.value);

    std::string checksummed;
    checksummed.reserve(lower.size());
    for (size_t i = 0; i < lower.size(); ++i) {
        const char c = lower[i];
        if (c >= '0' && c <= '9') {
            checksummed.push_back(c);
            continue;
        }
        // If corresponding hash nibble >= 8, uppercase
        const int nibble = std::stoi(std::string(1, hash_hex[i]), nullptr, 16);
        checksummed.push_back(nibble >= 8 ? static_cast<char>(std::toupper(c)) : c);
    }
    return checksummed;
}

std::vector<uint8_t> Address::to_bytes() const {
    return std::vector<uint8_t>(data_.begin(), data_.end());
}

bool Address::is_valid() const noexcept {
    return true; // Any 20-byte value is a valid address encoding
}

bool Address::is_contract() const noexcept {
    // Cannot know without state; never infer from last byte.
    return false;
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
    auto bytes = tokiwa::crypto::Random::generate_bytes(ADDRESS_SIZE);
    if (!bytes.success() || bytes.value.size() != ADDRESS_SIZE) {
        return Address::zero();
    }
    Address160 data{};
    std::copy(bytes.value.begin(), bytes.value.end(), data.begin());
    return Address(data);
}

Address Address::from_public_key(const std::vector<uint8_t>& public_key) {
    const auto key = normalize_pubkey(public_key);
    auto keccak = tokiwa::crypto::Hash::keccak256(key);
    if (!keccak.success()) {
        throw std::runtime_error("Failed to hash public key for address derivation");
    }
    // Ethereum address = last 20 bytes of keccak256(pubkey[0..64])
    Address160 addr{};
    std::copy(keccak.value.end() - ADDRESS_SIZE, keccak.value.end(), addr.begin());
    return Address(addr);
}

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
    // Simplified CREATE: keccak256(sender || nonce_be)[12:]
    // (Full Ethereum CREATE uses RLP([sender, nonce]).)
    std::vector<uint8_t> data;
    data.reserve(ADDRESS_SIZE + sizeof(nonce));
    auto sender_bytes = sender.to_bytes();
    data.insert(data.end(), sender_bytes.begin(), sender_bytes.end());
    for (int i = 7; i >= 0; --i) {
        data.push_back(static_cast<uint8_t>((nonce >> (i * 8)) & 0xFF));
    }

    auto hash = hash_keccak256(data);
    Address160 contract_addr{};
    auto bytes = hash.to_bytes();
    std::copy(bytes.end() - ADDRESS_SIZE, bytes.end(), contract_addr.begin());
    return Address(contract_addr);
}

} // namespace tokiwa::core
