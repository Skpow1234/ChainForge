#pragma once

#include "counter.hpp"
#include "gauge.hpp"
#include "histogram.hpp"
#include <memory>
#include <string>

namespace chainforge::metrics {

/// ChainForge-specific metrics collection
/// Provides domain-specific metrics for blockchain operations
class ChainForgeMetrics {
public:
    /// Get singleton instance
    static ChainForgeMetrics& instance();
    
    /// Initialize all ChainForge metrics
    void initialize();
    
    /// Destructor
    ~ChainForgeMetrics() = default;
    
    // Disable copy and move
    ChainForgeMetrics(const ChainForgeMetrics&) = delete;
    ChainForgeMetrics& operator=(const ChainForgeMetrics&) = delete;
    ChainForgeMetrics(ChainForgeMetrics&&) = delete;
    ChainForgeMetrics& operator=(ChainForgeMetrics&&) = delete;
    
    // === Block Processing Metrics ===
    
    /// Total blocks processed
    std::shared_ptr<Counter> blocks_processed_total() const { return blocks_processed_total_; }
    
    /// Block processing duration
    std::shared_ptr<Histogram> block_processing_duration() const { return block_processing_duration_; }
    
    /// Current block height
    std::shared_ptr<Gauge> current_block_height() const { return current_block_height_; }
    
    /// Block size in bytes
    std::shared_ptr<Histogram> block_size_bytes() const { return block_size_bytes_; }
    
    // === Transaction Metrics ===
    
    /// Total transactions processed
    std::shared_ptr<Counter> transactions_processed_total() const { return transactions_processed_total_; }
    
    /// Transaction processing duration
    std::shared_ptr<Histogram> transaction_processing_duration() const { return transaction_processing_duration_; }
    
    /// Pending transactions in mempool
    std::shared_ptr<Gauge> mempool_pending_transactions() const { return mempool_pending_transactions_; }
    
    /// Transaction fee distribution
    std::shared_ptr<Histogram> transaction_fees() const { return transaction_fees_; }
    
    // === P2P Network Metrics ===
    
    /// Connected peers
    std::shared_ptr<Gauge> connected_peers() const { return connected_peers_; }
    
    /// Total network messages sent
    std::shared_ptr<Counter> network_messages_sent_total() const { return network_messages_sent_total_; }
    
    /// Total network messages received
    std::shared_ptr<Counter> network_messages_received_total() const { return network_messages_received_total_; }
    
    /// Network bandwidth usage
    std::shared_ptr<Counter> network_bytes_sent_total() const { return network_bytes_sent_total_; }
    std::shared_ptr<Counter> network_bytes_received_total() const { return network_bytes_received_total_; }
    
    // === Consensus Metrics ===
    
    /// Consensus rounds
    std::shared_ptr<Counter> consensus_rounds_total() const { return consensus_rounds_total_; }
    
    /// Consensus duration
    std::shared_ptr<Histogram> consensus_duration() const { return consensus_duration_; }
    
    /// Validator participation
    std::shared_ptr<Gauge> active_validators() const { return active_validators_; }
    
    // === Storage Metrics ===
    
    /// Database operations
    std::shared_ptr<Counter> db_operations_total() const { return db_operations_total_; }
    
    /// Database operation duration
    std::shared_ptr<Histogram> db_operation_duration() const { return db_operation_duration_; }
    
    /// Database size
    std::shared_ptr<Gauge> db_size_bytes() const { return db_size_bytes_; }
    
    // === RPC Metrics ===
    
    /// RPC requests
    std::shared_ptr<Counter> rpc_requests_total() const { return rpc_requests_total_; }
    
    /// RPC request duration
    std::shared_ptr<Histogram> rpc_request_duration() const { return rpc_request_duration_; }
    
    /// Active RPC connections
    std::shared_ptr<Gauge> rpc_active_connections() const { return rpc_active_connections_; }
    
    // === System Metrics ===
    
    /// Memory usage
    std::shared_ptr<Gauge> memory_usage_bytes() const { return memory_usage_bytes_; }
    
    /// CPU usage percentage
    std::shared_ptr<Gauge> cpu_usage_percent() const { return cpu_usage_percent_; }
    
    /// Application uptime
    std::shared_ptr<Gauge> uptime_seconds() const { return uptime_seconds_; }

private:
    ChainForgeMetrics() = default;
    
    bool initialized_ = false;
    
    // Block metrics
    std::shared_ptr<Counter> blocks_processed_total_;
    std::shared_ptr<Histogram> block_processing_duration_;
    std::shared_ptr<Gauge> current_block_height_;
    std::shared_ptr<Histogram> block_size_bytes_;
    
    // Transaction metrics
    std::shared_ptr<Counter> transactions_processed_total_;
    std::shared_ptr<Histogram> transaction_processing_duration_;
    std::shared_ptr<Gauge> mempool_pending_transactions_;
    std::shared_ptr<Histogram> transaction_fees_;
    
    // P2P metrics
    std::shared_ptr<Gauge> connected_peers_;
    std::shared_ptr<Counter> network_messages_sent_total_;
    std::shared_ptr<Counter> network_messages_received_total_;
    std::shared_ptr<Counter> network_bytes_sent_total_;
    std::shared_ptr<Counter> network_bytes_received_total_;
    
    // Consensus metrics
    std::shared_ptr<Counter> consensus_rounds_total_;
    std::shared_ptr<Histogram> consensus_duration_;
    std::shared_ptr<Gauge> active_validators_;
    
    // Storage metrics
    std::shared_ptr<Counter> db_operations_total_;
    std::shared_ptr<Histogram> db_operation_duration_;
    std::shared_ptr<Gauge> db_size_bytes_;
    
    // RPC metrics
    std::shared_ptr<Counter> rpc_requests_total_;
    std::shared_ptr<Histogram> rpc_request_duration_;
    std::shared_ptr<Gauge> rpc_active_connections_;
    
    // System metrics
    std::shared_ptr<Gauge> memory_usage_bytes_;
    std::shared_ptr<Gauge> cpu_usage_percent_;
    std::shared_ptr<Gauge> uptime_seconds_;
};

/// Convenience function to get ChainForge metrics
ChainForgeMetrics& get_chainforge_metrics();

/// Initialize all ChainForge metrics
void initialize_chainforge_metrics();

} // namespace chainforge::metrics
