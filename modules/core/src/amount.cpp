#include "chainforge/core/amount.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <algorithm>

namespace chainforge::core {

Amount::Amount(value_type wei) : wei_(wei) {}

Amount::Amount(value_type whole, decimal_type fraction) {
    // Convert to wei: whole * 10^18 + fraction
    if (whole > MAX_VALUE / WEI_PER_ETHER) {
        throw std::overflow_error("Amount whole part too large");
    }
    wei_ = whole * WEI_PER_ETHER + fraction;
}

Amount::value_type Amount::whole() const noexcept {
    return wei_ / WEI_PER_ETHER;
}

Amount::decimal_type Amount::fraction() const noexcept {
    return static_cast<decimal_type>(wei_ % WEI_PER_ETHER);
}

std::string Amount::to_string() const {
    std::stringstream ss;
    ss << whole() << ".";

    auto frac = fraction();
    if (frac == 0) {
        ss << "0";
    } else {
        // Remove trailing zeros
        std::string frac_str = std::to_string(frac);
        // Pad with leading zeros if necessary
        while (frac_str.length() < DECIMALS) {
            frac_str = "0" + frac_str;
        }
        // Remove trailing zeros
        frac_str.erase(frac_str.find_last_not_of('0') + 1);
        if (frac_str.empty()) {
            frac_str = "0";
        }
        ss << frac_str;
    }

    return ss.str();
}

std::string Amount::to_hex() const {
    std::stringstream ss;
    ss << "0x" << std::hex << wei_;
    return ss.str();
}

double Amount::to_double() const {
    return static_cast<double>(wei_) / static_cast<double>(WEI_PER_ETHER);
}

Amount Amount::operator+(const Amount& other) const {
    validate_overflow(wei_ + other.wei_, "addition");
    return Amount(wei_ + other.wei_);
}

Amount Amount::operator-(const Amount& other) const {
    if (wei_ < other.wei_) {
        throw std::underflow_error("Amount subtraction underflow");
    }
    return Amount(wei_ - other.wei_);
}

Amount Amount::operator*(value_type multiplier) const {
    if (multiplier == 0) {
        return zero();
    }

    if (wei_ > MAX_VALUE / multiplier) {
        throw std::overflow_error("Amount multiplication overflow");
    }

    return Amount(wei_ * multiplier);
}

Amount Amount::operator/(value_type divisor) const {
    if (divisor == 0) {
        throw std::domain_error("Division by zero");
    }

    return Amount(wei_ / divisor);
}

Amount& Amount::operator+=(const Amount& other) {
    *this = *this + other;
    return *this;
}

Amount& Amount::operator-=(const Amount& other) {
    *this = *this - other;
    return *this;
}

Amount& Amount::operator*=(value_type multiplier) {
    *this = *this * multiplier;
    return *this;
}

Amount& Amount::operator/=(value_type divisor) {
    *this = *this / divisor;
    return *this;
}

std::strong_ordering Amount::operator<=>(const Amount& other) const noexcept {
    return wei_ <=> other.wei_;
}

bool Amount::operator==(const Amount& other) const noexcept {
    return wei_ == other.wei_;
}

bool Amount::is_zero() const noexcept {
    return wei_ == 0;
}

bool Amount::is_negative() const noexcept {
    return false;  // Amounts are always non-negative
}

Amount Amount::abs() const noexcept {
    return *this;  // Amounts are always non-negative
}

Amount Amount::from_wei(value_type wei) {
    return Amount(wei);
}

Amount Amount::from_ether(double ether) {
    if (ether < 0 || ether > static_cast<double>(MAX_VALUE) / WEI_PER_ETHER) {
        throw std::out_of_range("Ether amount out of range");
    }

    return Amount(static_cast<value_type>(ether * WEI_PER_ETHER));
}

Amount Amount::from_string(const std::string& str) {
    // Parse string like "123.456"
    size_t dot_pos = str.find('.');
    if (dot_pos == std::string::npos) {
        // No decimal point
        return Amount(std::stoull(str) * WEI_PER_ETHER);
    }

    // Parse whole and fractional parts
    value_type whole = std::stoull(str.substr(0, dot_pos));
    std::string frac_str = str.substr(dot_pos + 1);

    // Limit fractional part to DECIMALS digits
    if (frac_str.length() > DECIMALS) {
        frac_str = frac_str.substr(0, DECIMALS);
    }

    // Pad with zeros if necessary
    while (frac_str.length() < DECIMALS) {
        frac_str += "0";
    }

    decimal_type fraction = static_cast<decimal_type>(std::stoul(frac_str));

    return Amount(whole, fraction);
}

Amount Amount::zero() noexcept {
    return Amount(0);
}

Amount Amount::max() noexcept {
    return Amount(MAX_VALUE);
}

bool Amount::is_valid() const noexcept {
    return true;  // All constructed amounts are valid
}

void Amount::validate_overflow(value_type result, const char* operation) const {
    if (result < wei_ || result < wei_) {  // Check for overflow
        throw std::overflow_error(std::string("Amount ") + operation + " overflow");
    }
}

// Free functions
Amount operator*(Amount::value_type multiplier, const Amount& amount) {
    return amount * multiplier;
}

std::ostream& operator<<(std::ostream& os, const Amount& amount) {
    os << amount.to_string();
    return os;
}

std::istream& operator>>(std::istream& is, Amount& amount) {
    std::string str;
    is >> str;
    amount = Amount::from_string(str);
    return is;
}

Amount min(const Amount& a, const Amount& b) {
    return (a < b) ? a : b;
}

Amount max(const Amount& a, const Amount& b) {
    return (a > b) ? a : b;
}

} // namespace chainforge::core
