#pragma once

#include "types.hpp"
#include <string>
#include <array>
#include <vector>

namespace chainforge::core {

/**
 * Address class for blockchain addresses
 * Provides a type-safe wrapper around address values
 */
class Address {
public:
    // Constructors
    Address() = default;
    explicit Address(const Address160& data);
    explicit Address(const std::string& hex_string);
    explicit Address(const std::vector<uint8_t>& data);
    
    // Copy and move
    Address(const Address&) = default;
    Address(Address&&) = default;
    Address& operator=(const Address&) = default;
    Address& operator=(Address&&) = default;
    
    // Destructor
    ~Address() = default;
    
    // Accessors
    const Address160& data() const noexcept { return data_; }
    Address160& data() noexcept { return data_; }
    
    // Conversion methods
    std::string to_hex() const;
    std::vector<uint8_t> to_bytes() const;
    
    // Validation methods
    bool is_valid() const noexcept;
    bool is_contract() const noexcept;
    bool is_zero() const noexcept;
    
    // Comparison operators
    bool operator==(const Address& other) const noexcept;
    bool operator!=(const Address& other) const noexcept;
    bool operator<(const Address& other) const noexcept;
    
    // Utility methods
    static Address zero() noexcept;
    static Address random();
    static Address from_public_key(const std::vector<uint8_t>& public_key);
    
    // Size information
    static constexpr size_t size() noexcept { return ADDRESS_SIZE; }

private:
    Address160 data_;
};

// Free functions
Address address_from_hex(const std::string& hex_string);
std::string address_to_hex(const Address& address);
bool is_valid_address(const std::string& hex_string);

// Address derivation
Address derive_address_from_public_key(const std::vector<uint8_t>& public_key);
Address derive_contract_address(const Address& sender, uint64_t nonce);

} // namespace chainforge::core
