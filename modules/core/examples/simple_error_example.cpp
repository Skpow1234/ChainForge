#include "chainforge/core/error.hpp"
#include "chainforge/core/error_utils.hpp"
#include <iostream>

using namespace chainforge::core;

// Simple example demonstrating the new error handling system
int main() {
    std::cout << "=== Simple Error Handling Example ===\n\n";
    
    // 1. Basic success and error
    std::cout << "1. Basic Success and Error:\n";
    
    auto success_result = errors::success(42);
    if (success_result.has_value()) {
        std::cout << "Success: " << success_result.value() << "\n";
    }
    
    auto error_result = errors::error<int>(ErrorCode::INVALID_ARGUMENT, "Invalid input");
    if (!error_result.has_value()) {
        std::cout << "Error: " << error_result.error().to_string() << "\n";
    }
    
    // 2. Error with context
    std::cout << "\n2. Error with Context:\n";
    
    auto context_error = errors::error<int>(ErrorCode::FILE_NOT_FOUND, "File not found", "config.yaml");
    if (!context_error.has_value()) {
        std::cout << "Context Error: " << context_error.error().to_string() << "\n";
    }
    
    // 3. Error chaining
    std::cout << "\n3. Error Chaining:\n";
    
    auto original_error = errors::error<int>(ErrorCode::INVALID_ARGUMENT, "Original error");
    auto chained_error = propagation::chain_error(
        std::move(original_error),
        ErrorCode::EXECUTION_ERROR,
        "Execution failed"
    );
    
    if (!chained_error.has_value()) {
        std::cout << "Chained Error: " << chained_error.error().chain_to_string() << "\n";
    }
    
    // 4. User-friendly messages
    std::cout << "\n4. User-Friendly Messages:\n";
    
    std::cout << "Error code: " << error_code_to_string(ErrorCode::INVALID_ARGUMENT) << "\n";
    std::cout << "User message: " << get_user_friendly_message(ErrorCode::INVALID_ARGUMENT) << "\n";
    
    // 5. Simple retry example
    std::cout << "\n5. Simple Retry Example:\n";
    
    int attempt_count = 0;
    auto failing_function = [&attempt_count]() -> Result<int> {
        attempt_count++;
        std::cout << "Attempt " << attempt_count << "\n";
        
        if (attempt_count < 3) {
            return errors::error<int>(ErrorCode::TIMEOUT, "Temporary failure");
        }
        
        return errors::success(100);
    };
    
    auto retry_result = recovery::retry_with_backoff(
        5,                              // max attempts
        std::chrono::milliseconds(100), // initial delay
        2.0,                            // backoff multiplier
        failing_function
    );
    
    if (retry_result.has_value()) {
        std::cout << "Retry succeeded: " << retry_result.value() << "\n";
    } else {
        std::cout << "Retry failed: " << retry_result.error().to_string() << "\n";
    }
    
    // 6. Error monitoring
    std::cout << "\n6. Error Monitoring:\n";
    
    monitoring::ErrorTracker tracker;
    
    // Simulate some operations
    for (int i = 0; i < 10; ++i) {
        if (i % 3 == 0) {
            tracker.record_error(ErrorCode::INVALID_ARGUMENT);
        } else {
            tracker.record_success();
        }
    }
    
    std::cout << "Error rate: " << (tracker.get_error_rate() * 100) << "%\n";
    std::cout << "Invalid argument errors: " 
              << tracker.get_error_count(ErrorCode::INVALID_ARGUMENT) << "\n";
    
    std::cout << "\n=== Example Complete ===\n";
    
    return 0;
}
