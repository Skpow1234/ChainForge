#pragma once

#include "chainforge/mempool/mempool.hpp"
#include <unordered_map>
#include <queue>
#include <shared_mutex>
#include <chrono>

namespace chainforge::mempool {

/**
 * Internal implementation of the transaction mempool
 */
class MempoolImpl : public Mempool {
public:
    explicit MempoolImpl(const MempoolConfig& config = {});
    ~MempoolImpl() override = default;

    // Configuration
    void set_config(const MempoolConfig& config) override;
    const MempoolConfig& get_config() const override;

    // Transaction management
    MempoolError add_transaction(const chainforge::core::Transaction& transaction) override;
    MempoolError remove_transaction(const chainforge::core::Hash& tx_hash) override;
    std::optional<chainforge::core::Transaction> get_transaction(const chainforge::core::Hash& tx_hash) const override;
    bool has_transaction(const chainforge::core::Hash& tx_hash) const override;

    // RBF support
    MempoolError replace_transaction(const chainforge::core::Transaction& new_transaction) override;

    // Batch operations
    std::vector<chainforge::core::Transaction> get_top_transactions(size_t count) override;
    std::vector<chainforge::core::Transaction> get_transactions_for_block(size_t max_count, uint64_t max_gas_limit) override;
    std::vector<chainforge::core::Hash> get_all_transaction_hashes() const override;

    // Maintenance
    void evict_expired_transactions() override;
    void evict_low_fee_transactions() override;
    void clear() override;

    // Statistics
    MempoolStats get_stats() const override;

    // Validation
    bool validate_transaction(const chainforge::core::Transaction& transaction) const override;
    MempoolError check_replacement_policy(const chainforge::core::Transaction& old_tx,
                                         const chainforge::core::Transaction& new_tx) const override;

    // Callbacks
    void set_transaction_added_callback(TransactionAddedCallback callback) override;
    void set_transaction_removed_callback(TransactionRemovedCallback callback) override;

private:
    // Thread safety
    mutable std::shared_mutex mutex_;

    // Configuration
    MempoolConfig config_;

    // Storage
    std::unordered_map<chainforge::core::Hash, MempoolEntry> transactions_;
    std::unordered_map<chainforge::core::Address, std::unordered_map<uint64_t, chainforge::core::Hash>> account_nonces_;

    // Priority queue for efficient selection (max-heap by priority score)
    using PriorityQueueEntry = std::pair<double, chainforge::core::Hash>;  // (score, hash)
    std::priority_queue<PriorityQueueEntry> priority_queue_;

    // Callbacks
    TransactionAddedCallback on_transaction_added_;
    TransactionRemovedCallback on_transaction_removed_;

    // Helper methods
    uint64_t get_current_timestamp() const;
    TransactionPriority calculate_priority(const chainforge::core::Transaction& tx, uint64_t added_time) const;
    bool is_pool_full() const;
    void update_account_nonce(const chainforge::core::Address& address, uint64_t nonce, const chainforge::core::Hash& tx_hash);
    void remove_account_nonce(const chainforge::core::Address& address, uint64_t nonce);
    std::optional<uint64_t> get_account_nonce(const chainforge::core::Address& address) const;
    void rebuild_priority_queue();

    // Validation helpers
    bool validate_basic_properties(const chainforge::core::Transaction& tx) const;
    bool validate_fee(const chainforge::core::Transaction& tx) const;
    bool validate_nonce(const chainforge::core::Transaction& tx) const;
    bool validate_dependencies(const chainforge::core::Transaction& tx) const;
    bool validate_size(const chainforge::core::Transaction& tx) const;

    // Eviction helpers
    void evict_transactions_by_age(uint64_t max_age_seconds);
    void evict_transactions_by_fee(size_t target_count);
    std::vector<chainforge::core::Hash> select_transactions_to_evict(size_t count) const;
};

} // namespace chainforge::mempool
