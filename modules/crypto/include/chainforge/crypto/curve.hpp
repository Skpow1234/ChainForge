#pragma once

#include "types.hpp"
#include <optional>

namespace chainforge::crypto {

/**
 * Elliptic curve operations
 * Provides low-level curve operations for different curves
 */
class Curve {
public:
    // Constructor
    Curve() = default;

    // Copy and move
    Curve(const Curve&) = default;
    Curve& operator=(const Curve&) = default;
    Curve(Curve&&) = default;
    Curve& operator=(Curve&&) = default;

    // Destructor
    ~Curve() = default;

    // secp256k1 curve operations
    class Secp256k1 {
    public:
        // Point operations
        static CryptoResult<Secp256k1PublicKey> multiply_base(const Secp256k1PrivateKey& scalar);
        static CryptoResult<Secp256k1PublicKey> multiply(const Secp256k1PublicKey& point, const Secp256k1PrivateKey& scalar);
        static CryptoResult<Secp256k1PublicKey> add(const Secp256k1PublicKey& p1, const Secp256k1PublicKey& p2);

        // Point validation
        static bool is_valid_point(const Secp256k1PublicKey& point);
        static bool is_point_at_infinity(const Secp256k1PublicKey& point);

        // Serialization
        static CryptoResult<Secp256k1CompressedPublicKey> compress_point(const Secp256k1PublicKey& point);
        static CryptoResult<Secp256k1PublicKey> decompress_point(const Secp256k1CompressedPublicKey& compressed_point);

        // Order and generator
        static const Secp256k1PrivateKey& order();
        static const Secp256k1PublicKey& generator();
    };

    // Ed25519 curve operations
    class Ed25519 {
    public:
        // Point operations
        static CryptoResult<Ed25519PublicKey> multiply_base(const Ed25519PrivateKey& scalar);
        static CryptoResult<Ed25519PublicKey> multiply(const Ed25519PublicKey& point, const Ed25519PrivateKey& scalar);
        static CryptoResult<Ed25519PublicKey> add(const Ed25519PublicKey& p1, const Ed25519PublicKey& p2);

        // Point validation
        static bool is_valid_point(const Ed25519PublicKey& point);
        static bool is_point_at_infinity(const Ed25519PublicKey& point);

        // Generator
        static const Ed25519PublicKey& generator();
    };

    // BLS12-381 curve operations
    class Bls12_381 {
    public:
        // G1 operations (public keys)
        static CryptoResult<BlsPublicKey> g1_multiply_base(const BlsPrivateKey& scalar);
        static CryptoResult<BlsPublicKey> g1_multiply(const BlsPublicKey& point, const BlsPrivateKey& scalar);
        static CryptoResult<BlsPublicKey> g1_add(const BlsPublicKey& p1, const BlsPublicKey& p2);

        // G2 operations (signatures)
        static CryptoResult<BlsSignature> g2_multiply_base(const BlsPrivateKey& scalar);
        static CryptoResult<BlsSignature> g2_multiply(const BlsSignature& point, const BlsPrivateKey& scalar);
        static CryptoResult<BlsSignature> g2_add(const BlsSignature& s1, const BlsSignature& s2);

        // Point validation
        static bool is_valid_g1_point(const BlsPublicKey& point);
        static bool is_valid_g2_point(const BlsSignature& point);

        // Pairing operations
        static CryptoResult<bool> pairing_check(const BlsPublicKey& g1_point, const BlsSignature& g2_point);

        // Generators
        static const BlsPublicKey& g1_generator();
        static const BlsSignature& g2_generator();
    };

private:
    // Internal implementations would use the respective crypto libraries
};

} // namespace chainforge::crypto
