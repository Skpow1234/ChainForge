#include <gtest/gtest.h>
#include "chainforge/logging/logging.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>

using namespace chainforge::logging;

class LogManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temp directory for test logs
        test_log_dir_ = std::filesystem::temp_directory_path() / "chainforge_log_manager_test";
        std::filesystem::create_directories(test_log_dir_);
        
        // Clear any existing log manager state
        LogManager::instance().shutdown();
    }
    
    void TearDown() override {
        LogManager::instance().shutdown();
        
        // Clean up test files
        std::filesystem::remove_all(test_log_dir_);
    }
    
    std::string read_log_file(const std::string& filename = "test.log") {
        auto log_file = test_log_dir_ / filename;
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

TEST_F(LogManagerTest, BasicInitialization) {
    LogConfig config;
    config.console_level = LogLevel::INFO;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.max_file_size = 1024 * 1024; // 1MB
    config.max_files = 5;
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
}

TEST_F(LogManagerTest, DoubleInitialization) {
    LogConfig config;
    config.console_level = LogLevel::INFO;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    // First initialization should succeed
    auto result1 = LogManager::instance().initialize(config);
    EXPECT_TRUE(result1.has_value());
    
    // Second initialization should fail
    auto result2 = LogManager::instance().initialize(config);
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error().code, ErrorCode::ALREADY_INITIALIZED);
}

TEST_F(LogManagerTest, InvalidConfiguration) {
    LogConfig config;
    config.console_level = LogLevel::INFO;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = "/invalid/path/that/does/not/exist";
    config.log_file_pattern = "test.log";
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::INVALID_CONFIGURATION);
}

TEST_F(LogManagerTest, LogRotation) {
    LogConfig config;
    config.console_level = LogLevel::INFO;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.max_file_size = 1024; // 1KB - small for testing
    config.max_files = 3;
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    auto& logger = get_logger("rotation_test");
    
    // Generate enough logs to trigger rotation
    for (int i = 0; i < 100; ++i) {
        logger.info("This is a test log message number {} to trigger log rotation", i);
    }
    
    LogManager::instance().flush_all();
    
    // Check that log files were created
    EXPECT_TRUE(std::filesystem::exists(test_log_dir_ / "test.log"));
    
    // Check for rotated files
    bool found_rotated = false;
    for (int i = 1; i <= 3; ++i) {
        if (std::filesystem::exists(test_log_dir_ / ("test.log." + std::to_string(i)))) {
            found_rotated = true;
            break;
        }
    }
    EXPECT_TRUE(found_rotated);
}

TEST_F(LogManagerTest, LogLevelFiltering) {
    LogConfig config;
    config.console_level = LogLevel::WARN;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    auto& logger = get_logger("level_test");
    
    // Log messages at different levels
    logger.debug("Debug message - should not appear in console");
    logger.info("Info message - should not appear in console");
    logger.warn("Warning message - should appear in console");
    logger.error("Error message - should appear in console");
    
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    
    // All messages should appear in file
    EXPECT_NE(log_content.find("Debug message"), std::string::npos);
    EXPECT_NE(log_content.find("Info message"), std::string::npos);
    EXPECT_NE(log_content.find("Warning message"), std::string::npos);
    EXPECT_NE(log_content.find("Error message"), std::string::npos);
}

TEST_F(LogManagerTest, MultipleLoggers) {
    LogConfig config;
    config.console_level = LogLevel::DEBUG;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    auto& logger1 = get_logger("module1");
    auto& logger2 = get_logger("module2");
    auto& logger3 = get_logger("module3");
    
    logger1.info("Message from module1");
    logger2.warn("Message from module2");
    logger3.error("Message from module3");
    
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    
    EXPECT_NE(log_content.find("module1"), std::string::npos);
    EXPECT_NE(log_content.find("module2"), std::string::npos);
    EXPECT_NE(log_content.find("module3"), std::string::npos);
}

