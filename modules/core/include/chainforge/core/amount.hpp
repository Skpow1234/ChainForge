#pragma once

#include <cstdint>
#include <string>
#include <compare>
#include <stdexcept>
#include <limits>

namespace chainforge::core {

/**
 * Amount class for handling cryptocurrency amounts
 * Provides precise arithmetic operations with overflow protection
 */
class Amount {
public:
    // Type definitions
    using value_type = uint64_t;
    using decimal_type = uint32_t;
    
    // Constants
    static constexpr value_type MAX_VALUE = std::numeric_limits<value_type>::max();
    static constexpr decimal_type DECIMALS = 18; // Standard for most cryptocurrencies
    static constexpr value_type WEI_PER_ETHER = 1'000'000'000'000'000'000ULL; // 10^18
    
    // Constructors
    Amount() = default;
    explicit Amount(value_type wei);
    Amount(value_type whole, decimal_type fraction);
    
    // Copy and move
    Amount(const Amount&) = default;
    Amount(Amount&&) = default;
    Amount& operator=(const Amount&) = default;
    Amount& operator=(Amount&&) = default;
    
    // Destructor
    ~Amount() = default;
    
    // Accessors
    value_type wei() const noexcept { return wei_; }
    value_type whole() const noexcept;
    decimal_type fraction() const noexcept;
    
    // Conversion methods
    std::string to_string() const;
    std::string to_hex() const;
    double to_double() const;
    
    // Arithmetic operators
    Amount operator+(const Amount& other) const;
    Amount operator-(const Amount& other) const;
    Amount operator*(value_type multiplier) const;
    Amount operator/(value_type divisor) const;
    
    // Assignment operators
    Amount& operator+=(const Amount& other);
    Amount& operator-=(const Amount& other);
    Amount& operator*=(value_type multiplier);
    Amount& operator/=(value_type divisor);
    
    // Comparison operators (C++20 three-way comparison)
    std::strong_ordering operator<=>(const Amount& other) const noexcept;
    bool operator==(const Amount& other) const noexcept;
    
    // Utility methods
    bool is_zero() const noexcept;
    bool is_negative() const noexcept;
    Amount abs() const noexcept;
    
    // Static factory methods
    static Amount from_wei(value_type wei);
    static Amount from_ether(double ether);
    static Amount from_string(const std::string& str);
    static Amount zero() noexcept;
    static Amount max() noexcept;
    
    // Validation
    bool is_valid() const noexcept;

private:
    value_type wei_;
    
    // Helper methods
    void validate_overflow(value_type result, const char* operation) const;
    static value_type parse_decimal(const std::string& decimal_str);
};

// Free functions  
Amount operator*(Amount::value_type multiplier, const Amount& amount);
std::ostream& operator<<(std::ostream& os, const Amount& amount);
std::istream& operator>>(std::istream& is, Amount& amount);

// Utility functions
Amount min(const Amount& a, const Amount& b);
Amount max(const Amount& a, const Amount& b);

} // namespace chainforge::core
