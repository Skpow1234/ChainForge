#pragma once

#include <prometheus/gauge.h>
#include <memory>
#include <string>
#include <map>
#include <chrono>

namespace chainforge::metrics {

/// Thread-safe gauge metric for ChainForge
/// Gauges are values that can go up and down, representing point-in-time measurements
class Gauge {
public:
    /// Constructor
    explicit Gauge(prometheus::Gauge& prometheus_gauge, const std::string& name);
    
    /// Destructor
    ~Gauge() = default;
    
    // Disable copy, enable move
    Gauge(const Gauge&) = delete;
    Gauge& operator=(const Gauge&) = delete;
    Gauge(Gauge&&) = default;
    Gauge& operator=(Gauge&&) = default;
    
    /// Set gauge to specific value
    void set(double value);
    
    /// Increment gauge by specified value
    void increment(double value = 1.0);
    
    /// Decrement gauge by specified value
    void decrement(double value = 1.0);
    
    /// Get current gauge value
    double value() const;
    
    /// Get gauge name
    const std::string& name() const { return name_; }
    
    /// Set to current timestamp
    void set_to_current_time();

private:
    prometheus::Gauge& prometheus_gauge_;
    std::string name_;
};

/// RAII helper for tracking resource usage with gauges
class GaugeTracker {
public:
    /// Constructor - increments gauge
    explicit GaugeTracker(std::shared_ptr<Gauge> gauge, double value = 1.0);
    
    /// Destructor - decrements gauge
    ~GaugeTracker();
    
    // Disable copy and move
    GaugeTracker(const GaugeTracker&) = delete;
    GaugeTracker& operator=(const GaugeTracker&) = delete;
    GaugeTracker(GaugeTracker&&) = delete;
    GaugeTracker& operator=(GaugeTracker&&) = delete;
    
    /// Update the tracked value
    void update(double new_value);

private:
    std::shared_ptr<Gauge> gauge_;
    double tracked_value_;
};

/// Convenience macros for gauge metrics
#define CHAINFORGE_GAUGE_SET(gauge_name, value) \
    do { \
        auto gauge = ::chainforge::metrics::get_metrics_registry().create_gauge( \
            gauge_name, "Auto-generated gauge from macro"); \
        gauge->set(value); \
    } while(0)

#define CHAINFORGE_GAUGE_INC(gauge_name) \
    do { \
        auto gauge = ::chainforge::metrics::get_metrics_registry().create_gauge( \
            gauge_name, "Auto-generated gauge from macro"); \
        gauge->increment(); \
    } while(0)

#define CHAINFORGE_GAUGE_DEC(gauge_name) \
    do { \
        auto gauge = ::chainforge::metrics::get_metrics_registry().create_gauge( \
            gauge_name, "Auto-generated gauge from macro"); \
        gauge->decrement(); \
    } while(0)

#define CHAINFORGE_GAUGE_TRACK(gauge_name, value) \
    ::chainforge::metrics::GaugeTracker gauge_tracker_##__LINE__( \
        ::chainforge::metrics::get_metrics_registry().create_gauge( \
            gauge_name, "Auto-generated gauge from macro"), value)

} // namespace chainforge::metrics
