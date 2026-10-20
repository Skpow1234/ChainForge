#include "chainforge/core/hash.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/crypto/hash.hpp"
#include "chainforge/crypto/random.hpp"
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace chainforge::core {
namespace {

core::Hash from_crypto_hash256(const chainforge::crypto::Hash256& value) {
    Hash256 data{};
    std::copy(value.begin(), value.end(), data.begin());
    return Hash(data);
}

std::string strip_0x(std::string hex) {
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex.erase(0, 2);
    }
    return hex;
}

} // namespace

Hash::Hash(const Hash256& data) : data_(data) {}

Hash::Hash(const std::string& hex_string) {
    const std::string hex = strip_0x(hex_string);
    if (hex.length() != HASH_SIZE * 2) {
        throw std::invalid_argument("Invalid hex string length for hash");
    }

    for (size_t i = 0; i < HASH_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        data_[i] = static_cast<uint8_t>(std::stoi(byte_str, nullptr, 16));
    }
}

Hash::Hash(const std::vector<uint8_t>& data) {
    if (data.size() != HASH_SIZE) {
        throw std::invalid_argument("Invalid data size for hash");
    }
    std::copy(data.begin(), data.end(), data_.begin());
}

std::string Hash::to_hex() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t byte : data_) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::vector<uint8_t> Hash::to_bytes() const {
    return std::vector<uint8_t>(data_.begin(), data_.end());
}

bool Hash::operator==(const Hash& other) const noexcept {
    return data_ == other.data_;
}

bool Hash::operator!=(const Hash& other) const noexcept {
    return !(*this == other);
}

bool Hash::operator<(const Hash& other) const noexcept {
    return data_ < other.data_;
}

bool Hash::is_zero() const noexcept {
    return std::all_of(data_.begin(), data_.end(), [](uint8_t b) { return b == 0; });
}

Hash Hash::zero() noexcept {
    Hash256 zero_data{};
    return Hash(zero_data);
}

Hash Hash::random() {
    auto result = chainforge::crypto::Random::generate_hash256();
    if (!result.success()) {
        return Hash::zero();
    }
    return from_crypto_hash256(result.value);
}

Hash hash_sha256(const std::vector<uint8_t>& data) {
    auto result = chainforge::crypto::Hash::sha256(data);
    if (!result.success()) {
        throw std::runtime_error("SHA256 hashing failed");
    }
    return from_crypto_hash256(result.value);
}

Hash hash_keccak256(const std::vector<uint8_t>& data) {
    auto result = chainforge::crypto::Hash::keccak256(data);
    if (!result.success()) {
        throw std::runtime_error("Keccak256 hashing failed");
    }
    return from_crypto_hash256(result.value);
}

Hash hash_ripemd160(const std::vector<uint8_t>& data) {
    auto result = chainforge::crypto::Hash::ripemd160(data);
    if (!result.success()) {
        throw std::runtime_error("RIPEMD160 hashing failed");
    }
    Hash256 padded{};
    std::copy(result.value.begin(), result.value.end(), padded.begin());
    return Hash(padded);
}

Hash combine_hashes(const Hash& left, const Hash& right) {
    chainforge::crypto::Hash256 left_bytes{};
    chainforge::crypto::Hash256 right_bytes{};
    std::copy(left.data().begin(), left.data().end(), left_bytes.begin());
    std::copy(right.data().begin(), right.data().end(), right_bytes.begin());
    auto result = chainforge::crypto::Hash::hash_pair(left_bytes, right_bytes);
    if (!result.success()) {
        throw std::runtime_error("Hash combination failed");
    }
    return from_crypto_hash256(result.value);
}

std::string hash_to_hex(const Hash& hash) {
    return hash.to_hex();
}

Hash hash_from_hex(const std::string& hex_string) {
    return Hash(hex_string);
}

} // namespace chainforge::core

namespace std {
template<>
struct hash<chainforge::core::Hash> {
    size_t operator()(const chainforge::core::Hash& h) const noexcept {
        size_t result = 0;
        for (size_t i = 0; i < 8 && i < h.data().size(); ++i) {
            result = (result << 8) | h.data()[i];
        }
        return result;
    }
};

template<>
struct hash<chainforge::core::Address> {
    size_t operator()(const chainforge::core::Address& addr) const noexcept {
        size_t result = 0;
        for (size_t i = 0; i < 8 && i < addr.data().size(); ++i) {
            result = (result << 8) | addr.data()[i];
        }
        return result;
    }
};
} // namespace std
