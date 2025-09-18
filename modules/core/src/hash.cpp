#include "chainforge/core/hash.hpp"
#include "chainforge/core/address.hpp"
#include <iomanip>
#include <sstream>
#include <random>
#include <algorithm>

// TODO: Include crypto module when available
// #include "chainforge/crypto/hash.hpp"

namespace chainforge::core {

Hash::Hash(const Hash256& data) : data_(data) {}

Hash::Hash(const std::string& hex_string) {
    if (hex_string.length() != HASH_SIZE * 2) {
        throw std::invalid_argument("Invalid hex string length for hash");
    }

    for (size_t i = 0; i < HASH_SIZE; ++i) {
        std::string byte_str = hex_string.substr(i * 2, 2);
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
    Hash256 random_data;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);
    
    for (auto& byte : random_data) {
        byte = dis(gen);
    }
    return Hash(random_data);
}

// Free functions
Hash hash_sha256(const std::vector<uint8_t>& data) {
    // TODO: Implement actual SHA256 hashing
    // For now, return a placeholder hash
    Hash256 hash_data{};
    auto copy_size = static_cast<ptrdiff_t>(std::min(data.size(), HASH_SIZE));
    std::copy(data.begin(), data.begin() + copy_size, hash_data.begin());
    return Hash(hash_data);
}

Hash hash_keccak256(const std::vector<uint8_t>& data) {
    // TODO: Implement actual Keccak256 hashing
    // For now, return a placeholder hash
    Hash256 hash_data{};
    auto copy_size = static_cast<ptrdiff_t>(std::min(data.size(), HASH_SIZE));
    std::copy(data.begin(), data.begin() + copy_size, hash_data.begin());
    return Hash(hash_data);
}

Hash hash_ripemd160(const std::vector<uint8_t>& data) {
    // TODO: Implement actual RIPEMD160 hashing
    // For now, return a placeholder hash
    Hash256 hash_data{};
    auto copy_size = static_cast<ptrdiff_t>(std::min(data.size(), HASH_SIZE));
    std::copy(data.begin(), data.begin() + copy_size, hash_data.begin());
    return Hash(hash_data);
}

Hash combine_hashes(const Hash& left, const Hash& right) {
    // Simple combination for now - in practice, this would be more sophisticated
    Hash256 combined{};
    for (size_t i = 0; i < HASH_SIZE / 2; ++i) {
        combined[i] = left.data()[i];
        combined[i + HASH_SIZE / 2] = right.data()[i];
    }
    return Hash(combined);
}

std::string hash_to_hex(const Hash& hash) {
    return hash.to_hex();
}

Hash hash_from_hex(const std::string& hex_string) {
    return Hash(hex_string);
}

} // namespace chainforge::core

// Hash specializations for std::unordered_map
namespace std {
    template<>
    struct hash<chainforge::core::Hash> {
        size_t operator()(const chainforge::core::Hash& h) const noexcept {
            // Simple hash based on the first 8 bytes
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
            // Simple hash based on the first 8 bytes
            size_t result = 0;
            for (size_t i = 0; i < 8 && i < addr.data().size(); ++i) {
                result = (result << 8) | addr.data()[i];
            }
            return result;
        }
    };
}