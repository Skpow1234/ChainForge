#pragma once

#include "chainforge/core/error.hpp"
#include <functional>
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <random>
#include <deque>
#include <unordered_map>
#include <semaphore>

namespace chainforge::core {

// Error propagation utilities
namespace propagation {

// Transform error with custom function
template<typename T, typename F>
auto transform_error(Result<T>&& result, F&& transform_func) -> Result<T> {
    if (result.has_value()) {
        return std::move(result);
    }
    
    auto new_error = transform_func(result.error());
    return std::unexpected(new_error);
}

// Map error code to different error code
template<typename T>
auto map_error_code(Result<T>&& result, ErrorCode new_code) -> Result<T> {
    if (result.has_value()) {
        return std::move(result);
    }
    
    auto new_error = ErrorInfo(new_code, result.error().message, result.error().context);
    return std::unexpected(new_error);
}

// Add context to error
template<typename T>
auto add_context(Result<T>&& result, std::string_view context) -> Result<T> {
    if (result.has_value()) {
        return std::move(result);
    }
    
    auto new_error = ErrorInfo(result.error().code, result.error().message, 
                              context.empty() ? result.error().context : 
                              std::string(result.error().context) + " -> " + std::string(context));
    return std::unexpected(new_error);
}

// Chain errors (for error propagation)
template<typename T>
auto chain_error(Result<T>&& result, ErrorCode new_code, std::string_view new_message) -> Result<T> {
    if (result.has_value()) {
        return std::move(result);
    }
    
    auto cause = std::make_shared<ErrorInfo>(result.error());
    auto new_error = ErrorInfo(new_code, new_message, "", "", 0, cause);
    return std::unexpected(new_error);
}

} // namespace propagation

// Error recovery utilities
namespace recovery {

// Retry with exponential backoff
template<typename F, typename... Args>
auto retry_with_backoff(int max_attempts, std::chrono::milliseconds initial_delay, 
                       double backoff_multiplier, F&& func, Args&&... args) 
    -> Result<std::invoke_result_t<F, Args...>> {
    
    auto delay = initial_delay;
    
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        auto result = func(std::forward<Args>(args)...);
        if (result.has_value()) {
            return result;
        }
        
        if (attempt < max_attempts - 1) {
            std::this_thread::sleep_for(delay);
            delay = std::chrono::milliseconds(static_cast<long long>(delay.count() * backoff_multiplier));
        }
    }
    
    return errors::error(ErrorCode::TIMEOUT, "Operation failed after maximum retry attempts");
}

// Retry with jitter
template<typename F, typename... Args>
auto retry_with_jitter(int max_attempts, std::chrono::milliseconds base_delay, 
                      double jitter_factor, F&& func, Args&&... args) 
    -> Result<std::invoke_result_t<F, Args...>> {
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(1.0 - jitter_factor, 1.0 + jitter_factor);
    
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        auto result = func(std::forward<Args>(args)...);
        if (result.has_value()) {
            return result;
        }
        
        if (attempt < max_attempts - 1) {
            auto jittered_delay = std::chrono::milliseconds(
                static_cast<long long>(base_delay.count() * dis(gen)));
            std::this_thread::sleep_for(jittered_delay);
        }
    }
    
    return errors::error(ErrorCode::TIMEOUT, "Operation failed after maximum retry attempts");
}

// Circuit breaker pattern
class CircuitBreaker {
public:
    enum class State { CLOSED, OPEN, HALF_OPEN };
    
    CircuitBreaker(int failure_threshold, std::chrono::milliseconds timeout)
        : failure_threshold_(failure_threshold), timeout_(timeout), 
          failure_count_(0), last_failure_time_(std::chrono::steady_clock::now()),
          state_(State::CLOSED) {}
    
    template<typename F, typename... Args>
    auto execute(F&& func, Args&&... args) -> Result<std::invoke_result_t<F, Args...>> {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (state_ == State::OPEN) {
            if (std::chrono::steady_clock::now() - last_failure_time_ >= timeout_) {
                state_ = State::HALF_OPEN;
            } else {
                return errors::error(ErrorCode::SERVICE_UNAVAILABLE, "Circuit breaker is open");
            }
        }
        
        auto result = func(std::forward<Args>(args)...);
        
        if (result.has_value()) {
            on_success();
        } else {
            on_failure();
        }
        
        return result;
    }
    
