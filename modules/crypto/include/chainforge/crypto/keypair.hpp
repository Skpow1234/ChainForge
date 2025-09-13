#pragma once

#include "types.hpp"
#include <optional>

namespace chainforge::crypto {

/**
 * Key pair management for different cryptographic curves
 * Handles key generation, conversion, and validation
 */
class KeyPair {
public:
    // Constructor
    KeyPair() = default;

    // Copy and move
    KeyPair(const KeyPair&) = default;
    KeyPair& operator=(const KeyPair&) = default;
    KeyPair(KeyPair&&) = default;
    KeyPair& operator=(KeyPair&&) = default;

    // Destructor
    ~KeyPair() = default;

    // secp256k1 key pair operations
    struct Secp256k1KeyPair {
        Secp256k1PrivateKey private_key;
        Secp256k1PublicKey public_key;
        Secp256k1CompressedPublicKey compressed_public_key;
    };

    static CryptoResult<Secp256k1KeyPair> generate_secp256k1();
    static CryptoResult<Secp256k1KeyPair> from_secp256k1_private_key(const Secp256k1PrivateKey& private_key);
    static CryptoResult<Secp256k1PublicKey> derive_secp256k1_public_key(const Secp256k1PrivateKey& private_key);
    static CryptoResult<Secp256k1CompressedPublicKey> compress_secp256k1_public_key(const Secp256k1PublicKey& public_key);
    static CryptoResult<Secp256k1PublicKey> decompress_secp256k1_public_key(const Secp256k1CompressedPublicKey& compressed_key);

    // Ed25519 key pair operations
    struct Ed25519KeyPair {
        Ed25519PrivateKey private_key;
        Ed25519PublicKey public_key;
    };

    static CryptoResult<Ed25519KeyPair> generate_ed25519();
    static CryptoResult<Ed25519KeyPair> from_ed25519_private_key(const Ed25519PrivateKey& private_key);
    static CryptoResult<Ed25519PublicKey> derive_ed25519_public_key(const Ed25519PrivateKey& private_key);

    // BLS key pair operations
    struct BlsKeyPair {
        BlsPrivateKey private_key;
        BlsPublicKey public_key;
    };

    static CryptoResult<BlsKeyPair> generate_bls();
    static CryptoResult<BlsKeyPair> from_bls_private_key(const BlsPrivateKey& private_key);
    static CryptoResult<BlsPublicKey> derive_bls_public_key(const BlsPrivateKey& private_key);

    // Key validation
    static bool is_valid_secp256k1_private_key(const Secp256k1PrivateKey& private_key);
    static bool is_valid_secp256k1_public_key(const Secp256k1PublicKey& public_key);
    static bool is_valid_secp256k1_compressed_public_key(const Secp256k1CompressedPublicKey& compressed_key);

    static bool is_valid_ed25519_private_key(const Ed25519PrivateKey& private_key);
    static bool is_valid_ed25519_public_key(const Ed25519PublicKey& public_key);

    static bool is_valid_bls_private_key(const BlsPrivateKey& private_key);
    static bool is_valid_bls_public_key(const BlsPublicKey& public_key);

    // Key conversion to/from hex
    static std::string secp256k1_private_key_to_hex(const Secp256k1PrivateKey& key);
    static std::string secp256k1_public_key_to_hex(const Secp256k1PublicKey& key);
    static std::string secp256k1_compressed_public_key_to_hex(const Secp256k1CompressedPublicKey& key);

    static std::string ed25519_private_key_to_hex(const Ed25519PrivateKey& key);
    static std::string ed25519_public_key_to_hex(const Ed25519PublicKey& key);

    static std::string bls_private_key_to_hex(const BlsPrivateKey& key);
    static std::string bls_public_key_to_hex(const BlsPublicKey& key);

    static CryptoResult<Secp256k1PrivateKey> secp256k1_private_key_from_hex(const std::string& hex);
    static CryptoResult<Secp256k1PublicKey> secp256k1_public_key_from_hex(const std::string& hex);
    static CryptoResult<Secp256k1CompressedPublicKey> secp256k1_compressed_public_key_from_hex(const std::string& hex);

    static CryptoResult<Ed25519PrivateKey> ed25519_private_key_from_hex(const std::string& hex);
    static CryptoResult<Ed25519PublicKey> ed25519_public_key_from_hex(const std::string& hex);

    static CryptoResult<BlsPrivateKey> bls_private_key_from_hex(const std::string& hex);
    static CryptoResult<BlsPublicKey> bls_public_key_from_hex(const std::string& hex);

    // Address derivation (blockchain specific)
    static CryptoResult<Hash256> derive_ethereum_address(const Secp256k1PublicKey& public_key);
    static CryptoResult<Hash256> derive_bitcoin_address(const Secp256k1PublicKey& public_key);

private:
    // Internal key derivation implementations
    static CryptoResult<Secp256k1PublicKey> internal_derive_secp256k1_public_key(const Secp256k1PrivateKey& private_key);
    static CryptoResult<Ed25519PublicKey> internal_derive_ed25519_public_key(const Ed25519PrivateKey& private_key);
    static CryptoResult<BlsPublicKey> internal_derive_bls_public_key(const BlsPrivateKey& private_key);
};

} // namespace chainforge::crypto
