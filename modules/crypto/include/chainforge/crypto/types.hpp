#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <memory>

namespace chainforge::crypto {

// Type aliases for cryptographic sizes
using byte_t = uint8_t;

// Hash sizes
constexpr size_t SHA256_SIZE = 32;
constexpr size_t KECCAK256_SIZE = 32;
constexpr size_t RIPEMD160_SIZE = 20;
constexpr size_t HASH256_SIZE = 32;

// Key sizes
constexpr size_t SECP256K1_PRIVATE_KEY_SIZE = 32;
constexpr size_t SECP256K1_PUBLIC_KEY_SIZE = 64;  // Uncompressed without 04 prefix
constexpr size_t SECP256K1_COMPRESSED_PUBLIC_KEY_SIZE = 33;  // With 02/03 prefix
constexpr size_t SECP256K1_SIGNATURE_SIZE = 64;  // r + s

constexpr size_t ED25519_PRIVATE_KEY_SIZE = 32;
constexpr size_t ED25519_PUBLIC_KEY_SIZE = 32;
constexpr size_t ED25519_SIGNATURE_SIZE = 64;

constexpr size_t BLS_PRIVATE_KEY_SIZE = 32;
constexpr size_t BLS_PUBLIC_KEY_SIZE = 48;  // G1 point compressed
constexpr size_t BLS_SIGNATURE_SIZE = 96;  // G2 point compressed

// Array types for fixed-size crypto data
using Hash256 = std::array<byte_t, HASH256_SIZE>;
using Sha256Hash = std::array<byte_t, SHA256_SIZE>;
using Keccak256Hash = std::array<byte_t, KECCAK256_SIZE>;
using Ripemd160Hash = std::array<byte_t, RIPEMD160_SIZE>;

using Secp256k1PrivateKey = std::array<byte_t, SECP256K1_PRIVATE_KEY_SIZE>;
using Secp256k1PublicKey = std::array<byte_t, SECP256K1_PUBLIC_KEY_SIZE>;
using Secp256k1CompressedPublicKey = std::array<byte_t, SECP256K1_COMPRESSED_PUBLIC_KEY_SIZE>;
using Secp256k1Signature = std::array<byte_t, SECP256K1_SIGNATURE_SIZE>;

using Ed25519PrivateKey = std::array<byte_t, ED25519_PRIVATE_KEY_SIZE>;
using Ed25519PublicKey = std::array<byte_t, ED25519_PUBLIC_KEY_SIZE>;
using Ed25519Signature = std::array<byte_t, ED25519_SIGNATURE_SIZE>;

using BlsPrivateKey = std::array<byte_t, BLS_PRIVATE_KEY_SIZE>;
using BlsPublicKey = std::array<byte_t, BLS_PUBLIC_KEY_SIZE>;
using BlsSignature = std::array<byte_t, BLS_SIGNATURE_SIZE>;

// Variable-size containers
using ByteVector = std::vector<byte_t>;
using Message = ByteVector;

// Enum for curve types
enum class CurveType {
    SECP256K1,
    ED25519,
    BLS12_381
};

// Enum for hash algorithms
enum class HashAlgorithm {
    SHA256,
    KECCAK256,
    RIPEMD160,
    DOUBLE_SHA256,  // SHA256(SHA256(data))
    KECCAK256_RIPEMD160  // RIPEMD160(KECCAK256(data))
};

// Enum for signature algorithms
enum class SignatureAlgorithm {
    ECDSA_SECP256K1,
    EDDSA_ED25519,
    BLS
};

// Forward declarations for crypto classes
class Random;
class Hash;
class Signature;
class KeyPair;
class Curve;
class Keccak;

// Error codes for crypto operations
enum class CryptoError {
    SUCCESS = 0,
    INVALID_KEY = 1,
    INVALID_SIGNATURE = 2,
    INVALID_MESSAGE = 3,
    RANDOM_GENERATION_FAILED = 4,
    HASH_FAILED = 5,
    SIGNATURE_FAILED = 6,
    VERIFICATION_FAILED = 7,
    UNSUPPORTED_ALGORITHM = 8,
    INVALID_LENGTH = 9
};

// Result type for crypto operations
template<typename T>
struct CryptoResult {
    T value;
    CryptoError error;

    bool success() const { return error == CryptoError::SUCCESS; }
    explicit operator bool() const { return success(); }
};

// Specialization for void operations
template<>
struct CryptoResult<void> {
    CryptoError error;

    bool success() const { return error == CryptoError::SUCCESS; }
    explicit operator bool() const { return success(); }
};

} // namespace chainforge::crypto
