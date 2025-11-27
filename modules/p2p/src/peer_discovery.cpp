#include "chainforge/p2p/peer_discovery.hpp"
#include <spdlog/spdlog.h>
#include <boost/asio/ip/tcp.hpp>
#include <algorithm>
#include <random>
#include <cstring>

namespace chainforge {
namespace p2p {

core::ErrorInfo PeerDiscovery::make_discovery_error(DiscoveryError code, const std::string& message) {
    return core::ErrorInfo(
        static_cast<int>(code),
        message,
        "peer_discovery",
        __FILE__,
        __LINE__
    );
}

PeerDiscovery::PeerDiscovery(asio::io_context& io_context, const BootstrapConfig& config)
    : io_context_(io_context)
    , config_(config)
    , udp_transport_(std::make_unique<UdpTransport>(io_context))
    , discovery_timer_(std::make_unique<asio::steady_timer>(io_context)) {
}

PeerDiscovery::~PeerDiscovery() {
    stop();
}

DiscoveryResult<void> PeerDiscovery::start() {
    if (running_.load()) {
        return make_discovery_error(DiscoveryError::ALREADY_RUNNING, "Discovery already running");
    }
    
    spdlog::info("Starting peer discovery...");
    
    // Load bootstrap nodes
    load_bootstrap_nodes();
    
    // Bind UDP for discovery
    if (config_.enable_mdns) {
        auto bind_result = udp_transport_->bind(config_.discovery_port);
        if (!bind_result.has_value()) {
            return make_discovery_error(
                DiscoveryError::BROADCAST_FAILED,
                "Failed to bind UDP for discovery: " + bind_result.error().message
            );
        }
        
        // Start receiving discovery messages
        udp_transport_->start_receive([this](const std::vector<uint8_t>& data, const UdpEndpoint& sender) {
            handle_discovery_message(data, sender);
        });
    }
    
    // Resolve DNS seeds
    if (!config_.dns_seeds.empty()) {
        auto dns_result = resolve_dns_seeds();
        if (dns_result.has_value()) {
            spdlog::info("Resolved {} peers from DNS seeds", dns_result.value().size());
        }
    }
    
    running_.store(true);
    
    // Start periodic discovery
    if (config_.enable_mdns) {
        start_discovery_loop();
    }
    
    spdlog::info("Peer discovery started with {} initial peers", peer_count());
    return core::success();
}

void PeerDiscovery::stop() {
    if (!running_.exchange(false)) {
        return;
    }
    
    if (discovery_timer_) {
        boost::system::error_code ec;
        discovery_timer_->cancel(ec);
    }
    
    if (udp_transport_) {
        udp_transport_->stop_receive();
        udp_transport_->close();
    }
    
    spdlog::info("Peer discovery stopped");
}

std::vector<PeerAddress> PeerDiscovery::get_peers() const {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    return std::vector<PeerAddress>(peers_.begin(), peers_.end());
}

size_t PeerDiscovery::peer_count() const {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    return peers_.size();
}

bool PeerDiscovery::add_peer(const PeerAddress& addr) {
    // Validate address
    if (!addr.is_valid()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(peers_mutex_);
    
    // Check max peers
    if (peers_.size() >= config_.max_peers) {
        return false;
    }
    
    auto [it, inserted] = peers_.insert(addr);
    
    if (inserted) {
        spdlog::debug("Added peer: {}", addr.to_string());
        
        // Notify callback
        if (discovery_callback_) {
            discovery_callback_(addr);
        }
    }
    
    return inserted;
}

bool PeerDiscovery::remove_peer(const PeerAddress& addr) {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    return peers_.erase(addr) > 0;
}

bool PeerDiscovery::has_peer(const PeerAddress& addr) const {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    return peers_.find(addr) != peers_.end();
}

std::vector<PeerAddress> PeerDiscovery::get_random_peers(size_t count) const {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    
    std::vector<PeerAddress> all_peers(peers_.begin(), peers_.end());
    
    if (all_peers.size() <= count) {
        return all_peers;
    }
    
    // Shuffle and take first 'count' peers
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(all_peers.begin(), all_peers.end(), gen);
    
    all_peers.resize(count);
    return all_peers;
}

void PeerDiscovery::set_discovery_callback(PeerDiscoveredCallback callback) {
    discovery_callback_ = std::move(callback);
}

DiscoveryResult<std::vector<PeerAddress>> PeerDiscovery::resolve_dns_seeds() {
    std::vector<PeerAddress> discovered_peers;
    
    for (const auto& dns_seed : config_.dns_seeds) {
        try {
            auto peers = DnsSeedResolver::resolve(dns_seed, config_.discovery_port);
            
            for (const auto& peer : peers) {
                if (add_peer(peer)) {
                    discovered_peers.push_back(peer);
                }
            }
        } catch (const std::exception& e) {
            spdlog::warn("Failed to resolve DNS seed {}: {}", dns_seed, e.what());
        }
    }
    
    if (discovered_peers.empty()) {
        return make_discovery_error(
            DiscoveryError::NO_PEERS_FOUND,
            "No peers found from DNS seeds"
        );
    }
    
    return discovered_peers;
}

DiscoveryResult<void> PeerDiscovery::broadcast_discovery() {
    if (!running_.load()) {
        return make_discovery_error(DiscoveryError::NOT_RUNNING, "Discovery not running");
    }
    
    // Simple discovery message: "DISCOVER:<port>"
    std::string message = "DISCOVER:" + std::to_string(config_.discovery_port);
    std::vector<uint8_t> data(message.begin(), message.end());
    
    auto result = udp_transport_->broadcast(data, config_.discovery_port);
    
    if (!result.has_value()) {
        return make_discovery_error(
            DiscoveryError::BROADCAST_FAILED,
            "Broadcast failed: " + result.error().message
        );
    }
    
    spdlog::debug("Broadcasted discovery message");
    return core::success();
}

DiscoveryResult<void> PeerDiscovery::exchange_peers(
    const std::vector<PeerAddress>& their_peers,
    std::vector<PeerAddress>& our_peers) {
    
    // Add their peers
    for (const auto& peer : their_peers) {
        add_peer(peer);
    }
    
    // Select our peers to share
    our_peers = PeerExchange::select_peers_to_share(get_peers(), 10);
    
    return core::success();
}

void PeerDiscovery::cleanup_stale_peers(std::chrono::seconds max_age) {
    std::lock_guard<std::mutex> lock(peers_mutex_);
    
    auto it = peers_.begin();
    size_t removed = 0;
    
    while (it != peers_.end()) {
        if (it->age_seconds() > max_age.count()) {
            it = peers_.erase(it);
            removed++;
        } else {
            ++it;
        }
    }
    
    if (removed > 0) {
        spdlog::info("Cleaned up {} stale peers", removed);
    }
}

void PeerDiscovery::load_bootstrap_nodes() {
    for (const auto& node : config_.static_nodes) {
        add_peer(node);
    }
    
    spdlog::info("Loaded {} bootstrap nodes", config_.static_nodes.size());
}

void PeerDiscovery::start_discovery_loop() {
    if (!running_.load()) {
        return;
    }
    
    // Broadcast discovery
    broadcast_discovery();
    
    // Schedule next broadcast
    discovery_timer_->expires_after(config_.discovery_interval);
    discovery_timer_->async_wait([this](const boost::system::error_code& ec) {
        if (!ec && running_.load()) {
            start_discovery_loop();
        }
    });
}

void PeerDiscovery::handle_discovery_message(
    const std::vector<uint8_t>& data,
    const UdpEndpoint& sender) {
    
    std::string message(data.begin(), data.end());
    
    // Parse discovery message
    if (message.find("DISCOVER:") == 0) {
        // Extract port
        std::string port_str = message.substr(9);
        
        try {
            uint16_t port = static_cast<uint16_t>(std::stoi(port_str));
            
            PeerAddress peer_addr(sender.address, port);
            
            if (add_peer(peer_addr)) {
                spdlog::info("Discovered peer via mDNS: {}", peer_addr.to_string());
            }
        } catch (...) {
            spdlog::warn("Invalid discovery message from {}", sender.to_string());
        }
    }
    else if (message.find("PEERLIST:") == 0) {
        // Peer exchange message
        std::vector<uint8_t> peer_data(data.begin() + 9, data.end());
        auto peers = PeerExchange::deserialize_peers(peer_data);
        
        for (const auto& peer : peers) {
            add_peer(peer);
        }
        
        spdlog::info("Received {} peers via peer exchange", peers.size());
    }
}

// ============================================================================
// DnsSeedResolver Implementation
// ============================================================================

std::vector<PeerAddress> DnsSeedResolver::resolve(
    const std::string& dns_seed,
    uint16_t default_port) {
    
    std::vector<PeerAddress> peers;
    
    try {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        
        auto results = resolver.resolve(dns_seed, "");
        
        for (const auto& entry : results) {
            auto endpoint = entry.endpoint();
            PeerAddress peer(
                endpoint.address().to_string(),
                default_port,
                ServiceFlags::NODE_NETWORK
            );
            peers.push_back(peer);
        }
        
        spdlog::info("Resolved {} addresses from DNS seed {}", peers.size(), dns_seed);
        
    } catch (const std::exception& e) {
        spdlog::error("DNS resolution failed for {}: {}", dns_seed, e.what());
    }
    
    return peers;
}

std::vector<PeerAddress> DnsSeedResolver::resolve_multiple(
    const std::vector<std::string>& dns_seeds,
    uint16_t default_port) {
    
    std::vector<PeerAddress> all_peers;
    
    for (const auto& seed : dns_seeds) {
        auto peers = resolve(seed, default_port);
        all_peers.insert(all_peers.end(), peers.begin(), peers.end());
    }
    
    // Remove duplicates
    std::sort(all_peers.begin(), all_peers.end());
    all_peers.erase(std::unique(all_peers.begin(), all_peers.end()), all_peers.end());
    
    return all_peers;
}

// ============================================================================
// PeerExchange Implementation
// ============================================================================

std::vector<uint8_t> PeerExchange::serialize_peers(const std::vector<PeerAddress>& peers) {
    std::vector<uint8_t> data;
    
    // Format: count (4 bytes) + [ip_len (1 byte) + ip + port (2 bytes) + services (4 bytes)]*
    
    // Write count
    uint32_t count = static_cast<uint32_t>(peers.size());
    data.push_back((count >> 24) & 0xFF);
    data.push_back((count >> 16) & 0xFF);
    data.push_back((count >> 8) & 0xFF);
    data.push_back(count & 0xFF);
    
    for (const auto& peer : peers) {
        // IP length and IP
        uint8_t ip_len = static_cast<uint8_t>(peer.ip.size());
        data.push_back(ip_len);
        data.insert(data.end(), peer.ip.begin(), peer.ip.end());
        
        // Port
        data.push_back((peer.port >> 8) & 0xFF);
        data.push_back(peer.port & 0xFF);
        
        // Services
        data.push_back((peer.services >> 24) & 0xFF);
        data.push_back((peer.services >> 16) & 0xFF);
        data.push_back((peer.services >> 8) & 0xFF);
        data.push_back(peer.services & 0xFF);
    }
    
    return data;
}

std::vector<PeerAddress> PeerExchange::deserialize_peers(const std::vector<uint8_t>& data) {
    std::vector<PeerAddress> peers;
    
    if (data.size() < 4) {
        return peers;
    }
    
    size_t pos = 0;
    
    // Read count
    uint32_t count = (static_cast<uint32_t>(data[pos]) << 24) |
                     (static_cast<uint32_t>(data[pos + 1]) << 16) |
                     (static_cast<uint32_t>(data[pos + 2]) << 8) |
                     static_cast<uint32_t>(data[pos + 3]);
    pos += 4;
    
    for (uint32_t i = 0; i < count && pos < data.size(); ++i) {
        // Read IP length
        if (pos >= data.size()) break;
        uint8_t ip_len = data[pos++];
        
        // Read IP
        if (pos + ip_len > data.size()) break;
        std::string ip(data.begin() + pos, data.begin() + pos + ip_len);
        pos += ip_len;
        
        // Read port
        if (pos + 2 > data.size()) break;
        uint16_t port = (static_cast<uint16_t>(data[pos]) << 8) |
                        static_cast<uint16_t>(data[pos + 1]);
        pos += 2;
        
        // Read services
        if (pos + 4 > data.size()) break;
        uint32_t services = (static_cast<uint32_t>(data[pos]) << 24) |
                           (static_cast<uint32_t>(data[pos + 1]) << 16) |
                           (static_cast<uint32_t>(data[pos + 2]) << 8) |
                           static_cast<uint32_t>(data[pos + 3]);
        pos += 4;
        
        peers.emplace_back(ip, port, services);
    }
    
    return peers;
}

std::vector<PeerAddress> PeerExchange::select_peers_to_share(
    const std::vector<PeerAddress>& all_peers,
    size_t max_count) {
    
    if (all_peers.size() <= max_count) {
        return all_peers;
    }
    
    // Sort by last seen (most recent first)
    std::vector<PeerAddress> sorted_peers = all_peers;
    std::sort(sorted_peers.begin(), sorted_peers.end(),
        [](const PeerAddress& a, const PeerAddress& b) {
            return a.last_seen > b.last_seen;
        });
    
    // Take top max_count peers
    sorted_peers.resize(max_count);
    return sorted_peers;
}

} // namespace p2p
} // namespace chainforge

