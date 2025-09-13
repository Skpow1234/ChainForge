#pragma once

#include "types.hpp"
#include <string>
#include <memory>

namespace chainforge::crypto {

/**
 * Cryptographic hash functions
 * Provides secure hashing using vetted algorithms
 */
class Hash {
public:
    // Constructor
    Hash() = default;

    // Copy and move
    Hash(const Hash&) = default;
    Hash& operator=(const Hash&) = default;
    Hash(Hash&&) = default;
    Hash& operator=(Hash&&) = default;

    // Destructor
    ~Hash() = default;

    // Static hash functions for different algorithms
    static CryptoResult<Hash256> sha256(const ByteVector& data);
    static CryptoResult<Hash256> sha256(const byte_t* data, size_t length);
    static CryptoResult<Hash256> sha256(const std::string& data);

    static CryptoResult<Hash256> keccak256(const ByteVector& data);
    static CryptoResult<Hash256> keccak256(const byte_t* data, size_t length);
    static CryptoResult<Hash256> keccak256(const std::string& data);

    static CryptoResult<Ripemd160Hash> ripemd160(const ByteVector& data);
    static CryptoResult<Ripemd160Hash> ripemd160(const byte_t* data, size_t length);
    static CryptoResult<Ripemd160Hash> ripemd160(const std::string& data);

    // Double hash functions (common in blockchain)
    static CryptoResult<Hash256> double_sha256(const ByteVector& data);
    static CryptoResult<Hash256> double_sha256(const byte_t* data, size_t length);
    static CryptoResult<Hash256> double_sha256(const std::string& data);

    // Combined hash functions
    static CryptoResult<Ripemd160Hash> keccak256_ripemd160(const ByteVector& data);
    static CryptoResult<Ripemd160Hash> keccak256_ripemd160(const byte_t* data, size_t length);
    static CryptoResult<Ripemd160Hash> keccak256_ripemd160(const std::string& data);

    // Hash multiple inputs (Merkle tree construction)
    static CryptoResult<Hash256> hash_pair(const Hash256& left, const Hash256& right);
    static CryptoResult<Hash256> hash_many(const std::vector<Hash256>& hashes);

    // Utility functions
    static std::string to_hex(const Hash256& hash);
    static std::string to_hex(const Ripemd160Hash& hash);
    static CryptoResult<Hash256> from_hex(const std::string& hex_string);

    // Validation
    static bool is_valid_hex_hash(const std::string& hex_string, size_t expected_size = HASH256_SIZE);

private:
    // Internal hash implementations
    static CryptoResult<Hash256> internal_sha256(const byte_t* data, size_t length);
    static CryptoResult<Hash256> internal_keccak256(const byte_t* data, size_t length);
    static CryptoResult<Ripemd160Hash> internal_ripemd160(const byte_t* data, size_t length);
};

} // namespace chainforge::crypto
