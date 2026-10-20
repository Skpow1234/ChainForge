#include "chainforge/crypto/curve.hpp"
#include <algorithm>
#include <secp256k1.h>
#include <sodium.h>

namespace chainforge::crypto {
namespace {

secp256k1_context* secp_ctx() {
    static secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_NONE);
    return ctx;
}

bool parse_uncompressed(secp256k1_pubkey& out, const Secp256k1PublicKey& point) {
    uint8_t encoded[65];
    encoded[0] = 0x04;
    std::copy(point.begin(), point.end(), encoded + 1);
    return secp256k1_ec_pubkey_parse(secp_ctx(), &out, encoded, sizeof(encoded)) == 1;
}

bool serialize_uncompressed(Secp256k1PublicKey& out, const secp256k1_pubkey& pubkey) {
    uint8_t encoded[65];
    size_t len = sizeof(encoded);
    if (secp256k1_ec_pubkey_serialize(secp_ctx(), encoded, &len, &pubkey, SECP256K1_EC_UNCOMPRESSED) != 1) {
        return false;
    }
    if (len != 65 || encoded[0] != 0x04) {
        return false;
    }
    std::copy(encoded + 1, encoded + 65, out.begin());
    return true;
}

} // namespace

CryptoResult<Secp256k1PublicKey> Curve::Secp256k1::multiply_base(const Secp256k1PrivateKey& scalar) {
    if (!secp256k1_ec_seckey_verify(secp_ctx(), scalar.data())) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    secp256k1_pubkey pubkey;
    if (secp256k1_ec_pubkey_create(secp_ctx(), &pubkey, scalar.data()) != 1) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    Secp256k1PublicKey result{};
    if (!serialize_uncompressed(result, pubkey)) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }
    return CryptoResult<Secp256k1PublicKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1PublicKey> Curve::Secp256k1::multiply(const Secp256k1PublicKey& point, const Secp256k1PrivateKey& scalar) {
    if (!secp256k1_ec_seckey_verify(secp_ctx(), scalar.data())) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    secp256k1_pubkey pubkey;
    if (!parse_uncompressed(pubkey, point)) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    if (secp256k1_ec_pubkey_tweak_mul(secp_ctx(), &pubkey, scalar.data()) != 1) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    Secp256k1PublicKey result{};
    if (!serialize_uncompressed(result, pubkey)) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }
    return CryptoResult<Secp256k1PublicKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1PublicKey> Curve::Secp256k1::add(const Secp256k1PublicKey& p1, const Secp256k1PublicKey& p2) {
    secp256k1_pubkey a;
    secp256k1_pubkey b;
    if (!parse_uncompressed(a, p1) || !parse_uncompressed(b, p2)) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    const secp256k1_pubkey* ptrs[2] = {&a, &b};
    secp256k1_pubkey sum;
    if (secp256k1_ec_pubkey_combine(secp_ctx(), &sum, ptrs, 2) != 1) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    Secp256k1PublicKey result{};
    if (!serialize_uncompressed(result, sum)) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }
    return CryptoResult<Secp256k1PublicKey>{result, CryptoError::SUCCESS};
}

bool Curve::Secp256k1::is_valid_point(const Secp256k1PublicKey& point) {
    secp256k1_pubkey pubkey;
    return parse_uncompressed(pubkey, point);
}

bool Curve::Secp256k1::is_point_at_infinity(const Secp256k1PublicKey& point) {
    return std::all_of(point.begin(), point.end(), [](uint8_t b) { return b == 0; });
}

CryptoResult<Secp256k1CompressedPublicKey> Curve::Secp256k1::compress_point(const Secp256k1PublicKey& point) {
    secp256k1_pubkey pubkey;
    if (!parse_uncompressed(pubkey, point)) {
        return CryptoResult<Secp256k1CompressedPublicKey>{Secp256k1CompressedPublicKey{}, CryptoError::INVALID_KEY};
    }

    Secp256k1CompressedPublicKey compressed{};
    size_t len = compressed.size();
    if (secp256k1_ec_pubkey_serialize(secp_ctx(), compressed.data(), &len, &pubkey, SECP256K1_EC_COMPRESSED) != 1) {
        return CryptoResult<Secp256k1CompressedPublicKey>{Secp256k1CompressedPublicKey{}, CryptoError::INVALID_KEY};
    }
    return CryptoResult<Secp256k1CompressedPublicKey>{compressed, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1PublicKey> Curve::Secp256k1::decompress_point(const Secp256k1CompressedPublicKey& compressed_point) {
    secp256k1_pubkey pubkey;
    if (secp256k1_ec_pubkey_parse(secp_ctx(), &pubkey, compressed_point.data(), compressed_point.size()) != 1) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }

    Secp256k1PublicKey uncompressed{};
    if (!serialize_uncompressed(uncompressed, pubkey)) {
        return CryptoResult<Secp256k1PublicKey>{Secp256k1PublicKey{}, CryptoError::INVALID_KEY};
    }
    return CryptoResult<Secp256k1PublicKey>{uncompressed, CryptoError::SUCCESS};
}

const Secp256k1PrivateKey& Curve::Secp256k1::order() {
    static const Secp256k1PrivateKey secp256k1_order = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
        0xBA, 0xAE, 0xDC, 0xE6, 0xAF, 0x48, 0xA0, 0x3B,
        0xBF, 0xD2, 0x5E, 0x8C, 0xD0, 0x36, 0x41, 0x41};
    return secp256k1_order;
}

