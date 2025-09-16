#pragma once

#include "types.hpp"
#include <string>
#include <memory>

namespace chainforge::crypto {

/**
 * Keccak hash function implementation
 * Provides optimized Keccak operations for blockchain use
 */
class Keccak {
public:
    // Constructor
    Keccak() = default;

    // Copy and move
    Keccak(const Keccak&) = default;
    Keccak& operator=(const Keccak&) = default;
    Keccak(Keccak&&) = default;
    Keccak& operator=(Keccak&&) = default;

    // Destructor
    ~Keccak() = default;

    // Keccak hash functions (different output sizes)
    static CryptoResult<Hash256> keccak256(const ByteVector& data);
    static CryptoResult<Hash256> keccak256(const byte_t* data, size_t length);
    static CryptoResult<Hash256> keccak256(const std::string& data);

    // Keccak with different output sizes
    static CryptoResult<std::array<byte_t, 20>> keccak160(const ByteVector& data);
    static CryptoResult<std::array<byte_t, 20>> keccak160(const byte_t* data, size_t length);

    static CryptoResult<std::array<byte_t, 48>> keccak384(const ByteVector& data);
    static CryptoResult<std::array<byte_t, 48>> keccak384(const byte_t* data, size_t length);

    static CryptoResult<std::array<byte_t, 64>> keccak512(const ByteVector& data);
    static CryptoResult<std::array<byte_t, 64>> keccak512(const byte_t* data, size_t length);

    // Streaming Keccak interface for large data
    class StreamHasher {
    public:
        StreamHasher();
        ~StreamHasher();

        // Update with data
        void update(const ByteVector& data);
        void update(const byte_t* data, size_t length);
        void update(const std::string& data);

        // Finalize and get hash
        CryptoResult<Hash256> finalize_256();
        CryptoResult<std::array<byte_t, 20>> finalize_160();
        CryptoResult<std::array<byte_t, 48>> finalize_384();
        CryptoResult<std::array<byte_t, 64>> finalize_512();

        // Reset for reuse
        void reset();

    private:
        class Impl;
        std::unique_ptr<Impl> impl_;
    };

    // Utility functions
    static std::string to_hex(const Hash256& hash);
    static std::string to_hex(const std::array<byte_t, 20>& hash);
    static std::string to_hex(const std::array<byte_t, 48>& hash);
    static std::string to_hex(const std::array<byte_t, 64>& hash);

    // Ethereum-specific Keccak operations
    static CryptoResult<Hash256> ethereum_address_hash(const ByteVector& data);
    static CryptoResult<Hash256> ethereum_signature_hash(const ByteVector& data);

    // Mining-related operations (for PoW)
    static void keccak_f800_round(byte_t* state);
    static void keccak_f1600_round(byte_t* state);

private:
    // Internal Keccak implementation
    static CryptoResult<Hash256> internal_keccak(const byte_t* data, size_t length, size_t output_bits);
};

} // namespace chainforge::crypto
