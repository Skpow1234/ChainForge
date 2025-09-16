#include "chainforge/logging/config_loader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <iostream>
#include <stdexcept>

namespace chainforge::logging {

LogConfig ConfigLoader::load_from_yaml(const std::filesystem::path& config_file) {
    // For now, we'll use a simple YAML-to-JSON converter
    // In a production system, you'd use a proper YAML library like yaml-cpp
    auto json_config = yaml_to_json(config_file);
    return load_from_json(json_config);
}

LogConfig ConfigLoader::load_from_json(const std::filesystem::path& config_file) {
    std::ifstream file(config_file);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + config_file.string());
    }
    
    nlohmann::json config_json;
    file >> config_json;
    
    return load_from_json(config_json);
}

LogConfig ConfigLoader::load_from_json(const nlohmann::json& config_json) {
    LogConfig config = create_default();
    
    if (!config_json.contains("logging")) {
        return config;
    }
    
    const auto& logging_config = config_json["logging"];
    
    // Console configuration
    if (logging_config.contains("console")) {
        const auto& console_config = logging_config["console"];
        
        if (console_config.contains("enabled")) {
            config.enable_console_output = console_config["enabled"].get<bool>();
        }
        
        if (console_config.contains("level")) {
            config.console_level = parse_log_level(console_config["level"].get<std::string>());
        }
        
        if (console_config.contains("pretty_print")) {
            config.pretty_print_console = console_config["pretty_print"].get<bool>();
        }
    }
    
    // File configuration
    if (logging_config.contains("file")) {
        const auto& file_config = logging_config["file"];
        
        if (file_config.contains("enabled")) {
            config.enable_file_output = file_config["enabled"].get<bool>();
        }
        
        if (file_config.contains("level")) {
            config.file_level = parse_log_level(file_config["level"].get<std::string>());
        }
        
        if (file_config.contains("directory")) {
            config.log_directory = file_config["directory"].get<std::string>();
        }
        
        if (file_config.contains("filename")) {
            config.log_file_pattern = file_config["filename"].get<std::string>();
        }
        
        if (file_config.contains("max_size")) {
            config.max_file_size = parse_size_string(file_config["max_size"].get<std::string>());
        }
        
        if (file_config.contains("max_files")) {
            config.max_files = file_config["max_files"].get<size_t>();
        }
        
        if (file_config.contains("pretty_print")) {
            config.pretty_print_file = file_config["pretty_print"].get<bool>();
        }
    }
    
    // Format configuration
    if (logging_config.contains("format")) {
        std::string format = logging_config["format"].get<std::string>();
        config.enable_json_format = (format == "json");
    }
    
    // Global fields
    if (logging_config.contains("global_fields")) {
        config.global_fields = logging_config["global_fields"];
    }
    
    return config;
}

LogConfig ConfigLoader::create_default() {
    LogConfig config;
    config.console_level = LogLevel::INFO;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = "logs";
    config.log_file_pattern = "chainforge.log";
    config.max_file_size = 10 * 1024 * 1024; // 10MB
    config.max_files = 5;
    config.enable_json_format = true;
    config.enable_console_output = true;
    config.enable_file_output = true;
    config.pretty_print_console = false;
    config.pretty_print_file = false;
    
    // Default global fields
    config.global_fields["service"] = "chainforge";
    config.global_fields["version"] = "1.0.0";
    
    return config;
}

LogLevel ConfigLoader::parse_log_level(const std::string& level_str) {
    std::string level_lower = level_str;
    std::transform(level_lower.begin(), level_lower.end(), level_lower.begin(), ::tolower);
    
    if (level_lower == "trace") return LogLevel::TRACE;
    if (level_lower == "debug") return LogLevel::DEBUG;
    if (level_lower == "info") return LogLevel::INFO;
    if (level_lower == "warn" || level_lower == "warning") return LogLevel::WARN;
    if (level_lower == "error") return LogLevel::ERROR;
    if (level_lower == "critical" || level_lower == "fatal") return LogLevel::CRITICAL;
    if (level_lower == "off" || level_lower == "none") return LogLevel::OFF;
    
    // Default to INFO if unknown
    return LogLevel::INFO;
}

