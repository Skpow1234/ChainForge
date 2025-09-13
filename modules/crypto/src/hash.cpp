#include "chainforge/crypto/hash.hpp"
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <iomanip>
#include <sstream>

namespace chainforge::crypto {

CryptoResult<Hash256> Hash::sha256(const ByteVector& data) {
    return internal_sha256(data.data(), data.size());
}

CryptoResult<Hash256> Hash::sha256(const byte_t* data, size_t length) {
    return internal_sha256(data, length);
}

CryptoResult<Hash256> Hash::sha256(const std::string& data) {
    return internal_sha256(reinterpret_cast<const byte_t*>(data.data()), data.size());
}

CryptoResult<Hash256> Hash::keccak256(const ByteVector& data) {
    return internal_keccak256(data.data(), data.size());
}

CryptoResult<Hash256> Hash::keccak256(const byte_t* data, size_t length) {
    return internal_keccak256(data, length);
}

CryptoResult<Hash256> Hash::keccak256(const std::string& data) {
    return internal_keccak256(reinterpret_cast<const byte_t*>(data.data()), data.size());
}

CryptoResult<Ripemd160Hash> Hash::ripemd160(const ByteVector& data) {
    return internal_ripemd160(data.data(), data.size());
}

CryptoResult<Ripemd160Hash> Hash::ripemd160(const byte_t* data, size_t length) {
    return internal_ripemd160(data, length);
}

CryptoResult<Ripemd160Hash> Hash::ripemd160(const std::string& data) {
    return internal_ripemd160(reinterpret_cast<const byte_t*>(data.data()), data.size());
}

CryptoResult<Hash256> Hash::double_sha256(const ByteVector& data) {
    return double_sha256(data.data(), data.size());
}

CryptoResult<Hash256> Hash::double_sha256(const byte_t* data, size_t length) {
    auto first_hash = internal_sha256(data, length);
    if (!first_hash.success()) {
        return CryptoResult<Hash256>{Hash256{}, first_hash.error};
    }
    return internal_sha256(first_hash.value.data(), first_hash.value.size());
}

CryptoResult<Hash256> Hash::double_sha256(const std::string& data) {
    return double_sha256(reinterpret_cast<const byte_t*>(data.data()), data.size());
}

CryptoResult<Ripemd160Hash> Hash::keccak256_ripemd160(const ByteVector& data) {
    return keccak256_ripemd160(data.data(), data.size());
}

CryptoResult<Ripemd160Hash> Hash::keccak256_ripemd160(const byte_t* data, size_t length) {
    auto keccak_result = internal_keccak256(data, length);
    if (!keccak_result.success()) {
        return CryptoResult<Ripemd160Hash>{Ripemd160Hash{}, keccak_result.error};
    }
    return internal_ripemd160(keccak_result.value.data(), keccak_result.value.size());
}

CryptoResult<Ripemd160Hash> Hash::keccak256_ripemd160(const std::string& data) {
    return keccak256_ripemd160(reinterpret_cast<const byte_t*>(data.data()), data.size());
}

CryptoResult<Hash256> Hash::hash_pair(const Hash256& left, const Hash256& right) {
    ByteVector combined;
    combined.reserve(left.size() + right.size());
    combined.insert(combined.end(), left.begin(), left.end());
    combined.insert(combined.end(), right.begin(), right.end());
    return sha256(combined);
}

CryptoResult<Hash256> Hash::hash_many(const std::vector<Hash256>& hashes) {
    if (hashes.empty()) {
        Hash256 zero_hash{};
        return CryptoResult<Hash256>{zero_hash, CryptoError::SUCCESS};
    }

    if (hashes.size() == 1) {
        return CryptoResult<Hash256>{hashes[0], CryptoError::SUCCESS};
    }

    // Build Merkle tree
    std::vector<Hash256> current_level = hashes;

    while (current_level.size() > 1) {
        std::vector<Hash256> next_level;
        next_level.reserve((current_level.size() + 1) / 2);

        for (size_t i = 0; i < current_level.size(); i += 2) {
            if (i + 1 < current_level.size()) {
                auto hash_result = hash_pair(current_level[i], current_level[i + 1]);
                if (!hash_result.success()) {
                    return CryptoResult<Hash256>{Hash256{}, hash_result.error};
                }
                next_level.push_back(hash_result.value);
            } else {
                // Duplicate last hash if odd number
                auto hash_result = hash_pair(current_level[i], current_level[i]);
                if (!hash_result.success()) {
                    return CryptoResult<Hash256>{Hash256{}, hash_result.error};
                }
                next_level.push_back(hash_result.value);
            }
        }

        current_level = std::move(next_level);
    }

    return CryptoResult<Hash256>{current_level[0], CryptoError::SUCCESS};
}

std::string Hash::to_hex(const Hash256& hash) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : hash) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

