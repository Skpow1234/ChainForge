#include "chainforge/crypto/keccak.hpp"
#include "chainforge/crypto/hash.hpp"
#include <iomanip>
#include <sstream>

namespace chainforge::crypto {

// StreamHasher implementation
class Keccak::StreamHasher::Impl {
public:
    Impl() = default;
    ~Impl() = default;

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

    void reset() {
        buffer_.clear();
    }

private:
    ByteVector buffer_;
};

Keccak::StreamHasher::StreamHasher() : impl_(std::make_unique<Impl>()) {}
Keccak::StreamHasher::~StreamHasher() = default;

void Keccak::StreamHasher::update(const ByteVector& data) {
    impl_->update(data);
}

void Keccak::StreamHasher::update(const byte_t* data, size_t length) {
    impl_->update(data, length);
}

void Keccak::StreamHasher::update(const std::string& data) {
    impl_->update(data);
}

CryptoResult<Hash256> Keccak::StreamHasher::finalize_256() {
    return impl_->finalize_256();
}

CryptoResult<std::array<byte_t, 20>> Keccak::StreamHasher::finalize_160() {
    return impl_->finalize_160();
}

CryptoResult<std::array<byte_t, 48>> Keccak::StreamHasher::finalize_384() {
    return impl_->finalize_384();
}

CryptoResult<std::array<byte_t, 64>> Keccak::StreamHasher::finalize_512() {
    return impl_->finalize_512();
}

void Keccak::StreamHasher::reset() {
    impl_->reset();
}

// Static Keccak functions
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
    auto result = internal_keccak(data, length, 160);
    std::array<byte_t, 20> output;
    std::copy(result.value.begin(), result.value.begin() + 20, output.begin());
    return CryptoResult<std::array<byte_t, 20>>{output, result.error};
}

CryptoResult<std::array<byte_t, 48>> Keccak::keccak384(const ByteVector& data) {
    return keccak384(data.data(), data.size());
}

CryptoResult<std::array<byte_t, 48>> Keccak::keccak384(const byte_t* data, size_t length) {
    auto result = internal_keccak(data, length, 384);
    std::array<byte_t, 48> output;
    std::copy(result.value.begin(), result.value.begin() + 48, output.begin());
    return CryptoResult<std::array<byte_t, 48>>{output, result.error};
}

CryptoResult<std::array<byte_t, 64>> Keccak::keccak512(const ByteVector& data) {
    return keccak512(data.data(), data.size());
}

CryptoResult<std::array<byte_t, 64>> Keccak::keccak512(const byte_t* data, size_t length) {
    auto result = internal_keccak(data, length, 512);
    std::array<byte_t, 64> output;
    std::copy(result.value.begin(), result.value.begin() + 64, output.begin());
    return CryptoResult<std::array<byte_t, 64>>{output, result.error};
}

// Utility functions
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

// Ethereum-specific functions
CryptoResult<Hash256> Keccak::ethereum_address_hash(const ByteVector& data) {
    // Ethereum address: keccak256(data)[12:32]
    auto keccak_result = keccak256(data);
    if (!keccak_result.success()) {
        return keccak_result;
    }

    Hash256 address{};
    std::copy(keccak_result.value.begin() + 12, keccak_result.value.end(), address.begin());
    return CryptoResult<Hash256>{address, CryptoError::SUCCESS};
}

CryptoResult<Hash256> Keccak::ethereum_signature_hash(const ByteVector& data) {
    // Add Ethereum message prefix
    std::string prefix = std::string("\x19", 1) + "Ethereum Signed Message:\n" + std::to_string(data.size());
    ByteVector message;
    message.reserve(prefix.size() + data.size());
    message.insert(message.end(), prefix.begin(), prefix.end());
    message.insert(message.end(), data.begin(), data.end());

    return keccak256(message);
}

// Mining functions (placeholders)
void Keccak::keccak_f800_round(byte_t* state) {
    // TODO: Implement Keccak-f[800] round function
    (void)state; // Suppress unused parameter warning
}

void Keccak::keccak_f1600_round(byte_t* state) {
    // TODO: Implement Keccak-f[1600] round function
    (void)state; // Suppress unused parameter warning
}

// Internal implementation
CryptoResult<Hash256> Keccak::internal_keccak(const byte_t* data, size_t length, size_t output_bits) {
    // TODO: Implement proper Keccak hashing
    // For now, fall back to SHA256 as placeholder
    // This should be replaced with a proper Keccak implementation

    if (output_bits == 256) {
        return Hash::sha256(data, length);
    }

    // For other output sizes, return truncated/extended SHA256
    auto sha256_result = Hash::sha256(data, length);
    if (!sha256_result.success()) {
        return sha256_result;
    }

    // Adjust output size
    if (output_bits < 256) {
        // Truncate
        Hash256 truncated{};
        size_t bytes_to_copy = (output_bits + 7) / 8; // Round up to bytes
        std::copy(sha256_result.value.begin(), sha256_result.value.begin() + bytes_to_copy, truncated.begin());
        return CryptoResult<Hash256>{truncated, CryptoError::SUCCESS};
    } else {
        // For larger outputs, we'd need to extend, but for now just return SHA256
        return sha256_result;
    }
}

} // namespace chainforge::crypto
