#include "chainforge/metrics/internal/simple_metrics.hpp"
#include <algorithm>
#include <sstream>

namespace chainforge::metrics::internal {

SimpleHistogram::SimpleHistogram(const std::vector<double>& buckets) 
    : buckets_(buckets), count_(0), sum_(0.0) {
    
    if (buckets_.empty()) {
        // Default buckets
        buckets_ = {0.001, 0.01, 0.1, 1, 10, 100, 1000};
    }
    
    // Ensure buckets are sorted
    std::sort(buckets_.begin(), buckets_.end());
    
    // Initialize bucket counts
    for (double bucket : buckets_) {
        bucket_counts_[bucket] = 0;
    }
}

void SimpleHistogram::observe(double value) {
    count_.fetch_add(1, std::memory_order_relaxed);
    sum_.fetch_add(value, std::memory_order_relaxed);
    
    // Find appropriate bucket
    for (double bucket : buckets_) {
        if (value <= bucket) {
            bucket_counts_[bucket].fetch_add(1, std::memory_order_relaxed);
        }
    }
}

SimpleHistogram::Stats SimpleHistogram::get_stats() const {
    Stats stats;
    stats.count = count_.load(std::memory_order_relaxed);
    stats.sum = sum_.load(std::memory_order_relaxed);
    
    for (const auto& [bucket, counter] : bucket_counts_) {
        stats.buckets[bucket] = counter.load(std::memory_order_relaxed);
    }
    
    return stats;
}

SimpleRegistry& SimpleRegistry::instance() {
    static SimpleRegistry instance;
    return instance;
}

std::shared_ptr<SimpleCounter> SimpleRegistry::get_counter(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = counters_.find(name);
    if (it != counters_.end()) {
        return it->second;
    }
    
    auto counter = std::make_shared<SimpleCounter>();
    counters_[name] = counter;
    return counter;
}

std::shared_ptr<SimpleGauge> SimpleRegistry::get_gauge(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = gauges_.find(name);
    if (it != gauges_.end()) {
        return it->second;
    }
    
    auto gauge = std::make_shared<SimpleGauge>();
    gauges_[name] = gauge;
    return gauge;
}

std::shared_ptr<SimpleHistogram> SimpleRegistry::get_histogram(const std::string& name, const std::vector<double>& buckets) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = histograms_.find(name);
    if (it != histograms_.end()) {
        return it->second;
    }
    
    auto histogram = std::make_shared<SimpleHistogram>(buckets);
    histograms_[name] = histogram;
    return histogram;
}

void SimpleRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    counters_.clear();
    gauges_.clear();
    histograms_.clear();
}

size_t SimpleRegistry::metrics_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return counters_.size() + gauges_.size() + histograms_.size();
}

std::string SimpleRegistry::export_metrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    
    // Export counters
    for (const auto& [name, counter] : counters_) {
        oss << "# TYPE " << name << " counter\n";
        oss << name << " " << counter->value() << "\n";
    }
    
    // Export gauges
    for (const auto& [name, gauge] : gauges_) {
        oss << "# TYPE " << name << " gauge\n";
        oss << name << " " << gauge->value() << "\n";
    }
    
    // Export histograms
    for (const auto& [name, histogram] : histograms_) {
        auto stats = histogram->get_stats();
        
        oss << "# TYPE " << name << " histogram\n";
        
        // Bucket counts
        for (const auto& [bucket, count] : stats.buckets) {
            oss << name << "_bucket{le=\"" << bucket << "\"} " << count << "\n";
        }
        
        // +Inf bucket
        oss << name << "_bucket{le=\"+Inf\"} " << stats.count << "\n";
        
        // Total count and sum
        oss << name << "_count " << stats.count << "\n";
        oss << name << "_sum " << stats.sum << "\n";
    }
    
    return oss.str();
}

} // namespace chainforge::metrics::internal