std::string Hash::to_hex(const Ripemd160Hash& hash) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto byte : hash) {
        ss << std::setw(2) << static_cast<int>(byte);
    }
    return ss.str();
}

CryptoResult<Hash256> Hash::from_hex(const std::string& hex_string) {
    if (!is_valid_hex_hash(hex_string, HASH256_SIZE)) {
        return CryptoResult<Hash256>{Hash256{}, CryptoError::INVALID_LENGTH};
    }

    Hash256 result;
    for (size_t i = 0; i < HASH256_SIZE; ++i) {
        std::string byte_str = hex_string.substr(i * 2, 2);
        result[i] = static_cast<byte_t>(std::stoi(byte_str, nullptr, 16));
    }

    return CryptoResult<Hash256>{result, CryptoError::SUCCESS};
}

bool Hash::is_valid_hex_hash(const std::string& hex_string, size_t expected_size) {
    if (hex_string.length() != expected_size * 2) {
        return false;
    }

    // Check if all characters are valid hex
    for (char c : hex_string) {
        if (!std::isxdigit(c)) {
            return false;
        }
    }

    return true;
}

CryptoResult<Hash256> Hash::internal_sha256(const byte_t* data, size_t length) {
    Hash256 result;
    SHA256_CTX ctx;

    if (SHA256_Init(&ctx) != 1) {
        return CryptoResult<Hash256>{Hash256{}, CryptoError::HASH_FAILED};
    }

    if (SHA256_Update(&ctx, data, length) != 1) {
        return CryptoResult<Hash256>{Hash256{}, CryptoError::HASH_FAILED};
    }

    if (SHA256_Final(result.data(), &ctx) != 1) {
        return CryptoResult<Hash256>{Hash256{}, CryptoError::HASH_FAILED};
    }

    return CryptoResult<Hash256>{result, CryptoError::SUCCESS};
}

CryptoResult<Hash256> Hash::internal_keccak256(const byte_t* data, size_t length) {
    // TODO: Implement Keccak256 using a proper Keccak library
    // For now, fall back to SHA256 as placeholder
    return internal_sha256(data, length);
}

CryptoResult<Ripemd160Hash> Hash::internal_ripemd160(const byte_t* data, size_t length) {
    Ripemd160Hash result;
    RIPEMD160_CTX ctx;

    if (RIPEMD160_Init(&ctx) != 1) {
        return CryptoResult<Ripemd160Hash>{Ripemd160Hash{}, CryptoError::HASH_FAILED};
    }

    if (RIPEMD160_Update(&ctx, data, length) != 1) {
        return CryptoResult<Ripemd160Hash>{Ripemd160Hash{}, CryptoError::HASH_FAILED};
    }

    if (RIPEMD160_Final(result.data(), &ctx) != 1) {
        return CryptoResult<Ripemd160Hash>{Ripemd160Hash{}, CryptoError::HASH_FAILED};
    }

    return CryptoResult<Ripemd160Hash>{result, CryptoError::SUCCESS};
}

} // namespace chainforge::crypto
