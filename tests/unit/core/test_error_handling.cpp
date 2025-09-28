#include <gtest/gtest.h>
#include "chainforge/core/error.hpp"
#include "chainforge/core/error_utils.hpp"
#include <thread>
#include <chrono>

using namespace chainforge::core;

class ErrorHandlingTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test basic error creation and success
TEST_F(ErrorHandlingTest, BasicErrorCreation) {
    // Test success
    auto success_result = errors::success(42);
    EXPECT_TRUE(success_result.has_value());
    EXPECT_EQ(success_result.value(), 42);
    
    // Test error
    auto error_result = errors::error<int>(ErrorCode::INVALID_ARGUMENT, "Test error");
    EXPECT_FALSE(error_result.has_value());
    EXPECT_EQ(error_result.error().code, ErrorCode::INVALID_ARGUMENT);
    EXPECT_EQ(error_result.error().message, "Test error");
}

// Test void result
TEST_F(ErrorHandlingTest, VoidResult) {
    auto success_result = errors::success();
    EXPECT_TRUE(success_result.has_value());
    
    auto error_result = errors::error(ErrorCode::IO_ERROR, "IO failed");
    EXPECT_FALSE(error_result.has_value());
    EXPECT_EQ(error_result.error().code, ErrorCode::IO_ERROR);
}

// Test error chaining
TEST_F(ErrorHandlingTest, ErrorChaining) {
    auto original_error = errors::error<int>(ErrorCode::INVALID_ARGUMENT, "Original error");
    
    auto chained_error = propagation::chain_error(
        std::move(original_error),
        ErrorCode::EXECUTION_ERROR,
        "Execution failed"
    );
    
    EXPECT_FALSE(chained_error.has_value());
    EXPECT_EQ(chained_error.error().code, ErrorCode::EXECUTION_ERROR);
    EXPECT_TRUE(chained_error.error().cause != nullptr);
    EXPECT_EQ(chained_error.error().cause->code, ErrorCode::INVALID_ARGUMENT);
}

// Test error context addition
TEST_F(ErrorHandlingTest, ErrorContext) {
    auto error_result = errors::error<int>(ErrorCode::INVALID_ARGUMENT, "Test error");
    
    auto context_result = propagation::add_context(std::move(error_result), "in test_function");
    
    EXPECT_FALSE(context_result.has_value());
    EXPECT_TRUE(context_result.error().context.find("in test_function") != std::string::npos);
}

// Test error code mapping
TEST_F(ErrorHandlingTest, ErrorCodeMapping) {
    auto error_result = errors::error<int>(ErrorCode::INVALID_ARGUMENT, "Test error");
    
    auto mapped_result = propagation::map_error_code(std::move(error_result), ErrorCode::IO_ERROR);
    
    EXPECT_FALSE(mapped_result.has_value());
    EXPECT_EQ(mapped_result.error().code, ErrorCode::IO_ERROR);
    EXPECT_EQ(mapped_result.error().message, "Test error");
}

// Test retry mechanism
TEST_F(ErrorHandlingTest, RetryMechanism) {
    int attempt_count = 0;
    auto failing_function = [&attempt_count]() -> Result<int> {
        attempt_count++;
        if (attempt_count < 3) {
            return errors::error<int>(ErrorCode::TIMEOUT, "Temporary failure");
        }
        return errors::success(42);
    };
    
    auto result = recovery::retry_with_backoff(
        5,                              // max attempts
        std::chrono::milliseconds(10),  // initial delay
        1.5,                            // backoff multiplier
        failing_function
    );
    
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);
    EXPECT_EQ(attempt_count, 3);
}

// Test retry with jitter
TEST_F(ErrorHandlingTest, RetryWithJitter) {
    int attempt_count = 0;
    auto failing_function = [&attempt_count]() -> Result<int> {
        attempt_count++;
        if (attempt_count < 2) {
            return errors::error<int>(ErrorCode::NETWORK_ERROR, "Network failure");
        }
        return errors::success(100);
    };
    
    auto result = recovery::retry_with_jitter(
        3,                              // max attempts
        std::chrono::milliseconds(10),  // base delay
        0.1,                            // jitter factor
        failing_function
    );
    
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 100);
    EXPECT_EQ(attempt_count, 2);
}

