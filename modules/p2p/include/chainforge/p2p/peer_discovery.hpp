#pragma once

#include "peer_address.hpp"
#include "udp_transport.hpp"
#include "chainforge/core/expected.hpp"
#include <vector>
#include <string>
#include <set>
#include <mutex>
#include <memory>
#include <functional>
#include <atomic>

namespace chainforge {
namespace p2p {

/**
 * @brief Error codes for peer discovery
 */
enum class DiscoveryError {
    INVALID_ADDRESS = 1,
    NO_PEERS_FOUND = 2,
    DNS_RESOLUTION_FAILED = 3,
    BROADCAST_FAILED = 4,
    ALREADY_RUNNING = 5,
    NOT_RUNNING = 6
};

/**
 * @brief Result type for discovery operations
 */
template<typename T>
using DiscoveryResult = core::Result<T>;

/**
 * @brief Callback for discovered peers
 */
using PeerDiscoveredCallback = std::function<void(const PeerAddress&)>;

/**
 * @brief Bootstrap configuration
 */
struct BootstrapConfig {
    std::vector<PeerAddress> static_nodes;      // Hard-coded bootstrap nodes
    std::vector<std::string> dns_seeds;         // DNS seeds for discovery
    bool enable_mdns{true};                      // Enable mDNS discovery
    bool enable_peer_exchange{true};             // Enable peer exchange
    uint16_t discovery_port{8333};               // Port for discovery broadcast
    std::chrono::seconds discovery_interval{30}; // How often to broadcast
    size_t max_peers{125};                       // Maximum number of peers to track
};

/**
 * @brief Peer discovery manager
 * 
 * Implements multiple peer discovery mechanisms:
 * - Bootstrap node list (static peers)
 * - DNS seed resolution
 * - mDNS/local network discovery
 * - Peer exchange (PEX)
 */
class PeerDiscovery {
public:
    /**
     * @brief Construct peer discovery
     */
    explicit PeerDiscovery(asio::io_context& io_context, const BootstrapConfig& config);
    
    ~PeerDiscovery();
    
    // Non-copyable, non-movable
    PeerDiscovery(const PeerDiscovery&) = delete;
    PeerDiscovery& operator=(const PeerDiscovery&) = delete;
    PeerDiscovery(PeerDiscovery&&) = delete;
    PeerDiscovery& operator=(PeerDiscovery&&) = delete;
    
    /**
     * @brief Start discovery
     */
    DiscoveryResult<void> start();
    
    /**
     * @brief Stop discovery
     */
    void stop();
    
    /**
     * @brief Check if running
     */
    bool is_running() const noexcept { return running_.load(); }
    
    /**
     * @brief Get discovered peers
     */
    std::vector<PeerAddress> get_peers() const;
    
    /**
     * @brief Get peer count
     */
    size_t peer_count() const;
    
    /**
     * @brief Add peer manually
     */
    bool add_peer(const PeerAddress& addr);
    
    /**
     * @brief Remove peer
     */
    bool remove_peer(const PeerAddress& addr);
    
    /**
     * @brief Check if peer exists
     */
    bool has_peer(const PeerAddress& addr) const;
    
    /**
     * @brief Get random peers
     */
    std::vector<PeerAddress> get_random_peers(size_t count) const;
    
    /**
     * @brief Set discovery callback
     */
    void set_discovery_callback(PeerDiscoveredCallback callback);
    
    /**
     * @brief Perform DNS seed lookup
     */
    DiscoveryResult<std::vector<PeerAddress>> resolve_dns_seeds();
    
    /**
     * @brief Broadcast discovery message
     */
    DiscoveryResult<void> broadcast_discovery();
    
    /**
     * @brief Exchange peers with another node
     */
    DiscoveryResult<void> exchange_peers(
        const std::vector<PeerAddress>& their_peers,
        std::vector<PeerAddress>& our_peers
    );
    
    /**
     * @brief Clean up stale peers
     */
    void cleanup_stale_peers(std::chrono::seconds max_age = std::chrono::hours(24));

private:
    void load_bootstrap_nodes();
    void start_discovery_loop();
    void handle_discovery_message(const std::vector<uint8_t>& data, const UdpEndpoint& sender);
    
    static core::ErrorInfo make_discovery_error(DiscoveryError code, const std::string& message);
    
    asio::io_context& io_context_;
    BootstrapConfig config_;
    
    std::unique_ptr<UdpTransport> udp_transport_;
    std::unique_ptr<asio::steady_timer> discovery_timer_;
    
    mutable std::mutex peers_mutex_;
    std::set<PeerAddress> peers_;
    
    std::atomic<bool> running_{false};
    PeerDiscoveredCallback discovery_callback_;
};

/**
 * @brief DNS seed resolver
 */
class DnsSeedResolver {
public:
    /**
     * @brief Resolve DNS seed to peer addresses
     */
    static std::vector<PeerAddress> resolve(
        const std::string& dns_seed,
        uint16_t default_port = 8333
    );
    
    /**
     * @brief Resolve multiple DNS seeds
     */
    static std::vector<PeerAddress> resolve_multiple(
        const std::vector<std::string>& dns_seeds,
        uint16_t default_port = 8333
    );
};

/**
 * @brief Peer exchange (PEX) protocol
 */
class PeerExchange {
public:
    /**
     * @brief Serialize peer list for exchange
     */
    static std::vector<uint8_t> serialize_peers(const std::vector<PeerAddress>& peers);
    
    /**
     * @brief Deserialize peer list from exchange
     */
    static std::vector<PeerAddress> deserialize_peers(const std::vector<uint8_t>& data);
    
    /**
     * @brief Select peers to share (best peers)
     */
    static std::vector<PeerAddress> select_peers_to_share(
        const std::vector<PeerAddress>& all_peers,
        size_t max_count = 10
    );
};

} // namespace p2p
} // namespace chainforge

