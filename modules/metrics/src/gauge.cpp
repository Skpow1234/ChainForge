#include "chainforge/metrics/gauge.hpp"
#include <chrono>

namespace chainforge::metrics {

Gauge::Gauge(prometheus::Gauge& prometheus_gauge, const std::string& name)
    : prometheus_gauge_(prometheus_gauge), name_(name) {
}

void Gauge::set(double value) {
    prometheus_gauge_.Set(value);
}

void Gauge::increment(double value) {
    prometheus_gauge_.Increment(value);
}

void Gauge::decrement(double value) {
    prometheus_gauge_.Decrement(value);
}

double Gauge::value() const {
    return prometheus_gauge_.Value();
}

void Gauge::set_to_current_time() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    set(static_cast<double>(timestamp));
}

GaugeTracker::GaugeTracker(std::shared_ptr<Gauge> gauge, double value)
    : gauge_(std::move(gauge)), tracked_value_(value) {
    if (gauge_) {
        gauge_->increment(tracked_value_);
    }
}

GaugeTracker::~GaugeTracker() {
    if (gauge_) {
        gauge_->decrement(tracked_value_);
    }
}

void GaugeTracker::update(double new_value) {
    if (gauge_) {
        // Remove old value and add new value
        gauge_->decrement(tracked_value_);
        gauge_->increment(new_value);
        tracked_value_ = new_value;
    }
}

} // namespace chainforge::metrics
