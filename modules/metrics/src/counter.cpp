#include "chainforge/metrics/counter.hpp"
#include <stdexcept>

namespace chainforge::metrics {

Counter::Counter(std::shared_ptr<internal::SimpleCounter> simple_counter, const std::string& name)
    : simple_counter_(simple_counter), name_(name) {
}

void Counter::increment() {
    simple_counter_->increment();
}

void Counter::increment(double value) {
    if (value < 0) {
        throw std::invalid_argument("Counter increment value must be non-negative");
    }
    simple_counter_->increment(value);
}

double Counter::value() const {
    return simple_counter_->value();
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
