#pragma once

#include "chainforge/logging/logger.hpp"
#include "counter.hpp"
#include "histogram.hpp"
#include <memory>
#include <string>

namespace chainforge::metrics {

/// Integration between logging and metrics systems
/// Automatically tracks logging metrics for observability
class LoggingMetrics {
public:
    /// Get singleton instance
    static LoggingMetrics& instance();
    
    /// Initialize logging metrics
    void initialize();
    
    /// Record a log event
    void record_log_event(const std::string& level, const std::string& logger_name);
    
    /// Record log message size
    void record_log_message_size(size_t size_bytes);
    
    /// Get metrics
    std::shared_ptr<Counter> log_messages_total() const { return log_messages_total_; }
    std::shared_ptr<Histogram> log_message_size_bytes() const { return log_message_size_bytes_; }

private:
    LoggingMetrics() = default;
    ~LoggingMetrics() = default;
    
    // Disable copy and move
    LoggingMetrics(const LoggingMetrics&) = delete;
    LoggingMetrics& operator=(const LoggingMetrics&) = delete;
    LoggingMetrics(LoggingMetrics&&) = delete;
    LoggingMetrics& operator=(LoggingMetrics&&) = delete;
    
    bool initialized_ = false;
    std::shared_ptr<Counter> log_messages_total_;
    std::shared_ptr<Histogram> log_message_size_bytes_;
};

/// Metrics-aware logger wrapper
/// Automatically records metrics for log operations
class MetricsLogger {
public:
    /// Constructor
    explicit MetricsLogger(chainforge::logging::Logger& logger);
    
    /// Log with automatic metrics recording
    void log(chainforge::logging::LogLevel level, const std::string& message, 
             const chainforge::logging::LogContext& context = {});
    
    /// Convenience methods
    void trace(const std::string& message, const chainforge::logging::LogContext& context = {});
    void debug(const std::string& message, const chainforge::logging::LogContext& context = {});
    void info(const std::string& message, const chainforge::logging::LogContext& context = {});
    void warn(const std::string& message, const chainforge::logging::LogContext& context = {});
    void error(const std::string& message, const chainforge::logging::LogContext& context = {});
    void critical(const std::string& message, const chainforge::logging::LogContext& context = {});
    
    /// Get underlying logger
    chainforge::logging::Logger& logger() const { return logger_; }

private:
    chainforge::logging::Logger& logger_;
    LoggingMetrics& metrics_;
    
    std::string level_to_string(chainforge::logging::LogLevel level) const;
};

/// Create metrics-aware logger
std::unique_ptr<MetricsLogger> create_metrics_logger(const std::string& name);

} // namespace chainforge::metrics
