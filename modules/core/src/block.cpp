#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include <sstream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace chainforge::core {

Block::Block(BlockHeight height, const Hash& parent_hash, const Timestamp& timestamp)
    : header_{height, parent_hash, Hash::zero(), timestamp, 0, 8000000, 1, 1},
      transactions_{} {}

Block::Block(const BlockHeader& header, std::vector<Transaction> transactions)
    : header_(header), transactions_(std::move(transactions)) {
    update_merkle_root();
}

void Block::set_height(BlockHeight height) {
    header_.height = height;
    invalidate_cache();
}

void Block::set_parent_hash(const Hash& parent_hash) {
    header_.parent_hash = parent_hash;
    invalidate_cache();
}

void Block::set_timestamp(const Timestamp& timestamp) {
    header_.timestamp = timestamp;
    invalidate_cache();
}

void Block::set_nonce(BlockNonce nonce) {
    header_.nonce = nonce;
    invalidate_cache();
}

void Block::set_gas_limit(GasLimit gas_limit) {
    header_.gas_limit = gas_limit;
}

void Block::set_gas_price(GasPrice gas_price) {
    header_.gas_price = gas_price;
}

void Block::set_chain_id(ChainId chain_id) {
    header_.chain_id = chain_id;
}

void Block::add_transaction(const Transaction& transaction) {
    transactions_.push_back(transaction);
    update_merkle_root();
    invalidate_cache();
}

void Block::remove_transaction(size_t index) {
    if (index < transactions_.size()) {
        transactions_.erase(transactions_.begin() + index);
        update_merkle_root();
        invalidate_cache();
    }
}

void Block::clear_transactions() {
    transactions_.clear();
    header_.merkle_root = Hash::zero();
    invalidate_cache();
}

Hash Block::calculate_hash() const {
    if (cached_hash_.has_value()) {
        return cached_hash_.value();
    }

    // Create a simple hash from block header
    // TODO: Implement proper block header hashing
    std::vector<uint8_t> hash_data;

    // Add height (big-endian)
    for (int i = sizeof(header_.height) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((header_.height >> (i * 8)) & 0xFF));
    }

    // Add parent hash
    auto parent_hash_bytes = header_.parent_hash.to_bytes();
    hash_data.insert(hash_data.end(), parent_hash_bytes.begin(), parent_hash_bytes.end());

    // Add merkle root
    auto merkle_root_bytes = header_.merkle_root.to_bytes();
    hash_data.insert(hash_data.end(), merkle_root_bytes.begin(), merkle_root_bytes.end());

    // Add timestamp
    auto timestamp_seconds = header_.timestamp.seconds();
    for (int i = sizeof(timestamp_seconds) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((timestamp_seconds >> (i * 8)) & 0xFF));
    }

    // Add nonce
    for (int i = sizeof(header_.nonce) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((header_.nonce >> (i * 8)) & 0xFF));
    }

    // Add gas limit and price
    for (int i = sizeof(header_.gas_limit) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((header_.gas_limit >> (i * 8)) & 0xFF));
    }

    for (int i = sizeof(header_.gas_price) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((header_.gas_price >> (i * 8)) & 0xFF));
    }

    // Add chain id
    for (int i = sizeof(header_.chain_id) - 1; i >= 0; --i) {
        hash_data.push_back(static_cast<uint8_t>((header_.chain_id >> (i * 8)) & 0xFF));
    }

    cached_hash_ = hash_sha256(hash_data);
    return cached_hash_.value();
}

Hash Block::calculate_merkle_root() const {
    if (transactions_.empty()) {
        return Hash::zero();
    }

    if (transactions_.size() == 1) {
        return transactions_[0].calculate_hash();
    }

    // Simple binary merkle tree calculation
    std::vector<Hash> hashes;
    hashes.reserve(transactions_.size());

    for (const auto& tx : transactions_) {
        hashes.push_back(tx.calculate_hash());
    }

    while (hashes.size() > 1) {
        std::vector<Hash> new_hashes;
        new_hashes.reserve((hashes.size() + 1) / 2);

        for (size_t i = 0; i < hashes.size(); i += 2) {
            if (i + 1 < hashes.size()) {
                new_hashes.push_back(combine_hashes(hashes[i], hashes[i + 1]));
            } else {
                // Odd number of hashes, duplicate the last one
                new_hashes.push_back(combine_hashes(hashes[i], hashes[i]));
            }
        }

        hashes = std::move(new_hashes);
    }

    return hashes[0];
}

