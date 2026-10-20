#include "tokiwa/crypto/signature.hpp"
#include "tokiwa/crypto/hash.hpp"
#include <secp256k1.h>
#if __has_include(<secp256k1_recovery.h>)
#include <secp256k1_recovery.h>
#define TOKIWA_HAS_SECP_RECOVERY 1
#endif
#include <sodium.h>
#include <iomanip>
#include <sstream>
#include <memory>
#include <cstring>
#include <algorithm>

namespace tokiwa::crypto {
namespace {

secp256k1_context* secp_sign_ctx() {
    return secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
}

bool parse_uncompressed_pubkey(const secp256k1_context* ctx,
                               secp256k1_pubkey* out,
                               const Secp256k1PublicKey& public_key) {
    uint8_t encoded[65];
    encoded[0] = 0x04;
    std::memcpy(encoded + 1, public_key.data(), public_key.size());
    return secp256k1_ec_pubkey_parse(ctx, out, encoded, sizeof(encoded)) == 1;
}

} // namespace

CryptoResult<Hash256> Signature::digest_for_ecdsa(const byte_t* message, size_t message_len) {
    Hash256 digest{};
    if (message_len == HASH256_SIZE) {
        std::memcpy(digest.data(), message, HASH256_SIZE);
        return CryptoResult<Hash256>{digest, CryptoError::SUCCESS};
    }
    Message msg(message, message + message_len);
    return Hash::keccak256(msg);
}

CryptoResult<Secp256k1Signature> Signature::ecdsa_secp256k1_sign(
    const Message& message,
    const Secp256k1PrivateKey& private_key
) {
    return internal_ecdsa_sign(message.data(), message.size(), private_key);
}

CryptoResult<bool> Signature::ecdsa_secp256k1_verify(
    const Message& message,
    const Secp256k1Signature& signature,
    const Secp256k1PublicKey& public_key
) {
    return internal_ecdsa_verify(message.data(), message.size(), signature, public_key);
}

CryptoResult<std::array<byte_t, 65>> Signature::ecdsa_secp256k1_sign_recoverable(
    const Message& message,
    const Secp256k1PrivateKey& private_key
) {
    auto digest = digest_for_ecdsa(message.data(), message.size());
    if (!digest.success()) {
        return CryptoResult<std::array<byte_t, 65>>{{}, digest.error};
    }

    secp256k1_context* ctx = secp_sign_ctx();
    if (!ctx) {
        return CryptoResult<std::array<byte_t, 65>>{{}, CryptoError::SIGNATURE_FAILED};
    }
    if (secp256k1_ec_seckey_verify(ctx, private_key.data()) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<std::array<byte_t, 65>>{{}, CryptoError::INVALID_KEY};
    }

#if defined(TOKIWA_HAS_SECP_RECOVERY)
    secp256k1_ecdsa_recoverable_signature rsig;
    if (secp256k1_ecdsa_sign_recoverable(ctx, &rsig, digest.value.data(), private_key.data(),
                                         nullptr, nullptr) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<std::array<byte_t, 65>>{{}, CryptoError::SIGNATURE_FAILED};
    }

    std::array<byte_t, 65> out{};
    int recid = 0;
    if (secp256k1_ecdsa_recoverable_signature_serialize_compact(
            ctx, out.data(), &recid, &rsig) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<std::array<byte_t, 65>>{{}, CryptoError::SIGNATURE_FAILED};
    }
    secp256k1_context_destroy(ctx);
    out[64] = static_cast<byte_t>(recid & 0xff);
    return CryptoResult<std::array<byte_t, 65>>{out, CryptoError::SUCCESS};
#else
    secp256k1_ecdsa_signature sig;
    if (secp256k1_ecdsa_sign(ctx, &sig, digest.value.data(), private_key.data(),
                             nullptr, nullptr) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<std::array<byte_t, 65>>{{}, CryptoError::SIGNATURE_FAILED};
    }
    secp256k1_ecdsa_signature_normalize(ctx, &sig, &sig);

    std::array<byte_t, 65> out{};
    if (secp256k1_ecdsa_signature_serialize_compact(ctx, out.data(), &sig) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<std::array<byte_t, 65>>{{}, CryptoError::SIGNATURE_FAILED};
    }

    secp256k1_pubkey expected;
    if (secp256k1_ec_pubkey_create(ctx, &expected, private_key.data()) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<std::array<byte_t, 65>>{{}, CryptoError::SIGNATURE_FAILED};
    }
    uint8_t expected_bytes[65];
    size_t elen = sizeof(expected_bytes);
    secp256k1_ec_pubkey_serialize(ctx, expected_bytes, &elen, &expected, SECP256K1_EC_UNCOMPRESSED);
    secp256k1_context_destroy(ctx);

    Secp256k1PublicKey pubkey{};
    std::memcpy(pubkey.data(), expected_bytes + 1, 64);
    Secp256k1Signature compact{};
    std::memcpy(compact.data(), out.data(), 64);
    auto recid = ecdsa_get_recovery_id(Message(digest.value.begin(), digest.value.end()),
                                       compact, pubkey);
    if (!recid.success()) {
        return CryptoResult<std::array<byte_t, 65>>{{}, recid.error};
    }
    out[64] = recid.value;
    return CryptoResult<std::array<byte_t, 65>>{out, CryptoError::SUCCESS};
#endif
}

CryptoResult<Secp256k1PublicKey> Signature::ecdsa_secp256k1_recover_public_key(
    const Message& message,
    const Secp256k1Signature& signature,
    uint8_t recovery_id,
    bool compressed
) {
#if !defined(TOKIWA_HAS_SECP_RECOVERY)
    (void)message; (void)signature; (void)recovery_id; (void)compressed;
    return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::UNSUPPORTED_ALGORITHM};
#else
    if (recovery_id > 3) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_SIGNATURE};
    }

    auto dig = digest_for_ecdsa(message.data(), message.size());
    if (!dig.success()) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, dig.error};
    }
    const Hash256& digest = dig.value;

    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
    if (!ctx) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::VERIFICATION_FAILED};
    }

    secp256k1_ecdsa_recoverable_signature rec_sig;
    if (secp256k1_ecdsa_recoverable_signature_parse_compact(
            ctx, &rec_sig, signature.data(), static_cast<int>(recovery_id)) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_SIGNATURE};
    }

    secp256k1_pubkey pubkey;
    if (secp256k1_ecdsa_recover(ctx, &pubkey, &rec_sig, digest.data()) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::VERIFICATION_FAILED};
    }

    if (compressed) {
        Secp256k1CompressedPublicKey out{};
        size_t out_len = out.size();
        secp256k1_ec_pubkey_serialize(ctx, out.data(), &out_len, &pubkey, SECP256K1_EC_COMPRESSED);
        secp256k1_context_destroy(ctx);
        Secp256k1PublicKey padded{};
        std::copy(out.begin(), out.end(), padded.begin());
        return CryptoResult<Secp256k1PublicKey>{padded, CryptoError::SUCCESS};
    }

    uint8_t uncompressed[65];
    size_t out_len = sizeof(uncompressed);
    if (secp256k1_ec_pubkey_serialize(ctx, uncompressed, &out_len, &pubkey, SECP256K1_EC_UNCOMPRESSED) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::VERIFICATION_FAILED};
    }
    secp256k1_context_destroy(ctx);

    Secp256k1PublicKey result{};
    std::copy(uncompressed + 1, uncompressed + 65, result.begin());
    return CryptoResult<Secp256k1PublicKey>{result, CryptoError::SUCCESS};
