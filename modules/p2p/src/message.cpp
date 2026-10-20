#include "tokiwa/p2p/message.hpp"
#include <algorithm>
#include <cstring>

namespace tokiwa::p2p {
namespace {

void write_be32(uint8_t* dst, uint32_t v) {
    dst[0] = static_cast<uint8_t>((v >> 24) & 0xff);
    dst[1] = static_cast<uint8_t>((v >> 16) & 0xff);
    dst[2] = static_cast<uint8_t>((v >> 8) & 0xff);
    dst[3] = static_cast<uint8_t>(v & 0xff);
}

uint32_t read_be32(const uint8_t* src) {
    return (uint32_t(src[0]) << 24) | (uint32_t(src[1]) << 16) |
           (uint32_t(src[2]) << 8) | uint32_t(src[3]);
}

} // namespace

void append_u8(std::vector<uint8_t>& out, uint8_t v) {
    out.push_back(v);
}

void append_u16(std::vector<uint8_t>& out, uint16_t v) {
    out.push_back(static_cast<uint8_t>(v & 0xff));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xff));
}

void append_u32(std::vector<uint8_t>& out, uint32_t v) {
    out.push_back(static_cast<uint8_t>(v & 0xff));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xff));
    out.push_back(static_cast<uint8_t>((v >> 16) & 0xff));
    out.push_back(static_cast<uint8_t>((v >> 24) & 0xff));
}

void append_u64(std::vector<uint8_t>& out, uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        out.push_back(static_cast<uint8_t>((v >> (8 * i)) & 0xff));
    }
}

void append_bytes(std::vector<uint8_t>& out, const uint8_t* data, size_t len) {
    out.insert(out.end(), data, data + len);
}

void append_bytes(std::vector<uint8_t>& out, const std::vector<uint8_t>& data) {
    out.insert(out.end(), data.begin(), data.end());
}

void append_string(std::vector<uint8_t>& out, const std::string& s) {
    append_u16(out, static_cast<uint16_t>(s.size()));
    out.insert(out.end(), s.begin(), s.end());
}

void append_hash32(std::vector<uint8_t>& out, const std::vector<uint8_t>& hash32) {
    if (hash32.size() != 32) {
        std::vector<uint8_t> padded(32, 0);
        const size_t n = std::min<size_t>(32, hash32.size());
        std::memcpy(padded.data(), hash32.data(), n);
        append_bytes(out, padded);
        return;
    }
    append_bytes(out, hash32);
}

std::optional<uint8_t> read_u8(ReadCursor& c) {
    if (!c.remaining(1)) return std::nullopt;
    return c.data[c.pos++];
}

std::optional<uint16_t> read_u16(ReadCursor& c) {
    if (!c.remaining(2)) return std::nullopt;
    uint16_t v = uint16_t(c.data[c.pos]) | (uint16_t(c.data[c.pos + 1]) << 8);
    c.pos += 2;
    return v;
}

std::optional<uint32_t> read_u32(ReadCursor& c) {
    if (!c.remaining(4)) return std::nullopt;
    uint32_t v = uint32_t(c.data[c.pos]) | (uint32_t(c.data[c.pos + 1]) << 8) |
                 (uint32_t(c.data[c.pos + 2]) << 16) | (uint32_t(c.data[c.pos + 3]) << 24);
    c.pos += 4;
    return v;
}

std::optional<uint64_t> read_u64(ReadCursor& c) {
    if (!c.remaining(8)) return std::nullopt;
    uint64_t v = 0;
    for (int i = 0; i < 8; ++i) {
        v |= (uint64_t(c.data[c.pos + i]) << (8 * i));
    }
    c.pos += 8;
    return v;
}

std::optional<std::vector<uint8_t>> read_bytes(ReadCursor& c, size_t n) {
    if (!c.remaining(n)) return std::nullopt;
    std::vector<uint8_t> out(c.data + c.pos, c.data + c.pos + n);
    c.pos += n;
    return out;
}

std::optional<std::string> read_string(ReadCursor& c) {
    auto len = read_u16(c);
    if (!len) return std::nullopt;
    auto bytes = read_bytes(c, *len);
    if (!bytes) return std::nullopt;
    return std::string(bytes->begin(), bytes->end());
}

