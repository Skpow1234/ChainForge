#pragma once

#include "tokiwa/core/block.hpp"
#include "tokiwa/core/transaction.hpp"
#include "tokiwa/core/hash.hpp"
#include "tokiwa/core/address.hpp"
#include "tokiwa/storage/database.hpp"
#include "tokiwa/mempool/mempool.hpp"

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace tokiwa::rpc {

/**
 * Minimal chain state shared by the node and JSON-RPC layer.
 * Persists blocks/meta in the storage backend; keeps hot indexes in memory.
 */
class ChainState {
public:
    ChainState(std::shared_ptr<storage::Database> db,
               std::shared_ptr<mempool::Mempool> mempool,
               uint64_t chain_id = 1337);

    uint64_t chain_id() const { return chain_id_; }
    uint64_t latest_height() const;
    std::shared_ptr<mempool::Mempool> mempool() const { return mempool_; }

    /// Rebuild in-memory indexes from durable KV. Returns true if chain data was found.
    bool load_from_storage();
    bool initialize_with_genesis(const core::Block& genesis);
    bool import_block(const core::Block& block);

    std::optional<core::Block> get_block_by_number(uint64_t number) const;
    std::optional<core::Block> get_block_by_hash(const core::Hash& hash) const;

    std::optional<core::Transaction> get_transaction(const core::Hash& hash) const;
    std::string get_balance_hex(const core::Address& address) const;
    uint64_t get_nonce(const core::Address& address) const;

    /// Apply a simple value transfer locally (dev/demo accounting).
    bool credit(const core::Address& address, uint64_t wei);
    core::Hash submit_transaction(const core::Transaction& tx);

private:
    std::string height_key(uint64_t h) const;
    std::string hash_index_key(const core::Hash& h) const;
    std::string tx_key(const core::Hash& h) const;
    std::string balance_key(const core::Address& a) const;
    std::string nonce_key(const core::Address& a) const;

    bool write_block(const core::Block& block);
    void index_transactions(const core::Block& block);

    mutable std::mutex mutex_;
    std::shared_ptr<storage::Database> db_;
    std::shared_ptr<mempool::Mempool> mempool_;
    uint64_t chain_id_;
    uint64_t latest_height_ = 0;
    std::unordered_map<std::string, core::Block> blocks_by_hash_;
    std::unordered_map<uint64_t, std::string> hash_by_height_;
    std::unordered_map<std::string, core::Transaction> txs_by_hash_;
    std::unordered_map<std::string, uint64_t> balances_;
    std::unordered_map<std::string, uint64_t> nonces_;
};

} // namespace tokiwa::rpc
