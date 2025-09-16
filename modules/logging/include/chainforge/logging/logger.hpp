#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <nlohmann/json.hpp>

namespace chainforge::logging {

/// Log levels enum for ChainForge
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    CRITICAL = 5,
    OFF = 6
};

/// Convert LogLevel to spdlog level
constexpr spdlog::level::level_enum to_spdlog_level(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return spdlog::level::trace;
        case LogLevel::DEBUG: return spdlog::level::debug;
        case LogLevel::INFO: return spdlog::level::info;
        case LogLevel::WARN: return spdlog::level::warn;
        case LogLevel::ERROR: return spdlog::level::err;
        case LogLevel::CRITICAL: return spdlog::level::critical;
        case LogLevel::OFF: return spdlog::level::off;
    }
    return spdlog::level::info;
}

/// Structured logging context for adding key-value pairs
class LogContext {
public:
    LogContext() = default;
    
    template<typename T>
    LogContext& with(const std::string& key, T&& value) {
        fields_[key] = std::forward<T>(value);
        return *this;
    }
    
    const nlohmann::json& fields() const { return fields_; }
    
private:
    nlohmann::json fields_;
};

/// Main logger class providing structured JSON logging
class Logger {
public:
    /// Create logger with name and configuration
    explicit Logger(const std::string& name);
    
    /// Destructor
    ~Logger() = default;
    
    // Disable copy, enable move
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = default;
    Logger& operator=(Logger&&) = default;
    
    /// Set log level
    void set_level(LogLevel level);
    
    /// Get current log level
    LogLevel get_level() const;
    
    /// Check if level is enabled
    bool should_log(LogLevel level) const;
    
    /// Log with context (structured logging)
    void log(LogLevel level, const std::string& message, const LogContext& context = {});
    
    /// Convenience methods for different log levels
    void trace(const std::string& message, const LogContext& context = {});
    void debug(const std::string& message, const LogContext& context = {});
    void info(const std::string& message, const LogContext& context = {});
    void warn(const std::string& message, const LogContext& context = {});
    void error(const std::string& message, const LogContext& context = {});
    void critical(const std::string& message, const LogContext& context = {});
    
    /// Template convenience methods for format strings
    template<typename... Args>
    void trace(const std::string& format, Args&&... args) {
        if (should_log(LogLevel::TRACE)) {
            trace(fmt::format(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void debug(const std::string& format, Args&&... args) {
        if (should_log(LogLevel::DEBUG)) {
            debug(fmt::format(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void info(const std::string& format, Args&&... args) {
        if (should_log(LogLevel::INFO)) {
            info(fmt::format(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void warn(const std::string& format, Args&&... args) {
        if (should_log(LogLevel::WARN)) {
            warn(fmt::format(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void error(const std::string& format, Args&&... args) {
        if (should_log(LogLevel::ERROR)) {
            error(fmt::format(format, std::forward<Args>(args)...));
        }
    }
    
    template<typename... Args>
    void critical(const std::string& format, Args&&... args) {
        if (should_log(LogLevel::CRITICAL)) {
            critical(fmt::format(format, std::forward<Args>(args)...));
        }
    }
    
    /// Access underlying spdlog logger
    std::shared_ptr<spdlog::logger> spdlog_logger() const { return logger_; }

private:
    std::shared_ptr<spdlog::logger> logger_;
    std::string name_;
};

} // namespace chainforge::logging
