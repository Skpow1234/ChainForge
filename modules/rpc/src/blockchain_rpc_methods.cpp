#include "blockchain_rpc_methods.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace chainforge::rpc {

BlockchainRpcMethodsImpl::BlockchainRpcMethodsImpl() {
    // Initialize some mock data for demonstration
    mock_balances_["0x742d35cc6634c0532925a3b844bc454e4438f44e"] = "0x1000000000000000000"; // 1 ETH in wei
    mock_balances_["0x742d35cc6634c0532925a3b844bc454e4438f44f"] = "0x500000000000000000";  // 0.5 ETH in wei

    // Create a mock genesis block
    mock_blocks_["0x0000000000000000000000000000000000000000000000000000000000000000"] =
        create_mock_block(0, "0x0000000000000000000000000000000000000000000000000000000000000000");
}

BlockchainRpcMethodsImpl::~BlockchainRpcMethodsImpl() = default;

// Block-related methods
JsonRpcResponse BlockchainRpcMethodsImpl::eth_getBlockByHash(const nlohmann::json& params) {
    if (params.empty() || !params[0].is_string()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_params("Block hash required");
        return response;
    }

    std::string block_hash = params[0];
    bool include_transactions = params.size() > 1 ? params[1].get<bool>() : false;

    auto it = mock_blocks_.find(block_hash);
    if (it == mock_blocks_.end()) {
        JsonRpcResponse response;
        response.result = nullptr; // Block not found
        return response;
    }

    JsonRpcResponse response;
    response.result = it->second;
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getBlockByNumber(const nlohmann::json& params) {
    if (params.empty()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_params("Block number required");
        return response;
    }

    uint64_t block_number;
    if (params[0].is_string()) {
        std::string block_num_str = params[0];
        if (block_num_str == "latest") {
            block_number = 0; // Mock latest block
        } else if (block_num_str == "earliest") {
            block_number = 0; // Genesis block
        } else if (block_num_str == "pending") {
            block_number = 1; // Mock pending block
        } else {
            block_number = hex_to_number(block_num_str);
        }
    } else if (params[0].is_number()) {
        block_number = params[0];
    } else {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_params("Invalid block number format");
        return response;
    }

    // For demonstration, return the genesis block for any number
    std::string block_hash = "0x0000000000000000000000000000000000000000000000000000000000000000";

    JsonRpcResponse response;
    response.result = create_mock_block(block_number, block_hash);
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_blockNumber(const nlohmann::json& params) {
    JsonRpcResponse response;
    response.result = number_to_hex(0); // Mock current block number
    return response;
}

// Transaction-related methods
JsonRpcResponse BlockchainRpcMethodsImpl::eth_getTransactionByHash(const nlohmann::json& params) {
    if (params.empty() || !params[0].is_string()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_params("Transaction hash required");
        return response;
    }

    std::string tx_hash = params[0];
    auto it = mock_transactions_.find(tx_hash);
    if (it == mock_transactions_.end()) {
        JsonRpcResponse response;
        response.result = nullptr; // Transaction not found
        return response;
    }

    JsonRpcResponse response;
    response.result = it->second;
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getTransactionReceipt(const nlohmann::json& params) {
    if (params.empty() || !params[0].is_string()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_params("Transaction hash required");
        return response;
    }

    // Mock transaction receipt
    nlohmann::json receipt = {
        {"transactionHash", params[0]},
        {"transactionIndex", "0x0"},
        {"blockHash", "0x0000000000000000000000000000000000000000000000000000000000000000"},
        {"blockNumber", "0x0"},
        {"from", "0x742d35cc6634c0532925a3b844bc454e4438f44e"},
        {"to", "0x742d35cc6634c0532925a3b844bc454e4438f44f"},
        {"cumulativeGasUsed", "0x5208"},
        {"gasUsed", "0x5208"},
        {"contractAddress", nullptr},
        {"logs", nlohmann::json::array()},
        {"logsBloom", "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"},
        {"status", "0x1"}
    };

    JsonRpcResponse response;
    response.result = receipt;
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_sendRawTransaction(const nlohmann::json& params) {
    if (params.empty() || !params[0].is_string()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_params("Raw transaction data required");
        return response;
    }

    // Mock transaction hash - in real implementation, this would decode and validate the transaction
    std::string tx_hash = "0x" + std::string(64, '0'); // Mock hash

    JsonRpcResponse response;
    response.result = tx_hash;
    return response;
}

// Account-related methods
JsonRpcResponse BlockchainRpcMethodsImpl::eth_getBalance(const nlohmann::json& params) {
    if (params.empty() || !params[0].is_string()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_params("Account address required");
        return response;
    }

    std::string address = params[0];
    auto it = mock_balances_.find(address);
    std::string balance = (it != mock_balances_.end()) ? it->second : "0x0";

    JsonRpcResponse response;
    response.result = balance;
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_getTransactionCount(const nlohmann::json& params) {
    if (params.empty() || !params[0].is_string()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_params("Account address required");
        return response;
    }

    // Mock transaction count
    JsonRpcResponse response;
    response.result = "0x0"; // No transactions for mock accounts
    return response;
}

// Network-related methods
JsonRpcResponse BlockchainRpcMethodsImpl::net_version(const nlohmann::json& params) {
    JsonRpcResponse response;
    response.result = network_version_;
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_chainId(const nlohmann::json& params) {
    JsonRpcResponse response;
    response.result = number_to_hex(chain_id_);
    return response;
}

JsonRpcResponse BlockchainRpcMethodsImpl::eth_gasPrice(const nlohmann::json& params) {
    JsonRpcResponse response;
    response.result = "0x3b9aca00"; // 1 gwei in wei
    return response;
}

// Utility methods
JsonRpcResponse BlockchainRpcMethodsImpl::web3_clientVersion(const nlohmann::json& params) {
    JsonRpcResponse response;
    response.result = "ChainForge/v0.1.0";
    return response;
}

// Helper methods
nlohmann::json BlockchainRpcMethodsImpl::create_mock_block(uint64_t number, const std::string& hash) const {
    return {
        {"number", number_to_hex(number)},
        {"hash", hash},
        {"parentHash", "0x0000000000000000000000000000000000000000000000000000000000000000"},
        {"nonce", "0x0000000000000000"},
        {"sha3Uncles", "0x1dcc4de8dec75d7aab85b567b6ccd41ad312451b948a7413f0a142fd40d49347"},
        {"logsBloom", "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"},
        {"transactionsRoot", "0x56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421"},
        {"stateRoot", "0x0000000000000000000000000000000000000000000000000000000000000000"},
        {"receiptsRoot", "0x56e81f171bcc55a6ff8345e692c0f86e5b48e01b996cadc001622fb5e363b421"},
        {"miner", "0x0000000000000000000000000000000000000000"},
        {"difficulty", "0x0"},
        {"totalDifficulty", "0x0"},
        {"extraData", "0x"},
        {"size", "0x3e8"},
        {"gasLimit", "0x6691b7"},
        {"gasUsed", "0x0"},
        {"timestamp", number_to_hex(static_cast<uint64_t>(std::time(nullptr)))},
        {"transactions", nlohmann::json::array()},
        {"uncles", nlohmann::json::array()}
    };
}

nlohmann::json BlockchainRpcMethodsImpl::create_mock_transaction(const std::string& hash) const {
    return {
        {"hash", hash},
        {"nonce", "0x0"},
        {"blockHash", "0x0000000000000000000000000000000000000000000000000000000000000000"},
        {"blockNumber", "0x0"},
        {"transactionIndex", "0x0"},
        {"from", "0x742d35cc6634c0532925a3b844bc454e4438f44e"},
        {"to", "0x742d35cc6634c0532925a3b844bc454e4438f44f"},
        {"value", "0xde0b6b3a7640000"},
        {"gasPrice", "0x3b9aca00"},
        {"gas", "0x15f90"},
        {"input", "0x"},
        {"v", "0x1c"},
        {"r", "0x1c"},
        {"s", "0x1c"}
    };
}

std::string BlockchainRpcMethodsImpl::number_to_hex(uint64_t number) const {
    std::stringstream ss;
    ss << "0x" << std::hex << number;
    return ss.str();
}

uint64_t BlockchainRpcMethodsImpl::hex_to_number(const std::string& hex) const {
    if (hex.size() < 3 || hex.substr(0, 2) != "0x") {
        return 0;
    }

    try {
        return std::stoull(hex.substr(2), nullptr, 16);
    } catch (const std::exception&) {
        return 0;
    }
}

bool BlockchainRpcMethodsImpl::is_valid_hex(const std::string& hex, size_t expected_length) const {
    if (expected_length > 0 && hex.size() != expected_length + 2) { // +2 for "0x"
        return false;
    }

    if (hex.size() < 3 || hex.substr(0, 2) != "0x") {
        return false;
    }

    return hex.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos;
}

// Factory function
std::unique_ptr<BlockchainRpcMethods> create_blockchain_rpc_methods() {
    return std::make_unique<BlockchainRpcMethodsImpl>();
}

} // namespace chainforge::rpc
