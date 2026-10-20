#pragma once

#include "chainforge/p2p/message.hpp"
#include <functional>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace chainforge::p2p {

/**
 * Thin view over local chain state so P2P sync does not depend on RPC.
 */
struct ChainView {
    uint64_t chain_id = 0;
    std::string agent = "chainforge-node";

    std::function<uint64_t()> tip_height;
    std::function<std::vector<uint8_t>()> tip_hash; // 32 bytes
    std::function<std::optional<std::vector<uint8_t>>(uint64_t height)> header_hash_at;
    std::function<std::optional<std::string>(uint64_t height)> block_json_at;
    std::function<std::optional<std::string>(const std::vector<uint8_t>& hash32)> block_json_by_hash;
    std::function<bool(const std::string& block_json)> import_block_json;
    std::function<bool(const std::string& tx_json)> accept_tx_json;
};

struct SyncPeerState {
    uint64_t tip_height = 0;
    std::vector<uint8_t> tip_hash;
    std::string agent;
    bool hello_ok = false;
};

/**
 * Stateful sync / gossip session for one peer connection (or loopback test).
 * Handles Hello, header/block sync, and NewTx duplicate filtering.
 */
class SyncSession {
public:
    explicit SyncSession(ChainView view);

    Message make_hello() const;
    Message make_ping() const;
    Message make_get_headers(uint64_t from_height, uint32_t max_count = 192) const;
    Message make_get_blocks(const std::vector<std::vector<uint8_t>>& hashes) const;
    Message make_new_tx(const std::vector<uint8_t>& tx_hash32, const std::string& tx_json) const;

    /// Process an inbound message; returns zero or more replies / follow-ups.
    std::vector<Message> handle(const Message& inbound);

    const SyncPeerState& peer() const { return peer_; }
    size_t seen_tx_count() const { return seen_tx_.size(); }
    size_t imported_blocks() const { return imported_blocks_; }

    /// True if peer tip is ahead of local tip (sync needed).
    bool needs_sync() const;

    /// Build next GetHeaders/GetBlocks steps toward peer tip (best-effort).
    std::vector<Message> sync_step();

private:
    std::vector<Message> on_hello(const Message& inbound, bool is_ack);
    std::vector<Message> on_get_headers(const Message& inbound);
    std::vector<Message> on_headers(const Message& inbound);
    std::vector<Message> on_get_blocks(const Message& inbound);
    std::vector<Message> on_blocks(const Message& inbound);
    std::vector<Message> on_new_tx(const Message& inbound);

    static std::string hash_hex(const std::vector<uint8_t>& h);

    ChainView view_;
    SyncPeerState peer_;
    std::unordered_set<std::string> seen_tx_;
    std::unordered_set<std::string> pending_block_hashes_;
    size_t imported_blocks_ = 0;
    uint32_t gossip_fanout_limit_ = 16;
};

/// Encode/decode round-trip helper used by node self-check.
bool protocol_self_check(std::string* error_out = nullptr);

} // namespace chainforge::p2p
