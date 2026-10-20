#include "tokiwa/metrics/gauge.hpp"
#include <chrono>

namespace tokiwa::metrics {

Gauge::Gauge(std::shared_ptr<internal::SimpleGauge> simple_gauge, const std::string& name)
    : simple_gauge_(std::move(simple_gauge)), name_(name) {
}

void Gauge::set(double value) {
    simple_gauge_->set(value);
}

void Gauge::increment(double value) {
    simple_gauge_->increment(value);
}

void Gauge::decrement(double value) {
    simple_gauge_->decrement(value);
}

double Gauge::value() const {
    return simple_gauge_->value();
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
        gauge_->decrement(tracked_value_);
        gauge_->increment(new_value);
        tracked_value_ = new_value;
    }
}

} // namespace tokiwa::metrics
