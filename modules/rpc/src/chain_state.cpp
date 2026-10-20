#include "chainforge/rpc/chain_state.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cctype>
#include <sstream>

namespace chainforge::rpc {
namespace {

storage::Key to_key(const std::string& s) {
    return storage::Key(s.begin(), s.end());
}

std::string height_to_str(uint64_t h) {
    return std::to_string(h);
}

std::string addr_key(const core::Address& a) {
    auto hex = a.to_hex();
    std::transform(hex.begin(), hex.end(), hex.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return hex;
}

std::string normalize_hex(std::string hex) {
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex.erase(0, 2);
    }
    std::transform(hex.begin(), hex.end(), hex.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return hex;
}

} // namespace

ChainState::ChainState(std::shared_ptr<storage::Database> db,
                       std::shared_ptr<mempool::Mempool> mempool,
                       uint64_t chain_id)
    : db_(std::move(db)), mempool_(std::move(mempool)), chain_id_(chain_id) {
    if (db_ && !db_->is_open()) {
        storage::DatabaseConfig cfg;
        cfg.path = "./data/chainforge";
        cfg.create_if_missing = true;
        db_->open(cfg);
    }
}

uint64_t ChainState::latest_height() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return latest_height_;
}

std::string ChainState::height_key(uint64_t h) const {
    return "block:height:" + height_to_str(h);
}

std::string ChainState::hash_index_key(const core::Hash& h) const {
    return "block:hash:" + h.to_hex();
}

std::string ChainState::tx_key(const core::Hash& h) const {
    return "tx:" + h.to_hex();
}

std::string ChainState::balance_key(const core::Address& a) const {
    return "bal:" + addr_key(a);
}

std::string ChainState::nonce_key(const core::Address& a) const {
    return "nonce:" + addr_key(a);
}

bool ChainState::write_block(const core::Block& block) {
    if (!db_ || !db_->is_open()) {
        return false;
    }
    const auto hash = block.calculate_hash();
    const auto payload = block.to_json();
    const auto height_s = height_to_str(block.height());
    auto put1 = db_->put(to_key(height_key(block.height())),
                         storage::Value(payload.begin(), payload.end()));
    auto put2 = db_->put(to_key(hash_index_key(hash)),
                         storage::Value(height_s.begin(), height_s.end()));
    auto put3 = db_->put(to_key("meta:height"),
                         storage::Value(height_s.begin(), height_s.end()));
    return put1.success() && put2.success() && put3.success();
}

void ChainState::index_transactions(const core::Block& block) {
    for (const auto& tx : block.transactions()) {
        const auto h = tx.calculate_hash();
        txs_by_hash_[normalize_hex(h.to_hex())] = tx;
        if (db_ && db_->is_open()) {
            const auto json = tx.to_json();
            db_->put(to_key(tx_key(h)), storage::Value(json.begin(), json.end()));
        }
    }
}

bool ChainState::initialize_with_genesis(const core::Block& genesis) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!genesis.is_genesis()) {
        return false;
    }
    if (!write_block(genesis)) {
        return false;
    }
    const auto hash = genesis.calculate_hash();
    blocks_by_hash_[hash.to_hex()] = genesis;
    hash_by_height_[0] = hash.to_hex();
    latest_height_ = 0;
    index_transactions(genesis);
    return true;
}

bool ChainState::import_block(const core::Block& block) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (block.is_genesis()) {
        if (!blocks_by_hash_.empty()) {
            return false;
        }
    } else {
        if (block.height() != latest_height_ + 1) {
            return false;
        }
        auto parent_it = hash_by_height_.find(latest_height_);
        if (parent_it == hash_by_height_.end() ||
            parent_it->second != block.parent_hash().to_hex()) {
            return false;
        }
    }

    if (!write_block(block)) {
        return false;
    }

    const auto hash = block.calculate_hash();
    blocks_by_hash_[hash.to_hex()] = block;
    hash_by_height_[block.height()] = hash.to_hex();
    latest_height_ = block.height();
    index_transactions(block);

    if (mempool_) {
        for (const auto& tx : block.transactions()) {
            mempool_->remove_transaction(tx.calculate_hash());
        }
    }
    return true;
}

std::optional<core::Block> ChainState::get_block_by_number(uint64_t number) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = hash_by_height_.find(number);
    if (it == hash_by_height_.end()) {
        return std::nullopt;
    }
    auto bit = blocks_by_hash_.find(it->second);
    if (bit == blocks_by_hash_.end()) {
        return std::nullopt;
    }
    return bit->second;
}

std::optional<core::Block> ChainState::get_block_by_hash(const core::Hash& hash) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = blocks_by_hash_.find(hash.to_hex());
    if (it == blocks_by_hash_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<core::Transaction> ChainState::get_transaction(const core::Hash& hash) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = txs_by_hash_.find(normalize_hex(hash.to_hex()));
    if (it != txs_by_hash_.end()) {
        return it->second;
    }
    if (mempool_ && mempool_->has_transaction(hash)) {
        return mempool_->get_transaction(hash);
    }
    return std::nullopt;
}

std::string ChainState::get_balance_hex(const core::Address& address) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = balances_.find(addr_key(address));
    const uint64_t bal = (it == balances_.end()) ? 0 : it->second;
    std::ostringstream ss;
    ss << "0x" << std::hex << bal;
    return ss.str();
}

uint64_t ChainState::get_nonce(const core::Address& address) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = nonces_.find(addr_key(address));
    return (it == nonces_.end()) ? 0 : it->second;
}

bool ChainState::credit(const core::Address& address, uint64_t wei) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto key = addr_key(address);
    balances_[key] += wei;
    if (db_ && db_->is_open()) {
        std::ostringstream ss;
        ss << balances_[key];
        auto s = ss.str();
        db_->put(to_key(balance_key(address)), storage::Value(s.begin(), s.end()));
    }
    return true;
}

core::Hash ChainState::submit_transaction(const core::Transaction& tx) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto hash = tx.calculate_hash();
    if (mempool_) {
        mempool_->add_transaction(tx);
    }
    txs_by_hash_[normalize_hex(hash.to_hex())] = tx;
    nonces_[addr_key(tx.from())] = tx.nonce() + 1;
    return hash;
}

} // namespace chainforge::rpc
