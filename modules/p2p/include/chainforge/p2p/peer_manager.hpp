#pragma once

#include "peer_address.hpp"
#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace chainforge::p2p {

enum class PeerState {
    Discovered,
    Connecting,
    Connected,
    Banned
};

struct PeerInfo {
    PeerAddress address;
    PeerState state = PeerState::Discovered;
    int score = 0;
    std::chrono::steady_clock::time_point last_seen = std::chrono::steady_clock::now();
    std::string agent;
};

/**
 * Tracks known peers, scores, and connection state (Milestone 18 scaffolding).
 */
class PeerManager {
public:
    explicit PeerManager(size_t max_peers = 50);

    bool add_peer(const PeerAddress& address);
    bool remove_peer(const std::string& id);
    bool ban_peer(const std::string& id, int score_penalty = 100);
    bool set_state(const std::string& id, PeerState state);
    void touch(const std::string& id);
    void adjust_score(const std::string& id, int delta);

    std::optional<PeerInfo> get(const std::string& id) const;
    std::vector<PeerInfo> peers() const;
    std::vector<PeerInfo> peers_in_state(PeerState state) const;
    size_t size() const;
    size_t max_peers() const { return max_peers_; }

private:
    std::string make_id(const PeerAddress& address) const;

    mutable std::mutex mutex_;
    size_t max_peers_;
    std::unordered_map<std::string, PeerInfo> peers_;
};

} // namespace chainforge::p2p
