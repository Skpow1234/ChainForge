#include "chainforge/core/timestamp.hpp"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <regex>

namespace chainforge::core {

Timestamp::Timestamp(value_type seconds_since_epoch) : seconds_(seconds_since_epoch) {
    validate_timestamp(seconds_since_epoch);
}

Timestamp::Timestamp(const std::chrono::system_clock::time_point& time_point) {
    auto duration = time_point.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    seconds_ = static_cast<value_type>(seconds.count());
    validate_timestamp(seconds_);
}

Timestamp::value_type Timestamp::milliseconds() const noexcept {
    return seconds_ * 1000;
}

Timestamp::value_type Timestamp::microseconds() const noexcept {
    return seconds_ * 1'000'000;
}

std::string Timestamp::to_string() const {
    std::stringstream ss;
    ss << seconds_;
    return ss.str();
}

std::string Timestamp::to_iso8601() const {
    auto time_point = to_time_point();
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    std::stringstream ss;
    ss << std::put_time(std::gmtime(&time_t), "%Y-%m-%dT%H:%M:%SZ");
    return ss.str();
}

std::chrono::system_clock::time_point Timestamp::to_time_point() const {
    return std::chrono::system_clock::time_point(std::chrono::seconds(seconds_));
}

Timestamp Timestamp::operator+(duration_type duration) const {
    auto result_seconds = seconds_ + duration.count();
    if (result_seconds > MAX_VALUE) {
        throw std::overflow_error("Timestamp addition overflow");
    }
    return Timestamp(static_cast<value_type>(result_seconds));
}

Timestamp Timestamp::operator-(duration_type duration) const {
    if (duration.count() > static_cast<std::chrono::seconds::rep>(seconds_)) {
        throw std::underflow_error("Timestamp subtraction underflow");
    }
    return Timestamp(seconds_ - duration.count());
}

Timestamp::duration_type Timestamp::operator-(const Timestamp& other) const {
    if (seconds_ < other.seconds_) {
        throw std::underflow_error("Timestamp difference underflow");
    }
    return duration_type(seconds_ - other.seconds_);
}

Timestamp& Timestamp::operator+=(duration_type duration) {
    *this = *this + duration;
    return *this;
}

Timestamp& Timestamp::operator-=(duration_type duration) {
    *this = *this - duration;
    return *this;
}

std::strong_ordering Timestamp::operator<=>(const Timestamp& other) const noexcept {
    return seconds_ <=> other.seconds_;
}

bool Timestamp::operator==(const Timestamp& other) const noexcept {
    return seconds_ == other.seconds_;
}

bool Timestamp::is_zero() const noexcept {
    return seconds_ == 0;
}

bool Timestamp::is_valid() const noexcept {
    return is_valid_timestamp(seconds_);
}

bool Timestamp::is_future() const noexcept {
    auto now = std::chrono::system_clock::now();
    auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    return seconds_ > static_cast<value_type>(now_seconds);
}

bool Timestamp::is_past() const noexcept {
    auto now = std::chrono::system_clock::now();
    auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    return seconds_ < static_cast<value_type>(now_seconds);
}

Timestamp Timestamp::now() noexcept {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    return Timestamp(static_cast<value_type>(seconds.count()));
}

Timestamp Timestamp::from_seconds(value_type seconds) {
    return Timestamp(seconds);
}

Timestamp Timestamp::from_milliseconds(value_type milliseconds) {
    return Timestamp(milliseconds / 1000);
}

Timestamp Timestamp::from_iso8601(const std::string& iso_string) {
    return Timestamp(parse_iso8601(iso_string));
}

Timestamp Timestamp::zero() noexcept {
    return Timestamp(0);
}

Timestamp Timestamp::max() noexcept {
    return Timestamp(MAX_VALUE);
}

bool Timestamp::is_valid_timestamp(value_type seconds) {
    return seconds >= MIN_VALUE && seconds <= MAX_VALUE;
}

void Timestamp::validate_timestamp(value_type seconds) const {
    if (!is_valid_timestamp(seconds)) {
        throw std::out_of_range("Invalid timestamp value");
    }
}

Timestamp::value_type Timestamp::parse_iso8601(const std::string& iso_string) {
    // Simple ISO8601 parser for YYYY-MM-DDTHH:MM:SSZ format
    std::regex pattern(R"((\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})Z)");
    std::smatch matches;

    if (!std::regex_match(iso_string, matches, pattern)) {
        throw std::invalid_argument("Invalid ISO8601 format");
    }

    std::tm tm = {};
    tm.tm_year = std::stoi(matches[1]) - 1900;
    tm.tm_mon = std::stoi(matches[2]) - 1;
    tm.tm_mday = std::stoi(matches[3]);
    tm.tm_hour = std::stoi(matches[4]);
    tm.tm_min = std::stoi(matches[5]);
    tm.tm_sec = std::stoi(matches[6]);

    auto time_t = std::mktime(&tm);
    if (time_t == -1) {
        throw std::invalid_argument("Invalid date/time in ISO8601 string");
    }

    return static_cast<value_type>(time_t);
}

// Free functions
Timestamp operator+(duration_type duration, const Timestamp& timestamp) {
    return timestamp + duration;
}

std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp) {
    os << timestamp.to_string();
    return os;
}

std::istream& operator>>(std::istream& is, Timestamp& timestamp) {
    value_type seconds;
    is >> seconds;
    timestamp = Timestamp(seconds);
    return is;
}

Timestamp min(const Timestamp& a, const Timestamp& b) {
    return (a < b) ? a : b;
}

Timestamp max(const Timestamp& a, const Timestamp& b) {
    return (a > b) ? a : b;
}

bool is_valid_timestamp_range(const Timestamp& start, const Timestamp& end) {
    return start <= end;
}

std::chrono::seconds duration_between(const Timestamp& start, const Timestamp& end) {
    return end - start;
}

std::chrono::milliseconds duration_ms_between(const Timestamp& start, const Timestamp& end) {
    auto seconds = end.seconds() - start.seconds();
    return std::chrono::milliseconds(seconds * 1000);
}

} // namespace chainforge::core