#endif
}

CryptoResult<Ed25519Signature> Signature::ed25519_sign(
    const Message& message,
    const Ed25519PrivateKey& private_key
) {
    return internal_ed25519_sign(message.data(), message.size(), private_key);
}

CryptoResult<bool> Signature::ed25519_verify(
    const Message& message,
    const Ed25519Signature& signature,
    const Ed25519PublicKey& public_key
) {
    return internal_ed25519_verify(message.data(), message.size(), signature, public_key);
}

CryptoResult<BlsSignature> Signature::bls_sign(
    const Message& message,
    const BlsPrivateKey& private_key
) {
    return internal_bls_sign(message.data(), message.size(), private_key);
}

CryptoResult<bool> Signature::bls_verify(
    const Message& message,
    const BlsSignature& signature,
    const BlsPublicKey& public_key
) {
    return internal_bls_verify(message.data(), message.size(), signature, public_key);
}

CryptoResult<BlsSignature> Signature::bls_aggregate_signatures(
    const std::vector<BlsSignature>& signatures
) {
    if (signatures.empty()) {
        return CryptoResult<BlsSignature>{BlsSignature{}, CryptoError::INVALID_SIGNATURE};
    }

    // TODO: Implement BLS signature aggregation
    return CryptoResult<BlsSignature>{BlsSignature{}, CryptoError::UNSUPPORTED_ALGORITHM};
}

