#include "chainforge/metrics/histogram.hpp"

namespace chainforge::metrics {

Histogram::Histogram(prometheus::Histogram& prometheus_histogram, const std::string& name)
    : prometheus_histogram_(prometheus_histogram), name_(name) {
}

void Histogram::observe(double value) {
    prometheus_histogram_.Observe(value);
}

void Histogram::observe_duration(std::chrono::duration<double> duration) {
    prometheus_histogram_.Observe(duration.count());
}

void Histogram::observe_duration_ms(std::chrono::milliseconds duration) {
    // Convert to seconds for Prometheus
    double seconds = duration.count() / 1000.0;
    prometheus_histogram_.Observe(seconds);
}

void Histogram::observe_duration_us(std::chrono::microseconds duration) {
    // Convert to seconds for Prometheus
    double seconds = duration.count() / 1000000.0;
    prometheus_histogram_.Observe(seconds);
}

HistogramTimer::HistogramTimer(std::shared_ptr<Histogram> histogram)
    : histogram_(std::move(histogram))
    , start_time_(std::chrono::steady_clock::now()) {
}

HistogramTimer::~HistogramTimer() {
    if (histogram_) {
        auto duration = elapsed();
        histogram_->observe_duration_us(duration);
    }
}

std::chrono::microseconds HistogramTimer::elapsed() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - start_time_);
}

void HistogramTimer::record_and_reset() {
    if (histogram_) {
        auto duration = elapsed();
        histogram_->observe_duration_us(duration);
        start_time_ = std::chrono::steady_clock::now();
    }
}

// Histogram bucket definitions
namespace buckets {
    const std::vector<double> DEFAULT_TIMING = {0.001, 0.01, 0.1, 1, 10, 100, 1000};
    
    const std::vector<double> HTTP_REQUEST_DURATION = {
        0.001, 0.005, 0.01, 0.025, 0.05, 0.1, 0.25, 0.5, 1, 2.5, 5, 10
    };
    
    const std::vector<double> DB_QUERY_DURATION = {
        0.0001, 0.001, 0.01, 0.1, 1, 10
    };
    
    const std::vector<double> SIZE_BYTES = {
        1024,        // 1KB
        10240,       // 10KB
        102400,      // 100KB
        1048576,     // 1MB
        10485760,    // 10MB
        104857600,   // 100MB
        1073741824   // 1GB
    };
    
    const std::vector<double> BLOCK_PROCESSING = {
        0.001, 0.01, 0.1, 1, 5, 10, 30, 60
    };
    
    const std::vector<double> TRANSACTION_PROCESSING = {
        0.0001, 0.001, 0.01, 0.1, 1
    };
} // namespace buckets

} // namespace chainforge::metrics
