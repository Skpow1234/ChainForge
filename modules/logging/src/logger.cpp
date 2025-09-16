#include "chainforge/logging/logger.hpp"
#include "chainforge/logging/json_formatter.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fmt/format.h>

namespace chainforge::logging {

Logger::Logger(const std::string& name) : name_(name) {
    // Create a basic console logger by default
    // This will be overridden by LogManager when properly configured
    logger_ = spdlog::stdout_color_mt(name);
    logger_->set_level(spdlog::level::info);
    
    // Set a basic pattern until JSON formatter is applied
    logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
}

void Logger::set_level(LogLevel level) {
    logger_->set_level(to_spdlog_level(level));
}

LogLevel Logger::get_level() const {
    switch (logger_->level()) {
        case spdlog::level::trace: return LogLevel::TRACE;
        case spdlog::level::debug: return LogLevel::DEBUG;
        case spdlog::level::info: return LogLevel::INFO;
        case spdlog::level::warn: return LogLevel::WARN;
        case spdlog::level::err: return LogLevel::ERROR;
        case spdlog::level::critical: return LogLevel::CRITICAL;
        case spdlog::level::off: return LogLevel::OFF;
        default: return LogLevel::INFO;
    }
}

bool Logger::should_log(LogLevel level) const {
    return logger_->should_log(to_spdlog_level(level));
}

void Logger::log(LogLevel level, const std::string& message, const LogContext& context) {
    if (!should_log(level)) {
        return;
    }
    
    // If we have context fields, format as JSON payload
    if (!context.fields().empty()) {
        nlohmann::json log_entry;
        log_entry["message"] = message;
        log_entry["fields"] = context.fields();
        
        // Convert to string and log
        std::string json_payload = log_entry.dump();
        logger_->log(to_spdlog_level(level), json_payload);
    } else {
        // Simple message logging
        logger_->log(to_spdlog_level(level), message);
    }
}

void Logger::trace(const std::string& message, const LogContext& context) {
    log(LogLevel::TRACE, message, context);
}

void Logger::debug(const std::string& message, const LogContext& context) {
    log(LogLevel::DEBUG, message, context);
}

void Logger::info(const std::string& message, const LogContext& context) {
    log(LogLevel::INFO, message, context);
}

void Logger::warn(const std::string& message, const LogContext& context) {
    log(LogLevel::WARN, message, context);
}

void Logger::error(const std::string& message, const LogContext& context) {
    log(LogLevel::ERROR, message, context);
}

void Logger::critical(const std::string& message, const LogContext& context) {
    log(LogLevel::CRITICAL, message, context);
}

} // namespace chainforge::logging
