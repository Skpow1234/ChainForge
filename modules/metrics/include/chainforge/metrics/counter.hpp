#pragma once

#include <prometheus/counter.h>
#include <memory>
#include <string>
#include <map>

namespace chainforge::metrics {

/// Thread-safe counter metric for ChainForge
/// Counters are monotonically increasing values that represent cumulative metrics
class Counter {
public:
    /// Constructor
    explicit Counter(prometheus::Counter& prometheus_counter, const std::string& name);
    
    /// Destructor
    ~Counter() = default;
    
    // Disable copy, enable move
    Counter(const Counter&) = delete;
    Counter& operator=(const Counter&) = delete;
    Counter(Counter&&) = default;
    Counter& operator=(Counter&&) = default;
    
    /// Increment counter by 1
    void increment();
    
    /// Increment counter by specified value
    void increment(double value);
    
    /// Get current counter value
    double value() const;
    
    /// Get counter name
    const std::string& name() const { return name_; }
    
    /// Reset counter to 0 (for testing only)
    void reset();

private:
    prometheus::Counter& prometheus_counter_;
    std::string name_;
};

/// RAII helper for timing operations with counters
class CounterTimer {
public:
    /// Start timer and increment counter when destroyed
    explicit CounterTimer(std::shared_ptr<Counter> counter);
    
    /// Destructor - increments the counter
    ~CounterTimer();
    
    // Disable copy and move
    CounterTimer(const CounterTimer&) = delete;
    CounterTimer& operator=(const CounterTimer&) = delete;
    CounterTimer(CounterTimer&&) = delete;
    CounterTimer& operator=(CounterTimer&&) = delete;

private:
    std::shared_ptr<Counter> counter_;
};

/// Convenience macros for counter metrics
#define CHAINFORGE_COUNTER_INC(counter_name) \
    do { \
        auto counter = ::chainforge::metrics::get_metrics_registry().create_counter( \
            counter_name, "Auto-generated counter from macro"); \
        counter->increment(); \
    } while(0)

#define CHAINFORGE_COUNTER_ADD(counter_name, value) \
    do { \
        auto counter = ::chainforge::metrics::get_metrics_registry().create_counter( \
            counter_name, "Auto-generated counter from macro"); \
        counter->increment(value); \
    } while(0)

} // namespace chainforge::metrics
