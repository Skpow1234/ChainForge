#pragma once

#include <variant>
#include <type_traits>
#include <utility>

namespace chainforge::core {

// Forward declaration
template<typename E>
class unexpected;

// Custom implementation of std::expected for C++20 compatibility
// This provides the same interface as std::expected but works with older compilers

template<typename T, typename E>
class expected {
private:
    std::variant<T, E> data_;

public:
    // Constructors
    expected() = default;
    
    expected(const T& value) : data_(value) {}
    expected(T&& value) : data_(std::move(value)) {}
    
    expected(const E& error) : data_(error) {}
    expected(E&& error) : data_(std::move(error)) {}
    expected(const unexpected<E>& u) : data_(u.error()) {}
    expected(unexpected<E>&& u) : data_(std::move(u.error())) {}
    
    // Copy and move constructors
    expected(const expected&) = default;
    expected(expected&&) = default;
    
    // Assignment operators
    expected& operator=(const expected&) = default;
    expected& operator=(expected&&) = default;
    
    // Value access
    bool has_value() const noexcept {
        return std::holds_alternative<T>(data_);
    }
    
    explicit operator bool() const noexcept {
        return has_value();
    }
    
    const T& value() const & {
        if (!has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<T>(data_);
    }
    
    T& value() & {
        if (!has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<T>(data_);
    }
    
    T&& value() && {
        if (!has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<T>(std::move(data_));
    }
    
    const T&& value() const && {
        if (!has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<T>(std::move(data_));
    }
    
    // Error access
    const E& error() const & {
        if (has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<E>(data_);
    }
    
    E& error() & {
        if (has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<E>(data_);
    }
    
    E&& error() && {
        if (has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<E>(std::move(data_));
    }
    
    const E&& error() const && {
        if (has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<E>(std::move(data_));
    }
    
    // Dereference operators
    const T& operator*() const & {
        return value();
    }
    
    T& operator*() & {
        return value();
    }
    
    T&& operator*() && {
        return std::move(*this).value();
    }
    
    const T&& operator*() const && {
        return std::move(*this).value();
    }
    
    const T* operator->() const {
        return &value();
    }
    
    T* operator->() {
        return &value();
    }
    
    // Value or default
    template<typename U>
    T value_or(U&& default_value) const & {
        return has_value() ? value() : static_cast<T>(std::forward<U>(default_value));
    }
    
    template<typename U>
    T value_or(U&& default_value) && {
        return has_value() ? std::move(*this).value() : static_cast<T>(std::forward<U>(default_value));
    }
    
    // Transform
    template<typename F>
    auto transform(F&& f) const & -> expected<std::invoke_result_t<F, const T&>, E> {
        using ResultType = std::invoke_result_t<F, const T&>;
        if (has_value()) {
            return expected<ResultType, E>(std::forward<F>(f)(value()));
        } else {
            return expected<ResultType, E>(error());
        }
    }
    
    template<typename F>
    auto transform(F&& f) && -> expected<std::invoke_result_t<F, T&&>, E> {
        using ResultType = std::invoke_result_t<F, T&&>;
        if (has_value()) {
            return expected<ResultType, E>(std::forward<F>(f)(std::move(*this).value()));
        } else {
            return expected<ResultType, E>(std::move(*this).error());
        }
    }
    
    // Transform error
    template<typename F>
    auto transform_error(F&& f) const & -> expected<T, std::invoke_result_t<F, const E&>> {
        using ErrorType = std::invoke_result_t<F, const E&>;
        if (has_value()) {
            return expected<T, ErrorType>(value());
        } else {
            return expected<T, ErrorType>(std::forward<F>(f)(error()));
        }
    }
    
    template<typename F>
    auto transform_error(F&& f) && -> expected<T, std::invoke_result_t<F, E&&>> {
        using ErrorType = std::invoke_result_t<F, E&&>;
        if (has_value()) {
            return expected<T, ErrorType>(std::move(*this).value());
        } else {
            return expected<T, ErrorType>(std::forward<F>(f)(std::move(*this).error()));
        }
    }
    
    // And then
    template<typename F>
    auto and_then(F&& f) const & -> std::invoke_result_t<F, const T&> {
        if (has_value()) {
            return std::forward<F>(f)(value());
        } else {
            using ResultType = std::invoke_result_t<F, const T&>;
            return ResultType(error());
        }
    }
    
    template<typename F>
    auto and_then(F&& f) && -> std::invoke_result_t<F, T&&> {
        if (has_value()) {
            return std::forward<F>(f)(std::move(*this).value());
        } else {
            using ResultType = std::invoke_result_t<F, T&&>;
            return ResultType(std::move(*this).error());
        }
    }
    
    // Or else
    template<typename F>
    auto or_else(F&& f) const & -> expected<T, std::invoke_result_t<F, const E&>> {
        using ErrorType = std::invoke_result_t<F, const E&>;
        if (has_value()) {
            return expected<T, ErrorType>(value());
        } else {
            return std::forward<F>(f)(error());
        }
    }
    
    template<typename F>
    auto or_else(F&& f) && -> expected<T, std::invoke_result_t<F, E&&>> {
        using ErrorType = std::invoke_result_t<F, E&&>;
        if (has_value()) {
            return expected<T, ErrorType>(std::move(*this).value());
        } else {
            return std::forward<F>(f)(std::move(*this).error());
        }
    }
    
    // Comparison operators
    bool operator==(const expected& other) const {
        if (has_value() != other.has_value()) {
            return false;
        }
        if (has_value()) {
            return value() == other.value();
        } else {
            return error() == other.error();
        }
    }
    
    bool operator!=(const expected& other) const {
        return !(*this == other);
    }
};

// Specialization for void
template<typename E>
class expected<void, E> {
private:
    std::variant<std::monostate, E> data_;

public:
    // Constructors
    expected() : data_(std::monostate{}) {}
    expected(const E& error) : data_(error) {}
    expected(E&& error) : data_(std::move(error)) {}
    expected(const unexpected<E>& u) : data_(u.error()) {}
    expected(unexpected<E>&& u) : data_(std::move(u.error())) {}
    
    // Copy and move constructors
    expected(const expected&) = default;
    expected(expected&&) = default;
    
    // Assignment operators
    expected& operator=(const expected&) = default;
    expected& operator=(expected&&) = default;
    
    // Value access
    bool has_value() const noexcept {
        return std::holds_alternative<std::monostate>(data_);
    }
    
    explicit operator bool() const noexcept {
        return has_value();
    }
    
    void value() const {
        if (!has_value()) {
            throw std::bad_variant_access{};
        }
    }
    
    // Error access
    const E& error() const & {
        if (has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<E>(data_);
    }
    
    E& error() & {
        if (has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<E>(data_);
    }
    
    E&& error() && {
        if (has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<E>(std::move(data_));
    }
    
    const E&& error() const && {
        if (has_value()) {
            throw std::bad_variant_access{};
        }
        return std::get<E>(std::move(data_));
    }
    
    // Transform
    template<typename F>
    auto transform(F&& f) const & -> expected<std::invoke_result_t<F>, E> {
        using ResultType = std::invoke_result_t<F>;
        if (has_value()) {
            return expected<ResultType, E>(std::forward<F>(f)());
        } else {
            return expected<ResultType, E>(error());
        }
    }
    
    template<typename F>
    auto transform(F&& f) && -> expected<std::invoke_result_t<F>, E> {
        using ResultType = std::invoke_result_t<F>;
        if (has_value()) {
            return expected<ResultType, E>(std::forward<F>(f)());
        } else {
            return expected<ResultType, E>(std::move(*this).error());
        }
    }
    
    // Transform error
    template<typename F>
    auto transform_error(F&& f) const & -> expected<void, std::invoke_result_t<F, const E&>> {
        using ErrorType = std::invoke_result_t<F, const E&>;
        if (has_value()) {
            return expected<void, ErrorType>();
        } else {
            return expected<void, ErrorType>(std::forward<F>(f)(error()));
        }
    }
    
    template<typename F>
    auto transform_error(F&& f) && -> expected<void, std::invoke_result_t<F, E&&>> {
        using ErrorType = std::invoke_result_t<F, E&&>;
        if (has_value()) {
            return expected<void, ErrorType>();
        } else {
            return expected<void, ErrorType>(std::forward<F>(f)(std::move(*this).error()));
        }
    }
    
    // And then
    template<typename F>
    auto and_then(F&& f) const & -> std::invoke_result_t<F> {
        if (has_value()) {
            return std::forward<F>(f)();
        } else {
            using ResultType = std::invoke_result_t<F>;
            return ResultType(error());
        }
    }
    
    template<typename F>
    auto and_then(F&& f) && -> std::invoke_result_t<F> {
        if (has_value()) {
            return std::forward<F>(f)();
        } else {
            using ResultType = std::invoke_result_t<F>;
            return ResultType(std::move(*this).error());
        }
    }
    
    // Or else
    template<typename F>
    auto or_else(F&& f) const & -> expected<void, std::invoke_result_t<F, const E&>> {
        using ErrorType = std::invoke_result_t<F, const E&>;
        if (has_value()) {
            return expected<void, ErrorType>();
        } else {
            return std::forward<F>(f)(error());
        }
    }
    
    template<typename F>
    auto or_else(F&& f) && -> expected<void, std::invoke_result_t<F, E&&>> {
        using ErrorType = std::invoke_result_t<F, E&&>;
        if (has_value()) {
            return expected<void, ErrorType>();
        } else {
            return std::forward<F>(f)(std::move(*this).error());
        }
    }
    
    // Comparison operators
    bool operator==(const expected& other) const {
        if (has_value() != other.has_value()) {
            return false;
        }
        if (has_value()) {
            return true; // Both have value (void)
        } else {
            return error() == other.error();
        }
    }
    
    bool operator!=(const expected& other) const {
        return !(*this == other);
    }
};

// unexpected helper
template<typename E>
class unexpected {
private:
    E error_;

public:
    template<typename U = E>
    unexpected(U&& error) : error_(std::forward<U>(error)) {}
    
    const E& error() const & { return error_; }
    E& error() & { return error_; }
    E&& error() && { return std::move(error_); }
    const E&& error() const && { return std::move(error_); }
};

// Helper function to create unexpected
template<typename E>
unexpected<E> make_unexpected(E&& error) {
    return unexpected<E>(std::forward<E>(error));
}

} // namespace chainforge::core
