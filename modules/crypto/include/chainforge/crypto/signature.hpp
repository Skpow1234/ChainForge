#pragma once

#include "types.hpp"
#include <optional>

namespace chainforge::crypto {

/**
 * Digital signature operations
 * Supports ECDSA, EdDSA, and BLS signatures
 */
class Signature {
public:
    // Constructor
    Signature() = default;

    // Copy and move
    Signature(const Signature&) = default;
    Signature& operator=(const Signature&) = default;
    Signature(Signature&&) = default;
    Signature& operator=(Signature&&) = default;

    // Destructor
    ~Signature() = default;

    // ECDSA secp256k1 signatures
    static CryptoResult<Secp256k1Signature> ecdsa_secp256k1_sign(
        const Message& message,
        const Secp256k1PrivateKey& private_key
    );

    static CryptoResult<bool> ecdsa_secp256k1_verify(
        const Message& message,
        const Secp256k1Signature& signature,
        const Secp256k1PublicKey& public_key
    );

    static CryptoResult<Secp256k1PublicKey> ecdsa_secp256k1_recover_public_key(
        const Message& message,
        const Secp256k1Signature& signature,
        bool compressed = false
    );

    // Ed25519 signatures
    static CryptoResult<Ed25519Signature> ed25519_sign(
        const Message& message,
        const Ed25519PrivateKey& private_key
    );

    static CryptoResult<bool> ed25519_verify(
        const Message& message,
        const Ed25519Signature& signature,
        const Ed25519PublicKey& public_key
    );

    // BLS signatures
    static CryptoResult<BlsSignature> bls_sign(
        const Message& message,
        const BlsPrivateKey& private_key
    );

    static CryptoResult<bool> bls_verify(
        const Message& message,
        const BlsSignature& signature,
        const BlsPublicKey& public_key
    );

    // Aggregate BLS signatures
    static CryptoResult<BlsSignature> bls_aggregate_signatures(
        const std::vector<BlsSignature>& signatures
    );

    static CryptoResult<bool> bls_verify_aggregate(
        const std::vector<Message>& messages,
        const BlsSignature& aggregate_signature,
        const std::vector<BlsPublicKey>& public_keys
    );

    // Utility functions
    static std::string signature_to_hex(const Secp256k1Signature& sig);
    static std::string signature_to_hex(const Ed25519Signature& sig);
    static std::string signature_to_hex(const BlsSignature& sig);

    static CryptoResult<Secp256k1Signature> signature_from_hex_secp256k1(const std::string& hex);
    static CryptoResult<Ed25519Signature> signature_from_hex_ed25519(const std::string& hex);
    static CryptoResult<BlsSignature> signature_from_hex_bls(const std::string& hex);

    // Recovery ID for ECDSA (used in Ethereum)
    static CryptoResult<uint8_t> ecdsa_get_recovery_id(
        const Message& message,
        const Secp256k1Signature& signature,
        const Secp256k1PublicKey& public_key
    );

private:
    // Internal signature implementations
    static CryptoResult<Secp256k1Signature> internal_ecdsa_sign(
        const byte_t* message, size_t message_len,
        const Secp256k1PrivateKey& private_key
    );

    static CryptoResult<bool> internal_ecdsa_verify(
        const byte_t* message, size_t message_len,
        const Secp256k1Signature& signature,
        const Secp256k1PublicKey& public_key
    );

    static CryptoResult<Ed25519Signature> internal_ed25519_sign(
        const byte_t* message, size_t message_len,
        const Ed25519PrivateKey& private_key
    );

    static CryptoResult<bool> internal_ed25519_verify(
        const byte_t* message, size_t message_len,
        const Ed25519Signature& signature,
        const Ed25519PublicKey& public_key
    );

    static CryptoResult<BlsSignature> internal_bls_sign(
        const byte_t* message, size_t message_len,
        const BlsPrivateKey& private_key
    );

    static CryptoResult<bool> internal_bls_verify(
        const byte_t* message, size_t message_len,
        const BlsSignature& signature,
        const BlsPublicKey& public_key
    );
};

} // namespace chainforge::crypto
