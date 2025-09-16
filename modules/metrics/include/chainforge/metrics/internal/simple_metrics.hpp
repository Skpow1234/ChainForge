#pragma once

#include <atomic>
#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <memory>

namespace chainforge::metrics::internal {

/// Simple counter implementation without external dependencies
class SimpleCounter {
public:
    SimpleCounter() : value_(0.0) {}
    
    void increment() { value_.fetch_add(1.0, std::memory_order_relaxed); }
    void increment(double val) { 
        if (val >= 0) {
            value_.fetch_add(val, std::memory_order_relaxed);
        }
    }
    
    double value() const { return value_.load(std::memory_order_relaxed); }

private:
    std::atomic<double> value_;
};

/// Simple gauge implementation without external dependencies
class SimpleGauge {
public:
    SimpleGauge() : value_(0.0) {}
    
    void set(double val) { value_.store(val, std::memory_order_relaxed); }
    void increment(double val = 1.0) { value_.fetch_add(val, std::memory_order_relaxed); }
    void decrement(double val = 1.0) { value_.fetch_sub(val, std::memory_order_relaxed); }
    
    double value() const { return value_.load(std::memory_order_relaxed); }

private:
    std::atomic<double> value_;
};

/// Simple histogram implementation without external dependencies
class SimpleHistogram {
public:
    explicit SimpleHistogram(const std::vector<double>& buckets = {});
    
    void observe(double value);
    
    struct Stats {
        uint64_t count = 0;
        double sum = 0.0;
        std::map<double, uint64_t> buckets;
    };
    
    Stats get_stats() const;

private:
    mutable std::mutex mutex_;
    std::vector<double> buckets_;
    std::map<double, std::atomic<uint64_t>> bucket_counts_;
    std::atomic<uint64_t> count_;
    std::atomic<double> sum_;
};

/// Simple registry for managing metrics
class SimpleRegistry {
public:
    static SimpleRegistry& instance();
    
    std::shared_ptr<SimpleCounter> get_counter(const std::string& name);
    std::shared_ptr<SimpleGauge> get_gauge(const std::string& name);
    std::shared_ptr<SimpleHistogram> get_histogram(const std::string& name, const std::vector<double>& buckets = {});
    
    void clear();
    size_t metrics_count() const;
    
    /// Export metrics in Prometheus format
    std::string export_metrics() const;

private:
    SimpleRegistry() = default;
    
    mutable std::mutex mutex_;
    std::map<std::string, std::shared_ptr<SimpleCounter>> counters_;
    std::map<std::string, std::shared_ptr<SimpleGauge>> gauges_;
    std::map<std::string, std::shared_ptr<SimpleHistogram>> histograms_;
};

} // namespace chainforge::metrics::internal
