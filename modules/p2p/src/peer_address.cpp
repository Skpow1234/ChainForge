#include "chainforge/p2p/peer_address.hpp"
#include <boost/asio/ip/address.hpp>
#include <algorithm>
#include <regex>

namespace chainforge {
namespace p2p {

bool PeerAddress::is_valid() const {
    return address_validation::is_valid_peer_address(*this);
}

bool PeerAddress::is_local() const {
    return address_validation::is_localhost(ip) || address_validation::is_private_ip(ip);
}

bool PeerAddress::is_routable() const {
    return address_validation::is_routable_ip(ip);
}

namespace address_validation {

bool is_valid_ip(const std::string& ip) {
    try {
        boost::asio::ip::make_address(ip);
        return true;
    } catch (...) {
        return false;
    }
}

bool is_localhost(const std::string& ip) {
    if (ip == "127.0.0.1" || ip == "::1" || ip == "localhost") {
        return true;
    }
    
    try {
        auto addr = boost::asio::ip::make_address(ip);
        return addr.is_loopback();
    } catch (...) {
        return false;
    }
}

bool is_private_ip(const std::string& ip) {
    try {
        auto addr = boost::asio::ip::make_address(ip);
        
        if (addr.is_v4()) {
            auto v4 = addr.to_v4();
            auto bytes = v4.to_bytes();
            
            // 10.0.0.0/8
            if (bytes[0] == 10) return true;
            
            // 172.16.0.0/12
            if (bytes[0] == 172 && (bytes[1] >= 16 && bytes[1] <= 31)) return true;
            
            // 192.168.0.0/16
            if (bytes[0] == 192 && bytes[1] == 168) return true;
            
            // 169.254.0.0/16 (link-local)
            if (bytes[0] == 169 && bytes[1] == 254) return true;
        }
        else if (addr.is_v6()) {
            auto v6 = addr.to_v6();
            
            // fc00::/7 (unique local)
            if (v6.is_site_local()) return true;
            
            // fe80::/10 (link-local)
            if (v6.is_link_local()) return true;
        }
        
        return false;
    } catch (...) {
        return false;
    }
}

bool is_routable_ip(const std::string& ip) {
    try {
        auto addr = boost::asio::ip::make_address(ip);
        
        // Not routable if loopback, multicast, or unspecified
        if (addr.is_loopback() || addr.is_multicast() || addr.is_unspecified()) {
            return false;
        }
        
        // Not routable if private
        if (is_private_ip(ip)) {
            return false;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool is_valid_port(uint16_t port) {
    // Ports 0-1023 are privileged, typically avoid them for P2P
    // Port 0 is invalid
    return port > 0;
}

bool is_valid_peer_address(const PeerAddress& addr, bool allow_private) {
    // Validate IP format
    if (!is_valid_ip(addr.ip)) {
        return false;
    }
    
    // Validate port
    if (!is_valid_port(addr.port)) {
        return false;
    }
    
    // Check if routable (unless we allow private)
    if (!allow_private && !is_routable_ip(addr.ip)) {
        return false;
    }
    
    return true;
}

} // namespace address_validation

} // namespace p2p
} // namespace chainforge

