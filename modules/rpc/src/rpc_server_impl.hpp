#pragma once

#include "chainforge/rpc/rpc_server.hpp"
#include "http_server.hpp"
#include <unordered_map>
#include <memory>
#include <mutex>

namespace chainforge::rpc {

/**
 * JSON-RPC server implementation
 */
class RpcServerImpl : public RpcServer {
public:
    explicit RpcServerImpl();
    ~RpcServerImpl() override;

    // Server lifecycle
    bool start(const RpcServerConfig& config) override;
    void stop() override;
    bool is_running() const override;

    // Method registration
    void register_method(const std::string& method_name, RpcMethodHandler handler) override;
    void unregister_method(const std::string& method_name) override;
    bool has_method(const std::string& method_name) const override;

    // Server information
    RpcServerConfig get_config() const override;
    std::string get_server_info() const override;

private:
    RpcServerConfig config_;
    std::unique_ptr<HttpServer> http_server_;
    std::unordered_map<std::string, RpcMethodHandler> methods_;
    mutable std::mutex methods_mutex_;

    // HTTP request handling
    HttpResponse handle_http_request(const HttpRequest& request);

    // JSON-RPC processing
    JsonRpcResponse process_jsonrpc_request(const JsonRpcRequest& jsonrpc_request);
    JsonRpcResponse process_batch_request(const std::vector<JsonRpcRequest>& requests);

    // Request validation
    std::optional<JsonRpcRequest> validate_jsonrpc_request(const nlohmann::json& json) const;
    bool is_valid_jsonrpc_request(const nlohmann::json& json) const;

    // CORS handling
    void add_cors_headers(HttpResponse& response) const;
};

} // namespace chainforge::rpc
