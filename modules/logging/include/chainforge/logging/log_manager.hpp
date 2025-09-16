#pragma once

#include "logger.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <filesystem>

namespace chainforge::logging {

/// Configuration for logging system
struct LogConfig {
    LogLevel console_level = LogLevel::INFO;
    LogLevel file_level = LogLevel::DEBUG;
    std::string log_directory = "logs";
    std::string log_file_pattern = "chainforge.log";
    size_t max_file_size = 10 * 1024 * 1024; // 10MB
    size_t max_files = 5;
    bool enable_json_format = true;
    bool enable_console_output = true;
    bool enable_file_output = true;
    bool pretty_print_console = false;
    bool pretty_print_file = false;
    
    // Global fields to add to all log messages
    nlohmann::json global_fields;
};

/// Centralized logging manager
class LogManager {
public:
    /// Get singleton instance
    static LogManager& instance();
    
    /// Initialize logging system with configuration
    void initialize(const LogConfig& config);
    
    /// Shutdown logging system
    void shutdown();
    
    /// Get or create logger by name
    Logger& get_logger(const std::string& name);
    
    /// Set global log level
    void set_global_level(LogLevel level);
    
    /// Get current configuration
    const LogConfig& get_config() const { return config_; }
    
    /// Update configuration (will recreate loggers)
    void update_config(const LogConfig& config);
    
    /// Flush all loggers
    void flush_all();
    
    /// Get performance metrics logger
    Logger& get_performance_logger();

private:
    LogManager() = default;
    ~LogManager() = default;
    
    // Disable copy and move
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;
    LogManager(LogManager&&) = delete;
    LogManager& operator=(LogManager&&) = delete;
    
    void create_sinks();
    void setup_logger(const std::string& name, std::shared_ptr<spdlog::logger> spdlog_logger);
    
    mutable std::mutex mutex_;
    LogConfig config_;
    bool initialized_ = false;
    
    std::unordered_map<std::string, std::unique_ptr<Logger>> loggers_;
    
    // spdlog sinks
    std::vector<spdlog::sink_ptr> sinks_;
    spdlog::sink_ptr console_sink_;
    spdlog::sink_ptr file_sink_;
};

/// Convenience function to get logger
Logger& get_logger(const std::string& name);

/// Convenience function to get the default logger
Logger& get_default_logger();

/// Convenience function to get performance logger
Logger& get_performance_logger();

} // namespace chainforge::logging

/// Global convenience macros
#define CHAINFORGE_LOG_TRACE(message, ...) \
    ::chainforge::logging::get_default_logger().trace(message, ##__VA_ARGS__)

#define CHAINFORGE_LOG_DEBUG(message, ...) \
    ::chainforge::logging::get_default_logger().debug(message, ##__VA_ARGS__)

#define CHAINFORGE_LOG_INFO(message, ...) \
    ::chainforge::logging::get_default_logger().info(message, ##__VA_ARGS__)

#define CHAINFORGE_LOG_WARN(message, ...) \
    ::chainforge::logging::get_default_logger().warn(message, ##__VA_ARGS__)

#define CHAINFORGE_LOG_ERROR(message, ...) \
    ::chainforge::logging::get_default_logger().error(message, ##__VA_ARGS__)

#define CHAINFORGE_LOG_CRITICAL(message, ...) \
    ::chainforge::logging::get_default_logger().critical(message, ##__VA_ARGS__)
