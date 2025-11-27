#pragma once

#include "chainforge/core/expected.hpp"
#include "chainforge/core/error.hpp"
#include <string>
#include <cstdint>
#include <chrono>
#include <vector>

namespace chainforge {
namespace p2p {

/**
 * @brief Peer address representing a network endpoint
 */
struct PeerAddress {
    std::string ip;
    uint16_t port;
    std::chrono::system_clock::time_point last_seen;
    uint32_t services{0};  // Bitmask of services offered
    
    PeerAddress() = default;
    
    PeerAddress(std::string ip_, uint16_t port_)
        : ip(std::move(ip_))
        , port(port_)
        , last_seen(std::chrono::system_clock::now())
        , services(0) {}
    
    PeerAddress(std::string ip_, uint16_t port_, uint32_t services_)
        : ip(std::move(ip_))
        , port(port_)
        , last_seen(std::chrono::system_clock::now())
        , services(services_) {}
    
    /**
     * @brief Get address as string (ip:port)
     */
    std::string to_string() const {
        return ip + ":" + std::to_string(port);
    }
    
    /**
     * @brief Check if address is valid
     */
    bool is_valid() const;
    
    /**
     * @brief Check if address is local
     */
    bool is_local() const;
    
    /**
     * @brief Check if address is routable
     */
    bool is_routable() const;
    
    /**
     * @brief Update last seen time
     */
    void update_last_seen() {
        last_seen = std::chrono::system_clock::now();
    }
    
    /**
     * @brief Get age in seconds
     */
    int64_t age_seconds() const {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - last_seen).count();
    }
    
    // Comparison operators
    bool operator==(const PeerAddress& other) const {
        return ip == other.ip && port == other.port;
    }
    
    bool operator!=(const PeerAddress& other) const {
        return !(*this == other);
    }
    
    bool operator<(const PeerAddress& other) const {
        if (ip != other.ip) return ip < other.ip;
        return port < other.port;
    }
};

/**
 * @brief Service flags for peer capabilities
 */
namespace ServiceFlags {
    constexpr uint32_t NODE_NETWORK = 1 << 0;      // Full node
    constexpr uint32_t NODE_BLOOM = 1 << 1;        // Bloom filter support
    constexpr uint32_t NODE_WITNESS = 1 << 2;      // Witness data support
    constexpr uint32_t NODE_COMPACT = 1 << 3;      // Compact blocks support
    constexpr uint32_t NODE_NETWORK_LIMITED = 1 << 4;  // Limited network (recent blocks only)
}

/**
 * @brief Peer information including connection state
 */
struct PeerInfo {
    PeerAddress address;
    std::string version;
    uint64_t height{0};
    int32_t ping_time_ms{-1};
    bool connected{false};
    uint32_t connection_attempts{0};
    std::chrono::system_clock::time_point last_attempt;
    std::chrono::system_clock::time_point connected_since;
    
    PeerInfo() = default;
    
    explicit PeerInfo(const PeerAddress& addr)
        : address(addr) {}
    
    /**
     * @brief Check if peer is stale (not seen recently)
     */
    bool is_stale(std::chrono::seconds max_age = std::chrono::hours(24)) const {
        return address.age_seconds() > max_age.count();
    }
    
    /**
     * @brief Check if peer should be attempted again
     */
    bool should_retry(std::chrono::seconds retry_interval = std::chrono::minutes(5)) const {
        if (connected) return false;
        
        auto now = std::chrono::system_clock::now();
        auto time_since_last_attempt = std::chrono::duration_cast<std::chrono::seconds>(
            now - last_attempt
        ).count();
        
        return time_since_last_attempt >= retry_interval.count();
    }
    
    /**
     * @brief Record connection attempt
     */
    void record_attempt() {
        connection_attempts++;
        last_attempt = std::chrono::system_clock::now();
    }
    
    /**
     * @brief Mark as connected
     */
    void mark_connected() {
        connected = true;
        connected_since = std::chrono::system_clock::now();
        address.update_last_seen();
    }
    
    /**
     * @brief Mark as disconnected
     */
    void mark_disconnected() {
        connected = false;
    }
};

/**
 * @brief Address validation functions
 */
namespace address_validation {

/**
 * @brief Validate IP address format
 */
bool is_valid_ip(const std::string& ip);

/**
 * @brief Check if IP is localhost
 */
bool is_localhost(const std::string& ip);

/**
 * @brief Check if IP is private
 */
bool is_private_ip(const std::string& ip);

/**
 * @brief Check if IP is routable
 */
bool is_routable_ip(const std::string& ip);

/**
 * @brief Check if port is valid
 */
bool is_valid_port(uint16_t port);

/**
 * @brief Validate peer address
 */
bool is_valid_peer_address(const PeerAddress& addr, bool allow_private = false);

} // namespace address_validation

} // namespace p2p
} // namespace chainforge