std::optional<std::vector<uint8_t>> read_hash32(ReadCursor& c) {
    return read_bytes(c, 32);
}

bool is_known_message_type(MessageType type) {
    switch (type) {
        case MessageType::Hello:
        case MessageType::HelloAck:
        case MessageType::Ping:
        case MessageType::Pong:
        case MessageType::GetHeaders:
        case MessageType::Headers:
        case MessageType::GetBlocks:
        case MessageType::Blocks:
        case MessageType::NewBlock:
        case MessageType::NewTx:
        case MessageType::GetTx:
        case MessageType::Tx:
            return true;
    }
    return false;
}

std::string message_type_name(MessageType type) {
    switch (type) {
        case MessageType::Hello: return "Hello";
        case MessageType::HelloAck: return "HelloAck";
        case MessageType::Ping: return "Ping";
        case MessageType::Pong: return "Pong";
        case MessageType::GetHeaders: return "GetHeaders";
        case MessageType::Headers: return "Headers";
        case MessageType::GetBlocks: return "GetBlocks";
        case MessageType::Blocks: return "Blocks";
        case MessageType::NewBlock: return "NewBlock";
        case MessageType::NewTx: return "NewTx";
        case MessageType::GetTx: return "GetTx";
        case MessageType::Tx: return "Tx";
    }
    return "Unknown";
}

bool validate_message(const Message& msg) {
    if (msg.version == 0 || msg.version > PROTOCOL_VERSION) {
        return false;
    }
    if (!is_known_message_type(msg.type)) {
        return false;
    }
    if (msg.payload.size() > PROTOCOL_MAX_PAYLOAD) {
        return false;
    }
    return true;
}

std::vector<uint8_t> encode_message(const Message& msg) {
    std::vector<uint8_t> out;
    out.resize(PROTOCOL_HEADER_SIZE);
    std::memcpy(out.data(), PROTOCOL_MAGIC_BYTES, 4);
    out[4] = msg.version;
    out[5] = static_cast<uint8_t>(msg.type);
    out[6] = msg.flags;
    out[7] = 0;
    write_be32(out.data() + 8, static_cast<uint32_t>(msg.payload.size()));
    out.insert(out.end(), msg.payload.begin(), msg.payload.end());
    return out;
}

std::optional<Message> decode_message(const uint8_t* data, size_t len) {
    if (!data || len < PROTOCOL_HEADER_SIZE) {
        return std::nullopt;
    }
    if (std::memcmp(data, PROTOCOL_MAGIC_BYTES, 4) != 0) {
        return std::nullopt;
    }
    Message msg;
    msg.version = data[4];
    msg.type = static_cast<MessageType>(data[5]);
    msg.flags = data[6];
    const uint32_t plen = read_be32(data + 8);
    if (plen > PROTOCOL_MAX_PAYLOAD) {
        return std::nullopt;
    }
    if (len < PROTOCOL_HEADER_SIZE + plen) {
        return std::nullopt;
    }
    msg.payload.assign(data + PROTOCOL_HEADER_SIZE, data + PROTOCOL_HEADER_SIZE + plen);
    if (!validate_message(msg)) {
        return std::nullopt;
    }
    return msg;
}

std::vector<uint8_t> encode_hello(const HelloPayload& p) {
    std::vector<uint8_t> out;
    append_u64(out, p.chain_id);
    append_u64(out, p.tip_height);
    append_hash32(out, p.tip_hash);
    append_string(out, p.agent);
    append_u8(out, p.protocol_version);
    return out;
}

std::optional<HelloPayload> decode_hello(const std::vector<uint8_t>& payload) {
    ReadCursor c{payload.data(), payload.size(), 0};
    HelloPayload p;
    auto chain_id = read_u64(c);
    auto tip = read_u64(c);
    auto hash = read_hash32(c);
    auto agent = read_string(c);
    auto ver = read_u8(c);
    if (!chain_id || !tip || !hash || !agent || !ver) return std::nullopt;
    p.chain_id = *chain_id;
    p.tip_height = *tip;
    p.tip_hash = std::move(*hash);
    p.agent = std::move(*agent);
    p.protocol_version = *ver;
    return p;
}

