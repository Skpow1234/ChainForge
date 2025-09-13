#include "mempool_impl.hpp"
#include "chainforge/core/address.hpp"
#include <algorithm>
#include <numeric>

namespace chainforge::mempool {

MempoolImpl::MempoolImpl(const MempoolConfig& config) : config_(config) {}

void MempoolImpl::set_config(const MempoolConfig& config) {
    std::unique_lock lock(mutex_);
    config_ = config;
}

const MempoolConfig& MempoolImpl::get_config() const {
    std::shared_lock lock(mutex_);
    return config_;
}

MempoolError MempoolImpl::add_transaction(const chainforge::core::Transaction& transaction) {
    std::unique_lock lock(mutex_);

    auto tx_hash = transaction.calculate_hash();

    // Check if transaction already exists
    if (transactions_.find(tx_hash) != transactions_.end()) {
        return MempoolError::TRANSACTION_EXISTS;
    }

    // Validate transaction
    if (!validate_transaction(transaction)) {
        return MempoolError::INVALID_TRANSACTION;
    }

    // Check pool capacity
    if (is_pool_full()) {
        evict_low_fee_transactions();
        if (is_pool_full()) {
            return MempoolError::POOL_FULL;
        }
    }

    // Create mempool entry
    uint64_t current_time = get_current_timestamp();
    MempoolEntry entry{
        transaction,
        calculate_priority(transaction, current_time),
        current_time,
        current_time,
        {}  // No dependencies for now
    };

    // Update account nonce tracking
    update_account_nonce(transaction.from(), transaction.nonce(), tx_hash);

    // Add to storage
    transactions_[tx_hash] = std::move(entry);
    priority_queue_.emplace(entry.priority.calculate_score(), tx_hash);

    // Notify callback
    if (on_transaction_added_) {
        lock.unlock();  // Unlock before calling callback
        on_transaction_added_(tx_hash);
    }

    return MempoolError::SUCCESS;
}

MempoolError MempoolImpl::remove_transaction(const chainforge::core::Hash& tx_hash) {
    std::unique_lock lock(mutex_);

    auto it = transactions_.find(tx_hash);
    if (it == transactions_.end()) {
        return MempoolError::INVALID_TRANSACTION;
    }

    const auto& entry = it->second;

    // Update account nonce tracking
    remove_account_nonce(entry.transaction.from(), entry.transaction.nonce());

    // Remove from storage
    transactions_.erase(it);

    // Rebuild priority queue (inefficient but correct)
    rebuild_priority_queue();

    // Notify callback
    if (on_transaction_removed_) {
        lock.unlock();  // Unlock before calling callback
        on_transaction_removed_(tx_hash);
    }

    return MempoolError::SUCCESS;
}

std::optional<chainforge::core::Transaction> MempoolImpl::get_transaction(const chainforge::core::Hash& tx_hash) const {
    std::shared_lock lock(mutex_);

    auto it = transactions_.find(tx_hash);
    if (it == transactions_.end()) {
        return std::nullopt;
    }

    return it->second.transaction;
}

bool MempoolImpl::has_transaction(const chainforge::core::Hash& tx_hash) const {
    std::shared_lock lock(mutex_);
    return transactions_.find(tx_hash) != transactions_.end();
}

MempoolError MempoolImpl::replace_transaction(const chainforge::core::Transaction& new_transaction) {
    std::unique_lock lock(mutex_);

    auto new_hash = new_transaction.calculate_hash();
    auto existing_hash = new_transaction.calculate_hash();  // Same hash for same tx

    // Find existing transaction by sender and nonce
    auto account_it = account_nonces_.find(new_transaction.from());
    if (account_it == account_nonces_.end()) {
        return MempoolError::INVALID_TRANSACTION;
    }

    auto nonce_it = account_it->second.find(new_transaction.nonce());
    if (nonce_it == account_it->second.end()) {
        return MempoolError::INVALID_TRANSACTION;
    }

    const auto& old_tx_hash = nonce_it->second;
    auto old_tx_it = transactions_.find(old_tx_hash);
    if (old_tx_it == transactions_.end()) {
        return MempoolError::INVALID_TRANSACTION;
    }

    const auto& old_tx = old_tx_it->second.transaction;

    // Check replacement policy
    auto replacement_error = check_replacement_policy(old_tx, new_transaction);
    if (replacement_error != MempoolError::SUCCESS) {
        return replacement_error;
    }

    // Remove old transaction
    remove_account_nonce(old_tx.from(), old_tx.nonce());
    transactions_.erase(old_tx_it);

    // Validate and add new transaction
    if (!validate_transaction(new_transaction)) {
        return MempoolError::INVALID_TRANSACTION;
    }

    uint64_t current_time = get_current_timestamp();
    MempoolEntry entry{
        new_transaction,
        calculate_priority(new_transaction, current_time),
        current_time,
        current_time,
        {}
    };

    update_account_nonce(new_transaction.from(), new_transaction.nonce(), new_hash);
    transactions_[new_hash] = std::move(entry);
    rebuild_priority_queue();

    return MempoolError::SUCCESS;
}

std::vector<chainforge::core::Transaction> MempoolImpl::get_top_transactions(size_t count) {
    std::shared_lock lock(mutex_);

    std::vector<chainforge::core::Transaction> result;
    result.reserve(std::min(count, transactions_.size()));

    // Copy priority queue to avoid modifying the original
    auto temp_queue = priority_queue_;

    while (!temp_queue.empty() && result.size() < count) {
        auto [score, tx_hash] = temp_queue.top();
        temp_queue.pop();

        auto it = transactions_.find(tx_hash);
        if (it != transactions_.end()) {
            result.push_back(it->second.transaction);
        }
    }

    return result;
}

std::vector<chainforge::core::Transaction> MempoolImpl::get_transactions_for_block(size_t max_count, uint64_t max_gas_limit) {
    std::shared_lock lock(mutex_);

    std::vector<chainforge::core::Transaction> result;
    uint64_t total_gas = 0;

    // Copy priority queue to avoid modifying the original
    auto temp_queue = priority_queue_;

    while (!temp_queue.empty() && result.size() < max_count) {
        auto [score, tx_hash] = temp_queue.top();
        temp_queue.pop();

        auto it = transactions_.find(tx_hash);
        if (it != transactions_.end()) {
            const auto& tx = it->second.transaction;

            // Check gas limit
            if (total_gas + tx.gas_limit() <= max_gas_limit) {
                result.push_back(tx);
                total_gas += tx.gas_limit();
            }
        }
    }

    return result;
}

std::vector<chainforge::core::Hash> MempoolImpl::get_all_transaction_hashes() const {
    std::shared_lock lock(mutex_);

    std::vector<chainforge::core::Hash> hashes;
    hashes.reserve(transactions_.size());

    for (const auto& [hash, entry] : transactions_) {
        hashes.push_back(hash);
    }

    return hashes;
}

void MempoolImpl::evict_expired_transactions() {
    std::unique_lock lock(mutex_);
    evict_transactions_by_age(3600);  // 1 hour
}

void MempoolImpl::evict_low_fee_transactions() {
    std::unique_lock lock(mutex_);

    if (transactions_.size() < config_.max_transactions * config_.eviction_threshold_ratio) {
        return;  // Not full enough to evict
    }

    size_t target_count = static_cast<size_t>(config_.max_transactions * 0.8);  // Target 80% capacity
    evict_transactions_by_fee(transactions_.size() - target_count);
}

void MempoolImpl::clear() {
    std::unique_lock lock(mutex_);

    transactions_.clear();
    account_nonces_.clear();

    while (!priority_queue_.empty()) {
        priority_queue_.pop();
    }
}

MempoolStats MempoolImpl::get_stats() const {
    std::shared_lock lock(mutex_);

    MempoolStats stats;
    stats.transaction_count = transactions_.size();

    if (transactions_.empty()) {
        return stats;
    }

    uint64_t current_time = get_current_timestamp();
    uint64_t total_fee = 0;
    uint64_t min_age = UINT64_MAX;

    for (const auto& [hash, entry] : transactions_) {
        stats.total_size_bytes += entry.transaction.size();
        total_fee += entry.transaction.gas_price();
        min_age = std::min(min_age, current_time - entry.added_timestamp);

        if (stats.min_fee_per_gas == 0 || entry.transaction.gas_price() < stats.min_fee_per_gas) {
            stats.min_fee_per_gas = entry.transaction.gas_price();
        }
        if (entry.transaction.gas_price() > stats.max_fee_per_gas) {
            stats.max_fee_per_gas = entry.transaction.gas_price();
        }
    }

    stats.avg_fee_per_gas = static_cast<double>(total_fee) / transactions_.size();
    stats.oldest_transaction_age = min_age;

    return stats;
}

bool MempoolImpl::validate_transaction(const chainforge::core::Transaction& transaction) const {
    return validate_basic_properties(transaction) &&
           validate_fee(transaction) &&
           validate_nonce(transaction) &&
           validate_dependencies(transaction) &&
           validate_size(transaction);
}

MempoolError MempoolImpl::check_replacement_policy(const chainforge::core::Transaction& old_tx,
                                                  const chainforge::core::Transaction& new_tx) const {
    // Simple replacement policy: new fee must be at least 10% higher
    uint64_t old_fee_rate = old_tx.gas_price();
    uint64_t new_fee_rate = new_tx.gas_price();

    if (new_fee_rate < old_fee_rate * 1.1) {
        return MempoolError::REPLACE_UNDERPRICED;
    }

    return MempoolError::SUCCESS;
}

void MempoolImpl::set_transaction_added_callback(TransactionAddedCallback callback) {
    std::unique_lock lock(mutex_);
    on_transaction_added_ = std::move(callback);
}

void MempoolImpl::set_transaction_removed_callback(TransactionRemovedCallback callback) {
    std::unique_lock lock(mutex_);
    on_transaction_removed_ = std::move(callback);
}

// Helper methods implementation
uint64_t MempoolImpl::get_current_timestamp() const {
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );
}

