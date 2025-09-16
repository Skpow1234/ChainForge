#pragma once

#include "logger.hpp"
#include <chrono>
#include <string>
#include <memory>

namespace chainforge::logging {

/// RAII timer for performance logging
class ScopedTimer {
public:
    /// Start timer with given logger and operation name
    ScopedTimer(Logger& logger, const std::string& operation, LogLevel level = LogLevel::DEBUG);
    
    /// Destructor logs the elapsed time
    ~ScopedTimer();
    
    // Disable copy and move
    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;
    ScopedTimer(ScopedTimer&&) = delete;
    ScopedTimer& operator=(ScopedTimer&&) = delete;
    
    /// Get elapsed time so far
    std::chrono::microseconds elapsed() const;
    
    /// Log intermediate checkpoint
    void checkpoint(const std::string& checkpoint_name);

private:
    Logger& logger_;
    std::string operation_;
    LogLevel level_;
    std::chrono::steady_clock::time_point start_time_;
};

/// Performance metrics collector
class PerformanceMetrics {
public:
    /// Constructor
    explicit PerformanceMetrics(Logger& logger);
    
    /// Record operation duration
    void record_duration(const std::string& operation, std::chrono::microseconds duration);
    
    /// Record operation count
    void record_count(const std::string& operation, uint64_t count = 1);
    
    /// Record memory usage
    void record_memory_usage(const std::string& context, size_t bytes);
    
    /// Record throughput (operations per second)
    void record_throughput(const std::string& operation, double ops_per_second);
    
    /// Log periodic summary
    void log_summary();
    
    /// Reset all metrics
    void reset();

private:
    Logger& logger_;
    std::chrono::steady_clock::time_point last_summary_;
    
    // Simple metrics storage
    struct OperationStats {
        uint64_t count = 0;
        std::chrono::microseconds total_duration{0};
        std::chrono::microseconds min_duration{std::chrono::microseconds::max()};
        std::chrono::microseconds max_duration{0};
    };
    
    std::unordered_map<std::string, OperationStats> operation_stats_;
    std::unordered_map<std::string, uint64_t> counters_;
    std::unordered_map<std::string, size_t> memory_usage_;
    std::unordered_map<std::string, double> throughput_;
};

/// Convenience macros for performance logging
#define CHAINFORGE_PERF_TIMER(logger, operation) \
    ::chainforge::logging::ScopedTimer timer(logger, operation)

#define CHAINFORGE_PERF_TIMER_LEVEL(logger, operation, level) \
    ::chainforge::logging::ScopedTimer timer(logger, operation, level)

} // namespace chainforge::logging
