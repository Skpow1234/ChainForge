#include "tokiwa/crypto/keccak.hpp"
#include "tokiwa/crypto/hash.hpp"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace tokiwa::crypto {
namespace {

// Keccak-f[1600] round constants
constexpr uint64_t RC[24] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808AULL,
    0x8000000080008000ULL, 0x000000000000808BULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008AULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000AULL,
    0x000000008000808BULL, 0x800000000000008BULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800AULL, 0x800000008000000AULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

constexpr int ROTC[24] = {
    1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14,
    27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44
};

constexpr int PILN[24] = {
    10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4,
    15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1
};

inline uint64_t rotl64(uint64_t x, int n) {
    return (x << n) | (x >> (64 - n));
}

void keccak_f1600(uint64_t state[25]) {
    uint64_t bc[5];
    for (int round = 0; round < 24; ++round) {
        for (int i = 0; i < 5; ++i) {
            bc[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
        }
        for (int i = 0; i < 5; ++i) {
            const uint64_t t = bc[(i + 4) % 5] ^ rotl64(bc[(i + 1) % 5], 1);
            for (int j = 0; j < 25; j += 5) {
                state[j + i] ^= t;
            }
        }

        uint64_t t = state[1];
        for (int i = 0; i < 24; ++i) {
            const int j = PILN[i];
            bc[0] = state[j];
            state[j] = rotl64(t, ROTC[i]);
            t = bc[0];
        }

        for (int j = 0; j < 25; j += 5) {
            for (int i = 0; i < 5; ++i) {
                bc[i] = state[j + i];
            }
            for (int i = 0; i < 5; ++i) {
                state[j + i] = bc[i] ^ ((~bc[(i + 1) % 5]) & bc[(i + 2) % 5]);
            }
        }

        state[0] ^= RC[round];
    }
}

// Ethereum / original Keccak padding (0x01), not NIST SHA-3 (0x06).
CryptoResult<ByteVector> keccak_sponge(const byte_t* data, size_t length, size_t output_bytes) {
    if (output_bytes == 0 || output_bytes > 64) {
        return CryptoResult<ByteVector>{ByteVector{}, CryptoError::INVALID_ARGUMENT};
    }

    // rate = 1600 - 2 * (output_bits); for 256-bit digest -> rate 1088 bits = 136 bytes
    const size_t rate_bytes = 200 - 2 * output_bytes;
    if (rate_bytes == 0 || rate_bytes > 200) {
        return CryptoResult<ByteVector>{ByteVector{}, CryptoError::INVALID_ARGUMENT};
    }

    uint64_t state[25] = {};
    size_t offset = 0;

    while (offset + rate_bytes <= length) {
        for (size_t i = 0; i < rate_bytes; ++i) {
            reinterpret_cast<uint8_t*>(state)[i] ^= data[offset + i];
        }
        keccak_f1600(state);
        offset += rate_bytes;
    }

    uint8_t block[200] = {};
    const size_t remaining = length - offset;
    if (remaining > 0) {
        std::memcpy(block, data + offset, remaining);
    }
    block[remaining] = 0x01;           // Keccak pad
    block[rate_bytes - 1] |= 0x80;     // final bit

    for (size_t i = 0; i < rate_bytes; ++i) {
        reinterpret_cast<uint8_t*>(state)[i] ^= block[i];
    }
    keccak_f1600(state);

    ByteVector out(output_bytes);
    std::memcpy(out.data(), state, output_bytes);
    return CryptoResult<ByteVector>{out, CryptoError::SUCCESS};
}

} // namespace

class Keccak::StreamHasher::Impl {
public:
    void update(const ByteVector& data) {
        buffer_.insert(buffer_.end(), data.begin(), data.end());
    }

    void update(const byte_t* data, size_t length) {
        buffer_.insert(buffer_.end(), data, data + length);
    }

    void update(const std::string& data) {
        buffer_.insert(buffer_.end(), data.begin(), data.end());
    }

    CryptoResult<Hash256> finalize_256() {
        return Keccak::keccak256(buffer_);
    }

    CryptoResult<std::array<byte_t, 20>> finalize_160() {
        return Keccak::keccak160(buffer_);
    }

    CryptoResult<std::array<byte_t, 48>> finalize_384() {
        return Keccak::keccak384(buffer_);
    }

    CryptoResult<std::array<byte_t, 64>> finalize_512() {
        return Keccak::keccak512(buffer_);
    }

