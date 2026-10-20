#pragma once

#include <cstdint>
#include <string>

namespace tokiwa::node {

struct NodeConfig {
    uint64_t chain_id = 1337;
    std::string rpc_host = "127.0.0.1";
    uint16_t rpc_port = 8545;
    std::string storage_backend = "memory"; // memory | file | rocksdb
    std::string data_dir = "./data";
    uint64_t difficulty = 1;
    size_t max_peers = 50;
    bool enable_mining = false;
};

/// Load a subset of keys from config/config.yaml (best-effort line parser).
NodeConfig load_node_config(const std::string& path);

} // namespace tokiwa::node
