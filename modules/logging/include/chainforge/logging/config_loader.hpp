#pragma once

#include "log_manager.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <filesystem>

namespace chainforge::logging {

/// Configuration loader for logging system
class ConfigLoader {
public:
    /// Load logging configuration from YAML file
    static LogConfig load_from_yaml(const std::filesystem::path& config_file);
    
    /// Load logging configuration from JSON file
    static LogConfig load_from_json(const std::filesystem::path& config_file);
    
    /// Load logging configuration from JSON object
    static LogConfig load_from_json(const nlohmann::json& config_json);
    
    /// Create default configuration
    static LogConfig create_default();
    
    /// Parse log level from string
    static LogLevel parse_log_level(const std::string& level_str);
    
    /// Parse size string (e.g., "100MB", "1GB") to bytes
    static size_t parse_size_string(const std::string& size_str);

private:
    static nlohmann::json yaml_to_json(const std::filesystem::path& yaml_file);
};

/// Initialize logging system from configuration file
bool initialize_logging_from_config(const std::filesystem::path& config_file);

/// Initialize logging system with default configuration
bool initialize_logging_with_defaults();

} // namespace chainforge::logging
