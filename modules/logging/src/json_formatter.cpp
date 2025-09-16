#include "chainforge/logging/json_formatter.hpp"
#include <iomanip>
#include <sstream>

namespace chainforge::logging {

JsonFormatter::JsonFormatter(bool pretty_print) : pretty_print_(pretty_print) {
    // Add default global fields
    global_fields_["service"] = "chainforge";
    global_fields_["version"] = "1.0.0"; // TODO: Get from build system
}

void JsonFormatter::format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) {
    nlohmann::json log_record;
    
    // Add timestamp
    log_record["timestamp"] = format_timestamp(msg.time);
    
    // Add log level
    log_record["level"] = level_to_string(msg.level);
    
    // Add logger name
    log_record["logger"] = std::string(msg.logger_name.begin(), msg.logger_name.end());
    
    // Add thread ID
    log_record["thread_id"] = msg.thread_id;
    
    // Add source location if available
    if (msg.source.filename) {
        log_record["source"]["file"] = msg.source.filename;
        log_record["source"]["line"] = msg.source.line;
        log_record["source"]["function"] = msg.source.funcname;
    }
    
    // Parse message payload
    std::string payload(msg.payload.begin(), msg.payload.end());
    
    // Try to extract structured data from payload
    nlohmann::json structured_data = extract_structured_data(payload);
    
    if (structured_data.is_object() && structured_data.contains("message")) {
        // This is a structured log message
        log_record["message"] = structured_data["message"];
        if (structured_data.contains("fields")) {
            log_record["fields"] = structured_data["fields"];
        }
    } else {
        // Simple string message
        log_record["message"] = payload;
    }
    
    // Add global fields
    for (const auto& [key, value] : global_fields_.items()) {
        log_record[key] = value;
    }
    
    // Serialize to JSON string
    std::string json_str;
    if (pretty_print_) {
        json_str = log_record.dump(2);
    } else {
        json_str = log_record.dump();
    }
    
    // Append newline
    json_str += '\n';
    
    // Copy to destination buffer
    dest.append(json_str.data(), json_str.data() + json_str.size());
}

std::unique_ptr<spdlog::formatter> JsonFormatter::clone() const {
    auto cloned = std::make_unique<JsonFormatter>(pretty_print_);
    cloned->global_fields_ = global_fields_;
    return cloned;
}

void JsonFormatter::add_global_field(const std::string& key, const nlohmann::json& value) {
    global_fields_[key] = value;
}

void JsonFormatter::remove_global_field(const std::string& key) {
    global_fields_.erase(key);
}

std::string JsonFormatter::level_to_string(spdlog::level::level_enum level) {
    switch (level) {
        case spdlog::level::trace: return "TRACE";
        case spdlog::level::debug: return "DEBUG";
        case spdlog::level::info: return "INFO";
        case spdlog::level::warn: return "WARN";
        case spdlog::level::err: return "ERROR";
        case spdlog::level::critical: return "CRITICAL";
        case spdlog::level::off: return "OFF";
        default: return "UNKNOWN";
    }
}

std::string JsonFormatter::format_timestamp(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
        time.time_since_epoch()) % 1000000;
    
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(6) << microseconds.count();
    oss << 'Z';
    
    return oss.str();
}

nlohmann::json JsonFormatter::extract_structured_data(const std::string& payload) const {
    try {
        // Try to parse as JSON
        return nlohmann::json::parse(payload);
    } catch (const nlohmann::json::parse_error&) {
        // Not JSON, return as-is
        return nlohmann::json();
    }
}

} // namespace chainforge::logging
