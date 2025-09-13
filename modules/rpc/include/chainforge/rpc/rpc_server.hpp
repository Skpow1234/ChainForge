#pragma once

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <optional>

namespace chainforge {

// Forward declarations
namespace core {
class Block;
class Transaction;
class Address;
class Hash;
}

namespace rpc {

/**
 * JSON-RPC 2.0 request structure
 */
struct JsonRpcRequest {
    std::string jsonrpc = "2.0";
    std::string method;
    nlohmann::json params;
    std::optional<std::string> id;

    static std::optional<JsonRpcRequest> from_json(const std::string& json_str);
    nlohmann::json to_json() const;
};

/**
 * JSON-RPC 2.0 response structure
 */
struct JsonRpcResponse {
    std::string jsonrpc = "2.0";
    std::optional<nlohmann::json> result;
    std::optional<JsonRpcError> error;
    std::optional<std::string> id;

    nlohmann::json to_json() const;
};

/**
 * JSON-RPC error structure
 */
struct JsonRpcError {
    int code;
    std::string message;
    std::optional<nlohmann::json> data;

    static JsonRpcError parse_error(const std::string& message = "Parse error");
    static JsonRpcError invalid_request(const std::string& message = "Invalid request");
    static JsonRpcError method_not_found(const std::string& message = "Method not found");
    static JsonRpcError invalid_params(const std::string& message = "Invalid parameters");
    static JsonRpcError internal_error(const std::string& message = "Internal error");
    static JsonRpcError server_error(int code, const std::string& message);
};

/**
 * RPC method handler function type
 */
using RpcMethodHandler = std::function<JsonRpcResponse(const nlohmann::json& params)>;

/**
 * RPC server configuration
 */
struct RpcServerConfig {
    std::string host = "127.0.0.1";
    uint16_t port = 8545;
    int max_connections = 100;
    int timeout_seconds = 30;
    bool enable_cors = true;
    std::vector<std::string> allowed_origins = {"*"};
};

/**
 * HTTP request structure
 */
struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> query_params;
};

/**
 * HTTP response structure
 */
struct HttpResponse {
    int status_code = 200;
    std::string status_message = "OK";
    std::string body;
    std::unordered_map<std::string, std::string> headers;
    std::string content_type = "application/json";

    static HttpResponse ok(const std::string& body, const std::string& content_type = "application/json");
    static HttpResponse bad_request(const std::string& message = "Bad Request");
    static HttpResponse not_found(const std::string& message = "Not Found");
    static HttpResponse internal_error(const std::string& message = "Internal Server Error");
};

/**
 * RPC server interface
 */
class RpcServer {
public:
    virtual ~RpcServer() = default;

    // Server lifecycle
    virtual bool start(const RpcServerConfig& config) = 0;
    virtual void stop() = 0;
    virtual bool is_running() const = 0;

    // Method registration
    virtual void register_method(const std::string& method_name, RpcMethodHandler handler) = 0;
    virtual void unregister_method(const std::string& method_name) = 0;
    virtual bool has_method(const std::string& method_name) const = 0;

    // Server information
    virtual RpcServerConfig get_config() const = 0;
    virtual std::string get_server_info() const = 0;
};

/**
 * Blockchain RPC methods interface
 */
class BlockchainRpcMethods {
public:
    virtual ~BlockchainRpcMethods() = default;

    // Block-related methods
    virtual JsonRpcResponse eth_getBlockByHash(const nlohmann::json& params) = 0;
    virtual JsonRpcResponse eth_getBlockByNumber(const nlohmann::json& params) = 0;
    virtual JsonRpcResponse eth_blockNumber(const nlohmann::json& params) = 0;

    // Transaction-related methods
    virtual JsonRpcResponse eth_getTransactionByHash(const nlohmann::json& params) = 0;
    virtual JsonRpcResponse eth_getTransactionReceipt(const nlohmann::json& params) = 0;
    virtual JsonRpcResponse eth_sendRawTransaction(const nlohmann::json& params) = 0;

    // Account-related methods
    virtual JsonRpcResponse eth_getBalance(const nlohmann::json& params) = 0;
    virtual JsonRpcResponse eth_getTransactionCount(const nlohmann::json& params) = 0;

    // Network-related methods
    virtual JsonRpcResponse net_version(const nlohmann::json& params) = 0;
    virtual JsonRpcResponse eth_chainId(const nlohmann::json& params) = 0;
    virtual JsonRpcResponse eth_gasPrice(const nlohmann::json& params) = 0;

    // Utility methods
    virtual JsonRpcResponse web3_clientVersion(const nlohmann::json& params) = 0;
};

/**
 * Factory functions
 */
std::unique_ptr<RpcServer> create_rpc_server();
std::unique_ptr<BlockchainRpcMethods> create_blockchain_rpc_methods();

} // namespace rpc
} // namespace chainforge
