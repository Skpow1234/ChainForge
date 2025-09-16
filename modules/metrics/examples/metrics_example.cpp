#include "chainforge/metrics/metrics.hpp"
#include "chainforge/logging/logging.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace chainforge::metrics;
using namespace chainforge::logging;

void demonstrate_basic_metrics() {
    std::cout << "\n=== Basic Metrics Demo ===" << std::endl;
    
    auto& registry = get_metrics_registry();
    
    // Counter example
    auto requests_counter = registry.create_counter(
        "http_requests_total", 
        "Total HTTP requests",
        {{"method", "GET"}, {"status", "200"}}
    );
    
    // Gauge example
    auto active_connections = registry.create_gauge(
        "active_connections",
        "Number of active connections"
    );
    
    // Histogram example
    auto request_duration = registry.create_histogram(
        "http_request_duration_seconds",
        "HTTP request duration",
        buckets::HTTP_REQUEST_DURATION
    );
    
    // Simulate some metrics
    for (int i = 0; i < 10; ++i) {
        requests_counter->increment();
        active_connections->set(i + 1);
        
        // Simulate request processing time
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.001, 0.5);
        request_duration->observe(dis(gen));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    std::cout << "Counter value: " << requests_counter->value() << std::endl;
    std::cout << "Gauge value: " << active_connections->value() << std::endl;
}

void demonstrate_histogram_timer() {
    std::cout << "\n=== Histogram Timer Demo ===" << std::endl;
    
    auto& registry = get_metrics_registry();
    auto processing_time = registry.create_histogram(
        "data_processing_duration_seconds",
        "Time spent processing data"
    );
    
    // Using RAII timer
    {
        HistogramTimer timer(processing_time);
        
        std::cout << "Processing data..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        std::cout << "Checkpoint - elapsed: " << timer.elapsed().count() << " microseconds" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    } // Timer automatically records duration
    
    // Using convenience macro
    {
        CHAINFORGE_HISTOGRAM_TIMER("macro_processing_time");
        std::cout << "Processing with macro timer..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}

void demonstrate_gauge_tracker() {
    std::cout << "\n=== Gauge Tracker Demo ===" << std::endl;
    
    auto& registry = get_metrics_registry();
    auto memory_usage = registry.create_gauge("memory_usage_mb", "Memory usage in MB");
    
    std::cout << "Initial memory usage: " << memory_usage->value() << " MB" << std::endl;
    
    {
        GaugeTracker tracker(memory_usage, 50.0);
        std::cout << "Allocated 50MB, current usage: " << memory_usage->value() << " MB" << std::endl;
        
        tracker.update(75.0);
        std::cout << "Reallocated to 75MB, current usage: " << memory_usage->value() << " MB" << std::endl;
    }
    
    std::cout << "Memory freed, current usage: " << memory_usage->value() << " MB" << std::endl;
}

void demonstrate_chainforge_metrics() {
    std::cout << "\n=== ChainForge Metrics Demo ===" << std::endl;
    
    initialize_chainforge_metrics();
    auto& metrics = get_chainforge_metrics();
    
    // Simulate blockchain operations
    for (int block = 1; block <= 5; ++block) {
        std::cout << "Processing block " << block << "..." << std::endl;
        
        {
            HistogramTimer timer(metrics.block_processing_duration());
            
            // Simulate block processing
            metrics.current_block_height()->set(block);
            metrics.block_size_bytes()->observe(1024 * (100 + block * 50)); // Varying block sizes
            
            // Simulate transactions in block
            int tx_count = 5 + block * 2;
            for (int tx = 0; tx < tx_count; ++tx) {
                metrics.transactions_processed_total()->increment();
                
                // Simulate transaction processing time
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<> dis(0.001, 0.01);
                metrics.transaction_processing_duration()->observe(dis(gen));
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        
        metrics.blocks_processed_total()->increment();
    }
    
    // Simulate network activity
    metrics.connected_peers()->set(15);
    metrics.network_messages_sent_total()->increment(100);
    metrics.network_messages_received_total()->increment(120);
    
    // Simulate consensus
    metrics.active_validators()->set(21);
    metrics.consensus_rounds_total()->increment();
    
    std::cout << "Blocks processed: " << metrics.blocks_processed_total()->value() << std::endl;
    std::cout << "Current block height: " << metrics.current_block_height()->value() << std::endl;
    std::cout << "Transactions processed: " << metrics.transactions_processed_total()->value() << std::endl;
}

void demonstrate_metrics_logging_integration() {
    std::cout << "\n=== Metrics + Logging Integration Demo ===" << std::endl;
    
    // Initialize logging
    if (!initialize_logging_with_defaults()) {
        std::cerr << "Failed to initialize logging" << std::endl;
        return;
    }
    
    // Create metrics-aware logger
    auto metrics_logger = create_metrics_logger("integration_demo");
    
    // Log some messages - metrics will be automatically recorded
    metrics_logger->info("Application started");
    metrics_logger->debug("Debug information");
    metrics_logger->warn("Warning message");
    metrics_logger->error("Error occurred");
    
    std::cout << "Logged 4 messages with automatic metrics collection" << std::endl;
    
    // Use convenience macros
    CHAINFORGE_COUNTER_INC("custom_events");
    CHAINFORGE_GAUGE_SET("system_load", 0.75);
    
    LogManager::instance().shutdown();
}

void demonstrate_convenience_macros() {
    std::cout << "\n=== Convenience Macros Demo ===" << std::endl;
    
    // Counter macros
    CHAINFORGE_COUNTER_INC("api_calls");
    CHAINFORGE_COUNTER_ADD("bytes_processed", 1024);
    
    // Gauge macros
    CHAINFORGE_GAUGE_SET("cpu_usage", 65.5);
    CHAINFORGE_GAUGE_INC("active_sessions");
    CHAINFORGE_GAUGE_DEC("active_sessions");
    
    // Histogram timer macro
    {
        CHAINFORGE_HISTOGRAM_TIMER("operation_duration");
        std::cout << "Performing timed operation..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    
    std::cout << "Macros demonstration complete" << std::endl;
}

int main() {
    try {
        std::cout << "ChainForge Metrics System Demo" << std::endl;
        std::cout << "===============================" << std::endl;
        
        // Start metrics server
        MetricsServerConfig config;
        config.host = "127.0.0.1";
        config.port = 18080;
        
        if (start_metrics_server(config)) {
            std::cout << "Metrics server started at: " << get_metrics_url() << std::endl;
        } else {
            std::cout << "Failed to start metrics server, continuing with demo..." << std::endl;
        }
        
        // Run demonstrations
        demonstrate_basic_metrics();
        demonstrate_histogram_timer();
        demonstrate_gauge_tracker();
        demonstrate_chainforge_metrics();
        demonstrate_metrics_logging_integration();
        demonstrate_convenience_macros();
        
        std::cout << "\n=== Demo Complete ===" << std::endl;
        std::cout << "Total metrics created: " << get_metrics_registry().metrics_count() << std::endl;
        
        if (is_metrics_server_running()) {
            std::cout << "Visit " << get_metrics_url() << " to see Prometheus metrics" << std::endl;
            std::cout << "Press Enter to stop metrics server and exit...";
            std::cin.get();
        }
        
        // Stop metrics server
        stop_metrics_server();
        std::cout << "Metrics server stopped" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
