#pragma once

#include "tcp_connection.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <chrono>

namespace chainforge {
namespace p2p {

/**
 * @brief TCP client for outgoing connections
 * 
 * Features:
 * - Connect to remote servers
 * - Auto-reconnect support
 * - Connection timeout
 * - Connection state tracking
 */
class TcpClient {
public:
    /**
     * @brief Construct TCP client
     */
    explicit TcpClient(asio::io_context& io_context);

    ~TcpClient();

    // Non-copyable, movable
    TcpClient(const TcpClient&) = delete;
    TcpClient& operator=(const TcpClient&) = delete;
    TcpClient(TcpClient&&) = default;
    TcpClient& operator=(TcpClient&&) = default;

    /**
     * @brief Connect to remote server
     */
    TcpResult<void> connect(const std::string& host, uint16_t port);

    /**
     * @brief Connect with timeout
     */
    TcpResult<void> connect(
        const std::string& host,
        uint16_t port,
        std::chrono::milliseconds timeout
    );

    /**
     * @brief Async connect to remote server
     */
    void async_connect(
        const std::string& host,
        uint16_t port,
        std::function<void(const boost::system::error_code&)> handler
    );

    /**
     * @brief Disconnect from server
     */
    void disconnect();

    /**
     * @brief Check if connected
     */
    bool is_connected() const noexcept;

    /**
     * @brief Get the connection
     */
    std::shared_ptr<TcpConnection> connection() const noexcept;

    /**
     * @brief Enable auto-reconnect
     */
    void enable_auto_reconnect(
        std::chrono::milliseconds retry_interval = std::chrono::seconds(5)
    );

    /**
     * @brief Disable auto-reconnect
     */
    void disable_auto_reconnect();

    /**
     * @brief Send data
     */
    TcpResult<size_t> send(const std::vector<uint8_t>& data);

    /**
     * @brief Start receiving data
     */
    void start_receive(ReceiveCallback callback);

    /**
     * @brief Stop receiving data
     */
    void stop_receive();

    /**
     * @brief Set connection callback
     */
    void set_connection_callback(ConnectionCallback callback);

private:
    void do_reconnect();
    void schedule_reconnect();

    asio::io_context& io_context_;
    std::shared_ptr<TcpConnection> connection_;
    
    std::string host_;
    uint16_t port_{0};
    
    bool auto_reconnect_{false};
    std::chrono::milliseconds reconnect_interval_{5000};
    std::unique_ptr<asio::steady_timer> reconnect_timer_;
    
    ConnectionCallback connection_callback_;
};

} // namespace p2p
} // namespace chainforge

