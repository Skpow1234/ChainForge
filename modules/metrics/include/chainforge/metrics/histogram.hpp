#pragma once

#include <prometheus/histogram.h>
#include <memory>
#include <string>
#include <map>
#include <chrono>

namespace chainforge::metrics {

/// Thread-safe histogram metric for ChainForge
/// Histograms track distributions of values and are ideal for timing measurements
class Histogram {
public:
    /// Constructor
    explicit Histogram(prometheus::Histogram& prometheus_histogram, const std::string& name);
    
    /// Destructor
    ~Histogram() = default;
    
    // Disable copy, enable move
    Histogram(const Histogram&) = delete;
    Histogram& operator=(const Histogram&) = delete;
    Histogram(Histogram&&) = default;
    Histogram& operator=(Histogram&&) = default;
    
    /// Observe a value
    void observe(double value);
    
    /// Observe timing in seconds
    void observe_duration(std::chrono::duration<double> duration);
    
    /// Observe timing in milliseconds
    void observe_duration_ms(std::chrono::milliseconds duration);
    
    /// Observe timing in microseconds
    void observe_duration_us(std::chrono::microseconds duration);
    
    /// Get histogram name
    const std::string& name() const { return name_; }

private:
    prometheus::Histogram& prometheus_histogram_;
    std::string name_;
};

/// RAII timer for histogram timing measurements
class HistogramTimer {
public:
    /// Start timer
    explicit HistogramTimer(std::shared_ptr<Histogram> histogram);
    
    /// Destructor - records elapsed time
    ~HistogramTimer();
    
    // Disable copy and move
    HistogramTimer(const HistogramTimer&) = delete;
    HistogramTimer& operator=(const HistogramTimer&) = delete;
    HistogramTimer(HistogramTimer&&) = delete;
    HistogramTimer& operator=(HistogramTimer&&) = delete;
    
    /// Get elapsed time so far
    std::chrono::microseconds elapsed() const;
    
    /// Record elapsed time and reset timer
    void record_and_reset();

private:
    std::shared_ptr<Histogram> histogram_;
    std::chrono::steady_clock::time_point start_time_;
};

/// Convenience macros for histogram metrics
#define CHAINFORGE_HISTOGRAM_OBSERVE(histogram_name, value) \
    do { \
        auto histogram = ::chainforge::metrics::get_metrics_registry().create_histogram( \
            histogram_name, "Auto-generated histogram from macro"); \
        histogram->observe(value); \
    } while(0)

#define CHAINFORGE_HISTOGRAM_TIMER(histogram_name) \
    ::chainforge::metrics::HistogramTimer histogram_timer_##__LINE__( \
        ::chainforge::metrics::get_metrics_registry().create_histogram( \
            histogram_name, "Auto-generated histogram from macro"))

/// Common histogram bucket configurations
namespace buckets {
    /// Default buckets: 0.001, 0.01, 0.1, 1, 10, 100, 1000 seconds
    extern const std::vector<double> DEFAULT_TIMING;
    
    /// HTTP request duration buckets: 0.001, 0.005, 0.01, 0.025, 0.05, 0.1, 0.25, 0.5, 1, 2.5, 5, 10 seconds
    extern const std::vector<double> HTTP_REQUEST_DURATION;
    
    /// Database query duration buckets: 0.0001, 0.001, 0.01, 0.1, 1, 10 seconds
    extern const std::vector<double> DB_QUERY_DURATION;
    
    /// Size buckets for bytes: 1KB, 10KB, 100KB, 1MB, 10MB, 100MB, 1GB
    extern const std::vector<double> SIZE_BYTES;
    
    /// Block processing time buckets: 0.001, 0.01, 0.1, 1, 5, 10, 30, 60 seconds
    extern const std::vector<double> BLOCK_PROCESSING;
    
    /// Transaction processing time buckets: 0.0001, 0.001, 0.01, 0.1, 1 seconds
    extern const std::vector<double> TRANSACTION_PROCESSING;
} // namespace buckets

} // namespace chainforge::metrics