TransactionPriority MempoolImpl::calculate_priority(const chainforge::core::Transaction& tx, uint64_t added_time) const {
    uint64_t current_time = get_current_timestamp();
    return TransactionPriority{
        static_cast<double>(tx.gas_price()),
        current_time - added_time,
        tx.size()
    };
}

bool MempoolImpl::is_pool_full() const {
    return transactions_.size() >= config_.max_transactions ||
           get_stats().total_size_bytes >= config_.max_size_bytes;
}

void MempoolImpl::update_account_nonce(const chainforge::core::Address& address, uint64_t nonce, const chainforge::core::Hash& tx_hash) {
    account_nonces_[address][nonce] = tx_hash;
}

void MempoolImpl::remove_account_nonce(const chainforge::core::Address& address, uint64_t nonce) {
    auto account_it = account_nonces_.find(address);
    if (account_it != account_nonces_.end()) {
        account_it->second.erase(nonce);
        if (account_it->second.empty()) {
            account_nonces_.erase(account_it);
        }
    }
}

std::optional<uint64_t> MempoolImpl::get_account_nonce(const chainforge::core::Address& address) const {
    auto account_it = account_nonces_.find(address);
    if (account_it == account_nonces_.end() || account_it->second.empty()) {
        return std::nullopt;
    }

    // Return the highest nonce
    auto nonce_it = std::max_element(account_it->second.begin(), account_it->second.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    return nonce_it->first;
}

void MempoolImpl::rebuild_priority_queue() {
    while (!priority_queue_.empty()) {
        priority_queue_.pop();
    }

    for (const auto& [hash, entry] : transactions_) {
        priority_queue_.emplace(entry.priority.calculate_score(), hash);
    }
}

bool MempoolImpl::validate_basic_properties(const chainforge::core::Transaction& tx) const {
    return tx.is_valid();
}

bool MempoolImpl::validate_fee(const chainforge::core::Transaction& tx) const {
    return tx.gas_price() >= config_.min_fee_per_gas &&
           tx.gas_price() <= config_.max_fee_per_gas;
}

bool MempoolImpl::validate_nonce(const chainforge::core::Transaction& tx) const {
    // Check if nonce conflicts with existing transactions from same sender
    auto existing_nonce = get_account_nonce(tx.from());
    if (existing_nonce && *existing_nonce >= tx.nonce()) {
        return false;  // Nonce too low
    }
    return true;
}

bool MempoolImpl::validate_dependencies(const chainforge::core::Transaction& tx) const {
    // For now, no dependency validation
    return true;
}

bool MempoolImpl::validate_size(const chainforge::core::Transaction& tx) const {
    return !tx.is_too_large();
}

void MempoolImpl::evict_transactions_by_age(uint64_t max_age_seconds) {
    uint64_t current_time = get_current_timestamp();
    std::vector<chainforge::core::Hash> to_remove;

    for (const auto& [hash, entry] : transactions_) {
        if (entry.is_expired(current_time, max_age_seconds)) {
            to_remove.push_back(hash);
        }
    }

    for (const auto& hash : to_remove) {
        remove_transaction(hash);
    }
}

void MempoolImpl::evict_transactions_by_fee(size_t count) {
    auto to_evict = select_transactions_to_evict(count);

    for (const auto& hash : to_evict) {
        remove_transaction(hash);
    }
}

std::vector<chainforge::core::Hash> MempoolImpl::select_transactions_to_evict(size_t count) const {
    std::vector<std::pair<double, chainforge::core::Hash>> candidates;

    for (const auto& [hash, entry] : transactions_) {
        candidates.emplace_back(entry.priority.calculate_score(), hash);
    }

    // Sort by score (ascending - lowest priority first)
    std::sort(candidates.begin(), candidates.end());

    std::vector<chainforge::core::Hash> to_evict;
    to_evict.reserve(std::min(count, candidates.size()));

    for (size_t i = 0; i < std::min(count, candidates.size()); ++i) {
        to_evict.push_back(candidates[i].second);
    }

    return to_evict;
}

// Factory function
std::unique_ptr<Mempool> create_mempool(const MempoolConfig& config) {
    return std::make_unique<MempoolImpl>(config);
}

// Error conversion
std::string mempool_error_to_string(MempoolError error) {
    switch (error) {
        case MempoolError::SUCCESS: return "Success";
        case MempoolError::TRANSACTION_EXISTS: return "Transaction already exists";
        case MempoolError::INVALID_TRANSACTION: return "Invalid transaction";
        case MempoolError::INSUFFICIENT_FEE: return "Insufficient fee";
        case MempoolError::POOL_FULL: return "Pool is full";
        case MempoolError::NONCE_TOO_LOW: return "Nonce too low";
        case MempoolError::NONCE_TOO_HIGH: return "Nonce too high";
        case MempoolError::REPLACE_UNDERPRICED: return "Replacement underpriced";
        case MempoolError::DEPENDENCY_MISSING: return "Dependency missing";
        default: return "Unknown error";
    }
}

// Transaction selection
std::vector<chainforge::core::Transaction> select_transactions_for_block(
    const Mempool& mempool,
    SelectionStrategy strategy,
    size_t max_count,
    uint64_t max_gas_limit
) {
    switch (strategy) {
        case SelectionStrategy::HIGHEST_FEE_FIRST:
        case SelectionStrategy::PRIORITY_SCORE:
            return mempool.get_transactions_for_block(max_count, max_gas_limit);
        case SelectionStrategy::OLDEST_FIRST:
            // For oldest first, we would need a different implementation
            return mempool.get_transactions_for_block(max_count, max_gas_limit);
        default:
            return {};
    }
}

} // namespace chainforge::mempool
