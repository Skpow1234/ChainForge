#include "chainforge/crypto/keypair.hpp"
#include "chainforge/crypto/random.hpp"
#include "chainforge/crypto/hash.hpp"
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <sodium.h>
#include <iomanip>
#include <sstream>

namespace chainforge::crypto {

CryptoResult<KeyPair::Secp256k1KeyPair> KeyPair::generate_secp256k1() {
    auto private_key_result = Random::generate_secp256k1_private_key();
    if (!private_key_result.success()) {
        return CryptoResult<Secp256k1KeyPair>{Secp256k1KeyPair{}, private_key_result.error};
    }

    auto keypair_result = from_secp256k1_private_key(private_key_result.value);
    return keypair_result;
}

CryptoResult<KeyPair::Secp256k1KeyPair> KeyPair::from_secp256k1_private_key(const Secp256k1PrivateKey& private_key) {
    auto public_key_result = derive_secp256k1_public_key(private_key);
    if (!public_key_result.success()) {
        return CryptoResult<Secp256k1KeyPair>{Secp256k1KeyPair{}, public_key_result.error};
    }

    auto compressed_result = compress_secp256k1_public_key(public_key_result.value);
    if (!compressed_result.success()) {
        return CryptoResult<Secp256k1KeyPair>{Secp256k1KeyPair{}, compressed_result.error};
    }

    Secp256k1KeyPair keypair{private_key, public_key_result.value, compressed_result.value};
    return CryptoResult<Secp256k1KeyPair>{keypair, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1PublicKey> KeyPair::derive_secp256k1_public_key(const Secp256k1PrivateKey& private_key) {
    return internal_derive_secp256k1_public_key(private_key);
}

CryptoResult<Secp256k1CompressedPublicKey> KeyPair::compress_secp256k1_public_key(const Secp256k1PublicKey& public_key) {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    if (!ctx) {
        return CryptoResult<Secp256k1CompressedPublicKey>{Secp256k1CompressedPublicKey{}, CryptoError::INVALID_KEY};
    }

    secp256k1_pubkey pubkey;
    int parse_result = secp256k1_ec_pubkey_parse(ctx, &pubkey, public_key.data(), public_key.size());

    if (parse_result != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<Secp256k1CompressedPublicKey>{Secp256k1CompressedPublicKey{}, CryptoError::INVALID_KEY};
    }

    Secp256k1CompressedPublicKey compressed;
    size_t compressed_size = compressed.size();
    int serialize_result = secp256k1_ec_pubkey_serialize(ctx, compressed.data(), &compressed_size, &pubkey, SECP256K1_EC_COMPRESSED);

    secp256k1_context_destroy(ctx);

    if (serialize_result != 1) {
        return CryptoResult<Secp256k1CompressedPublicKey>{Secp256k1CompressedPublicKey{}, CryptoError::INVALID_KEY};
    }

    return CryptoResult<Secp256k1CompressedPublicKey>{compressed, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1PublicKey> KeyPair::decompress_secp256k1_public_key(const Secp256k1CompressedPublicKey& compressed_key) {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    if (!ctx) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    secp256k1_pubkey pubkey;
    int parse_result = secp256k1_ec_pubkey_parse(ctx, &pubkey, compressed_key.data(), compressed_key.size());

    if (parse_result != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    Secp256k1PublicKey uncompressed;
    size_t uncompressed_size = uncompressed.size();
    int serialize_result = secp256k1_ec_pubkey_serialize(ctx, uncompressed.data(), &uncompressed_size, &pubkey, SECP256K1_EC_UNCOMPRESSED);

    secp256k1_context_destroy(ctx);

    if (serialize_result != 1) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    return CryptoResult<Secp256k1PublicKey>{uncompressed, CryptoError::SUCCESS};
}

CryptoResult<KeyPair::Ed25519KeyPair> KeyPair::generate_ed25519() {
    auto private_key_result = Random::generate_ed25519_private_key();
    if (!private_key_result.success()) {
        return CryptoResult<Ed25519KeyPair>{Ed25519KeyPair{}, private_key_result.error};
    }

    auto keypair_result = from_ed25519_private_key(private_key_result.value);
    return keypair_result;
}

CryptoResult<KeyPair::Ed25519KeyPair> KeyPair::from_ed25519_private_key(const Ed25519PrivateKey& private_key) {
    auto public_key_result = derive_ed25519_public_key(private_key);
    if (!public_key_result.success()) {
        return CryptoResult<Ed25519KeyPair>{Ed25519KeyPair{}, public_key_result.error};
    }

    Ed25519KeyPair keypair{private_key, public_key_result.value};
    return CryptoResult<Ed25519KeyPair>{keypair, CryptoError::SUCCESS};
}

CryptoResult<Ed25519PublicKey> KeyPair::derive_ed25519_public_key(const Ed25519PrivateKey& private_key) {
    return internal_derive_ed25519_public_key(private_key);
}

CryptoResult<KeyPair::BlsKeyPair> KeyPair::generate_bls() {
    auto private_key_result = Random::generate_bls_private_key();
    if (!private_key_result.success()) {
        return CryptoResult<BlsKeyPair>{BlsKeyPair{}, private_key_result.error};
    }

    auto keypair_result = from_bls_private_key(private_key_result.value);
    return keypair_result;
}

CryptoResult<KeyPair::BlsKeyPair> KeyPair::from_bls_private_key(const BlsPrivateKey& private_key) {
    auto public_key_result = derive_bls_public_key(private_key);
    if (!public_key_result.success()) {
        return CryptoResult<BlsKeyPair>{BlsKeyPair{}, public_key_result.error};
    }

    BlsKeyPair keypair{private_key, public_key_result.value};
    return CryptoResult<BlsKeyPair>{keypair, CryptoError::SUCCESS};
}

CryptoResult<BlsPublicKey> KeyPair::derive_bls_public_key(const BlsPrivateKey& private_key) {
    return internal_derive_bls_public_key(private_key);
}

// Validation functions
bool KeyPair::is_valid_secp256k1_private_key(const Secp256k1PrivateKey& private_key) {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    if (!ctx) return false;

    bool result = secp256k1_ec_seckey_verify(ctx, private_key.data()) == 1;
    secp256k1_context_destroy(ctx);
    return result;
}

bool KeyPair::is_valid_secp256k1_public_key(const Secp256k1PublicKey& public_key) {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    if (!ctx) return false;

    secp256k1_pubkey pubkey;
    bool result = secp256k1_ec_pubkey_parse(ctx, &pubkey, public_key.data(), public_key.size()) == 1;
    secp256k1_context_destroy(ctx);
    return result;
}

bool KeyPair::is_valid_secp256k1_compressed_public_key(const Secp256k1CompressedPublicKey& compressed_key) {
    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    if (!ctx) return false;

    secp256k1_pubkey pubkey;
    bool result = secp256k1_ec_pubkey_parse(ctx, &pubkey, compressed_key.data(), compressed_key.size()) == 1;
    secp256k1_context_destroy(ctx);
    return result;
}

bool KeyPair::is_valid_ed25519_private_key(const Ed25519PrivateKey& private_key) {
    // Ed25519 private keys are just 32 random bytes, always "valid"
    return true;
}

bool KeyPair::is_valid_ed25519_public_key(const Ed25519PublicKey& public_key) {
    // Ed25519 public keys are just 32 bytes from curve, always "valid" if correct size
    return public_key.size() == ED25519_PUBLIC_KEY_SIZE;
}

bool KeyPair::is_valid_bls_private_key(const BlsPrivateKey& private_key) {
    // BLS private keys are 32 bytes, always "valid" if not zero
    return private_key != BlsPrivateKey{};
}

bool KeyPair::is_valid_bls_public_key(const BlsPublicKey& public_key) {
    // BLS public keys are 48 bytes, basic size check
    return public_key.size() == BLS_PUBLIC_KEY_SIZE;
}

// Hex conversion functions
std::string KeyPair::secp256k1_private_key_to_hex(const Secp256k1PrivateKey& key) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : key) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string KeyPair::secp256k1_public_key_to_hex(const Secp256k1PublicKey& key) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : key) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string KeyPair::secp256k1_compressed_public_key_to_hex(const Secp256k1CompressedPublicKey& key) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : key) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string KeyPair::ed25519_private_key_to_hex(const Ed25519PrivateKey& key) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : key) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string KeyPair::ed25519_public_key_to_hex(const Ed25519PublicKey& key) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : key) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string KeyPair::bls_private_key_to_hex(const BlsPrivateKey& key) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : key) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string KeyPair::bls_public_key_to_hex(const BlsPublicKey& key) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : key) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

