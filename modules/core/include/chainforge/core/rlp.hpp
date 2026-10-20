#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace chainforge::core::rlp {

using Bytes = std::vector<uint8_t>;

struct Item {
    bool is_list = false;
    Bytes bytes;
    std::vector<Item> items;
};

Bytes encode_bytes(const uint8_t* data, size_t len);
Bytes encode_bytes(const Bytes& data);
Bytes encode_string(const std::string& s);
Bytes encode_uint(uint64_t value);
Bytes encode_address20(const uint8_t* addr20);
Bytes encode_list(const std::vector<Bytes>& elements);
Bytes encode_list_items(const std::vector<Item>& items);

std::optional<Item> decode(const uint8_t* data, size_t len);
std::optional<Item> decode(const Bytes& data);

uint64_t item_as_uint(const Item& item);
Bytes item_as_bytes(const Item& item);

} // namespace chainforge::core::rlp
