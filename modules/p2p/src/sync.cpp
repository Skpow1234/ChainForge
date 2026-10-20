#include "chainforge/p2p/sync.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace chainforge::p2p {

SyncSession::SyncSession(ChainView view) : view_(std::move(view)) {}

std::string SyncSession::hash_hex(const std::vector<uint8_t>& h) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t b : h) {
        ss << std::setw(2) << static_cast<int>(b);
    }
    return ss.str();
}

Message SyncSession::make_hello() const {
    HelloPayload p;
    p.chain_id = view_.chain_id;
    p.tip_height = view_.tip_height ? view_.tip_height() : 0;
    p.tip_hash = view_.tip_hash ? view_.tip_hash() : std::vector<uint8_t>(32, 0);
    p.agent = view_.agent;
    p.protocol_version = PROTOCOL_VERSION;
    Message m;
    m.type = MessageType::Hello;
    m.payload = encode_hello(p);
    return m;
}

Message SyncSession::make_ping() const {
    Message m;
    m.type = MessageType::Ping;
    return m;
}

Message SyncSession::make_get_headers(uint64_t from_height, uint32_t max_count) const {
    Message m;
    m.type = MessageType::GetHeaders;
    m.payload = encode_get_headers(GetHeadersPayload{from_height, max_count});
    return m;
}

Message SyncSession::make_get_blocks(const std::vector<std::vector<uint8_t>>& hashes) const {
    Message m;
    m.type = MessageType::GetBlocks;
    m.payload = encode_get_blocks(GetBlocksPayload{hashes});
    return m;
}

Message SyncSession::make_new_tx(const std::vector<uint8_t>& tx_hash32, const std::string& tx_json) const {
    Message m;
    m.type = MessageType::NewTx;
    m.payload = encode_new_tx(NewTxPayload{tx_hash32, tx_json});
    return m;
}

bool SyncSession::needs_sync() const {
    if (!peer_.hello_ok || !view_.tip_height) {
        return false;
    }
    return peer_.tip_height > view_.tip_height();
}

std::vector<Message> SyncSession::sync_step() {
    std::vector<Message> out;
    if (!needs_sync()) {
        return out;
    }
    const uint64_t local = view_.tip_height();
    out.push_back(make_get_headers(local + 1, 64));
    if (!pending_block_hashes_.empty()) {
        GetBlocksPayload gb;
        size_t n = 0;
        for (const auto& hex : pending_block_hashes_) {
            if (n >= 16) break;
            std::vector<uint8_t> hash;
            hash.reserve(32);
            for (size_t i = 0; i + 1 < hex.size(); i += 2) {
                hash.push_back(static_cast<uint8_t>(std::stoul(hex.substr(i, 2), nullptr, 16)));
            }
            if (hash.size() == 32) {
                gb.hashes.push_back(std::move(hash));
                ++n;
            }
        }
        if (!gb.hashes.empty()) {
            Message m;
            m.type = MessageType::GetBlocks;
            m.payload = encode_get_blocks(gb);
            out.push_back(std::move(m));
        }
    }
    return out;
}

std::vector<Message> SyncSession::on_hello(const Message& inbound, bool is_ack) {
    auto payload = decode_hello(inbound.payload);
    std::vector<Message> out;
    if (!payload) {
        return out;
    }
    if (payload->chain_id != view_.chain_id) {
        return out;
    }
    peer_.tip_height = payload->tip_height;
    peer_.tip_hash = payload->tip_hash;
    peer_.agent = payload->agent;
    peer_.hello_ok = true;
    if (!is_ack) {
        HelloPayload ack = *payload;
        ack.chain_id = view_.chain_id;
        ack.tip_height = view_.tip_height ? view_.tip_height() : 0;
        ack.tip_hash = view_.tip_hash ? view_.tip_hash() : std::vector<uint8_t>(32, 0);
        ack.agent = view_.agent;
        ack.protocol_version = PROTOCOL_VERSION;
        Message m;
        m.type = MessageType::HelloAck;
        m.payload = encode_hello(ack);
        out.push_back(std::move(m));
    }
    return out;
}

std::vector<Message> SyncSession::on_get_headers(const Message& inbound) {
    std::vector<Message> out;
    auto req = decode_get_headers(inbound.payload);
    if (!req || !view_.header_hash_at) {
        return out;
    }
    HeadersPayload resp;
    const uint32_t limit = std::min<uint32_t>(req->max_count, 512);
    const uint64_t tip = view_.tip_height ? view_.tip_height() : 0;
    for (uint32_t i = 0; i < limit; ++i) {
        const uint64_t h = req->from_height + i;
        if (h > tip) break;
        auto hash = view_.header_hash_at(h);
        if (!hash) break;
        resp.headers.push_back(HeaderEntry{h, std::move(*hash)});
    }
    Message m;
    m.type = MessageType::Headers;
    m.payload = encode_headers(resp);
    out.push_back(std::move(m));
    return out;
}