bool Block::is_genesis() const noexcept {
    return header_.height == 0;
}

bool Block::is_valid() const {
    return validate_header() && validate_transactions() && validate_size();
}

size_t Block::size() const {
    size_t size = sizeof(BlockHeader);
    for (const auto& tx : transactions_) {
        size += tx.size();
    }
    return size;
}

bool Block::is_full() const {
    return size() >= MAX_BLOCK_SIZE;
}

bool Block::validate_header() const {
    return header_.height >= 0 &&
           header_.timestamp.is_valid() &&
           header_.gas_limit > 0 &&
           header_.chain_id > 0;
}

bool Block::validate_transactions() const {
    for (const auto& tx : transactions_) {
        if (!tx.is_valid()) {
            return false;
        }
    }
    return validate_gas_limits();
}

bool Block::validate_size() const {
    return size() <= MAX_BLOCK_SIZE;
}

std::string Block::to_string() const {
    std::stringstream ss;
    ss << "Block{"
       << "height: " << header_.height
       << ", hash: " << calculate_hash().to_hex().substr(0, 16) << "..."
       << ", parent: " << header_.parent_hash.to_hex().substr(0, 16) << "..."
       << ", transactions: " << transactions_.size()
       << ", timestamp: " << header_.timestamp.seconds()
       << ", gas_limit: " << header_.gas_limit
       << "}";
    return ss.str();
}

std::string Block::to_json() const {
    nlohmann::json j;
    j["height"] = header_.height;
    j["hash"] = calculate_hash().to_hex();
    j["parentHash"] = header_.parent_hash.to_hex();
    j["merkleRoot"] = header_.merkle_root.to_hex();
    j["timestamp"] = header_.timestamp.seconds();
    j["nonce"] = header_.nonce;
    j["gasLimit"] = header_.gas_limit;
    j["gasPrice"] = header_.gas_price;
    j["chainId"] = header_.chain_id;

    nlohmann::json txs = nlohmann::json::array();
    for (const auto& tx : transactions_) {
        txs.push_back(nlohmann::json::parse(tx.to_json()));
    }
    j["transactions"] = txs;

    return j.dump(2);
}

void Block::update_merkle_root() {
    header_.merkle_root = calculate_merkle_root();
}

void Block::invalidate_cache() {
    cached_hash_.reset();
}

bool Block::validate_gas_limits() const {
    GasLimit total_gas_used = 0;
    for (const auto& tx : transactions_) {
        total_gas_used += tx.gas_limit();
        if (total_gas_used > header_.gas_limit) {
            return false;
        }
    }
    return true;
}

// Free functions
std::ostream& operator<<(std::ostream& os, const Block& block) {
    os << block.to_string();
    return os;
}

bool operator==(const Block& lhs, const Block& rhs) {
    return lhs.calculate_hash() == rhs.calculate_hash();
}

bool operator!=(const Block& lhs, const Block& rhs) {
    return !(lhs == rhs);
}

Block create_genesis_block(ChainId chain_id) {
    Timestamp genesis_timestamp = Timestamp::from_seconds(1609459200); // 2021-01-01 00:00:00 UTC
    Block genesis(0, Hash::zero(), genesis_timestamp);
    genesis.set_chain_id(chain_id);
    genesis.set_nonce(0x12345678); // Arbitrary nonce for genesis
    return genesis;
}

Block create_block(BlockHeight height, const Hash& parent_hash, const Timestamp& timestamp) {
    return Block(height, parent_hash, timestamp);
}

bool is_valid_block(const Block& block) {
    return block.is_valid();
}

bool is_valid_block_header(const BlockHeader& header) {
    return header.height >= 0 &&
           header.timestamp.is_valid() &&
           header.gas_limit > 0 &&
           header.chain_id > 0;
}

} // namespace chainforge::core