std::vector<uint8_t> encode_get_headers(const GetHeadersPayload& p) {
    std::vector<uint8_t> out;
    append_u64(out, p.from_height);
    append_u32(out, p.max_count);
    return out;
}

std::optional<GetHeadersPayload> decode_get_headers(const std::vector<uint8_t>& payload) {
    ReadCursor c{payload.data(), payload.size(), 0};
    auto from = read_u64(c);
    auto maxc = read_u32(c);
    if (!from || !maxc) return std::nullopt;
    return GetHeadersPayload{*from, *maxc};
}

std::vector<uint8_t> encode_headers(const HeadersPayload& p) {
    std::vector<uint8_t> out;
    append_u32(out, static_cast<uint32_t>(p.headers.size()));
    for (const auto& h : p.headers) {
        append_u64(out, h.height);
        append_hash32(out, h.hash);
    }
    return out;
}

std::optional<HeadersPayload> decode_headers(const std::vector<uint8_t>& payload) {
    ReadCursor c{payload.data(), payload.size(), 0};
    auto count = read_u32(c);
    if (!count) return std::nullopt;
    HeadersPayload p;
    p.headers.reserve(*count);
    for (uint32_t i = 0; i < *count; ++i) {
        auto height = read_u64(c);
        auto hash = read_hash32(c);
        if (!height || !hash) return std::nullopt;
        p.headers.push_back(HeaderEntry{*height, std::move(*hash)});
    }
    return p;
}

std::vector<uint8_t> encode_get_blocks(const GetBlocksPayload& p) {
    std::vector<uint8_t> out;
    append_u32(out, static_cast<uint32_t>(p.hashes.size()));
    for (const auto& h : p.hashes) {
        append_hash32(out, h);
    }
    return out;
}

std::optional<GetBlocksPayload> decode_get_blocks(const std::vector<uint8_t>& payload) {
    ReadCursor c{payload.data(), payload.size(), 0};
    auto count = read_u32(c);
    if (!count) return std::nullopt;
    GetBlocksPayload p;
    p.hashes.reserve(*count);
    for (uint32_t i = 0; i < *count; ++i) {
        auto hash = read_hash32(c);
        if (!hash) return std::nullopt;
        p.hashes.push_back(std::move(*hash));
    }
    return p;
}

std::vector<uint8_t> encode_blocks(const BlocksPayload& p) {
    std::vector<uint8_t> out;
    append_u32(out, static_cast<uint32_t>(p.block_jsons.size()));
    for (const auto& j : p.block_jsons) {
        append_u32(out, static_cast<uint32_t>(j.size()));
        out.insert(out.end(), j.begin(), j.end());
    }
    return out;
}

std::optional<BlocksPayload> decode_blocks(const std::vector<uint8_t>& payload) {
    ReadCursor c{payload.data(), payload.size(), 0};
    auto count = read_u32(c);
    if (!count) return std::nullopt;
    BlocksPayload p;
    p.block_jsons.reserve(*count);
    for (uint32_t i = 0; i < *count; ++i) {
        auto len = read_u32(c);
        if (!len) return std::nullopt;
        auto bytes = read_bytes(c, *len);
        if (!bytes) return std::nullopt;
        p.block_jsons.emplace_back(bytes->begin(), bytes->end());
    }
    return p;
}

std::vector<uint8_t> encode_new_tx(const NewTxPayload& p) {
    std::vector<uint8_t> out;
    append_hash32(out, p.tx_hash);
    append_u32(out, static_cast<uint32_t>(p.tx_json.size()));
    out.insert(out.end(), p.tx_json.begin(), p.tx_json.end());
    return out;
}

std::optional<NewTxPayload> decode_new_tx(const std::vector<uint8_t>& payload) {
    ReadCursor c{payload.data(), payload.size(), 0};
    auto hash = read_hash32(c);
    auto len = read_u32(c);
    if (!hash || !len) return std::nullopt;
    auto body = read_bytes(c, *len);
    if (!body) return std::nullopt;
    NewTxPayload p;
    p.tx_hash = std::move(*hash);
    p.tx_json.assign(body->begin(), body->end());
    return p;
}

} // namespace tokiwa::p2p
