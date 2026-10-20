#include "chainforge/core/rlp.hpp"
#include <stdexcept>

namespace chainforge::core::rlp {
namespace {

Bytes encode_length(size_t len, uint8_t offset) {
    if (len < 56) {
        return Bytes{static_cast<uint8_t>(offset + len)};
    }
    Bytes len_bytes;
    size_t tmp = len;
    while (tmp > 0) {
        len_bytes.insert(len_bytes.begin(), static_cast<uint8_t>(tmp & 0xFF));
        tmp >>= 8;
    }
    Bytes out;
    out.push_back(static_cast<uint8_t>(offset + 55 + len_bytes.size()));
    out.insert(out.end(), len_bytes.begin(), len_bytes.end());
    return out;
}

} // namespace

Bytes encode_bytes(const uint8_t* data, size_t len) {
    if (len == 1 && data[0] < 0x80) {
        return Bytes{data[0]};
    }
    auto prefix = encode_length(len, 0x80);
    Bytes out = prefix;
    out.insert(out.end(), data, data + len);
    return out;
}

Bytes encode_bytes(const Bytes& data) {
    return encode_bytes(data.data(), data.size());
}

Bytes encode_string(const std::string& s) {
    return encode_bytes(reinterpret_cast<const uint8_t*>(s.data()), s.size());
}

Bytes encode_uint(uint64_t value) {
    if (value == 0) {
        return encode_bytes(nullptr, 0);
    }
    Bytes be;
    for (int i = 7; i >= 0; --i) {
        uint8_t b = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
        if (!be.empty() || b != 0) {
            be.push_back(b);
        }
    }
    return encode_bytes(be);
}

Bytes encode_address20(const uint8_t* addr20) {
    bool zero = true;
    for (size_t i = 0; i < 20; ++i) {
        if (addr20[i] != 0) {
            zero = false;
            break;
        }
    }
    if (zero) {
        return encode_bytes(nullptr, 0);
    }
    return encode_bytes(addr20, 20);
}

Bytes encode_list(const std::vector<Bytes>& elements) {
    Bytes payload;
    for (const auto& el : elements) {
        payload.insert(payload.end(), el.begin(), el.end());
    }
    auto prefix = encode_length(payload.size(), 0xc0);
    Bytes out = prefix;
    out.insert(out.end(), payload.begin(), payload.end());
    return out;
}

Bytes encode_list_items(const std::vector<Item>& items) {
    std::vector<Bytes> encoded;
    encoded.reserve(items.size());
    for (const auto& item : items) {
        if (item.is_list) {
            encoded.push_back(encode_list_items(item.items));
        } else {
            encoded.push_back(encode_bytes(item.bytes));
        }
    }
    return encode_list(encoded);
}

std::optional<Item> decode(const uint8_t* data, size_t len) {
    if (len == 0) {
        return std::nullopt;
    }

    size_t pos = 0;
    auto decode_one = [&](auto&& self) -> std::optional<Item> {
        if (pos >= len) {
            return std::nullopt;
        }
        const uint8_t prefix = data[pos++];
        Item item;

        if (prefix <= 0x7f) {
            item.is_list = false;
            item.bytes = Bytes{prefix};
            return item;
        }

        if (prefix <= 0xb7) {
            const size_t str_len = prefix - 0x80;
            if (pos + str_len > len) return std::nullopt;
            item.is_list = false;
            item.bytes.assign(data + pos, data + pos + str_len);
            pos += str_len;
            return item;
        }

        if (prefix <= 0xbf) {
            const size_t len_of_len = prefix - 0xb7;
            if (pos + len_of_len > len) return std::nullopt;
            size_t str_len = 0;
            for (size_t i = 0; i < len_of_len; ++i) {
                str_len = (str_len << 8) | data[pos++];
            }
            if (pos + str_len > len) return std::nullopt;
            item.is_list = false;
            item.bytes.assign(data + pos, data + pos + str_len);
            pos += str_len;
            return item;
        }

        if (prefix <= 0xf7) {
            const size_t list_len = prefix - 0xc0;
            if (pos + list_len > len) return std::nullopt;
            const size_t end = pos + list_len;
            item.is_list = true;
            while (pos < end) {
                auto child = self(self);
                if (!child) return std::nullopt;
                item.items.push_back(std::move(*child));
            }
            return item;
        }

        const size_t len_of_len = prefix - 0xf7;
        if (pos + len_of_len > len) return std::nullopt;
        size_t list_len = 0;
        for (size_t i = 0; i < len_of_len; ++i) {
            list_len = (list_len << 8) | data[pos++];
        }
        if (pos + list_len > len) return std::nullopt;
        const size_t end = pos + list_len;
        item.is_list = true;
        while (pos < end) {
            auto child = self(self);
            if (!child) return std::nullopt;
            item.items.push_back(std::move(*child));
        }
        return item;
    };

    auto root = decode_one(decode_one);
    if (!root || pos != len) {
        // Allow trailing data? Require exact for safety.
        if (!root) return std::nullopt;
    }
    return root;
}

std::optional<Item> decode(const Bytes& data) {
    return decode(data.data(), data.size());
}

uint64_t item_as_uint(const Item& item) {
    if (item.is_list) {
        throw std::invalid_argument("RLP item is a list");
    }
    uint64_t v = 0;
    for (uint8_t b : item.bytes) {
        v = (v << 8) | b;
    }
    return v;
}

Bytes item_as_bytes(const Item& item) {
    if (item.is_list) {
        throw std::invalid_argument("RLP item is a list");
    }
    return item.bytes;
}

} // namespace chainforge::core::rlp
