#include <gtest/gtest.h>
#include "chainforge/logging/logging.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

using namespace chainforge::logging;

class LoggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directory for test logs
        test_log_dir_ = std::filesystem::temp_directory_path() / "chainforge_test_logs";
        std::filesystem::create_directories(test_log_dir_);
        
        // Initialize logging for tests
        LogConfig config;
        config.console_level = LogLevel::DEBUG;
        config.file_level = LogLevel::TRACE;
        config.log_directory = test_log_dir_.string();
        config.log_file_pattern = "test.log";
        config.max_file_size = 1024 * 1024; // 1MB
        config.max_files = 1;
        config.enable_json_format = true;
        config.enable_console_output = false; // Disable console for tests
        config.enable_file_output = true;
        
        LogManager::instance().initialize(config);
    }
    
    void TearDown() override {
        LogManager::instance().shutdown();
        
        // Clean up test files
        std::filesystem::remove_all(test_log_dir_);
    }
    
    std::string read_log_file() {
        auto log_file = test_log_dir_ / "test.log";
        if (!std::filesystem::exists(log_file)) {
            return "";
        }
        
        std::ifstream file(log_file);
        std::ostringstream content;
        content << file.rdbuf();
        return content.str();
    }
    
    std::filesystem::path test_log_dir_;
};

TEST_F(LoggerTest, BasicLogging) {
    auto& logger = get_logger("test");
    
    logger.info("Test message");
    logger.debug("Debug message");
    logger.error("Error message");
    
    // Flush to ensure messages are written
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    EXPECT_FALSE(log_content.empty());
    
    // Check that messages appear in log
    EXPECT_NE(log_content.find("Test message"), std::string::npos);
    EXPECT_NE(log_content.find("Debug message"), std::string::npos);
    EXPECT_NE(log_content.find("Error message"), std::string::npos);
}

TEST_F(LoggerTest, StructuredLogging) {
    auto& logger = get_logger("structured_test");
    
    LogContext context;
    context.with("user_id", 12345)
           .with("action", "login")
           .with("ip_address", "192.168.1.1");
    
    logger.info("User login", context);
    
    // Flush to ensure messages are written
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    EXPECT_FALSE(log_content.empty());
    
    // Check that structured data appears in log
    EXPECT_NE(log_content.find("user_id"), std::string::npos);
    EXPECT_NE(log_content.find("12345"), std::string::npos);
    EXPECT_NE(log_content.find("login"), std::string::npos);
    EXPECT_NE(log_content.find("192.168.1.1"), std::string::npos);
}

TEST_F(LoggerTest, LogLevels) {
    auto& logger = get_logger("level_test");
    
    // Set logger to WARN level
    logger.set_level(LogLevel::WARN);
    
    logger.debug("Debug message - should not appear");
    logger.info("Info message - should not appear");
    logger.warn("Warning message - should appear");
    logger.error("Error message - should appear");
    
    // Flush to ensure messages are written
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    
    // Only WARN and ERROR should appear
    EXPECT_EQ(log_content.find("Debug message"), std::string::npos);
    EXPECT_EQ(log_content.find("Info message"), std::string::npos);
    EXPECT_NE(log_content.find("Warning message"), std::string::npos);
    EXPECT_NE(log_content.find("Error message"), std::string::npos);
}

TEST_F(LoggerTest, PerformanceLogging) {
    auto& logger = get_logger("perf_test");
    
    {
        ScopedTimer timer(logger, "test_operation", LogLevel::INFO);
        
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        timer.checkpoint("checkpoint1");
        
        // More work
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    // Flush to ensure messages are written
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    
    // Check that performance logs appear
    EXPECT_NE(log_content.find("test_operation"), std::string::npos);
    EXPECT_NE(log_content.find("checkpoint1"), std::string::npos);
    EXPECT_NE(log_content.find("duration_ms"), std::string::npos);
}

TEST_F(LoggerTest, PerformanceMetrics) {
    auto& logger = get_logger("metrics_test");
    PerformanceMetrics metrics(logger);
    
    // Record some metrics
    metrics.record_duration("database_query", std::chrono::microseconds(1500));
    metrics.record_duration("database_query", std::chrono::microseconds(2000));
    metrics.record_count("api_requests", 10);
    metrics.record_memory_usage("heap", 1024 * 1024); // 1MB
    metrics.record_throughput("transactions", 150.5);
    
    // Log summary
    metrics.log_summary();
    
    // Flush to ensure messages are written
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    
    // Check that metrics appear in log
    EXPECT_NE(log_content.find("database_query"), std::string::npos);
    EXPECT_NE(log_content.find("api_requests"), std::string::npos);
    EXPECT_NE(log_content.find("heap"), std::string::npos);
    EXPECT_NE(log_content.find("transactions"), std::string::npos);
}

TEST_F(LoggerTest, JsonFormat) {
    auto& logger = get_logger("json_test");
    
    logger.info("JSON test message");
    
    // Flush to ensure messages are written
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    
    // Should be valid JSON
    std::istringstream iss(log_content);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty()) {
            // Try to parse as JSON
            EXPECT_NO_THROW({
                auto json = nlohmann::json::parse(line);
                
                // Check required fields
                EXPECT_TRUE(json.contains("timestamp"));
                EXPECT_TRUE(json.contains("level"));
                EXPECT_TRUE(json.contains("logger"));
                EXPECT_TRUE(json.contains("message"));
                
                // Check values
                EXPECT_EQ(json["level"], "INFO");
                EXPECT_EQ(json["logger"], "json_test");
                EXPECT_EQ(json["message"], "JSON test message");
            });
            break; // Only test first line
        }
    }
}

TEST_F(LoggerTest, MacroConvenience) {
    // Test convenience macros
    CHAINFORGE_LOG_INFO("Info macro test");
    CHAINFORGE_LOG_DEBUG("Debug macro test with value: {}", 42);
    CHAINFORGE_LOG_ERROR("Error macro test");
    
    // Flush to ensure messages are written
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    
    // Check that macro messages appear
    EXPECT_NE(log_content.find("Info macro test"), std::string::npos);
    EXPECT_NE(log_content.find("Debug macro test with value: 42"), std::string::npos);
    EXPECT_NE(log_content.find("Error macro test"), std::string::npos);
}
