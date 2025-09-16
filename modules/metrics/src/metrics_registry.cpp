#include "chainforge/metrics/metrics_registry.hpp"
#include "chainforge/metrics/counter.hpp"
#include "chainforge/metrics/gauge.hpp"
#include "chainforge/metrics/histogram.hpp"
#include <sstream>

namespace chainforge::metrics {

MetricsRegistry& MetricsRegistry::instance() {
    static MetricsRegistry instance;
    return instance;
}

MetricsRegistry::MetricsRegistry() {
    registry_ = std::make_shared<prometheus::Registry>();
}

std::shared_ptr<Counter> MetricsRegistry::create_counter(
    const std::string& name,
    const std::string& help,
    const std::map<std::string, std::string>& labels) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string key = generate_metric_key(name, labels);
    
    // Check if counter already exists
    auto it = counters_.find(key);
    if (it != counters_.end()) {
        return it->second;
    }
    
    // Get or create counter family
    auto family_it = counter_families_.find(name);
    prometheus::Family<prometheus::Counter>* family;
    
    if (family_it == counter_families_.end()) {
        family = &prometheus::BuildCounter()
                     .Name(name)
                     .Help(help)
                     .Register(*registry_);
        counter_families_[name] = family;
    } else {
        family = family_it->second;
    }
    
    // Create counter with labels
    auto& prometheus_counter = family->Add(labels);
    auto counter = std::make_shared<Counter>(prometheus_counter, name);
    
    counters_[key] = counter;
    return counter;
}

std::shared_ptr<Gauge> MetricsRegistry::create_gauge(
    const std::string& name,
    const std::string& help,
    const std::map<std::string, std::string>& labels) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string key = generate_metric_key(name, labels);
    
    // Check if gauge already exists
    auto it = gauges_.find(key);
    if (it != gauges_.end()) {
        return it->second;
    }
    
    // Get or create gauge family
    auto family_it = gauge_families_.find(name);
    prometheus::Family<prometheus::Gauge>* family;
    
    if (family_it == gauge_families_.end()) {
        family = &prometheus::BuildGauge()
                     .Name(name)
                     .Help(help)
                     .Register(*registry_);
        gauge_families_[name] = family;
    } else {
        family = family_it->second;
    }
    
    // Create gauge with labels
    auto& prometheus_gauge = family->Add(labels);
    auto gauge = std::make_shared<Gauge>(prometheus_gauge, name);
    
    gauges_[key] = gauge;
    return gauge;
}

std::shared_ptr<Histogram> MetricsRegistry::create_histogram(
    const std::string& name,
    const std::string& help,
    const std::vector<double>& buckets,
    const std::map<std::string, std::string>& labels) {
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string key = generate_metric_key(name, labels);
    
    // Check if histogram already exists
    auto it = histograms_.find(key);
    if (it != histograms_.end()) {
        return it->second;
    }
    
    // Get or create histogram family
    auto family_it = histogram_families_.find(name);
    prometheus::Family<prometheus::Histogram>* family;
    
    if (family_it == histogram_families_.end()) {
        auto builder = prometheus::BuildHistogram()
                          .Name(name)
                          .Help(help);
        
        family = &builder.Register(*registry_);
        histogram_families_[name] = family;
    } else {
        family = family_it->second;
    }
    
    // Use provided buckets or default
    std::vector<double> final_buckets = buckets.empty() ? get_default_histogram_buckets() : buckets;
    
    // Create histogram with labels and buckets
    auto& prometheus_histogram = family->Add(labels, final_buckets);
    auto histogram = std::make_shared<Histogram>(prometheus_histogram, name);
    
    histograms_[key] = histogram;
    return histogram;
}

void MetricsRegistry::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    counters_.clear();
    gauges_.clear();
    histograms_.clear();
    counter_families_.clear();
    gauge_families_.clear();
    histogram_families_.clear();
    
    // Create new registry
    registry_ = std::make_shared<prometheus::Registry>();
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

} // namespace chainforge::metrics
