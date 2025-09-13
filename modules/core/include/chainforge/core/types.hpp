#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>

namespace chainforge::core {

// Forward declarations
class Block;
class Transaction;
class Address;
class Amount;
class Hash;
class Timestamp;

// Type aliases
using BlockHeight = uint64_t;
using BlockNonce = uint64_t;
using GasLimit = uint64_t;
using GasPrice = uint64_t;
using ChainId = uint32_t;

// Hash type (32 bytes for SHA256)
using Hash256 = std::array<uint8_t, 32>;

// Address type (20 bytes for Ethereum-style addresses)
using Address160 = std::array<uint8_t, 20>;

// Block header structure
struct BlockHeader {
    BlockHeight height;
    Hash parent_hash;
    Hash merkle_root;
    Timestamp timestamp;
    BlockNonce nonce;
    GasLimit gas_limit;
    GasPrice gas_price;
    ChainId chain_id;
};

// Transaction structure
struct TransactionData {
    Address from;
    Address to;
    Amount value;
    GasLimit gas_limit;
    GasPrice gas_price;
    std::vector<uint8_t> data;
    uint64_t nonce;
};

// Block structure
struct BlockData {
    BlockHeader header;
    std::vector<Transaction> transactions;
    Hash256 hash;
};

// Common constants
constexpr size_t HASH_SIZE = 32;
constexpr size_t ADDRESS_SIZE = 20;
constexpr size_t MAX_BLOCK_SIZE = 1024 * 1024; // 1MB
constexpr size_t MAX_TRANSACTION_SIZE = 128 * 1024; // 128KB

} // namespace chainforge::core
