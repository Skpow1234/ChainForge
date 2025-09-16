#include "chainforge/logging/performance_logger.hpp"
#include <fmt/format.h>
#include <thread>

namespace chainforge::logging {

ScopedTimer::ScopedTimer(Logger& logger, const std::string& operation, LogLevel level)
    : logger_(logger)
    , operation_(operation)
    , level_(level)
    , start_time_(std::chrono::steady_clock::now()) {
    
    if (logger_.should_log(level_)) {
        LogContext context;
        context.with("operation", operation_)
               .with("event", "start");
        logger_.log(level_, fmt::format("Starting operation: {}", operation_), context);
    }
}

ScopedTimer::~ScopedTimer() {
    auto duration = elapsed();
    
    if (logger_.should_log(level_)) {
        LogContext context;
        context.with("operation", operation_)
               .with("event", "complete")
               .with("duration_us", duration.count())
               .with("duration_ms", duration.count() / 1000.0);
        
        logger_.log(level_, 
                   fmt::format("Completed operation: {} in {:.2f}ms", 
                              operation_, duration.count() / 1000.0), 
                   context);
    }
}

std::chrono::microseconds ScopedTimer::elapsed() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - start_time_);
}

void ScopedTimer::checkpoint(const std::string& checkpoint_name) {
    auto duration = elapsed();
    
    if (logger_.should_log(level_)) {
        LogContext context;
        context.with("operation", operation_)
               .with("event", "checkpoint")
               .with("checkpoint", checkpoint_name)
               .with("duration_us", duration.count())
               .with("duration_ms", duration.count() / 1000.0);
        
        logger_.log(level_, 
                   fmt::format("Checkpoint {} in operation {}: {:.2f}ms", 
                              checkpoint_name, operation_, duration.count() / 1000.0), 
                   context);
    }
}

PerformanceMetrics::PerformanceMetrics(Logger& logger) 
    : logger_(logger)
    , last_summary_(std::chrono::steady_clock::now()) {
}

void PerformanceMetrics::record_duration(const std::string& operation, std::chrono::microseconds duration) {
    auto& stats = operation_stats_[operation];
    stats.count++;
    stats.total_duration += duration;
    
    if (duration < stats.min_duration) {
        stats.min_duration = duration;
    }
    if (duration > stats.max_duration) {
        stats.max_duration = duration;
    }
    
    // Log individual measurement at debug level
    if (logger_.should_log(LogLevel::DEBUG)) {
        LogContext context;
        context.with("operation", operation)
               .with("duration_us", duration.count())
               .with("duration_ms", duration.count() / 1000.0)
               .with("metric_type", "duration");
        
        logger_.debug(fmt::format("Performance: {} took {:.2f}ms", operation, duration.count() / 1000.0), context);
    }
}

void PerformanceMetrics::record_count(const std::string& operation, uint64_t count) {
    counters_[operation] += count;
    
    if (logger_.should_log(LogLevel::DEBUG)) {
        LogContext context;
        context.with("operation", operation)
               .with("count", count)
               .with("total_count", counters_[operation])
               .with("metric_type", "count");
        
        logger_.debug(fmt::format("Performance: {} count: {}", operation, count), context);
    }
}

void PerformanceMetrics::record_memory_usage(const std::string& context, size_t bytes) {
    memory_usage_[context] = bytes;
    
    if (logger_.should_log(LogLevel::DEBUG)) {
        LogContext log_context;
        log_context.with("context", context)
                   .with("bytes", bytes)
                   .with("kb", bytes / 1024.0)
                   .with("mb", bytes / (1024.0 * 1024.0))
                   .with("metric_type", "memory");
        
        logger_.debug(fmt::format("Performance: {} memory: {:.2f} MB", context, bytes / (1024.0 * 1024.0)), log_context);
    }
}

void PerformanceMetrics::record_throughput(const std::string& operation, double ops_per_second) {
    throughput_[operation] = ops_per_second;
    
    if (logger_.should_log(LogLevel::DEBUG)) {
        LogContext context;
        context.with("operation", operation)
               .with("ops_per_second", ops_per_second)
               .with("metric_type", "throughput");
        
        logger_.debug(fmt::format("Performance: {} throughput: {:.2f} ops/sec", operation, ops_per_second), context);
    }
}

void PerformanceMetrics::log_summary() {
    LogContext summary_context;
    summary_context.with("metric_type", "summary");
    
    // Operation statistics
    nlohmann::json operations_summary;
    for (const auto& [operation, stats] : operation_stats_) {
        if (stats.count > 0) {
            auto avg_duration = stats.total_duration / stats.count;
            operations_summary[operation] = {
                {"count", stats.count},
                {"total_duration_ms", stats.total_duration.count() / 1000.0},
                {"avg_duration_ms", avg_duration.count() / 1000.0},
                {"min_duration_ms", stats.min_duration.count() / 1000.0},
                {"max_duration_ms", stats.max_duration.count() / 1000.0}
            };
        }
    }
    
    if (!operations_summary.empty()) {
        summary_context.with("operations", operations_summary);
    }
    
    // Counters
    if (!counters_.empty()) {
        summary_context.with("counters", counters_);
    }
    
    // Memory usage
    if (!memory_usage_.empty()) {
        nlohmann::json memory_summary;
        for (const auto& [context, bytes] : memory_usage_) {
            memory_summary[context] = {
                {"bytes", bytes},
                {"mb", bytes / (1024.0 * 1024.0)}
            };
        }
        summary_context.with("memory", memory_summary);
    }
    
    // Throughput
    if (!throughput_.empty()) {
        summary_context.with("throughput", throughput_);
    }
    
    logger_.info("Performance metrics summary", summary_context);
    last_summary_ = std::chrono::steady_clock::now();
}

void PerformanceMetrics::reset() {
    operation_stats_.clear();
    counters_.clear();
    memory_usage_.clear();
    throughput_.clear();
    last_summary_ = std::chrono::steady_clock::now();
    
    logger_.info("Performance metrics reset");
}

} // namespace chainforge::logging