// Test circuit breaker
TEST_F(ErrorHandlingTest, CircuitBreaker) {
    recovery::CircuitBreaker breaker(2, std::chrono::milliseconds(100));
    
    auto failing_function = []() -> Result<bool> {
        return errors::error<bool>(ErrorCode::SERVICE_UNAVAILABLE, "Service down");
    };
    
    // First failure
    auto result1 = breaker.execute(failing_function);
    EXPECT_FALSE(result1.has_value());
    EXPECT_EQ(breaker.get_state(), recovery::CircuitBreaker::State::CLOSED);
    
    // Second failure - should open circuit
    auto result2 = breaker.execute(failing_function);
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(breaker.get_state(), recovery::CircuitBreaker::State::OPEN);
    
    // Third call should fail immediately (circuit open)
    auto result3 = breaker.execute(failing_function);
    EXPECT_FALSE(result3.has_value());
    EXPECT_EQ(result3.error().code, ErrorCode::SERVICE_UNAVAILABLE);
    
    // Wait for timeout and try again
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    auto result4 = breaker.execute(failing_function);
    EXPECT_FALSE(result4.has_value());
    EXPECT_EQ(breaker.get_state(), recovery::CircuitBreaker::State::HALF_OPEN);
}

// Test timeout wrapper
TEST_F(ErrorHandlingTest, TimeoutWrapper) {
    auto slow_function = []() -> Result<std::string> {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return errors::success("Completed");
    };
    
    // Should succeed with enough time
    auto result1 = recovery::with_timeout(
        std::chrono::milliseconds(200),
        slow_function
    );
    EXPECT_TRUE(result1.has_value());
    EXPECT_EQ(result1.value(), "Completed");
    
    // Should timeout with insufficient time
    auto result2 = recovery::with_timeout(
        std::chrono::milliseconds(50),
        slow_function
    );
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error().code, ErrorCode::TIMEOUT);
}

// Test fallback chain
TEST_F(ErrorHandlingTest, FallbackChain) {
    auto primary = []() -> Result<std::string> {
        return errors::error<std::string>(ErrorCode::SERVICE_UNAVAILABLE, "Primary down");
    };

    auto secondary = []() -> Result<std::string> {
        return errors::error<std::string>(ErrorCode::SERVICE_UNAVAILABLE, "Secondary down");
    };

    auto tertiary = []() -> Result<std::string> {
        return errors::success(std::string("Tertiary working"));
    };

    auto result = recovery::fallback_chain(primary, secondary, tertiary);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), std::string("Tertiary working"));
}

// Test error monitoring
TEST_F(ErrorHandlingTest, ErrorMonitoring) {
    monitoring::ErrorTracker tracker;
    
    // Record some successes and errors
    tracker.record_success();
    tracker.record_success();
    tracker.record_error(ErrorCode::INVALID_ARGUMENT);
    tracker.record_error(ErrorCode::IO_ERROR);
    tracker.record_error(ErrorCode::INVALID_ARGUMENT);
    
    EXPECT_EQ(tracker.get_error_rate(), 0.6); // 3 errors out of 5 total
    EXPECT_EQ(tracker.get_error_count(ErrorCode::INVALID_ARGUMENT), 2);
    EXPECT_EQ(tracker.get_error_count(ErrorCode::IO_ERROR), 1);
    EXPECT_EQ(tracker.get_error_count(ErrorCode::NETWORK_ERROR), 0);
}

// Test error rate limiter
TEST_F(ErrorHandlingTest, ErrorRateLimiter) {
    monitoring::ErrorRateLimiter limiter(0.5, std::chrono::milliseconds(1000));
    
    // Should allow operations initially
    EXPECT_TRUE(limiter.should_allow_operation());
    
    // Record some errors
    limiter.record_error();
    limiter.record_error();
    limiter.record_error();
    
    // Should still allow (3 errors in 1 second = 3 errors/second, which is < 0.5 * 1000)
    EXPECT_TRUE(limiter.should_allow_operation());
    
    // Record many more errors
    for (int i = 0; i < 10; ++i) {
        limiter.record_error();
    }
    
    // Should now limit operations
    EXPECT_FALSE(limiter.should_allow_operation());
}

// Test error string conversion
TEST_F(ErrorHandlingTest, ErrorStringConversion) {
    auto error_info = ErrorInfo(ErrorCode::INVALID_ARGUMENT, "Test error", "test context", "test.cpp", 42);
    
    std::string error_str = error_info.to_string();
    EXPECT_TRUE(error_str.find("INVALID_ARGUMENT") != std::string::npos);
    EXPECT_TRUE(error_str.find("Test error") != std::string::npos);
    EXPECT_TRUE(error_str.find("test context") != std::string::npos);
    EXPECT_TRUE(error_str.find("test.cpp:42") != std::string::npos);
}

