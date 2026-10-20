#include "tokiwa/metrics/tokiwa_metrics.hpp"
#include "tokiwa/metrics/metrics_registry.hpp"
#include "tokiwa/metrics/histogram.hpp"

namespace tokiwa::metrics {

TokiwaMetrics& TokiwaMetrics::instance() {
    static TokiwaMetrics instance;
    return instance;
}

void TokiwaMetrics::initialize() {
    if (initialized_) {
        return;
    }
    
    auto& registry = get_metrics_registry();
    
    // === Block Processing Metrics ===
    blocks_processed_total_ = registry.create_counter(
        "tokiwa_blocks_processed_total",
        "Total number of blocks processed"
    );
    
    block_processing_duration_ = registry.create_histogram(
        "tokiwa_block_processing_duration_seconds",
        "Time spent processing blocks",
        buckets::BLOCK_PROCESSING
    );
    
    current_block_height_ = registry.create_gauge(
        "tokiwa_current_block_height",
        "Current blockchain height"
    );
    
    block_size_bytes_ = registry.create_histogram(
        "tokiwa_block_size_bytes",
        "Size of blocks in bytes",
        buckets::SIZE_BYTES
    );
    
    // === Transaction Metrics ===
    transactions_processed_total_ = registry.create_counter(
        "tokiwa_transactions_processed_total",
        "Total number of transactions processed"
    );
    
    transaction_processing_duration_ = registry.create_histogram(
        "tokiwa_transaction_processing_duration_seconds",
        "Time spent processing transactions",
        buckets::TRANSACTION_PROCESSING
    );
    
    mempool_pending_transactions_ = registry.create_gauge(
        "tokiwa_mempool_pending_transactions",
        "Number of pending transactions in mempool"
    );
    
    transaction_fees_ = registry.create_histogram(
        "tokiwa_transaction_fees",
        "Distribution of transaction fees"
    );
    
    // === P2P Network Metrics ===
    connected_peers_ = registry.create_gauge(
        "tokiwa_connected_peers",
        "Number of connected peers"
    );
    
    network_messages_sent_total_ = registry.create_counter(
        "tokiwa_network_messages_sent_total",
        "Total number of network messages sent"
    );
    
    network_messages_received_total_ = registry.create_counter(
        "tokiwa_network_messages_received_total",
        "Total number of network messages received"
    );
    
    network_bytes_sent_total_ = registry.create_counter(
        "tokiwa_network_bytes_sent_total",
        "Total bytes sent over network"
    );
    
    network_bytes_received_total_ = registry.create_counter(
        "tokiwa_network_bytes_received_total",
        "Total bytes received over network"
    );
    
    // === Consensus Metrics ===
    consensus_rounds_total_ = registry.create_counter(
        "tokiwa_consensus_rounds_total",
        "Total number of consensus rounds"
    );
    
    consensus_duration_ = registry.create_histogram(
        "tokiwa_consensus_duration_seconds",
        "Time spent in consensus rounds",
        buckets::DEFAULT_TIMING
    );
    
    active_validators_ = registry.create_gauge(
        "tokiwa_active_validators",
        "Number of active validators"
    );
    
    // === Storage Metrics ===
    db_operations_total_ = registry.create_counter(
        "tokiwa_db_operations_total",
        "Total number of database operations"
    );
    
    db_operation_duration_ = registry.create_histogram(
        "tokiwa_db_operation_duration_seconds",
        "Time spent on database operations",
        buckets::DB_QUERY_DURATION
    );
    
    db_size_bytes_ = registry.create_gauge(
        "tokiwa_db_size_bytes",
        "Database size in bytes"
    );
    
    // === RPC Metrics ===
    rpc_requests_total_ = registry.create_counter(
        "tokiwa_rpc_requests_total",
        "Total number of RPC requests"
    );
    
    rpc_request_duration_ = registry.create_histogram(
        "tokiwa_rpc_request_duration_seconds",
        "Time spent processing RPC requests",
        buckets::HTTP_REQUEST_DURATION
    );
    
    rpc_active_connections_ = registry.create_gauge(
        "tokiwa_rpc_active_connections",
        "Number of active RPC connections"
    );
    
    // === System Metrics ===
    memory_usage_bytes_ = registry.create_gauge(
        "tokiwa_memory_usage_bytes",
        "Memory usage in bytes"
    );
    
    cpu_usage_percent_ = registry.create_gauge(
        "tokiwa_cpu_usage_percent",
        "CPU usage percentage"
    );
    
    uptime_seconds_ = registry.create_gauge(
        "tokiwa_uptime_seconds",
        "Application uptime in seconds"
    );
    
    initialized_ = true;
}

TokiwaMetrics& get_tokiwa_metrics() {
    return TokiwaMetrics::instance();
}

void initialize_tokiwa_metrics() {
    TokiwaMetrics::instance().initialize();
}

} // namespace tokiwa::metrics
