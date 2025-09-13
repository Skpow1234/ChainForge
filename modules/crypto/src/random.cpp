#include "chainforge/crypto/random.hpp"
#include <algorithm>
#include <stdexcept>

namespace chainforge::crypto {

// Thread-local random device and generator
thread_local std::random_device Random::random_device_;
thread_local std::mt19937 Random::generator_;
std::mutex Random::mutex_;

void Random::initialize_generator() {
    static thread_local bool initialized = false;
    if (!initialized) {
        generator_.seed(random_device_());
        initialized = true;
    }
}

CryptoResult<ByteVector> Random::generate_bytes(size_t length) {
    if (length == 0) {
        return CryptoResult<ByteVector>{ByteVector{}, CryptoError::INVALID_LENGTH};
    }

    ByteVector result(length);
    auto fill_result = fill_bytes(result);
    if (!fill_result.success()) {
        return CryptoResult<ByteVector>{ByteVector{}, fill_result.error};
    }

    return CryptoResult<ByteVector>{result, CryptoError::SUCCESS};
}

CryptoResult<void> Random::fill_bytes(ByteVector& buffer) {
    if (buffer.empty()) {
        return CryptoResult<void>{CryptoError::INVALID_LENGTH};
    }

    return fill_bytes(buffer.data(), buffer.size());
}

CryptoResult<void> Random::fill_bytes(byte_t* buffer, size_t length) {
    if (!buffer || length == 0) {
        return CryptoResult<void>{CryptoError::INVALID_LENGTH};
    }

    try {
        // Use system random device for cryptographic security
        std::lock_guard<std::mutex> lock(mutex_);
        for (size_t i = 0; i < length; ++i) {
            buffer[i] = static_cast<byte_t>(random_device_() & 0xFF);
        }
        return CryptoResult<void>{CryptoError::SUCCESS};
    } catch (const std::exception&) {
        return CryptoResult<void>{CryptoError::RANDOM_GENERATION_FAILED};
    }
}

CryptoResult<Hash256> Random::generate_hash256() {
    Hash256 result;
    auto fill_result = fill_bytes(result.data(), result.size());
    if (!fill_result.success()) {
        return CryptoResult<Hash256>{Hash256{}, fill_result.error};
    }
    return CryptoResult<Hash256>{result, CryptoError::SUCCESS};
}

CryptoResult<Secp256k1PrivateKey> Random::generate_secp256k1_private_key() {
    Secp256k1PrivateKey result;
    auto fill_result = fill_bytes(result.data(), result.size());
    if (!fill_result.success()) {
        return CryptoResult<Secp256k1PrivateKey>{Secp256k1PrivateKey{}, fill_result.error};
    }
    return CryptoResult<Secp256k1PrivateKey>{result, CryptoError::SUCCESS};
}

CryptoResult<Ed25519PrivateKey> Random::generate_ed25519_private_key() {
    Ed25519PrivateKey result;
    auto fill_result = fill_bytes(result.data(), result.size());
    if (!fill_result.success()) {
        return CryptoResult<Ed25519PrivateKey>{Ed25519PrivateKey{}, fill_result.error};
    }
    return CryptoResult<Ed25519PrivateKey>{result, CryptoError::SUCCESS};
}

CryptoResult<BlsPrivateKey> Random::generate_bls_private_key() {
    BlsPrivateKey result;
    auto fill_result = fill_bytes(result.data(), result.size());
    if (!fill_result.success()) {
        return CryptoResult<BlsPrivateKey>{BlsPrivateKey{}, fill_result.error};
    }
    return CryptoResult<BlsPrivateKey>{result, CryptoError::SUCCESS};
}

CryptoResult<uint64_t> Random::generate_uint64() {
    return generate_uint64(0, std::numeric_limits<uint64_t>::max());
}

CryptoResult<uint64_t> Random::generate_uint64(uint64_t min, uint64_t max) {
    if (max <= min) {
        return CryptoResult<uint64_t>{0, CryptoError::INVALID_LENGTH};
    }

    try {
        initialize_generator();
        std::uniform_int_distribution<uint64_t> dist(min, max);
        return CryptoResult<uint64_t>{dist(generator_), CryptoError::SUCCESS};
    } catch (const std::exception&) {
        return CryptoResult<uint64_t>{0, CryptoError::RANDOM_GENERATION_FAILED};
    }
}

CryptoResult<uint32_t> Random::generate_uint32() {
    return generate_uint32(0, std::numeric_limits<uint32_t>::max());
}

CryptoResult<uint32_t> Random::generate_uint32(uint32_t min, uint32_t max) {
    if (max <= min) {
        return CryptoResult<uint32_t>{0, CryptoError::INVALID_LENGTH};
    }

    try {
        initialize_generator();
        std::uniform_int_distribution<uint32_t> dist(min, max);
        return CryptoResult<uint32_t>{dist(generator_), CryptoError::SUCCESS};
    } catch (const std::exception&) {
        return CryptoResult<uint32_t>{0, CryptoError::RANDOM_GENERATION_FAILED};
    }
}

std::random_device& Random::get_random_device() {
    return random_device_;
}

} // namespace chainforge::crypto