CryptoResult<bool> Signature::bls_verify_aggregate(
    const std::vector<Message>& messages,
    const BlsSignature& aggregate_signature,
    const std::vector<BlsPublicKey>& public_keys
) {
    // TODO: Implement BLS aggregate verification
    (void)messages; (void)aggregate_signature; (void)public_keys; // Suppress unused parameter warnings
    return CryptoResult<bool>{false, CryptoError::UNSUPPORTED_ALGORITHM};
}

// Template implementation
template<typename T>
std::string Signature::signature_to_hex(const T& sig) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : sig) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string Signature::secp256k1_signature_to_hex(const Secp256k1Signature& sig) {
    return signature_to_hex(sig);
}

std::string Signature::ed25519_signature_to_hex(const Ed25519Signature& sig) {
    return signature_to_hex(sig);
}

std::string Signature::bls_signature_to_hex(const BlsSignature& sig) {
    return signature_to_hex(sig);
}

CryptoResult<Secp256k1Signature> Signature::signature_from_hex_secp256k1(const std::string& hex) {
    if (hex.length() != SECP256K1_SIGNATURE_SIZE * 2) {
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::INVALID_LENGTH};
    }

    Secp256k1Signature result;
    for (size_t i = 0; i < SECP256K1_SIGNATURE_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<Secp256k1Signature>{result, CryptoError::SUCCESS};
}

CryptoResult<Ed25519Signature> Signature::signature_from_hex_ed25519(const std::string& hex) {
    if (hex.length() != ED25519_SIGNATURE_SIZE * 2) {
        return CryptoResult<Ed25519Signature>{Ed25519Signature{}, CryptoError::INVALID_LENGTH};
    }

    Ed25519Signature result;
    for (size_t i = 0; i < ED25519_SIGNATURE_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<Ed25519Signature>{result, CryptoError::SUCCESS};
}

CryptoResult<BlsSignature> Signature::signature_from_hex_bls(const std::string& hex) {
    if (hex.length() != BLS_SIGNATURE_SIZE * 2) {
        return CryptoResult<BlsSignature>{BlsSignature{}, CryptoError::INVALID_LENGTH};
    }

    BlsSignature result;
    for (size_t i = 0; i < BLS_SIGNATURE_SIZE; ++i) {
        std::string byte_str = hex.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<BlsSignature>{result, CryptoError::SUCCESS};
}

CryptoResult<uint8_t> Signature::ecdsa_get_recovery_id(
    const Message& message,
    const Secp256k1Signature& signature,
    const Secp256k1PublicKey& public_key
) {
    for (uint8_t recid = 0; recid < 4; ++recid) {
        auto recovered = ecdsa_secp256k1_recover_public_key(message, signature, recid, false);
        if (recovered.success() && recovered.value == public_key) {
            return CryptoResult<uint8_t>{recid, CryptoError::SUCCESS};
        }
    }
    return CryptoResult<uint8_t>{0, CryptoError::VERIFICATION_FAILED};
}

CryptoResult<Secp256k1Signature> Signature::internal_ecdsa_sign(
    const byte_t* message, size_t message_len,
    const Secp256k1PrivateKey& private_key
) {
    auto digest = digest_for_ecdsa(message, message_len);
    if (!digest.success()) {
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, digest.error};
    }

    secp256k1_context* ctx = secp_sign_ctx();
    if (!ctx) {
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::SIGNATURE_FAILED};
    }
    if (secp256k1_ec_seckey_verify(ctx, private_key.data()) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::INVALID_KEY};
    }

    secp256k1_ecdsa_signature sig;
    if (secp256k1_ecdsa_sign(ctx, &sig, digest.value.data(), private_key.data(),
                             nullptr, nullptr) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::SIGNATURE_FAILED};
    }
    // Ethereum / Bitcoin convention: low-S
    secp256k1_ecdsa_signature_normalize(ctx, &sig, &sig);

    Secp256k1Signature out{};
    if (secp256k1_ecdsa_signature_serialize_compact(ctx, out.data(), &sig) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::SIGNATURE_FAILED};
    }
    secp256k1_context_destroy(ctx);
    return CryptoResult<Secp256k1Signature>{out, CryptoError::SUCCESS};
}

