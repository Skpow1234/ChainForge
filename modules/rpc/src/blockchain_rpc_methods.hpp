#pragma once

#include "chainforge/rpc/rpc_server.hpp"
#include "chainforge/rpc/chain_state.hpp"
#include <memory>
#include <unordered_map>

namespace chainforge::rpc {

class BlockchainRpcMethodsImpl : public BlockchainRpcMethods {
public:
    explicit BlockchainRpcMethodsImpl(std::shared_ptr<ChainState> state);
    ~BlockchainRpcMethodsImpl() override;

    JsonRpcResponse eth_getBlockByHash(const nlohmann::json& params) override;
    JsonRpcResponse eth_getBlockByNumber(const nlohmann::json& params) override;
    JsonRpcResponse eth_blockNumber(const nlohmann::json& params) override;

    JsonRpcResponse eth_getTransactionByHash(const nlohmann::json& params) override;
    JsonRpcResponse eth_getTransactionReceipt(const nlohmann::json& params) override;
    JsonRpcResponse eth_sendRawTransaction(const nlohmann::json& params) override;

    JsonRpcResponse eth_getBalance(const nlohmann::json& params) override;
    JsonRpcResponse eth_getTransactionCount(const nlohmann::json& params) override;

    JsonRpcResponse net_version(const nlohmann::json& params) override;
    JsonRpcResponse eth_chainId(const nlohmann::json& params) override;
    JsonRpcResponse eth_gasPrice(const nlohmann::json& params) override;

    JsonRpcResponse web3_clientVersion(const nlohmann::json& params) override;

private:
    std::shared_ptr<ChainState> state_;

    nlohmann::json block_to_rpc(const core::Block& block, bool full_txs) const;
    nlohmann::json tx_to_rpc(const core::Transaction& tx) const;
    std::string number_to_hex(uint64_t number) const;
    uint64_t hex_to_number(const std::string& hex) const;
    std::string strip_0x(std::string hex) const;
};

} // namespace chainforge::rpc
