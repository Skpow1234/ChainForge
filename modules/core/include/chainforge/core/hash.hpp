#pragma once

#include "types.hpp"
#include <string>
#include <array>
#include <vector>

namespace chainforge::core {

/**
 * Hash class for cryptographic hash operations
 * Provides a type-safe wrapper around hash values
 */
class Hash {
public:
    // Constructors
    Hash() = default;
    explicit Hash(const Hash256& data);
    explicit Hash(const std::string& hex_string);
    explicit Hash(const std::vector<uint8_t>& data);
    
    // Copy and move
    Hash(const Hash&) = default;
    Hash(Hash&&) = default;
    Hash& operator=(const Hash&) = default;
    Hash& operator=(Hash&&) = default;
    
    // Destructor
    ~Hash() = default;
    
    // Accessors
    const Hash256& data() const noexcept { return data_; }
    Hash256& data() noexcept { return data_; }
    
    // Conversion methods
    std::string to_hex() const;
    std::vector<uint8_t> to_bytes() const;
    
    // Comparison operators
    bool operator==(const Hash& other) const noexcept;
    bool operator!=(const Hash& other) const noexcept;
    bool operator<(const Hash& other) const noexcept;
    
    // Utility methods
    bool is_zero() const noexcept;
    static Hash zero() noexcept;
    static Hash random();
    
    // Size information
    static constexpr size_t size() noexcept { return HASH_SIZE; }

private:
    Hash256 data_;
};

// Free functions
Hash hash_sha256(const std::vector<uint8_t>& data);
Hash hash_keccak256(const std::vector<uint8_t>& data);
Hash hash_ripemd160(const std::vector<uint8_t>& data);

// Hash combination (for Merkle trees)
Hash combine_hashes(const Hash& left, const Hash& right);

// String conversion helpers
std::string hash_to_hex(const Hash& hash);
Hash hash_from_hex(const std::string& hex_string);

} // namespace chainforge::core
