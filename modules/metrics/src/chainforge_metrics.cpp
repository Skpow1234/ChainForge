#include "chainforge/metrics/chainforge_metrics.hpp"
#include "chainforge/metrics/metrics_registry.hpp"
#include "chainforge/metrics/histogram.hpp"

namespace chainforge::metrics {

ChainForgeMetrics& ChainForgeMetrics::instance() {
    static ChainForgeMetrics instance;
    return instance;
}

void ChainForgeMetrics::initialize() {
    if (initialized_) {
        return;
    }
    
    auto& registry = get_metrics_registry();
    
    // === Block Processing Metrics ===
    blocks_processed_total_ = registry.create_counter(
        "chainforge_blocks_processed_total",
        "Total number of blocks processed"
    );
    
    block_processing_duration_ = registry.create_histogram(
        "chainforge_block_processing_duration_seconds",
        "Time spent processing blocks",
        buckets::BLOCK_PROCESSING
    );
    
    current_block_height_ = registry.create_gauge(
        "chainforge_current_block_height",
        "Current blockchain height"
    );
    
    block_size_bytes_ = registry.create_histogram(
        "chainforge_block_size_bytes",
        "Size of blocks in bytes",
        buckets::SIZE_BYTES
    );
    
    // === Transaction Metrics ===
    transactions_processed_total_ = registry.create_counter(
        "chainforge_transactions_processed_total",
        "Total number of transactions processed"
    );
    
    transaction_processing_duration_ = registry.create_histogram(
        "chainforge_transaction_processing_duration_seconds",
        "Time spent processing transactions",
        buckets::TRANSACTION_PROCESSING
    );
    
    mempool_pending_transactions_ = registry.create_gauge(
        "chainforge_mempool_pending_transactions",
        "Number of pending transactions in mempool"
    );
    
    transaction_fees_ = registry.create_histogram(
        "chainforge_transaction_fees",
        "Distribution of transaction fees"
    );
    
    // === P2P Network Metrics ===
    connected_peers_ = registry.create_gauge(
        "chainforge_connected_peers",
        "Number of connected peers"
    );
    
    network_messages_sent_total_ = registry.create_counter(
        "chainforge_network_messages_sent_total",
        "Total number of network messages sent"
    );
    
    network_messages_received_total_ = registry.create_counter(
        "chainforge_network_messages_received_total",
        "Total number of network messages received"
    );
    
    network_bytes_sent_total_ = registry.create_counter(
        "chainforge_network_bytes_sent_total",
        "Total bytes sent over network"
    );
    
    network_bytes_received_total_ = registry.create_counter(
        "chainforge_network_bytes_received_total",
        "Total bytes received over network"
    );
    
    // === Consensus Metrics ===
    consensus_rounds_total_ = registry.create_counter(
        "chainforge_consensus_rounds_total",
        "Total number of consensus rounds"
    );
    
    consensus_duration_ = registry.create_histogram(
        "chainforge_consensus_duration_seconds",
        "Time spent in consensus rounds",
        buckets::DEFAULT_TIMING
    );
    
    active_validators_ = registry.create_gauge(
        "chainforge_active_validators",
        "Number of active validators"
    );
    
    // === Storage Metrics ===
    db_operations_total_ = registry.create_counter(
        "chainforge_db_operations_total",
        "Total number of database operations"
    );
    
    db_operation_duration_ = registry.create_histogram(
        "chainforge_db_operation_duration_seconds",
        "Time spent on database operations",
        buckets::DB_QUERY_DURATION
    );
    
    db_size_bytes_ = registry.create_gauge(
        "chainforge_db_size_bytes",
        "Database size in bytes"
    );
    
    // === RPC Metrics ===
    rpc_requests_total_ = registry.create_counter(
        "chainforge_rpc_requests_total",
        "Total number of RPC requests"
    );
    
    rpc_request_duration_ = registry.create_histogram(
        "chainforge_rpc_request_duration_seconds",
        "Time spent processing RPC requests",
        buckets::HTTP_REQUEST_DURATION
    );
    
    rpc_active_connections_ = registry.create_gauge(
        "chainforge_rpc_active_connections",
        "Number of active RPC connections"
    );
    
    // === System Metrics ===
    memory_usage_bytes_ = registry.create_gauge(
        "chainforge_memory_usage_bytes",
        "Memory usage in bytes"
    );
    
    cpu_usage_percent_ = registry.create_gauge(
        "chainforge_cpu_usage_percent",
        "CPU usage percentage"
    );
    
    uptime_seconds_ = registry.create_gauge(
        "chainforge_uptime_seconds",
        "Application uptime in seconds"
    );
    
    initialized_ = true;
}

ChainForgeMetrics& get_chainforge_metrics() {
    return ChainForgeMetrics::instance();
}

void initialize_chainforge_metrics() {
    ChainForgeMetrics::instance().initialize();
}

} // namespace chainforge::metrics
