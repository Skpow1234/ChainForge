#include "chainforge/core/error.hpp"
#include "chainforge/core/error_utils.hpp"
#include <iostream>
#include <random>
#include <thread>

using namespace chainforge::core;

// Example function that can fail
Result<int> risky_operation(int input) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1, 10);
    
    // Simulate 30% failure rate
    if (dis(gen) <= 3) {
        return errors::error(ErrorCode::INVALID_ARGUMENT, "Random failure occurred");
    }
    
    return errors::success(input * 2);
}

// Example function with timeout
Result<std::string> slow_operation(int delay_ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
    return errors::success("Operation completed");
}

// Example function that uses circuit breaker
Result<bool> unreliable_service() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1, 10);
    
    // Simulate 70% failure rate
    if (dis(gen) <= 7) {
        return errors::error(ErrorCode::SERVICE_UNAVAILABLE, "Service is down");
    }
    
    return errors::success(true);
}

// Example of error chaining
Result<int> operation_a() {
    auto result = risky_operation(5);
    if (!result.has_value()) {
        return propagation::chain_error(std::move(result), 
                                      ErrorCode::EXECUTION_ERROR, 
                                      "Operation A failed");
    }
    return result;
}

Result<int> operation_b() {
    auto result = operation_a();
    if (!result.has_value()) {
        return propagation::add_context(std::move(result), "in operation_b");
    }
    return result;
}

// Example of retry with backoff
Result<int> retry_example() {
    return recovery::retry_with_backoff(
        3,                           // max attempts
        std::chrono::milliseconds(100), // initial delay
        2.0,                         // backoff multiplier
        risky_operation,             // function to retry
        10                          // argument
    );
}

// Example of circuit breaker usage
void circuit_breaker_example() {
    recovery::CircuitBreaker breaker(3, std::chrono::seconds(5));
    
    for (int i = 0; i < 10; ++i) {
        auto result = breaker.execute(unreliable_service);
        
        if (result.has_value()) {
            std::cout << "Service call " << i << " succeeded\n";
        } else {
            std::cout << "Service call " << i << " failed: " 
                      << result.error().to_string() << "\n";
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

// Example of timeout usage
void timeout_example() {
    // This should succeed
    auto result1 = recovery::with_timeout(
        std::chrono::milliseconds(2000),
        slow_operation,
        1000  // 1 second delay
    );
    
    if (result1.has_value()) {
        std::cout << "Timeout test 1 succeeded: " << result1.value() << "\n";
    } else {
        std::cout << "Timeout test 1 failed: " << result1.error().to_string() << "\n";
    }
    
    // This should timeout
    auto result2 = recovery::with_timeout(
        std::chrono::milliseconds(500),
        slow_operation,
        1000  // 1 second delay
    );
    
    if (result2.has_value()) {
        std::cout << "Timeout test 2 succeeded: " << result2.value() << "\n";
    } else {
        std::cout << "Timeout test 2 failed: " << result2.error().to_string() << "\n";
    }
}

// Example of error monitoring
void monitoring_example() {
    monitoring::ErrorTracker tracker;
    
    // Simulate some operations
    for (int i = 0; i < 100; ++i) {
        auto result = risky_operation(i);
        if (result.has_value()) {
            tracker.record_success();
        } else {
            tracker.record_error(result.error().code);
        }
    }
    
    std::cout << "Error rate: " << (tracker.get_error_rate() * 100) << "%\n";
    std::cout << "Invalid argument errors: " 
              << tracker.get_error_count(ErrorCode::INVALID_ARGUMENT) << "\n";
}

// Example of fallback chain
Result<std::string> primary_service() {
    return errors::error(ErrorCode::SERVICE_UNAVAILABLE, "Primary service down");
}

Result<std::string> secondary_service() {
    return errors::error(ErrorCode::SERVICE_UNAVAILABLE, "Secondary service down");
}

Result<std::string> tertiary_service() {
    return errors::success("Tertiary service working");
}

void fallback_example() {
    auto result = recovery::fallback_chain(
        primary_service,
        secondary_service,
        tertiary_service
    );
    
    if (result.has_value()) {
        std::cout << "Fallback succeeded: " << result.value() << "\n";
    } else {
        std::cout << "All services failed: " << result.error().to_string() << "\n";
    }
}

int main() {
    std::cout << "=== ChainForge Error Handling Examples ===\n\n";
    
    // Basic error handling
    std::cout << "1. Basic Error Handling:\n";
    auto result = risky_operation(5);
    if (result.has_value()) {
        std::cout << "Success: " << result.value() << "\n";
    } else {
        std::cout << "Error: " << result.error().to_string() << "\n";
    }
    
    // Error chaining
    std::cout << "\n2. Error Chaining:\n";
    auto chained_result = operation_b();
    if (!chained_result.has_value()) {
        std::cout << "Chained error: " << chained_result.error().chain_to_string() << "\n";
    }
    
    // Retry with backoff
    std::cout << "\n3. Retry with Backoff:\n";
    auto retry_result = retry_example();
    if (retry_result.has_value()) {
        std::cout << "Retry succeeded: " << retry_result.value() << "\n";
    } else {
        std::cout << "Retry failed: " << retry_result.error().to_string() << "\n";
    }
    
    // Circuit breaker
    std::cout << "\n4. Circuit Breaker:\n";
    circuit_breaker_example();
    
    // Timeout
    std::cout << "\n5. Timeout:\n";
    timeout_example();
    
    // Error monitoring
    std::cout << "\n6. Error Monitoring:\n";
    monitoring_example();
    
    // Fallback chain
    std::cout << "\n7. Fallback Chain:\n";
    fallback_example();
    
    return 0;
}
