#pragma once

#include <spdlog/formatter.h>
#include <spdlog/details/log_msg.h>
#include <nlohmann/json.hpp>
#include <chrono>
#include <string>

namespace chainforge::logging {

/// Custom JSON formatter for structured logging
class JsonFormatter : public spdlog::formatter {
public:
    /// Constructor
    explicit JsonFormatter(bool pretty_print = false);
    
    /// Format log message as JSON
    void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override;
    
    /// Clone formatter
    std::unique_ptr<spdlog::formatter> clone() const override;
    
    /// Set whether to pretty print JSON
    void set_pretty_print(bool enable) { pretty_print_ = enable; }
    
    /// Add global field that will be included in all log messages
    void add_global_field(const std::string& key, const nlohmann::json& value);
    
    /// Remove global field
    void remove_global_field(const std::string& key);

private:
    bool pretty_print_;
    nlohmann::json global_fields_;
    
    /// Convert log level to string
    static std::string level_to_string(spdlog::level::level_enum level);
    
    /// Format timestamp as ISO8601
    static std::string format_timestamp(const std::chrono::system_clock::time_point& time);
    
    /// Extract structured data from log message
    nlohmann::json extract_structured_data(const std::string& payload) const;
};

} // namespace chainforge::logging