// Test error chain string conversion
TEST_F(ErrorHandlingTest, ErrorChainStringConversion) {
    auto cause = std::make_shared<ErrorInfo>(ErrorCode::INVALID_ARGUMENT, "Root cause");
    auto error_info = ErrorInfo(ErrorCode::EXECUTION_ERROR, "Execution failed", "", "", 0, cause);
    
    std::string chain_str = error_info.chain_to_string();
    EXPECT_TRUE(chain_str.find("EXECUTION_ERROR") != std::string::npos);
    EXPECT_TRUE(chain_str.find("Execution failed") != std::string::npos);
    EXPECT_TRUE(chain_str.find("Caused by") != std::string::npos);
    EXPECT_TRUE(chain_str.find("INVALID_ARGUMENT") != std::string::npos);
    EXPECT_TRUE(chain_str.find("Root cause") != std::string::npos);
}

// Test error transformation
TEST_F(ErrorHandlingTest, ErrorTransformation) {
    auto error_result = errors::error<int>(ErrorCode::INVALID_ARGUMENT, "Original error");
    
    auto transformed_result = propagation::transform_error(
        std::move(error_result),
        [](const ErrorInfo& error) -> ErrorInfo {
            return ErrorInfo(ErrorCode::IO_ERROR, "Transformed: " + error.message);
        }
    );
    
    EXPECT_FALSE(transformed_result.has_value());
    EXPECT_EQ(transformed_result.error().code, ErrorCode::IO_ERROR);
    EXPECT_TRUE(transformed_result.error().message.find("Transformed: Original error") != std::string::npos);
}

// Test success transformation (should not transform)
TEST_F(ErrorHandlingTest, SuccessTransformation) {
    auto success_result = errors::success(42);
    
    auto transformed_result = propagation::transform_error(
        std::move(success_result),
        [](const ErrorInfo& error) -> ErrorInfo {
            return ErrorInfo(ErrorCode::IO_ERROR, "Should not be called");
        }
    );
    
    EXPECT_TRUE(transformed_result.has_value());
    EXPECT_EQ(transformed_result.value(), 42);
}

// Test error code to string conversion
TEST_F(ErrorHandlingTest, ErrorCodeToString) {
    EXPECT_EQ(error_code_to_string(ErrorCode::SUCCESS), "SUCCESS");
    EXPECT_EQ(error_code_to_string(ErrorCode::INVALID_ARGUMENT), "INVALID_ARGUMENT");
    EXPECT_EQ(error_code_to_string(ErrorCode::IO_ERROR), "IO_ERROR");
    EXPECT_EQ(error_code_to_string(ErrorCode::CRYPTO_ERROR), "CRYPTO_ERROR");
}

// Test user-friendly error messages
TEST_F(ErrorHandlingTest, UserFriendlyMessages) {
    EXPECT_EQ(get_user_friendly_message(ErrorCode::SUCCESS), "Operation completed successfully");
    EXPECT_EQ(get_user_friendly_message(ErrorCode::INVALID_ARGUMENT), "Invalid argument provided");
    EXPECT_EQ(get_user_friendly_message(ErrorCode::FILE_NOT_FOUND), "File not found");
    EXPECT_EQ(get_user_friendly_message(ErrorCode::INVALID_KEY), "Invalid cryptographic key");
}

// Test error conversion from std::error_code
TEST_F(ErrorHandlingTest, StdErrorCodeConversion) {
    std::error_code ec(ENOENT, std::system_category());
    auto error_info = conversion::from_std_error(ec, "File operation");
    
    EXPECT_EQ(error_info.code, ErrorCode::FILE_NOT_FOUND);
    EXPECT_TRUE(error_info.context.find("File operation") != std::string::npos);
}

// Test error conversion from exception
TEST_F(ErrorHandlingTest, ExceptionConversion) {
    std::runtime_error ex("Runtime error occurred");
    auto error_info = conversion::from_exception(ex, "Exception handling");
    
    EXPECT_EQ(error_info.code, ErrorCode::UNKNOWN_ERROR);
    EXPECT_EQ(error_info.message, "Runtime error occurred");
    EXPECT_TRUE(error_info.context.find("Exception handling") != std::string::npos);
}
