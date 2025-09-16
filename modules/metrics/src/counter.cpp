#include "chainforge/metrics/counter.hpp"
#include <stdexcept>

namespace chainforge::metrics {

Counter::Counter(prometheus::Counter& prometheus_counter, const std::string& name)
    : prometheus_counter_(prometheus_counter), name_(name) {
}

void Counter::increment() {
    prometheus_counter_.Increment();
}

void Counter::increment(double value) {
    if (value < 0) {
        throw std::invalid_argument("Counter increment value must be non-negative");
    }
    prometheus_counter_.Increment(value);
}

double Counter::value() const {
    return prometheus_counter_.Value();
}

void Counter::reset() {
    // Note: Prometheus counters don't support reset in production
    // This is mainly for testing purposes
    // In real Prometheus, counters are monotonic
}

CounterTimer::CounterTimer(std::shared_ptr<Counter> counter) 
    : counter_(std::move(counter)) {
}

CounterTimer::~CounterTimer() {
    if (counter_) {
        counter_->increment();
    }
}

} // namespace chainforge::metrics
