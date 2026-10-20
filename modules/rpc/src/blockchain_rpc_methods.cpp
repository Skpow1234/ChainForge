#include "blockchain_rpc_methods.hpp"
#include "tokiwa/core/address.hpp"
#include "tokiwa/core/hash.hpp"
#include <sstream>
#include <ctime>

namespace tokiwa::rpc {

BlockchainRpcMethodsImpl::BlockchainRpcMethodsImpl(std::shared_ptr<ChainState> state)
    : state_(std::move(state)) {}

BlockchainRpcMethodsImpl::~BlockchainRpcMethodsImpl() = default;

std::string BlockchainRpcMethodsImpl::strip_0x(std::string hex) const {
    if (hex.size() >= 2 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        hex.erase(0, 2);
    }
    return hex;
}

std::string BlockchainRpcMethodsImpl::number_to_hex(uint64_t number) const {
    std::stringstream ss;
    ss << "0x" << std::hex << number;
    return ss.str();
}

uint64_t BlockchainRpcMethodsImpl::hex_to_number(const std::string& hex) const {
    try {
        return std::stoull(strip_0x(hex), nullptr, 16);
    } catch (const std::exception&) {
        return 0;
    }
}

nlohmann::json BlockchainRpcMethodsImpl::tx_to_rpc(const core::Transaction& tx) const {
    return nlohmann::json::parse(tx.to_json());
}

nlohmann::json BlockchainRpcMethodsImpl::block_to_rpc(const core::Block& block, bool full_txs) const {
    nlohmann::json j;
    j["number"] = number_to_hex(block.height());
    j["hash"] = "0x" + block.calculate_hash().to_hex();
    j["parentHash"] = "0x" + block.parent_hash().to_hex();
    j["nonce"] = number_to_hex(block.nonce());
    j["transactionsRoot"] = "0x" + block.merkle_root().to_hex();
    j["gasLimit"] = number_to_hex(block.gas_limit());
    j["gasUsed"] = "0x0";
    j["timestamp"] = number_to_hex(block.timestamp().seconds());
    j["miner"] = "0x0000000000000000000000000000000000000000";
    j["difficulty"] = "0x1";
    j["totalDifficulty"] = "0x1";
    j["extraData"] = "0x";
    j["size"] = number_to_hex(block.size());
    j["uncles"] = nlohmann::json::array();

    nlohmann::json txs = nlohmann::json::array();
    for (const auto& tx : block.transactions()) {
        if (full_txs) {
            txs.push_back(tx_to_rpc(tx));
        } else {
            txs.push_back("0x" + tx.calculate_hash().to_hex());
        }
    }
    j["transactions"] = txs;
    return j;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getBlockByHash(const nlohmann::json& params) {
    JsonRpcResponse response;
    if (!state_ || params.empty() || !params[0].is_string()) {
        response.error = JsonRpcError::invalid_params("Block hash required");
        return response;
    }
    const bool full = params.size() > 1 && params[1].is_boolean() && params[1].get<bool>();
    try {
        core::Hash hash(strip_0x(params[0].get<std::string>()));
        auto block = state_->get_block_by_hash(hash);
        response.result = block ? block_to_rpc(*block, full) : nullptr;
    } catch (const std::exception& e) {
        response.error = JsonRpcError::invalid_params(e.what());
    }
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getBlockByNumber(const nlohmann::json& params) {
    JsonRpcResponse response;
    if (!state_ || params.empty()) {
        response.error = JsonRpcError::invalid_params("Block number required");
        return response;
    }

    uint64_t number = 0;
    if (params[0].is_string()) {
        const std::string s = params[0];
        if (s == "latest" || s == "pending") {
            number = state_->latest_height();
        } else if (s == "earliest") {
            number = 0;
        } else {
            number = hex_to_number(s);
        }
    } else if (params[0].is_number()) {
        number = params[0];
    } else {
        response.error = JsonRpcError::invalid_params("Invalid block number format");
        return response;
    }

    const bool full = params.size() > 1 && params[1].is_boolean() && params[1].get<bool>();
    auto block = state_->get_block_by_number(number);
    response.result = block ? block_to_rpc(*block, full) : nullptr;
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_blockNumber(const nlohmann::json& /*params*/) {
    JsonRpcResponse response;
    response.result = number_to_hex(state_ ? state_->latest_height() : 0);
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getTransactionByHash(const nlohmann::json& params) {
    JsonRpcResponse response;
    if (!state_ || params.empty() || !params[0].is_string()) {
        response.error = JsonRpcError::invalid_params("Transaction hash required");
        return response;
    }
    try {
        core::Hash hash(strip_0x(params[0].get<std::string>()));
        auto tx = state_->get_transaction(hash);
        response.result = tx ? tx_to_rpc(*tx) : nullptr;
    } catch (const std::exception& e) {
        response.error = JsonRpcError::invalid_params(e.what());
    }
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getTransactionReceipt(const nlohmann::json& params) {
    JsonRpcResponse response;
    if (!state_ || params.empty() || !params[0].is_string()) {
        response.error = JsonRpcError::invalid_params("Transaction hash required");
        return response;
    }
    try {
        core::Hash hash(strip_0x(params[0].get<std::string>()));
        auto tx = state_->get_transaction(hash);
        if (!tx) {
            response.result = nullptr;
            return response;
        }
        response.result = nlohmann::json{
            {"transactionHash", "0x" + hash.to_hex()},
            {"transactionIndex", "0x0"},
            {"blockNumber", number_to_hex(state_->latest_height())},
            {"from", "0x" + tx->from().to_hex()},
            {"to", "0x" + tx->to().to_hex()},
            {"gasUsed", "0x5208"},
            {"cumulativeGasUsed", "0x5208"},
            {"contractAddress", nullptr},
            {"logs", nlohmann::json::array()},
            {"status", "0x1"}
        };
    } catch (const std::exception& e) {
        response.error = JsonRpcError::invalid_params(e.what());
    }
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_sendRawTransaction(const nlohmann::json& params) {
    JsonRpcResponse response;
    if (!state_ || params.empty() || !params[0].is_string()) {
        response.error = JsonRpcError::invalid_params("Raw transaction data required");
        return response;
    }

    const std::string raw = strip_0x(params[0].get<std::string>());
    std::vector<uint8_t> bytes;
    bytes.reserve(raw.size() / 2);
    try {
        for (size_t i = 0; i + 1 < raw.size(); i += 2) {
            bytes.push_back(static_cast<uint8_t>(std::stoi(raw.substr(i, 2), nullptr, 16)));
        }
    } catch (const std::exception&) {
        response.error = JsonRpcError::invalid_params("Invalid hex in raw transaction");
        return response;
    }
    if (bytes.empty()) {
        response.error = JsonRpcError::invalid_params("Empty raw transaction");
        return response;
    }

    try {
        auto tx = core::Transaction::from_rlp(bytes);
        if (!tx.is_valid() || !tx.validate_signature()) {
            response.error = JsonRpcError::invalid_params("Transaction invalid or bad signature");
            return response;
        }
        if (tx.chain_id() != 0 && tx.chain_id() != state_->chain_id()) {
            response.error = JsonRpcError::invalid_params("chainId mismatch");
            return response;
        }
        const auto hash = state_->submit_transaction(tx);
        response.result = "0x" + hash.to_hex();
    } catch (const std::exception& e) {
        response.error = JsonRpcError::invalid_params(e.what());
    }
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getBalance(const nlohmann::json& params) {
    JsonRpcResponse response;
    if (!state_ || params.empty() || !params[0].is_string()) {
        response.error = JsonRpcError::invalid_params("Account address required");
        return response;
    }
    try {
        core::Address addr(params[0].get<std::string>());
        response.result = state_->get_balance_hex(addr);
    } catch (const std::exception& e) {
        response.error = JsonRpcError::invalid_params(e.what());
    }
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getTransactionCount(const nlohmann::json& params) {
    JsonRpcResponse response;
    if (!state_ || params.empty() || !params[0].is_string()) {
        response.error = JsonRpcError::invalid_params("Account address required");
        return response;
    }
    try {
        core::Address addr(params[0].get<std::string>());
        response.result = number_to_hex(state_->get_nonce(addr));
    } catch (const std::exception& e) {
        response.error = JsonRpcError::invalid_params(e.what());
    }
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::net_version(const nlohmann::json& /*params*/) {
    JsonRpcResponse response;
    response.result = state_ ? std::to_string(state_->chain_id()) : "1337";
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_chainId(const nlohmann::json& /*params*/) {
    JsonRpcResponse response;
    response.result = number_to_hex(state_ ? state_->chain_id() : 1337);
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_gasPrice(const nlohmann::json& /*params*/) {
    JsonRpcResponse response;
    response.result = "0x3b9aca00";
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::web3_clientVersion(const nlohmann::json& /*params*/) {
    JsonRpcResponse response;
    response.result = "Tokiwa/v0.1.0";
    return response;
}

std::unique_ptr<BlockchainRpcMethods> create_blockchain_rpc_methods() {
    return std::make_unique<BlockchainRpcMethodsImpl>(nullptr);
}

std::unique_ptr<BlockchainRpcMethods> create_blockchain_rpc_methods(std::shared_ptr<ChainState> state) {
    return std::make_unique<BlockchainRpcMethodsImpl>(std::move(state));
}

void register_blockchain_rpc_methods(RpcServer& server, BlockchainRpcMethods& methods) {
    server.register_method("eth_getBlockByHash", [&](const nlohmann::json& p) { return methods.eth_getBlockByHash(p); });
    server.register_method("eth_getBlockByNumber", [&](const nlohmann::json& p) { return methods.eth_getBlockByNumber(p); });
    server.register_method("eth_blockNumber", [&](const nlohmann::json& p) { return methods.eth_blockNumber(p); });
    server.register_method("eth_getTransactionByHash", [&](const nlohmann::json& p) { return methods.eth_getTransactionByHash(p); });
    server.register_method("eth_getTransactionReceipt", [&](const nlohmann::json& p) { return methods.eth_getTransactionReceipt(p); });
    server.register_method("eth_sendRawTransaction", [&](const nlohmann::json& p) { return methods.eth_sendRawTransaction(p); });
    server.register_method("eth_getBalance", [&](const nlohmann::json& p) { return methods.eth_getBalance(p); });
    server.register_method("eth_getTransactionCount", [&](const nlohmann::json& p) { return methods.eth_getTransactionCount(p); });
    server.register_method("net_version", [&](const nlohmann::json& p) { return methods.net_version(p); });
    server.register_method("eth_chainId", [&](const nlohmann::json& p) { return methods.eth_chainId(p); });
    server.register_method("eth_gasPrice", [&](const nlohmann::json& p) { return methods.eth_gasPrice(p); });
    server.register_method("web3_clientVersion", [&](const nlohmann::json& p) { return methods.web3_clientVersion(p); });
}

} // namespace tokiwa::rpc
