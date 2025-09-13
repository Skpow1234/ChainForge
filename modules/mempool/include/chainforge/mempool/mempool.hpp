#pragma once

#include "chainforge/core/transaction.hpp"
#include "chainforge/core/hash.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <mutex>

namespace chainforge::mempool {

/**
 * Mempool error codes
 */
enum class MempoolError {
    SUCCESS = 0,
    TRANSACTION_EXISTS = 1,
    INVALID_TRANSACTION = 2,
    INSUFFICIENT_FEE = 3,
    POOL_FULL = 4,
    NONCE_TOO_LOW = 5,
    NONCE_TOO_HIGH = 6,
    REPLACE_UNDERPRICED = 7,
    DEPENDENCY_MISSING = 8
};

/**
 * Transaction priority based on fee and age
 */
struct TransactionPriority {
    double fee_per_gas = 0.0;
    uint64_t age_seconds = 0;
    uint64_t size_bytes = 0;

    double calculate_score() const {
        // Score = (fee_per_gas * age_bonus) / size_penalty
        double age_bonus = 1.0 + (static_cast<double>(age_seconds) / 3600.0); // 1 hour bonus
        double size_penalty = static_cast<double>(size_bytes) / 1000.0; // Size in KB
        return (fee_per_gas * age_bonus) / std::max(size_penalty, 1.0);
    }

    bool operator<(const TransactionPriority& other) const {
        return calculate_score() < other.calculate_score();
    }
};

/**
 * Mempool configuration
 */
struct MempoolConfig {
    size_t max_size_bytes = 100 * 1024 * 1024;  // 100MB
    size_t max_transactions = 10000;            // Max transaction count
    uint64_t min_fee_per_gas = 1;               // Minimum fee per gas
    uint64_t max_fee_per_gas = 1000000;         // Maximum fee per gas
    uint64_t eviction_interval_seconds = 60;    // Eviction check interval
    double eviction_threshold_ratio = 0.9;      // Evict when 90% full
};

/**
 * Transaction entry in the mempool
 */
struct MempoolEntry {
    chainforge::core::Transaction transaction;
    TransactionPriority priority;
    uint64_t added_timestamp;
    uint64_t last_seen_timestamp;
    std::vector<chainforge::core::Hash> dependencies;  // Transaction hashes this depends on

    bool is_expired(uint64_t current_time, uint64_t max_age_seconds = 3600) const {
        return (current_time - added_timestamp) > max_age_seconds;
    }
};

/**
 * Mempool statistics
 */
struct MempoolStats {
    size_t transaction_count = 0;
    size_t total_size_bytes = 0;
    uint64_t min_fee_per_gas = 0;
    uint64_t max_fee_per_gas = 0;
    double avg_fee_per_gas = 0.0;
    uint64_t oldest_transaction_age = 0;
};

/**
 * Transaction pool interface
 */
class Mempool {
public:
    virtual ~Mempool() = default;

    // Configuration
    virtual void set_config(const MempoolConfig& config) = 0;
    virtual const MempoolConfig& get_config() const = 0;

    // Transaction management
    virtual MempoolError add_transaction(const chainforge::core::Transaction& transaction) = 0;
    virtual MempoolError remove_transaction(const chainforge::core::Hash& tx_hash) = 0;
    virtual std::optional<chainforge::core::Transaction> get_transaction(const chainforge::core::Hash& tx_hash) const = 0;
    virtual bool has_transaction(const chainforge::core::Hash& tx_hash) const = 0;

    // RBF (Replace-By-Fee) support
    virtual MempoolError replace_transaction(const chainforge::core::Transaction& new_transaction) = 0;

    // Batch operations
    virtual std::vector<chainforge::core::Transaction> get_top_transactions(size_t count) = 0;
    virtual std::vector<chainforge::core::Transaction> get_transactions_for_block(size_t max_count, uint64_t max_gas_limit) = 0;
    virtual std::vector<chainforge::core::Hash> get_all_transaction_hashes() const = 0;

    // Maintenance
    virtual void evict_expired_transactions() = 0;
    virtual void evict_low_fee_transactions() = 0;
    virtual void clear() = 0;

    // Statistics
    virtual MempoolStats get_stats() const = 0;

    // Validation
    virtual bool validate_transaction(const chainforge::core::Transaction& transaction) const = 0;
    virtual MempoolError check_replacement_policy(const chainforge::core::Transaction& old_tx,
                                                 const chainforge::core::Transaction& new_tx) const = 0;

    // Callbacks
    using TransactionAddedCallback = std::function<void(const chainforge::core::Hash&)>;
    using TransactionRemovedCallback = std::function<void(const chainforge::core::Hash&)>;

    virtual void set_transaction_added_callback(TransactionAddedCallback callback) = 0;
    virtual void set_transaction_removed_callback(TransactionRemovedCallback callback) = 0;
};

/**
 * Mempool factory function
 */
std::unique_ptr<Mempool> create_mempool(const MempoolConfig& config = {});

/**
 * Mempool error to string conversion
 */
std::string mempool_error_to_string(MempoolError error);

/**
 * Transaction selection strategies
 */
enum class SelectionStrategy {
    HIGHEST_FEE_FIRST,
    OLDEST_FIRST,
    PRIORITY_SCORE
};

std::vector<chainforge::core::Transaction> select_transactions_for_block(
    const Mempool& mempool,
    SelectionStrategy strategy,
    size_t max_count,
    uint64_t max_gas_limit
);

} // namespace chainforge::mempool
