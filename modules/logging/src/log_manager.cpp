#include "chainforge/logging/log_manager.hpp"
#include "chainforge/logging/json_formatter.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>

namespace chainforge::logging {

LogManager& LogManager::instance() {
    static LogManager instance;
    return instance;
}

void LogManager::initialize(const LogConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    config_ = config;
    
    // Create log directory if it doesn't exist
    if (config_.enable_file_output) {
        std::filesystem::create_directories(config_.log_directory);
    }
    
    // Create sinks
    create_sinks();
    
    // Clear existing loggers (they will be recreated on demand)
    loggers_.clear();
    
    // Set global spdlog level to the minimum of console and file levels
    auto global_level = std::min(to_spdlog_level(config_.console_level), 
                                to_spdlog_level(config_.file_level));
    spdlog::set_level(global_level);
    
    initialized_ = true;
}

void LogManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Flush all loggers
    for (auto& [name, logger] : loggers_) {
        if (logger && logger->spdlog_logger()) {
            logger->spdlog_logger()->flush();
        }
    }
    
    // Shutdown spdlog
    spdlog::shutdown();
    
    loggers_.clear();
    sinks_.clear();
    console_sink_.reset();
    file_sink_.reset();
    
    initialized_ = false;
}

Logger& LogManager::get_logger(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = loggers_.find(name);
    if (it != loggers_.end()) {
        return *it->second;
    }
    
    // Create new logger
    auto spdlog_logger = std::make_shared<spdlog::logger>(name, sinks_.begin(), sinks_.end());
    setup_logger(name, spdlog_logger);
    
    auto logger = std::make_unique<Logger>(name);
    logger->spdlog_logger() = spdlog_logger;
    
    auto& logger_ref = *logger;
    loggers_[name] = std::move(logger);
    
    return logger_ref;
}

void LogManager::set_global_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto spdlog_level = to_spdlog_level(level);
    spdlog::set_level(spdlog_level);
    
    for (auto& [name, logger] : loggers_) {
        if (logger && logger->spdlog_logger()) {
            logger->spdlog_logger()->set_level(spdlog_level);
        }
    }
}

void LogManager::update_config(const LogConfig& config) {
    // Re-initialize with new config
    initialize(config);
}

void LogManager::flush_all() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& [name, logger] : loggers_) {
        if (logger && logger->spdlog_logger()) {
            logger->spdlog_logger()->flush();
        }
    }
}

Logger& LogManager::get_performance_logger() {
    return get_logger("performance");
}

void LogManager::create_sinks() {
    sinks_.clear();
    
    if (config_.enable_console_output) {
        console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink_->set_level(to_spdlog_level(config_.console_level));
        
        if (config_.enable_json_format) {
            auto json_formatter = std::make_unique<JsonFormatter>(config_.pretty_print_console);
            
            // Add global fields from config
            for (const auto& [key, value] : config_.global_fields.items()) {
                json_formatter->add_global_field(key, value);
            }
            
            console_sink_->set_formatter(std::move(json_formatter));
        } else {
            // Use default pattern
            console_sink_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%^%l%$] %v");
        }
        
        sinks_.push_back(console_sink_);
    }
    
    if (config_.enable_file_output) {
        std::filesystem::path log_path = std::filesystem::path(config_.log_directory) / config_.log_file_pattern;
        
        file_sink_ = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            log_path.string(), config_.max_file_size, config_.max_files);
        file_sink_->set_level(to_spdlog_level(config_.file_level));
        
        if (config_.enable_json_format) {
            auto json_formatter = std::make_unique<JsonFormatter>(config_.pretty_print_file);
            
            // Add global fields from config
            for (const auto& [key, value] : config_.global_fields.items()) {
                json_formatter->add_global_field(key, value);
            }
            
            file_sink_->set_formatter(std::move(json_formatter));
        } else {
            // Use default pattern
            file_sink_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v");
        }
        
        sinks_.push_back(file_sink_);
    }
}

void LogManager::setup_logger(const std::string& name, std::shared_ptr<spdlog::logger> spdlog_logger) {
    // Set level to minimum of console and file levels
    auto level = std::min(to_spdlog_level(config_.console_level), 
                         to_spdlog_level(config_.file_level));
    spdlog_logger->set_level(level);
    
    // Flush on error and above
    spdlog_logger->flush_on(spdlog::level::err);
    
    // Register with spdlog
    spdlog::register_logger(spdlog_logger);
}

// Convenience functions
Logger& get_logger(const std::string& name) {
    return LogManager::instance().get_logger(name);
}

Logger& get_default_logger() {
    return LogManager::instance().get_logger("chainforge");
}

Logger& get_performance_logger() {
    return LogManager::instance().get_performance_logger();
}

} // namespace chainforge::logging