std::vector<Message> SyncSession::on_headers(const Message& inbound) {
    std::vector<Message> out;
    auto headers = decode_headers(inbound.payload);
    if (!headers) {
        return out;
    }
    GetBlocksPayload gb;
    for (const auto& h : headers->headers) {
        const auto hex = hash_hex(h.hash);
        pending_block_hashes_.insert(hex);
        gb.hashes.push_back(h.hash);
        if (gb.hashes.size() >= 16) break;
    }
    if (!gb.hashes.empty()) {
        Message m;
        m.type = MessageType::GetBlocks;
        m.payload = encode_get_blocks(gb);
        out.push_back(std::move(m));
    }
    return out;
}

std::vector<Message> SyncSession::on_get_blocks(const Message& inbound) {
    std::vector<Message> out;
    auto req = decode_get_blocks(inbound.payload);
    if (!req) {
        return out;
    }
    BlocksPayload resp;
    for (const auto& hash : req->hashes) {
        std::optional<std::string> json;
        if (view_.block_json_by_hash) {
            json = view_.block_json_by_hash(hash);
        }
        if (!json && view_.block_json_at) {
            // fall through not available without height index
        }
        if (json) {
            resp.block_jsons.push_back(std::move(*json));
        }
    }
    Message m;
    m.type = MessageType::Blocks;
    m.payload = encode_blocks(resp);
    out.push_back(std::move(m));
    return out;
}

std::vector<Message> SyncSession::on_blocks(const Message& inbound) {
    std::vector<Message> out;
    auto blocks = decode_blocks(inbound.payload);
    if (!blocks || !view_.import_block_json) {
        return out;
    }
    for (const auto& json : blocks->block_jsons) {
        if (view_.import_block_json(json)) {
            ++imported_blocks_;
        }
    }
    return out;
}

std::vector<Message> SyncSession::on_new_tx(const Message& inbound) {
    std::vector<Message> out;
    auto tx = decode_new_tx(inbound.payload);
    if (!tx) {
        return out;
    }
    const auto hex = hash_hex(tx->tx_hash);
    if (seen_tx_.count(hex)) {
        return out; // duplicate — drop
    }
    if (seen_tx_.size() > 10000) {
        seen_tx_.clear();
    }
    seen_tx_.insert(hex);
    if (!tx->tx_json.empty() && view_.accept_tx_json) {
        view_.accept_tx_json(tx->tx_json);
    }
    // Propagation: re-announce hash-only; caller fans out to peers.
    (void)gossip_fanout_limit_;
    Message relay;
    relay.type = MessageType::NewTx;
    relay.payload = encode_new_tx(NewTxPayload{tx->tx_hash, {}});
    out.push_back(std::move(relay));
    return out;
}

std::vector<Message> SyncSession::handle(const Message& inbound) {
    if (!validate_message(inbound)) {
        return {};
    }
    switch (inbound.type) {
        case MessageType::Hello:
            return on_hello(inbound, false);
        case MessageType::HelloAck:
            return on_hello(inbound, true);
        case MessageType::Ping: {
            Message m;
            m.type = MessageType::Pong;
            m.payload = inbound.payload;
            return {m};
        }
        case MessageType::Pong:
            return {};
        case MessageType::GetHeaders:
            return on_get_headers(inbound);
        case MessageType::Headers:
            return on_headers(inbound);
        case MessageType::GetBlocks:
            return on_get_blocks(inbound);
        case MessageType::Blocks:
            return on_blocks(inbound);
        case MessageType::NewTx:
            return on_new_tx(inbound);
        case MessageType::NewBlock:
        case MessageType::GetTx:
        case MessageType::Tx:
            return {};
    }
    return {};
}

