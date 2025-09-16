#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <prometheus/registry.h>
#include <prometheus/counter.h>
#include <prometheus/gauge.h>
#include <prometheus/histogram.h>

namespace chainforge::metrics {

/// Forward declarations
class Counter;
class Gauge;
class Histogram;

/// Metrics registry for managing all metrics in ChainForge
class MetricsRegistry {
public:
    /// Get singleton instance
    static MetricsRegistry& instance();
    
    /// Destructor
    ~MetricsRegistry() = default;
    
    // Disable copy and move
    MetricsRegistry(const MetricsRegistry&) = delete;
    MetricsRegistry& operator=(const MetricsRegistry&) = delete;
    MetricsRegistry(MetricsRegistry&&) = delete;
    MetricsRegistry& operator=(MetricsRegistry&&) = delete;
    
    /// Create or get counter metric
    std::shared_ptr<Counter> create_counter(
        const std::string& name,
        const std::string& help,
        const std::map<std::string, std::string>& labels = {}
    );
    
    /// Create or get gauge metric
    std::shared_ptr<Gauge> create_gauge(
        const std::string& name,
        const std::string& help,
        const std::map<std::string, std::string>& labels = {}
    );
    
    /// Create or get histogram metric
    std::shared_ptr<Histogram> create_histogram(
        const std::string& name,
        const std::string& help,
        const std::vector<double>& buckets = {},
        const std::map<std::string, std::string>& labels = {}
    );
    
    /// Get underlying Prometheus registry
    std::shared_ptr<prometheus::Registry> prometheus_registry() const {
        return registry_;
    }
    
    /// Clear all metrics (for testing)
    void clear();
    
    /// Get metrics count
    size_t metrics_count() const;

private:
    MetricsRegistry();
    
    mutable std::mutex mutex_;
    std::shared_ptr<prometheus::Registry> registry_;
    
    // Metric families
    std::unordered_map<std::string, prometheus::Family<prometheus::Counter>*> counter_families_;
    std::unordered_map<std::string, prometheus::Family<prometheus::Gauge>*> gauge_families_;
    std::unordered_map<std::string, prometheus::Family<prometheus::Histogram>*> histogram_families_;
    
    // Cached metrics
    std::unordered_map<std::string, std::shared_ptr<Counter>> counters_;
    std::unordered_map<std::string, std::shared_ptr<Gauge>> gauges_;
    std::unordered_map<std::string, std::shared_ptr<Histogram>> histograms_;
    
    /// Generate unique key for metric with labels
    std::string generate_metric_key(const std::string& name, const std::map<std::string, std::string>& labels) const;
    
    /// Get default histogram buckets
    std::vector<double> get_default_histogram_buckets() const;
};

/// Convenience function to get the metrics registry
MetricsRegistry& get_metrics_registry();

} // namespace chainforge::metrics
