#pragma once

#include <cstdint>
#include <string>
#include <chrono>
#include <compare>
#include <stdexcept>

namespace chainforge::core {

/**
 * Timestamp class for blockchain timestamps
 * Provides a type-safe wrapper around timestamp values
 */
class Timestamp {
public:
    // Type definitions
    using value_type = uint64_t;
    using duration_type = std::chrono::seconds;
    
    // Constants
    static constexpr value_type MAX_VALUE = std::numeric_limits<value_type>::max();
    static constexpr value_type MIN_VALUE = 0;
    
    // Constructors
    Timestamp() = default;
    explicit Timestamp(value_type seconds_since_epoch);
    explicit Timestamp(const std::chrono::system_clock::time_point& time_point);
    
    // Copy and move
    Timestamp(const Timestamp&) = default;
    Timestamp(Timestamp&&) = default;
    Timestamp& operator=(const Timestamp&) = default;
    Timestamp& operator=(Timestamp&&) = default;
    
    // Destructor
    ~Timestamp() = default;
    
    // Accessors
    value_type seconds() const noexcept { return seconds_; }
    value_type milliseconds() const noexcept;
    value_type microseconds() const noexcept;
    
    // Conversion methods
    std::string to_string() const;
    std::string to_iso8601() const;
    std::chrono::system_clock::time_point to_time_point() const;
    
    // Arithmetic operators
    Timestamp operator+(duration_type duration) const;
    Timestamp operator-(duration_type duration) const;
    duration_type operator-(const Timestamp& other) const;
    
    // Assignment operators
    Timestamp& operator+=(duration_type duration);
    Timestamp& operator-=(duration_type duration);
    
    // Comparison operators (C++20 three-way comparison)
    std::strong_ordering operator<=>(const Timestamp& other) const noexcept;
    bool operator==(const Timestamp& other) const noexcept;
    
    // Utility methods
    bool is_zero() const noexcept;
    bool is_valid() const noexcept;
    bool is_future() const noexcept;
    bool is_past() const noexcept;
    
    // Static factory methods
    static Timestamp now() noexcept;
    static Timestamp from_seconds(value_type seconds);
    static Timestamp from_milliseconds(value_type milliseconds);
    static Timestamp from_iso8601(const std::string& iso_string);
    static Timestamp zero() noexcept;
    static Timestamp max() noexcept;
    
    // Validation
    static bool is_valid_timestamp(value_type seconds);

private:
    value_type seconds_;
    
    // Helper methods
    void validate_timestamp(value_type seconds) const;
    static value_type parse_iso8601(const std::string& iso_string);
};

// Free functions
Timestamp operator+(duration_type duration, const Timestamp& timestamp);
std::ostream& operator<<(std::ostream& os, const Timestamp& timestamp);
std::istream& operator>>(std::istream& is, Timestamp& timestamp);

// Utility functions
Timestamp min(const Timestamp& a, const Timestamp& b);
Timestamp max(const Timestamp& a, const Timestamp& b);
bool is_valid_timestamp_range(const Timestamp& start, const Timestamp& end);

// Duration helpers
std::chrono::seconds duration_between(const Timestamp& start, const Timestamp& end);
std::chrono::milliseconds duration_ms_between(const Timestamp& start, const Timestamp& end);

} // namespace chainforge::core
