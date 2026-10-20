#include "chainforge/p2p/peer_manager.hpp"

namespace chainforge::p2p {

PeerManager::PeerManager(size_t max_peers) : max_peers_(max_peers) {}

std::string PeerManager::make_id(const PeerAddress& address) const {
    return address.to_string();
}

bool PeerManager::add_peer(const PeerAddress& address) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!address.is_valid()) {
        return false;
    }
    const auto id = make_id(address);
    if (peers_.find(id) != peers_.end()) {
        peers_[id].last_seen = std::chrono::steady_clock::now();
        return true;
    }
    if (peers_.size() >= max_peers_) {
        return false;
    }
    PeerInfo info;
    info.address = address;
    info.state = PeerState::Discovered;
    info.last_seen = std::chrono::steady_clock::now();
    peers_.emplace(id, std::move(info));
    return true;
}

bool PeerManager::remove_peer(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    return peers_.erase(id) > 0;
}

bool PeerManager::ban_peer(const std::string& id, int score_penalty) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = peers_.find(id);
    if (it == peers_.end()) {
        return false;
    }
    it->second.state = PeerState::Banned;
    it->second.score -= score_penalty;
    return true;
}

bool PeerManager::set_state(const std::string& id, PeerState state) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = peers_.find(id);
    if (it == peers_.end()) {
        return false;
    }
    it->second.state = state;
    it->second.last_seen = std::chrono::steady_clock::now();
    return true;
}

void PeerManager::touch(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = peers_.find(id);
    if (it != peers_.end()) {
        it->second.last_seen = std::chrono::steady_clock::now();
    }
}

void PeerManager::adjust_score(const std::string& id, int delta) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = peers_.find(id);
    if (it != peers_.end()) {
        it->second.score += delta;
        if (it->second.score <= -100) {
            it->second.state = PeerState::Banned;
        }
    }
}

std::optional<PeerInfo> PeerManager::get(const std::string& id) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = peers_.find(id);
    if (it == peers_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::vector<PeerInfo> PeerManager::peers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PeerInfo> out;
    out.reserve(peers_.size());
    for (const auto& [_, info] : peers_) {
        out.push_back(info);
    }
    return out;
}

std::vector<PeerInfo> PeerManager::peers_in_state(PeerState state) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PeerInfo> out;
    for (const auto& [_, info] : peers_) {
        if (info.state == state) {
            out.push_back(info);
        }
    }
    return out;
}

size_t PeerManager::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return peers_.size();
}

} // namespace chainforge::p2p