const Secp256k1PublicKey& Curve::Secp256k1::generator() {
    static const Secp256k1PublicKey generator = {
        0x79, 0xBE, 0x66, 0x7E, 0xF9, 0xDC, 0xBB, 0xAC, 0x55, 0xA0, 0x62, 0x95, 0xCE, 0x87, 0x0B, 0x07,
        0x02, 0x9B, 0xFC, 0xDB, 0x2D, 0xCE, 0x28, 0xD9, 0x59, 0xF2, 0x81, 0x5B, 0x16, 0xF8, 0x17, 0x98,
        0x48, 0x3A, 0xDA, 0x77, 0x26, 0xA3, 0xC4, 0x65, 0x5D, 0xA4, 0xFB, 0xFC, 0x0E, 0x11, 0x08, 0xA8,
        0xFD, 0x17, 0xB4, 0x48, 0xA6, 0x85, 0x54, 0x19, 0x9C, 0x47, 0xD0, 0x8F, 0xFB, 0x10, 0xD4, 0xB8
    };
    return generator;
}

CryptoResult<Ed25519PublicKey> Curve::Ed25519::multiply_base(const Ed25519PrivateKey& scalar) {
    if (sodium_init() < 0) {
        return CryptoResult<Ed25519PublicKey>{Ed25519PublicKey{}, CryptoError::RANDOM_GENERATION_FAILED};
    }

    Ed25519PublicKey result{};
    if (crypto_scalarmult_ed25519_base(result.data(), scalar.data()) != 0) {
        return CryptoResult<Ed25519PublicKey>{Ed25519PublicKey{}, CryptoError::INVALID_KEY};
    }
    return CryptoResult<Ed25519PublicKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Ed25519PublicKey> Curve::Ed25519::multiply(const Ed25519PublicKey& point, const Ed25519PrivateKey& scalar) {
    if (sodium_init() < 0) {
        return CryptoResult<Ed25519PublicKey>{Ed25519PublicKey{}, CryptoError::RANDOM_GENERATION_FAILED};
    }

    Ed25519PublicKey result{};
    if (crypto_scalarmult_ed25519(result.data(), scalar.data(), point.data()) != 0) {
        return CryptoResult<Ed25519PublicKey>{Ed25519PublicKey{}, CryptoError::INVALID_KEY};
    }
    return CryptoResult<Ed25519PublicKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Ed25519PublicKey> Curve::Ed25519::add(const Ed25519PublicKey& /*p1*/, const Ed25519PublicKey& /*p2*/) {
    // libsodium does not expose a stable ed25519 point-add for this encoding; leave explicit.
    return CryptoResult<Ed25519PublicKey>{Ed25519PublicKey{}, CryptoError::UNSUPPORTED_ALGORITHM};
}

bool Curve::Ed25519::is_valid_point(const Ed25519PublicKey& point) {
    return !std::all_of(point.begin(), point.end(), [](uint8_t b) { return b == 0; });
}

bool Curve::Ed25519::is_point_at_infinity(const Ed25519PublicKey& point) {
    return std::all_of(point.begin(), point.end(), [](uint8_t b) { return b == 0; });
}

const Ed25519PublicKey& Curve::Ed25519::generator() {
    static const Ed25519PublicKey gen = {
        0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
        0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66
    };
    return gen;
}

CryptoResult<BlsPublicKey> Curve::Bls12_381::g1_multiply_base(const BlsPrivateKey& /*scalar*/) {
    return CryptoResult<BlsPublicKey>{BlsPublicKey{}, CryptoError::UNSUPPORTED_ALGORITHM};
}

CryptoResult<BlsPublicKey> Curve::Bls12_381::g1_multiply(const BlsPublicKey& /*point*/, const BlsPrivateKey& /*scalar*/) {
    return CryptoResult<BlsPublicKey>{BlsPublicKey{}, CryptoError::UNSUPPORTED_ALGORITHM};
}

CryptoResult<BlsPublicKey> Curve::Bls12_381::g1_add(const BlsPublicKey& /*p1*/, const BlsPublicKey& /*p2*/) {
    return CryptoResult<BlsPublicKey>{BlsPublicKey{}, CryptoError::UNSUPPORTED_ALGORITHM};
}

CryptoResult<BlsSignature> Curve::Bls12_381::g2_multiply_base(const BlsPrivateKey& /*scalar*/) {
    return CryptoResult<BlsSignature>{BlsSignature{}, CryptoError::UNSUPPORTED_ALGORITHM};
}

CryptoResult<BlsSignature> Curve::Bls12_381::g2_multiply(const BlsSignature& /*point*/, const BlsPrivateKey& /*scalar*/) {
    return CryptoResult<BlsSignature>{BlsSignature{}, CryptoError::UNSUPPORTED_ALGORITHM};
}

CryptoResult<BlsSignature> Curve::Bls12_381::g2_add(const BlsSignature& /*s1*/, const BlsSignature& /*s2*/) {
    return CryptoResult<BlsSignature>{BlsSignature{}, CryptoError::UNSUPPORTED_ALGORITHM};
}

bool Curve::Bls12_381::is_valid_g1_point(const BlsPublicKey& /*point*/) { return false; }
bool Curve::Bls12_381::is_valid_g2_point(const BlsSignature& /*point*/) { return false; }

CryptoResult<bool> Curve::Bls12_381::pairing_check(const BlsPublicKey& /*g1_point*/, const BlsSignature& /*g2_point*/) {
    return CryptoResult<bool>{false, CryptoError::UNSUPPORTED_ALGORITHM};
}

const BlsPublicKey& Curve::Bls12_381::g1_generator() {
    static const BlsPublicKey gen{};
    return gen;
}

const BlsSignature& Curve::Bls12_381::g2_generator() {
    static const BlsSignature gen{};
    return gen;
}

} // namespace chainforge::crypto
