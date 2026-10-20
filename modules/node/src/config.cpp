#include "tokiwa/node/config.hpp"
#include <fstream>
#include <sstream>

namespace tokiwa::node {
namespace {

std::string trim(std::string s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\t')) s.pop_back();
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) ++i;
    return s.substr(i);
}

std::string unquote(std::string s) {
    s = trim(s);
    if (s.size() >= 2 && ((s.front() == '"' && s.back() == '"') || (s.front() == '\'' && s.back() == '\''))) {
        return s.substr(1, s.size() - 2);
    }
    // strip inline comments
    auto hash = s.find('#');
    if (hash != std::string::npos) {
        s = trim(s.substr(0, hash));
    }
    return s;
}

} // namespace

NodeConfig load_node_config(const std::string& path) {
    NodeConfig cfg;
    std::ifstream in(path);
    if (!in) {
        return cfg;
    }

    std::string section;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        // section headers like "rpc:" or "network:"
        if (!line.empty() && line.back() == ':' && line.find(' ') == std::string::npos) {
            section = line.substr(0, line.size() - 1);
            continue;
        }

        auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        auto key = trim(line.substr(0, colon));
        auto value = unquote(line.substr(colon + 1));

        if (section == "network" && key == "chain_id") {
            cfg.chain_id = std::stoull(value);
        } else if (section == "rpc" && key == "host") {
            cfg.rpc_host = value;
        } else if (section == "rpc" && key == "port") {
            cfg.rpc_port = static_cast<uint16_t>(std::stoul(value));
        } else if (section == "storage" && key == "database") {
            cfg.storage_backend = value;
        } else if (section == "storage" && key == "data_dir") {
            cfg.data_dir = value;
        } else if (section == "consensus" && key == "difficulty") {
            // allow hex 0x...
            if (value.rfind("0x", 0) == 0 || value.rfind("0X", 0) == 0) {
                cfg.difficulty = std::stoull(value.substr(2), nullptr, 16);
            } else {
                cfg.difficulty = std::stoull(value);
            }
        } else if (section == "p2p" && key == "max_peers") {
            cfg.max_peers = static_cast<size_t>(std::stoull(value));
        }
    }
    return cfg;
}

} // namespace tokiwa::node