bool protocol_self_check(std::string* error_out) {
    auto fail = [&](const char* msg) {
        if (error_out) *error_out = msg;
        return false;
    };

    HelloPayload hello;
    hello.chain_id = 1337;
    hello.tip_height = 3;
    hello.tip_hash.assign(32, 0xab);
    hello.agent = "self-check";
    hello.protocol_version = PROTOCOL_VERSION;

    Message m;
    m.type = MessageType::Hello;
    m.payload = encode_hello(hello);
    auto wire = encode_message(m);
    auto decoded = decode_message(wire);
    if (!decoded || decoded->type != MessageType::Hello) {
        return fail("frame decode failed");
    }
    auto hp = decode_hello(decoded->payload);
    if (!hp || hp->chain_id != 1337 || hp->tip_height != 3 || hp->agent != "self-check") {
        return fail("hello payload mismatch");
    }

    // Two sessions: A has tip 0, B has tip 2 — sync headers/blocks via handle().
    std::vector<std::string> chain_json = {
        R"({"height":0,"parentHash":"0000000000000000000000000000000000000000000000000000000000000000","timestamp":1,"nonce":0,"gasLimit":8000000,"gasPrice":1,"chainId":1337,"transactions":[]})",
        R"({"height":1,"parentHash":"1111111111111111111111111111111111111111111111111111111111111111","timestamp":2,"nonce":1,"gasLimit":8000000,"gasPrice":1,"chainId":1337,"transactions":[]})",
        R"({"height":2,"parentHash":"2222222222222222222222222222222222222222222222222222222222222222","timestamp":3,"nonce":2,"gasLimit":8000000,"gasPrice":1,"chainId":1337,"transactions":[]})",
    };
    std::vector<std::vector<uint8_t>> hashes = {
        std::vector<uint8_t>(32, 0x10),
        std::vector<uint8_t>(32, 0x11),
        std::vector<uint8_t>(32, 0x12),
    };

    uint64_t tip_a = 0;
    ChainView view_a;
    view_a.chain_id = 1337;
    view_a.agent = "A";
    view_a.tip_height = [&] { return tip_a; };
    view_a.tip_hash = [&] { return hashes[tip_a]; };
    view_a.header_hash_at = [&](uint64_t h) -> std::optional<std::vector<uint8_t>> {
        if (h > tip_a) return std::nullopt;
        return hashes[h];
    };
    view_a.block_json_at = [&](uint64_t h) -> std::optional<std::string> {
        if (h > tip_a) return std::nullopt;
        return chain_json[h];
    };
    view_a.block_json_by_hash = [&](const std::vector<uint8_t>& hash) -> std::optional<std::string> {
        for (size_t i = 0; i <= tip_a; ++i) {
            if (hashes[i] == hash) return chain_json[i];
        }
        return std::nullopt;
    };
    view_a.import_block_json = [&](const std::string& json) {
        for (size_t i = 0; i < chain_json.size(); ++i) {
            if (chain_json[i] == json) {
                if (i > tip_a) {
                    tip_a = i;
                }
                return true;
            }
        }
        return false;
    };

    ChainView view_b = view_a;
    view_b.agent = "B";
    uint64_t tip_b = 2;
    view_b.tip_height = [&] { return tip_b; };
    view_b.tip_hash = [&] { return hashes[tip_b]; };
    view_b.header_hash_at = [&](uint64_t h) -> std::optional<std::vector<uint8_t>> {
        if (h > tip_b) return std::nullopt;
        return hashes[h];
    };
    view_b.block_json_by_hash = [&](const std::vector<uint8_t>& hash) -> std::optional<std::string> {
        for (size_t i = 0; i <= tip_b; ++i) {
            if (hashes[i] == hash) return chain_json[i];
        }
        return std::nullopt;
    };

    SyncSession a(view_a);
    SyncSession b(view_b);

    auto to_b = b.handle(a.make_hello());
    if (to_b.size() != 1 || to_b[0].type != MessageType::HelloAck) {
        return fail("hello handshake failed");
    }
    a.handle(to_b[0]);
    if (!a.peer().hello_ok || a.peer().tip_height != 2) {
        return fail("peer tip not learned");
    }

    auto reqs = a.sync_step();
    if (reqs.empty() || reqs[0].type != MessageType::GetHeaders) {
        return fail("expected GetHeaders");
    }
    auto headers_msg = b.handle(reqs[0]);
    if (headers_msg.empty() || headers_msg[0].type != MessageType::Headers) {
        return fail("expected Headers");
    }
    auto get_blocks = a.handle(headers_msg[0]);
    if (get_blocks.empty() || get_blocks[0].type != MessageType::GetBlocks) {
        return fail("expected GetBlocks");
    }
    auto blocks_msg = b.handle(get_blocks[0]);
    if (blocks_msg.empty() || blocks_msg[0].type != MessageType::Blocks) {
        return fail("expected Blocks");
    }
    a.handle(blocks_msg[0]);
    if (tip_a < 1) {
        return fail("blocks not imported");
    }

    // Tx gossip duplicate detection
    std::vector<uint8_t> txh(32, 0xcd);
    auto n1 = a.handle(a.make_new_tx(txh, R"({"from":"0x00","to":"0x00","value":"0"})"));
    auto n2 = a.handle(a.make_new_tx(txh, R"({"from":"0x00","to":"0x00","value":"0"})"));
    if (n1.empty() || !n2.empty()) {
        return fail("tx gossip duplicate handling failed");
    }
    if (a.seen_tx_count() != 1) {
        return fail("seen tx count wrong");
    }

    return true;
}

} // namespace chainforge::p2p
