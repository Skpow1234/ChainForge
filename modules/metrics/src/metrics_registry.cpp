#include "tokiwa/metrics/metrics_registry.hpp"
#include "tokiwa/metrics/counter.hpp"
#include "tokiwa/metrics/gauge.hpp"
#include "tokiwa/metrics/histogram.hpp"
#include <sstream>

namespace tokiwa::metrics {

MetricsRegistry& MetricsRegistry::instance() {
    static MetricsRegistry instance;
    return instance;
}

MetricsRegistry::MetricsRegistry() = default;

std::shared_ptr<Counter> MetricsRegistry::create_counter(
    const std::string& name,
    const std::string& /*help*/,
    const std::map<std::string, std::string>& labels) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string key = generate_metric_key(name, labels);
    
    auto it = counters_.find(key);
    if (it != counters_.end()) {
        return it->second;
    }
    
    auto simple_counter = internal::SimpleRegistry::instance().get_counter(key);
    auto counter = std::make_shared<Counter>(simple_counter, name);
    
    counters_[key] = counter;
    return counter;
}

std::shared_ptr<Gauge> MetricsRegistry::create_gauge(
    const std::string& name,
    const std::string& /*help*/,
    const std::map<std::string, std::string>& labels) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string key = generate_metric_key(name, labels);
    
    auto it = gauges_.find(key);
    if (it != gauges_.end()) {
        return it->second;
    }
    
    auto simple_gauge = internal::SimpleRegistry::instance().get_gauge(key);
    auto gauge = std::make_shared<Gauge>(simple_gauge, name);
    
    gauges_[key] = gauge;
    return gauge;
}

std::shared_ptr<Histogram> MetricsRegistry::create_histogram(
    const std::string& name,
    const std::string& /*help*/,
    const std::vector<double>& buckets,
    const std::map<std::string, std::string>& labels) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string key = generate_metric_key(name, labels);
    
    auto it = histograms_.find(key);
    if (it != histograms_.end()) {
        return it->second;
    }
    
    std::vector<double> final_buckets = buckets.empty() ? get_default_histogram_buckets() : buckets;
    auto simple_histogram = internal::SimpleRegistry::instance().get_histogram(key, final_buckets);
    auto histogram = std::make_shared<Histogram>(simple_histogram, name);
    
    histograms_[key] = histogram;
    return histogram;
}

std::string MetricsRegistry::export_metrics() const {
    return internal::SimpleRegistry::instance().export_metrics();
}

void MetricsRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    counters_.clear();
    gauges_.clear();
    histograms_.clear();
    
    internal::SimpleRegistry::instance().clear();
}

size_t MetricsRegistry::metrics_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return counters_.size() + gauges_.size() + histograms_.size();
}

std::string MetricsRegistry::generate_metric_key(
    const std::string& name,
    const std::map<std::string, std::string>& labels) const {
    
    std::ostringstream oss;
    oss << name;
    
    if (!labels.empty()) {
        oss << "{";
        bool first = true;
        for (const auto& [key, value] : labels) {
            if (!first) oss << ",";
            oss << key << "=" << value;
            first = false;
        }
        oss << "}";
    }
    
    return oss.str();
}

std::vector<double> MetricsRegistry::get_default_histogram_buckets() const {
    return {0.001, 0.01, 0.1, 1, 10, 100, 1000};
}

MetricsRegistry& get_metrics_registry() {
    return MetricsRegistry::instance();
}

} // namespace tokiwa::metrics