    State get_state() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return state_;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        state_ = State::CLOSED;
        failure_count_ = 0;
    }
    
private:
    void on_success() {
        failure_count_ = 0;
        state_ = State::CLOSED;
    }
    
    void on_failure() {
        ++failure_count_;
        last_failure_time_ = std::chrono::steady_clock::now();
        
        if (failure_count_ >= failure_threshold_) {
            state_ = State::OPEN;
        }
    }
    
    int failure_threshold_;
    std::chrono::milliseconds timeout_;
    std::atomic<int> failure_count_;
    std::chrono::steady_clock::time_point last_failure_time_;
    mutable std::mutex mutex_;
    State state_;
};

// Timeout wrapper
template<typename F, typename... Args>
auto with_timeout(std::chrono::milliseconds timeout, F&& func, Args&&... args) 
    -> Result<std::invoke_result_t<F, Args...>> {
    
    auto future = std::async(std::launch::async, [&]() {
        return func(std::forward<Args>(args)...);
    });
    
    if (future.wait_for(timeout) == std::future_status::timeout) {
        return errors::error(ErrorCode::TIMEOUT, "Operation timed out");
    }
    
    return future.get();
}

// Fallback with multiple strategies
template<typename F1, typename F2, typename F3, typename... Args>
auto fallback_chain(F1&& primary, F2&& secondary, F3&& tertiary, Args&&... args) 
    -> Result<std::invoke_result_t<F1, Args...>> {
    
    // Try primary
    auto result = primary(std::forward<Args>(args)...);
    if (result.has_value()) {
        return result;
    }
    
    // Try secondary
    result = secondary(std::forward<Args>(args)...);
    if (result.has_value()) {
        return result;
    }
    
    // Try tertiary
    return tertiary(std::forward<Args>(args)...);
}

// Bulkhead pattern for resource isolation
template<typename F, typename... Args>
auto with_bulkhead(std::shared_ptr<std::counting_semaphore<>> semaphore, 
                  F&& func, Args&&... args) 
    -> Result<std::invoke_result_t<F, Args...>> {
    
    if (!semaphore->try_acquire()) {
        return errors::error(ErrorCode::RESOURCE_EXHAUSTED, "Bulkhead capacity exceeded");
    }
    
    auto result = func(std::forward<Args>(args)...);
    semaphore->release();
    return result;
}

} // namespace recovery

// Error monitoring and metrics
namespace monitoring {

class ErrorTracker {
public:
    void record_error(ErrorCode code) {
        std::lock_guard<std::mutex> lock(mutex_);
        error_counts_[code]++;
        total_errors_++;
    }
    
    void record_success() {
        std::lock_guard<std::mutex> lock(mutex_);
        total_successes_++;
    }
    
    double get_error_rate() const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto total = total_errors_ + total_successes_;
        return total > 0 ? static_cast<double>(total_errors_) / total : 0.0;
    }
    
    int get_error_count(ErrorCode code) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = error_counts_.find(code);
        return it != error_counts_.end() ? it->second : 0;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        error_counts_.clear();
        total_errors_ = 0;
        total_successes_ = 0;
    }
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<ErrorCode, int> error_counts_;
    int total_errors_ = 0;
    int total_successes_ = 0;
};

// Error rate limiter
class ErrorRateLimiter {
public:
    ErrorRateLimiter(double max_error_rate, std::chrono::milliseconds window_size)
        : max_error_rate_(max_error_rate), window_size_(window_size) {}
    
    bool should_allow_operation() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto now = std::chrono::steady_clock::now();
        auto window_start = now - window_size_;
        
        // Remove old entries
        while (!error_times_.empty() && error_times_.front() < window_start) {
            error_times_.pop_front();
        }
        
        // Check if we're within the error rate limit
        double current_rate = static_cast<double>(error_times_.size()) / 
                             std::chrono::duration<double>(window_size_).count();
        
        return current_rate <= max_error_rate_;
    }
    
    void record_error() {
        std::lock_guard<std::mutex> lock(mutex_);
        error_times_.push_back(std::chrono::steady_clock::now());
    }
    
private:
    double max_error_rate_;
    std::chrono::milliseconds window_size_;
    mutable std::mutex mutex_;
    std::deque<std::chrono::steady_clock::time_point> error_times_;
};

} // namespace monitoring

} // namespace chainforge::core