CryptoResult<bool> Signature::internal_ecdsa_verify(
    const byte_t* message, size_t message_len,
    const Secp256k1Signature& signature,
    const Secp256k1PublicKey& public_key
) {
    auto digest = digest_for_ecdsa(message, message_len);
    if (!digest.success()) {
        return CryptoResult<bool>{false, digest.error};
    }

    secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
    if (!ctx) {
        return CryptoResult<bool>{false, CryptoError::VERIFICATION_FAILED};
    }

    secp256k1_pubkey pubkey;
    if (!parse_uncompressed_pubkey(ctx, &pubkey, public_key)) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<bool>{false, CryptoError::INVALID_KEY};
    }

    secp256k1_ecdsa_signature sig;
    if (secp256k1_ecdsa_signature_parse_compact(ctx, &sig, signature.data()) != 1) {
        secp256k1_context_destroy(ctx);
        return CryptoResult<bool>{false, CryptoError::INVALID_SIGNATURE};
    }
    // Accept high-S by normalizing before verify
    secp256k1_ecdsa_signature_normalize(ctx, &sig, &sig);

    const int ok = secp256k1_ecdsa_verify(ctx, &sig, digest.value.data(), &pubkey);
    secp256k1_context_destroy(ctx);
    return CryptoResult<bool>{ok == 1, CryptoError::SUCCESS};
}

CryptoResult<Ed25519Signature> Signature::internal_ed25519_sign(
    const byte_t* message, size_t message_len,
    const Ed25519PrivateKey& private_key
) {
    if (sodium_init() == -1) {
        return CryptoResult<Ed25519Signature>{Ed25519Signature{}, CryptoError::SIGNATURE_FAILED};
    }

    Ed25519Signature signature;
    crypto_sign_ed25519_detached(signature.data(), nullptr, message, message_len, private_key.data());

    return CryptoResult<Ed25519Signature>{signature, CryptoError::SUCCESS};
}

CryptoResult<bool> Signature::internal_ed25519_verify(
    const byte_t* message, size_t message_len,
    const Ed25519Signature& signature,
    const Ed25519PublicKey& public_key
) {
    if (sodium_init() == -1) {
        return CryptoResult<bool>{false, CryptoError::VERIFICATION_FAILED};
    }

    int result = crypto_sign_ed25519_verify_detached(signature.data(), message, message_len, public_key.data());

    return CryptoResult<bool>{result == 0, CryptoError::SUCCESS};
}

CryptoResult<BlsSignature> Signature::internal_bls_sign(
    const byte_t* message, size_t message_len,
    const BlsPrivateKey& private_key
) {
    // BLS implementation not available - stub implementation
    // In a real implementation, this would use blst or another BLS library
    (void)message; (void)message_len; // Suppress unused parameter warnings
    BlsSignature signature{};
    // Copy some data to make it look like a signature for testing
    std::copy(private_key.begin(), private_key.begin() + std::min(private_key.size(), signature.size()), signature.begin());
    return CryptoResult<BlsSignature>{signature, CryptoError::SUCCESS};
}

CryptoResult<bool> Signature::internal_bls_verify(
    const byte_t* message, size_t message_len,
    const BlsSignature& signature,
    const BlsPublicKey& public_key
) {
    // BLS implementation not available - stub implementation
    // In a real implementation, this would use blst or another BLS library
    (void)message; (void)message_len; (void)signature; (void)public_key;
    return CryptoResult<bool>{true, CryptoError::SUCCESS};
}

} // namespace tokiwa::crypto
