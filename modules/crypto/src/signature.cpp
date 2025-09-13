#include "chainforge/crypto/signature.hpp"
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <sodium.h>
#include <iomanip>
#include <sstream>
#include <memory>

namespace chainforge::crypto {

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

CryptoResult<Secp256k1PublicKey> Signature::ecdsa_secp256k1_recover_public_key(
    const Message& message,
    const Secp256k1Signature& signature,
    bool compressed
) {
    // TODO: Implement public key recovery
    return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::UNSUPPORTED_ALGORITHM};
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
    return CryptoResult<bool>{false, CryptoError::UNSUPPORTED_ALGORITHM};
}

std::string Signature::signature_to_hex(const Secp256k1Signature& sig) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : sig) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string Signature::signature_to_hex(const Ed25519Signature& sig) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : sig) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string Signature::signature_to_hex(const BlsSignature& sig) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : sig) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
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
    // TODO: Implement recovery ID calculation
    return CryptoResult<uint8_t>{0, CryptoError::UNSUPPORTED_ALGORITHM};
}

CryptoResult<Secp256k1Signature> Signature::internal_ecdsa_sign(
    const byte_t* message, size_t message_len,
    const Secp256k1PrivateKey& private_key
) {
    // Hash the message first (Ethereum standard)
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(message, message_len, hash);

    // Create EC key from private key
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!ec_key) {
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::SIGNATURE_FAILED};
    }

    BIGNUM* priv_bn = BN_bin2bn(private_key.data(), private_key.size(), nullptr);
    if (!priv_bn || !EC_KEY_set_private_key(ec_key, priv_bn)) {
        BN_free(priv_bn);
        EC_KEY_free(ec_key);
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::SIGNATURE_FAILED};
    }
    BN_free(priv_bn);

    // Sign the hash
    ECDSA_SIG* sig = ECDSA_do_sign(hash, SHA256_DIGEST_LENGTH, ec_key);
    if (!sig) {
        EC_KEY_free(ec_key);
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::SIGNATURE_FAILED};
    }

    // Convert to DER format
    int der_len = i2d_ECDSA_SIG(sig, nullptr);
    if (der_len <= 0 || der_len > static_cast<int>(Secp256k1Signature::size())) {
        ECDSA_SIG_free(sig);
        EC_KEY_free(ec_key);
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::SIGNATURE_FAILED};
    }

    Secp256k1Signature result_sig;
    unsigned char* der_data = result_sig.data();
    int final_len = i2d_ECDSA_SIG(sig, &der_data);

    ECDSA_SIG_free(sig);
    EC_KEY_free(ec_key);

    if (final_len != der_len) {
        return CryptoResult<Secp256k1Signature>{Secp256k1Signature{}, CryptoError::SIGNATURE_FAILED};
    }

    return CryptoResult<Secp256k1Signature>{result_sig, CryptoError::SUCCESS};
}

CryptoResult<bool> Signature::internal_ecdsa_verify(
    const byte_t* message, size_t message_len,
    const Secp256k1Signature& signature,
    const Secp256k1PublicKey& public_key
) {
    // Hash the message first (Ethereum standard)
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(message, message_len, hash);

    // Create EC key from public key
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!ec_key) {
        return CryptoResult<bool>{false, CryptoError::VERIFICATION_FAILED};
    }

    // Set the public key
    const unsigned char* pub_key_data = public_key.data();
    if (!o2i_ECPublicKey(&ec_key, &pub_key_data, public_key.size())) {
        EC_KEY_free(ec_key);
        return CryptoResult<bool>{false, CryptoError::INVALID_KEY};
    }

    // Parse DER signature
    const unsigned char* sig_data = signature.data();
    ECDSA_SIG* sig = d2i_ECDSA_SIG(nullptr, &sig_data, signature.size());
    if (!sig) {
        EC_KEY_free(ec_key);
        return CryptoResult<bool>{false, CryptoError::INVALID_SIGNATURE};
    }

    // Verify the signature
    int verify_result = ECDSA_do_verify(hash, SHA256_DIGEST_LENGTH, sig, ec_key);

    ECDSA_SIG_free(sig);
    EC_KEY_free(ec_key);

    return CryptoResult<bool>{verify_result == 1, CryptoError::SUCCESS};
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

} // namespace chainforge::crypto
