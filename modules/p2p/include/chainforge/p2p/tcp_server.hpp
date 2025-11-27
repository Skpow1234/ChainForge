#pragma once

#include "tcp_connection.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <mutex>

namespace chainforge {
namespace p2p {

/**
 * @brief Callback for new connections
 */
using AcceptCallback = std::function<void(std::shared_ptr<TcpConnection>)>;

/**
 * @brief TCP server for accepting incoming connections
 * 
 * Features:
 * - Accepts incoming TCP connections
 * - Manages multiple client connections
 * - Async accept loop
 * - Connection tracking
 */
class TcpServer {
public:
    /**
     * @brief Construct TCP server
     */
    explicit TcpServer(asio::io_context& io_context);

    ~TcpServer();

    // Non-copyable, non-movable (has threads)
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;
    TcpServer(TcpServer&&) = delete;
    TcpServer& operator=(TcpServer&&) = delete;

    /**
     * @brief Start listening on port
     */
    TcpResult<void> start(uint16_t port, const std::string& address = "0.0.0.0");

    /**
     * @brief Stop listening and close all connections
     */
    void stop();

    /**
     * @brief Check if server is running
     */
    bool is_running() const noexcept;

    /**
     * @brief Get number of active connections
     */
    size_t connection_count() const noexcept;

    /**
     * @brief Get all active connections
     */
    std::vector<std::shared_ptr<TcpConnection>> get_connections() const;

    /**
     * @brief Get connection by ID
     */
    std::shared_ptr<TcpConnection> get_connection(uint64_t id) const;

    /**
     * @brief Remove connection by ID
     */
    void remove_connection(uint64_t id);

    /**
     * @brief Set callback for new connections
     */
    void set_accept_callback(AcceptCallback callback);

    /**
     * @brief Broadcast data to all connections
     */
    void broadcast(const std::vector<uint8_t>& data);

    /**
     * @brief Get listening port
     */
    uint16_t port() const noexcept { return port_; }

private:
    void do_accept();
    void handle_accept(
        const boost::system::error_code& error,
        tcp::socket socket
    );

    asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::atomic<bool> running_{false};
    uint16_t port_{0};
    
    // Connection tracking
    mutable std::mutex connections_mutex_;
    std::unordered_map<uint64_t, std::shared_ptr<TcpConnection>> connections_;
    
    AcceptCallback accept_callback_;
};

} // namespace p2p
} // namespace chainforge