TEST_F(LogManagerTest, ConcurrentLogging) {
    LogConfig config;
    config.console_level = LogLevel::DEBUG;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    const int num_threads = 4;
    const int messages_per_thread = 100;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([t, messages_per_thread]() {
            auto& logger = get_logger("thread_" + std::to_string(t));
            
            for (int i = 0; i < messages_per_thread; ++i) {
                logger.info("Thread {} message {}", t, i);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    
    // Check that messages from all threads appear
    for (int t = 0; t < num_threads; ++t) {
        EXPECT_NE(log_content.find("Thread " + std::to_string(t)), std::string::npos);
    }
}

TEST_F(LogManagerTest, LogFlushing) {
    LogConfig config;
    config.console_level = LogLevel::DEBUG;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    auto& logger = get_logger("flush_test");
    
    logger.info("Message before flush");
    
    // Message should not be in file yet
    std::string log_content = read_log_file();
    EXPECT_EQ(log_content.find("Message before flush"), std::string::npos);
    
    // Flush and check again
    LogManager::instance().flush_all();
    log_content = read_log_file();
    EXPECT_NE(log_content.find("Message before flush"), std::string::npos);
}

TEST_F(LogManagerTest, LogManagerShutdown) {
    LogConfig config;
    config.console_level = LogLevel::DEBUG;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    auto& logger = get_logger("shutdown_test");
    logger.info("Message before shutdown");
    
    // Shutdown should flush remaining logs
    LogManager::instance().shutdown();
    
    std::string log_content = read_log_file();
    EXPECT_NE(log_content.find("Message before shutdown"), std::string::npos);
}

TEST_F(LogManagerTest, LogManagerReinitialization) {
    LogConfig config;
    config.console_level = LogLevel::DEBUG;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    // First initialization
    auto result1 = LogManager::instance().initialize(config);
    EXPECT_TRUE(result1.has_value());
    
    auto& logger1 = get_logger("test");
    logger1.info("Message from first initialization");
    
    LogManager::instance().shutdown();
    
    // Second initialization
    auto result2 = LogManager::instance().initialize(config);
    EXPECT_TRUE(result2.has_value());
    
    auto& logger2 = get_logger("test");
    logger2.info("Message from second initialization");
    
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file();
    EXPECT_NE(log_content.find("Message from first initialization"), std::string::npos);
    EXPECT_NE(log_content.find("Message from second initialization"), std::string::npos);
}

TEST_F(LogManagerTest, LogManagerConfiguration) {
    LogConfig config;
    config.console_level = LogLevel::INFO;
    config.file_level = LogLevel::WARN;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "config_test.log";
    config.max_file_size = 2048;
    config.max_files = 2;
    config.enable_json_format = true;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    auto& logger = get_logger("config_test");
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warn("Warning message");
    logger.error("Error message");
    
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file("config_test.log");
    
    // Only WARN and ERROR should appear in file
    EXPECT_EQ(log_content.find("Debug message"), std::string::npos);
    EXPECT_EQ(log_content.find("Info message"), std::string::npos);
    EXPECT_NE(log_content.find("Warning message"), std::string::npos);
    EXPECT_NE(log_content.find("Error message"), std::string::npos);
}

TEST_F(LogManagerTest, LogManagerPerformance) {
    LogConfig config;
    config.console_level = LogLevel::DEBUG;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "perf_test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    auto& logger = get_logger("perf_test");
    
    const int num_messages = 10000;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_messages; ++i) {
        logger.info("Performance test message {}", i);
    }
    
    LogManager::instance().flush_all();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Performance should be reasonable (less than 1 second for 10k messages)
    EXPECT_LT(duration.count(), 1000);
    
    std::string log_content = read_log_file("perf_test.log");
    EXPECT_NE(log_content.find("Performance test message"), std::string::npos);
}

TEST_F(LogManagerTest, LogManagerErrorHandling) {
    LogConfig config;
    config.console_level = LogLevel::DEBUG;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "error_test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    auto& logger = get_logger("error_test");
    
    // Test logging with various error conditions
    logger.error("Error message with details: {}", "test error");
    logger.error("Error with exception: {}", std::runtime_error("test exception"));
    
    LogManager::instance().flush_all();
    
    std::string log_content = read_log_file("error_test.log");
    EXPECT_NE(log_content.find("Error message with details"), std::string::npos);
    EXPECT_NE(log_content.find("test error"), std::string::npos);
}

TEST_F(LogManagerTest, LogManagerThreadSafety) {
    LogConfig config;
    config.console_level = LogLevel::DEBUG;
    config.file_level = LogLevel::DEBUG;
    config.log_directory = test_log_dir_.string();
    config.log_file_pattern = "thread_safety_test.log";
    config.enable_json_format = false;
    config.enable_console_output = false;
    config.enable_file_output = true;
    
    auto result = LogManager::instance().initialize(config);
    EXPECT_TRUE(result.has_value());
    
    const int num_threads = 8;
    const int messages_per_thread = 1000;
    
    std::vector<std::thread> threads;
    std::vector<bool> results(num_threads, true);
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&results, t, messages_per_thread]() {
            try {
                auto& logger = get_logger("thread_" + std::to_string(t));
                
                for (int i = 0; i < messages_per_thread; ++i) {
                    logger.info("Thread {} message {}", t, i);
                }
            } catch (...) {
                results[t] = false;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    LogManager::instance().flush_all();
    
    // All threads should have completed successfully
    for (bool result : results) {
        EXPECT_TRUE(result);
    }
    
    std::string log_content = read_log_file("thread_safety_test.log");
    
    // Check that messages from all threads appear
    for (int t = 0; t < num_threads; ++t) {
        EXPECT_NE(log_content.find("Thread " + std::to_string(t)), std::string::npos);
    }
}

} // namespace chainforge::logging::test