size_t ConfigLoader::parse_size_string(const std::string& size_str) {
    std::regex size_regex(R"((\d+(?:\.\d+)?)\s*(B|KB|MB|GB|TB)?)", std::regex_constants::icase);
    std::smatch match;
    
    if (!std::regex_match(size_str, match, size_regex)) {
        throw std::invalid_argument("Invalid size format: " + size_str);
    }
    
    double value = std::stod(match[1].str());
    std::string unit = match[2].str();
    
    // Convert to uppercase
    std::transform(unit.begin(), unit.end(), unit.begin(), ::toupper);
    
    size_t multiplier = 1;
    if (unit == "KB") {
        multiplier = 1024;
    } else if (unit == "MB") {
        multiplier = 1024 * 1024;
    } else if (unit == "GB") {
        multiplier = 1024 * 1024 * 1024;
    } else if (unit == "TB") {
        multiplier = 1024ULL * 1024 * 1024 * 1024;
    }
    
    return static_cast<size_t>(value * multiplier);
}

nlohmann::json ConfigLoader::yaml_to_json(const std::filesystem::path& yaml_file) {
    // Simple YAML to JSON converter for basic use cases
    // This is a simplified implementation - in production use yaml-cpp
    
    std::ifstream file(yaml_file);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open YAML file: " + yaml_file.string());
    }
    
    nlohmann::json result;
    nlohmann::json* current_object = &result;
    std::vector<nlohmann::json*> object_stack;
    std::string line;
    int current_indent = 0;
    std::vector<int> indent_stack;
    
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line.find_first_not_of(" \t") == std::string::npos || 
            line.find_first_not_of(" \t#") == std::string::npos) {
            continue;
        }
        
        // Calculate indentation
        int indent = 0;
        for (char c : line) {
            if (c == ' ') indent++;
            else break;
        }
        
        // Trim line
        line = line.substr(line.find_first_not_of(" \t"));
        
        // Handle indentation changes
        while (!indent_stack.empty() && indent <= indent_stack.back()) {
            indent_stack.pop_back();
            object_stack.pop_back();
            if (!object_stack.empty()) {
                current_object = object_stack.back();
            } else {
                current_object = &result;
            }
        }
        
        // Parse key-value pair
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            
            // Trim key and value
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (value.empty()) {
                // This is an object
                (*current_object)[key] = nlohmann::json::object();
                object_stack.push_back(current_object);
                current_object = &((*current_object)[key]);
                indent_stack.push_back(indent);
            } else {
                // Parse value
                nlohmann::json json_value;
                
                // Remove quotes if present
                if ((value.front() == '"' && value.back() == '"') ||
                    (value.front() == '\'' && value.back() == '\'')) {
                    value = value.substr(1, value.length() - 2);
                    json_value = value;
                } else if (value == "true") {
                    json_value = true;
                } else if (value == "false") {
                    json_value = false;
                } else if (value == "null") {
                    json_value = nullptr;
                } else {
                    // Try to parse as number
                    try {
                        if (value.find('.') != std::string::npos) {
                            json_value = std::stod(value);
                        } else {
                            json_value = std::stoll(value);
                        }
                    } catch (...) {
                        // Default to string
                        json_value = value;
                    }
                }
                
                (*current_object)[key] = json_value;
            }
        }
    }
    
    return result;
}

bool initialize_logging_from_config(const std::filesystem::path& config_file) {
    try {
        LogConfig config;
        
        std::string extension = config_file.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        if (extension == ".yaml" || extension == ".yml") {
            config = ConfigLoader::load_from_yaml(config_file);
        } else if (extension == ".json") {
            config = ConfigLoader::load_from_json(config_file);
        } else {
            throw std::runtime_error("Unsupported config file format: " + extension);
        }
        
        LogManager::instance().initialize(config);
        return true;
    } catch (const std::exception& e) {
        // Fall back to default configuration
        std::cerr << "Failed to load logging config: " << e.what() << std::endl;
        std::cerr << "Using default logging configuration" << std::endl;
        return initialize_logging_with_defaults();
    }
}

bool initialize_logging_with_defaults() {
    try {
        LogConfig config = ConfigLoader::create_default();
        LogManager::instance().initialize(config);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize logging with defaults: " << e.what() << std::endl;
        return false;
    }
}

} // namespace chainforge::logging
