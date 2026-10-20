#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace tokiwa::p2p {

/// Wire protocol magic bytes: TKW1 (Tokiwa Wire v1)
constexpr uint8_t PROTOCOL_MAGIC_BYTES[4] = {'T', 'K', 'W', '1'};
constexpr uint8_t PROTOCOL_VERSION = 1;
constexpr size_t PROTOCOL_MAX_PAYLOAD = 2 * 1024 * 1024;
constexpr size_t PROTOCOL_HEADER_SIZE = 12; // magic(4)+ver(1)+type(1)+flags(1)+pad(1)+len(4)

enum class MessageType : uint8_t {
    Hello = 1,
    HelloAck = 2,
    Ping = 3,
    Pong = 4,
    GetHeaders = 10,
    Headers = 11,
    GetBlocks = 12,
    Blocks = 13,
    NewBlock = 14,
    NewTx = 20,
    GetTx = 21,
    Tx = 22,
};

struct Message {
    uint8_t version = PROTOCOL_VERSION;
    MessageType type = MessageType::Ping;
    uint8_t flags = 0;
    std::vector<uint8_t> payload;
};

/// Encode a framed message: [magic|ver|type|flags|pad|len_be|payload]
std::vector<uint8_t> encode_message(const Message& msg);

/// Decode one framed message from buffer start. Returns nullopt on invalid frame.
std::optional<Message> decode_message(const uint8_t* data, size_t len);
inline std::optional<Message> decode_message(const std::vector<uint8_t>& data) {
    return decode_message(data.data(), data.size());
}

/// Validate type + version + payload size limits (does not parse payload).
bool validate_message(const Message& msg);

std::string message_type_name(MessageType type);
bool is_known_message_type(MessageType type);

// ---- Payload helpers (little-endian integers, length-prefixed strings) ----

void append_u8(std::vector<uint8_t>& out, uint8_t v);
void append_u16(std::vector<uint8_t>& out, uint16_t v);
void append_u32(std::vector<uint8_t>& out, uint32_t v);
void append_u64(std::vector<uint8_t>& out, uint64_t v);
void append_bytes(std::vector<uint8_t>& out, const uint8_t* data, size_t len);
void append_bytes(std::vector<uint8_t>& out, const std::vector<uint8_t>& data);
void append_string(std::vector<uint8_t>& out, const std::string& s);
void append_hash32(std::vector<uint8_t>& out, const std::vector<uint8_t>& hash32);

struct ReadCursor {
    const uint8_t* data = nullptr;
    size_t size = 0;
    size_t pos = 0;

    bool remaining(size_t n) const { return pos + n <= size; }
};

std::optional<uint8_t> read_u8(ReadCursor& c);
std::optional<uint16_t> read_u16(ReadCursor& c);
std::optional<uint32_t> read_u32(ReadCursor& c);
std::optional<uint64_t> read_u64(ReadCursor& c);
std::optional<std::vector<uint8_t>> read_bytes(ReadCursor& c, size_t n);
std::optional<std::string> read_string(ReadCursor& c);
std::optional<std::vector<uint8_t>> read_hash32(ReadCursor& c);

struct HelloPayload {
    uint64_t chain_id = 0;
    uint64_t tip_height = 0;
    std::vector<uint8_t> tip_hash; // 32 bytes
    std::string agent;
    uint8_t protocol_version = PROTOCOL_VERSION;
};

std::vector<uint8_t> encode_hello(const HelloPayload& p);
std::optional<HelloPayload> decode_hello(const std::vector<uint8_t>& payload);

struct GetHeadersPayload {
    uint64_t from_height = 0;
    uint32_t max_count = 192;
};

std::vector<uint8_t> encode_get_headers(const GetHeadersPayload& p);
std::optional<GetHeadersPayload> decode_get_headers(const std::vector<uint8_t>& payload);

struct HeaderEntry {
    uint64_t height = 0;
    std::vector<uint8_t> hash; // 32
};

struct HeadersPayload {
    std::vector<HeaderEntry> headers;
};

std::vector<uint8_t> encode_headers(const HeadersPayload& p);
std::optional<HeadersPayload> decode_headers(const std::vector<uint8_t>& payload);

struct GetBlocksPayload {
    std::vector<std::vector<uint8_t>> hashes; // each 32
};

std::vector<uint8_t> encode_get_blocks(const GetBlocksPayload& p);
std::optional<GetBlocksPayload> decode_get_blocks(const std::vector<uint8_t>& payload);

struct BlocksPayload {
    std::vector<std::string> block_jsons;
};

std::vector<uint8_t> encode_blocks(const BlocksPayload& p);
std::optional<BlocksPayload> decode_blocks(const std::vector<uint8_t>& payload);

struct NewTxPayload {
    std::vector<uint8_t> tx_hash; // 32
    std::string tx_json;          // optional full body
};

std::vector<uint8_t> encode_new_tx(const NewTxPayload& p);
std::optional<NewTxPayload> decode_new_tx(const std::vector<uint8_t>& payload);

} // namespace tokiwa::p2p
