#include "chainforge/logging/logging.hpp"
#include <thread>
#include <chrono>
#include <iostream>

using namespace chainforge::logging;

void demonstrate_basic_logging() {
    std::cout << "\n=== Basic Logging Demo ===" << std::endl;
    
    auto& logger = get_logger("demo");
    
    logger.trace("This is a trace message");
    logger.debug("This is a debug message");
    logger.info("This is an info message");
    logger.warn("This is a warning message");
    logger.error("This is an error message");
    logger.critical("This is a critical message");
    
    // Template format strings
    logger.info("User {} logged in from IP {}", "john_doe", "192.168.1.100");
    logger.debug("Processing {} items in {:.2f}ms", 1000, 25.4);
}

void demonstrate_structured_logging() {
    std::cout << "\n=== Structured Logging Demo ===" << std::endl;
    
    auto& logger = get_logger("structured");
    
    // Basic structured logging
    LogContext context;
    context.with("user_id", 12345)
           .with("action", "purchase")
           .with("amount", 99.99)
           .with("currency", "USD")
           .with("transaction_id", "tx_123456789");
    
    logger.info("User completed purchase", context);
    
    // Another example with different context
    LogContext error_context;
    error_context.with("error_code", 500)
                 .with("endpoint", "/api/v1/users")
                 .with("method", "POST")
                 .with("user_agent", "Mozilla/5.0")
                 .with("response_time_ms", 1500);
    
    logger.error("API request failed", error_context);
}

void demonstrate_performance_logging() {
    std::cout << "\n=== Performance Logging Demo ===" << std::endl;
    
    auto& logger = get_logger("performance");
    PerformanceMetrics metrics(logger);
    
    // RAII timer example
    {
        CHAINFORGE_PERF_TIMER(logger, "database_operation");
        
        // Simulate database work
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // Add checkpoints
        ScopedTimer timer(logger, "complex_calculation");
        timer.checkpoint("validation_complete");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        timer.checkpoint("processing_complete");
        
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    
    // Manual metrics recording
    metrics.record_duration("api_call", std::chrono::microseconds(2500));
    metrics.record_duration("api_call", std::chrono::microseconds(1800));
    metrics.record_count("cache_hits", 15);
    metrics.record_count("cache_misses", 3);
    metrics.record_memory_usage("heap", 1024 * 1024 * 50); // 50MB
    metrics.record_throughput("requests", 150.5);
    
    // Log summary
    metrics.log_summary();
}

void demonstrate_convenience_macros() {
    std::cout << "\n=== Convenience Macros Demo ===" << std::endl;
    
    CHAINFORGE_LOG_INFO("Application startup complete");
    CHAINFORGE_LOG_DEBUG("Debug info: version={}, build={}", "1.0.0", "abc123");
    CHAINFORGE_LOG_WARN("Resource usage is high: {}%", 85);
    CHAINFORGE_LOG_ERROR("Failed to connect to service: {}", "database");
}

void demonstrate_different_loggers() {
    std::cout << "\n=== Multiple Loggers Demo ===" << std::endl;
    
    auto& auth_logger = get_logger("auth");
    auto& db_logger = get_logger("database");
    auto& api_logger = get_logger("api");
    
    auth_logger.info("User authentication successful");
    db_logger.debug("Executing query: SELECT * FROM users WHERE id = ?");
    api_logger.warn("Rate limit approaching for client IP");
    
    // Performance logger
    auto& perf_logger = get_performance_logger();
    perf_logger.info("System performance metrics recorded");
}

int main() {
    try {
        std::cout << "ChainForge Logging System Demo" << std::endl;
        std::cout << "===============================" << std::endl;
        
        // Initialize logging with defaults
        if (!initialize_logging_with_defaults()) {
            std::cerr << "Failed to initialize logging system" << std::endl;
            return 1;
        }
        
        std::cout << "Logging system initialized successfully" << std::endl;
        
        // Run demonstrations
        demonstrate_basic_logging();
        demonstrate_structured_logging();
        demonstrate_performance_logging();
        demonstrate_convenience_macros();
        demonstrate_different_loggers();
        
        std::cout << "\n=== Demo Complete ===" << std::endl;
        std::cout << "Check the 'logs/chainforge.log' file for JSON formatted output" << std::endl;
        
        // Flush all logs before shutdown
        LogManager::instance().flush_all();
        
        // Shutdown logging system
        LogManager::instance().shutdown();
        
        std::cout << "Logging system shutdown complete" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
