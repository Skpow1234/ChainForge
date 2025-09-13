#pragma once

#include "types.hpp"
#include <random>
#include <mutex>

namespace chainforge::crypto {

/**
 * Random number generation for cryptographic operations
 * Provides secure random bytes using system entropy
 */
class Random {
public:
    // Constructor
    Random() = default;

    // Copy and move (not allowed for security)
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;
    Random(Random&&) = delete;
    Random& operator=(Random&&) = delete;

    // Destructor
    ~Random() = default;

    // Generate random bytes
    static CryptoResult<ByteVector> generate_bytes(size_t length);

    // Fill existing buffer with random bytes
    static CryptoResult<void> fill_bytes(ByteVector& buffer);
    static CryptoResult<void> fill_bytes(byte_t* buffer, size_t length);

    // Generate random values of specific types
    static CryptoResult<Hash256> generate_hash256();
    static CryptoResult<Secp256k1PrivateKey> generate_secp256k1_private_key();
    static CryptoResult<Ed25519PrivateKey> generate_ed25519_private_key();
    static CryptoResult<BlsPrivateKey> generate_bls_private_key();

    // Generate random uint64_t
    static CryptoResult<uint64_t> generate_uint64();
    static CryptoResult<uint64_t> generate_uint64(uint64_t min, uint64_t max);

    // Generate random uint32_t
    static CryptoResult<uint32_t> generate_uint32();
    static CryptoResult<uint32_t> generate_uint32(uint32_t min, uint32_t max);

    // Thread-safe random device access
    static std::random_device& get_random_device();

private:
    // Thread-local random device for better performance
    static thread_local std::random_device random_device_;
    static thread_local std::mt19937 generator_;
    static std::mutex mutex_;  // For any non-thread-local operations

    // Initialize thread-local generator
    static void initialize_generator();
};

} // namespace chainforge::crypto
