#pragma once

#include "chainforge/rpc/rpc_server.hpp"
#include <memory>

namespace chainforge::rpc {

/**
 * Blockchain RPC methods implementation
 * Provides Ethereum-compatible JSON-RPC API endpoints
 */
class BlockchainRpcMethodsImpl : public BlockchainRpcMethods {
public:
    BlockchainRpcMethodsImpl();
    ~BlockchainRpcMethodsImpl() override;

    // Block-related methods
    JsonRpcResponse eth_getBlockByHash(const nlohmann::json& params) override;
    JsonRpcResponse eth_getBlockByNumber(const nlohmann::json& params) override;
    JsonRpcResponse eth_blockNumber(const nlohmann::json& params) override;

    // Transaction-related methods
    JsonRpcResponse eth_getTransactionByHash(const nlohmann::json& params) override;
    JsonRpcResponse eth_getTransactionReceipt(const nlohmann::json& params) override;
    JsonRpcResponse eth_sendRawTransaction(const nlohmann::json& params) override;

    // Account-related methods
    JsonRpcResponse eth_getBalance(const nlohmann::json& params) override;
    JsonRpcResponse eth_getTransactionCount(const nlohmann::json& params) override;

    // Network-related methods
    JsonRpcResponse net_version(const nlohmann::json& params) override;
    JsonRpcResponse eth_chainId(const nlohmann::json& params) override;
    JsonRpcResponse eth_gasPrice(const nlohmann::json& params) override;

    // Utility methods
    JsonRpcResponse web3_clientVersion(const nlohmann::json& params) override;

private:
    // Chain and network information
    uint64_t chain_id_ = 1;  // Mainnet by default
    std::string network_version_ = "1";

    // Mock data for demonstration (in a real implementation, this would connect to actual blockchain state)
    std::unordered_map<std::string, nlohmann::json> mock_blocks_;
    std::unordered_map<std::string, nlohmann::json> mock_transactions_;
    std::unordered_map<std::string, std::string> mock_balances_;

    // Helper methods
    nlohmann::json create_mock_block(uint64_t number, const std::string& hash) const;
    nlohmann::json create_mock_transaction(const std::string& hash) const;
    std::string number_to_hex(uint64_t number) const;
    uint64_t hex_to_number(const std::string& hex) const;
    bool is_valid_hex(const std::string& hex, size_t expected_length = 0) const;
};

} // namespace chainforge::rpc
