#pragma once

#include "chainforge/rpc/rpc_server.hpp"
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

namespace chainforge::rpc {

/**
 * Simple HTTP server implementation
 * Provides basic HTTP/1.1 support for RPC calls
 */
class HttpServer {
public:
    explicit HttpServer(const RpcServerConfig& config);
    ~HttpServer();

    // Server control
    bool start();
    void stop();
    bool is_running() const;

    // Request handling
    using RequestHandler = std::function<HttpResponse(const HttpRequest&)>;

    void set_request_handler(RequestHandler handler);
    void remove_request_handler();

private:
    RpcServerConfig config_;
    std::atomic<bool> running_;
    std::thread server_thread_;
    RequestHandler request_handler_;

    // Platform-specific socket handling
    class SocketImpl;
    std::unique_ptr<SocketImpl> socket_impl_;

    // Request parsing
    HttpRequest parse_http_request(const std::string& request_data) const;
    std::string format_http_response(const HttpResponse& response) const;

    // URL parsing utilities
    void parse_query_string(const std::string& query, std::unordered_map<std::string, std::string>& params) const;
    void parse_headers(const std::vector<std::string>& header_lines,
                      std::unordered_map<std::string, std::string>& headers) const;

    // Server loop
    void server_loop();

    // Connection handling
    void handle_connection(int client_socket);
};

} // namespace chainforge::rpc