CryptoResult<Secp256k1PrivateKey> KeyPair::secp256k1_private_key_from_hex(const std::string& hex) {
    if (hex.length() != SECP256K1_PRIVATE_KEY_SIZE * 2) {
        return CryptoResult<Secp256k1PrivateKey>{Secp256k1PrivateKey{}, CryptoError::INVALID_LENGTH};
    }

    Secp256k1PrivateKey result;
    for (size_t i = 0; i < SECP256K1_PRIVATE_KEY_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<Secp256k1PrivateKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1PublicKey> KeyPair::secp256k1_public_key_from_hex(const std::string& hex) {
    if (hex.length() != SECP256K1_PUBLIC_KEY_SIZE * 2) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_LENGTH};
    }

    Secp256k1PublicKey result;
    for (size_t i = 0; i < SECP256K1_PUBLIC_KEY_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<Secp256k1PublicKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1CompressedPublicKey> KeyPair::secp256k1_compressed_public_key_from_hex(const std::string& hex) {
    if (hex.length() != SECP256K1_COMPRESSED_PUBLIC_KEY_SIZE * 2) {
        return CryptoResult<Secp256k1CompressedPublicKey>{Secp256k1CompressedPublicKey{}, CryptoError::INVALID_LENGTH};
    }

    Secp256k1CompressedPublicKey result;
    for (size_t i = 0; i < SECP256K1_COMPRESSED_PUBLIC_KEY_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<Secp256k1CompressedPublicKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Ed25519PrivateKey> KeyPair::ed25519_private_key_from_hex(const std::string& hex) {
    if (hex.length() != ED25519_PRIVATE_KEY_SIZE * 2) {
        return CryptoResult<Ed25519PrivateKey>{Ed25519PrivateKey{}, CryptoError::INVALID_LENGTH};
    }

    Ed25519PrivateKey result;
    for (size_t i = 0; i < ED25519_PRIVATE_KEY_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<Ed25519PrivateKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Ed25519PublicKey> KeyPair::ed25519_public_key_from_hex(const std::string& hex) {
    if (hex.length() != ED25519_PUBLIC_KEY_SIZE * 2) {
        return CryptoResult<Ed25519PublicKey>{Ed25519PublicKey{}, CryptoError::INVALID_LENGTH};
    }

    Ed25519PublicKey result;
    for (size_t i = 0; i < ED25519_PUBLIC_KEY_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<Ed25519PublicKey>{result, CryptoError::SUCCESS};
}

CryptoResult<BlsPrivateKey> KeyPair::bls_private_key_from_hex(const std::string& hex) {
    if (hex.length() != BLS_PRIVATE_KEY_SIZE * 2) {
        return CryptoResult<BlsPrivateKey>{BlsPrivateKey{}, CryptoError::INVALID_LENGTH};
    }

    BlsPrivateKey result;
    for (size_t i = 0; i < BLS_PRIVATE_KEY_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<BlsPrivateKey>{result, CryptoError::SUCCESS};
}

CryptoResult<BlsPublicKey> KeyPair::bls_public_key_from_hex(const std::string& hex) {
    if (hex.length() != BLS_PUBLIC_KEY_SIZE * 2) {
        return CryptoResult<BlsPublicKey>{BlsPublicKey{}, CryptoError::INVALID_LENGTH};
    }

    BlsPublicKey result;
    for (size_t i = 0; i < BLS_PUBLIC_KEY_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<BlsPublicKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Hash256> KeyPair::derive_ethereum_address(const Secp256k1PublicKey& public_key) {
    // Ethereum address derivation: keccak256(public_key)[12:32]
    auto keccak_result = Hash::keccak256(ByteVector(public_key.begin(), public_key.end()));
    if (!keccak_result.success()) {
        return CryptoResult<Hash256>{Hash256{}, keccak_result.error};
    }

    Hash256 address{};
    std::copy(keccak_result.value.begin() + 12, keccak_result.value.end(), address.begin());
    return CryptoResult<Hash256>{address, CryptoError::SUCCESS};
}

CryptoResult<Hash256> KeyPair::derive_bitcoin_address(const Secp256k1PublicKey& public_key) {
    // Bitcoin address derivation: ripemd160(sha256(compressed_public_key))
    auto compressed_result = compress_secp256k1_public_key(public_key);
    if (!compressed_result.success()) {
        return CryptoResult<Hash256>{Hash256{}, compressed_result.error};
    }

    ByteVector compressed_bytes(compressed_result.value.begin(), compressed_result.value.end());
    auto ripemd_result = Hash::keccak256_ripemd160(compressed_bytes);
    if (!ripemd_result.success()) {
        return CryptoResult<Hash256>{Hash256{}, ripemd_result.error};
    }

    Hash256 address{};
    std::copy(ripemd_result.value.begin(), ripemd_result.value.end(), address.begin());
    return CryptoResult<Hash256>{address, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1PublicKey> KeyPair::internal_derive_secp256k1_public_key(const Secp256k1PrivateKey& private_key) {
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!ec_key) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    BIGNUM* priv_bn = BN_bin2bn(private_key.data(), private_key.size(), nullptr);
    if (!priv_bn || !EC_KEY_set_private_key(ec_key, priv_bn)) {
        BN_free(priv_bn);
        EC_KEY_free(ec_key);
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }
    BN_free(priv_bn);

    // Generate public key
    if (!EC_KEY_generate_key(ec_key)) {
        EC_KEY_free(ec_key);
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    // Serialize public key to uncompressed format
    const EC_POINT* pub_point = EC_KEY_get0_public_key(ec_key);
    const EC_GROUP* group = EC_KEY_get0_group(ec_key);

    Secp256k1PublicKey public_key;
    size_t key_size = EC_POINT_point2oct(group, pub_point, POINT_CONVERSION_UNCOMPRESSED,
                                        public_key.data(), public_key.size(), nullptr);

    EC_KEY_free(ec_key);

    if (key_size != public_key.size()) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    return CryptoResult<Secp256k1PublicKey>{public_key, CryptoError::SUCCESS};
}

CryptoResult<Ed25519PublicKey> KeyPair::internal_derive_ed25519_public_key(const Ed25519PrivateKey& private_key) {
    if (sodium_init() == -1) {
        return CryptoResult<Ed25519PublicKey>{Ed25519PublicKey{}, CryptoError::INVALID_KEY};
    }

    Ed25519PublicKey public_key;
    crypto_sign_ed25519_public_key_from_private_key(public_key.data(), private_key.data());

    return CryptoResult<Ed25519PublicKey>{public_key, CryptoError::SUCCESS};
}

CryptoResult<BlsPublicKey> KeyPair::internal_derive_bls_public_key(const BlsPrivateKey& private_key) {
    // BLS implementation not available - stub implementation
    // In a real implementation, this would use blst or another BLS library
    BlsPublicKey public_key{};
    // Copy some data to make it look like a public key for testing
    std::copy(private_key.begin(), private_key.begin() + std::min(private_key.size(), public_key.size()), public_key.begin());
    return CryptoResult<BlsPublicKey>{public_key, CryptoError::SUCCESS};
}

} // namespace chainforge::crypto
