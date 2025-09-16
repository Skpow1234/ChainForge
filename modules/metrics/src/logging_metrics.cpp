#include "chainforge/metrics/logging_metrics.hpp"
#include "chainforge/metrics/metrics_registry.hpp"
#include "chainforge/logging/log_manager.hpp"

namespace chainforge::metrics {

LoggingMetrics& LoggingMetrics::instance() {
    static LoggingMetrics instance;
    return instance;
}

void LoggingMetrics::initialize() {
    if (initialized_) {
        return;
    }
    
    auto& registry = get_metrics_registry();
    
    log_messages_total_ = registry.create_counter(
        "chainforge_log_messages_total",
        "Total number of log messages by level and logger"
    );
    
    log_message_size_bytes_ = registry.create_histogram(
        "chainforge_log_message_size_bytes", 
        "Size of log messages in bytes",
        {64, 256, 1024, 4096, 16384, 65536} // Common log message sizes
    );
    
    initialized_ = true;
}

void LoggingMetrics::record_log_event(const std::string& level, const std::string& logger_name) {
    if (!initialized_) {
        return;
    }
    
    auto& registry = get_metrics_registry();
    std::map<std::string, std::string> labels = {
        {"level", level},
        {"logger", logger_name}
    };
    
    auto counter = registry.create_counter(
        "chainforge_log_messages_total",
        "Total number of log messages by level and logger",
        labels
    );
    
    counter->increment();
}

void LoggingMetrics::record_log_message_size(size_t size_bytes) {
    if (!initialized_ || !log_message_size_bytes_) {
        return;
    }
    
    log_message_size_bytes_->observe(static_cast<double>(size_bytes));
}

MetricsLogger::MetricsLogger(chainforge::logging::Logger& logger)
    : logger_(logger), metrics_(LoggingMetrics::instance()) {
    
    // Ensure metrics are initialized
    metrics_.initialize();
}

void MetricsLogger::log(chainforge::logging::LogLevel level, const std::string& message, 
                       const chainforge::logging::LogContext& context) {
    // Record metrics first
    std::string level_str = level_to_string(level);
    std::string logger_name = std::string(logger_.spdlog_logger()->name());
    metrics_.record_log_event(level_str, logger_name);
    metrics_.record_log_message_size(message.size());
    
    // Forward to actual logger
    logger_.log(level, message, context);
}

void MetricsLogger::trace(const std::string& message, const chainforge::logging::LogContext& context) {
    log(chainforge::logging::LogLevel::TRACE, message, context);
}

void MetricsLogger::debug(const std::string& message, const chainforge::logging::LogContext& context) {
    log(chainforge::logging::LogLevel::DEBUG, message, context);
}

void MetricsLogger::info(const std::string& message, const chainforge::logging::LogContext& context) {
    log(chainforge::logging::LogLevel::INFO, message, context);
}

void MetricsLogger::warn(const std::string& message, const chainforge::logging::LogContext& context) {
    log(chainforge::logging::LogLevel::WARN, message, context);
}

void MetricsLogger::error(const std::string& message, const chainforge::logging::LogContext& context) {
    log(chainforge::logging::LogLevel::ERROR, message, context);
}

void MetricsLogger::critical(const std::string& message, const chainforge::logging::LogContext& context) {
    log(chainforge::logging::LogLevel::CRITICAL, message, context);
}

std::string MetricsLogger::level_to_string(chainforge::logging::LogLevel level) const {
    switch (level) {
        case chainforge::logging::LogLevel::TRACE: return "trace";
        case chainforge::logging::LogLevel::DEBUG: return "debug";
        case chainforge::logging::LogLevel::INFO: return "info";
        case chainforge::logging::LogLevel::WARN: return "warn";
        case chainforge::logging::LogLevel::ERROR: return "error";
        case chainforge::logging::LogLevel::CRITICAL: return "critical";
        case chainforge::logging::LogLevel::OFF: return "off";
        default: return "unknown";
    }
}

std::unique_ptr<MetricsLogger> create_metrics_logger(const std::string& name) {
    auto& logger = chainforge::logging::get_logger(name);
    return std::make_unique<MetricsLogger>(logger);
}

} // namespace chainforge::metrics