    void reset() { buffer_.clear(); }

private:
    ByteVector buffer_;
};

Keccak::StreamHasher::StreamHasher() : impl_(std::make_unique<Impl>()) {}
Keccak::StreamHasher::~StreamHasher() = default;

void Keccak::StreamHasher::update(const ByteVector& data) { impl_->update(data); }
void Keccak::StreamHasher::update(const byte_t* data, size_t length) { impl_->update(data, length); }
void Keccak::StreamHasher::update(const std::string& data) { impl_->update(data); }
CryptoResult<Hash256> Keccak::StreamHasher::finalize_256() { return impl_->finalize_256(); }
CryptoResult<std::array<byte_t, 20>> Keccak::StreamHasher::finalize_160() { return impl_->finalize_160(); }
CryptoResult<std::array<byte_t, 48>> Keccak::StreamHasher::finalize_384() { return impl_->finalize_384(); }
CryptoResult<std::array<byte_t, 64>> Keccak::StreamHasher::finalize_512() { return impl_->finalize_512(); }
void Keccak::StreamHasher::reset() { impl_->reset(); }

CryptoResult<Hash256> Keccak::keccak256(const ByteVector& data) {
    return internal_keccak(data.data(), data.size(), 256);
}

CryptoResult<Hash256> Keccak::keccak256(const byte_t* data, size_t length) {
    return internal_keccak(data, length, 256);
}

CryptoResult<Hash256> Keccak::keccak256(const std::string& data) {
    return internal_keccak(reinterpret_cast<const byte_t*>(data.data()), data.size(), 256);
}

CryptoResult<std::array<byte_t, 20>> Keccak::keccak160(const ByteVector& data) {
    return keccak160(data.data(), data.size());
}

CryptoResult<std::array<byte_t, 20>> Keccak::keccak160(const byte_t* data, size_t length) {
    auto result = keccak_sponge(data, length, 20);
    if (!result.success()) {
        return CryptoResult<std::array<byte_t, 20>>{{}, result.error};
    }
    std::array<byte_t, 20> output{};
    std::copy_n(result.value.begin(), 20, output.begin());
    return CryptoResult<std::array<byte_t, 20>>{output, CryptoError::SUCCESS};
}

CryptoResult<std::array<byte_t, 48>> Keccak::keccak384(const ByteVector& data) {
    return keccak384(data.data(), data.size());
}

CryptoResult<std::array<byte_t, 48>> Keccak::keccak384(const byte_t* data, size_t length) {
    auto result = keccak_sponge(data, length, 48);
    if (!result.success()) {
        return CryptoResult<std::array<byte_t, 48>>{{}, result.error};
    }
    std::array<byte_t, 48> output{};
    std::copy_n(result.value.begin(), 48, output.begin());
    return CryptoResult<std::array<byte_t, 48>>{output, CryptoError::SUCCESS};
}

CryptoResult<std::array<byte_t, 64>> Keccak::keccak512(const ByteVector& data) {
    return keccak512(data.data(), data.size());
}

CryptoResult<std::array<byte_t, 64>> Keccak::keccak512(const byte_t* data, size_t length) {
    auto result = keccak_sponge(data, length, 64);
    if (!result.success()) {
        return CryptoResult<std::array<byte_t, 64>>{{}, result.error};
    }
    std::array<byte_t, 64> output{};
    std::copy_n(result.value.begin(), 64, output.begin());
    return CryptoResult<std::array<byte_t, 64>>{output, CryptoError::SUCCESS};
}

std::string Keccak::to_hex(const Hash256& hash) {
    return Hash::to_hex(hash);
}

std::string Keccak::to_hex(const std::array<byte_t, 20>& hash) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : hash) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string Keccak::to_hex(const std::array<byte_t, 48>& hash) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : hash) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string Keccak::to_hex(const std::array<byte_t, 64>& hash) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : hash) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

CryptoResult<Hash256> Keccak::ethereum_address_hash(const ByteVector& data) {
    auto keccak_result = keccak256(data);
    if (!keccak_result.success()) {
        return keccak_result;
    }

    Hash256 address{};
    std::copy(keccak_result.value.begin() + 12, keccak_result.value.end(), address.begin());
    return CryptoResult<Hash256>{address, CryptoError::SUCCESS};
}

CryptoResult<Hash256> Keccak::ethereum_signature_hash(const ByteVector& data) {
    std::string prefix = std::string("\x19", 1) + "Ethereum Signed Message:\n" + std::to_string(data.size());
    ByteVector message;
    message.reserve(prefix.size() + data.size());
    message.insert(message.end(), prefix.begin(), prefix.end());
    message.insert(message.end(), data.begin(), data.end());
    return keccak256(message);
}

void Keccak::keccak_f800_round(byte_t* state) {
    // Keccak-f[800] not required for current Eth-style hashing; keep as no-op hook.
    (void)state;
}

void Keccak::keccak_f1600_round(byte_t* state) {
    if (!state) {
        return;
    }
    uint64_t words[25];
    std::memcpy(words, state, sizeof(words));
    keccak_f1600(words);
    std::memcpy(state, words, sizeof(words));
}

CryptoResult<Hash256> Keccak::internal_keccak(const byte_t* data, size_t length, size_t output_bits) {
    const size_t output_bytes = (output_bits + 7) / 8;
    auto sponge = keccak_sponge(data, length, output_bytes);
    if (!sponge.success()) {
        return CryptoResult<Hash256>{Hash256{}, sponge.error};
    }

    Hash256 out{};
    const size_t copy_n = std::min(output_bytes, HASH256_SIZE);
    std::copy_n(sponge.value.begin(), copy_n, out.begin());
    return CryptoResult<Hash256>{out, CryptoError::SUCCESS};
}

} // namespace tokiwa::crypto
