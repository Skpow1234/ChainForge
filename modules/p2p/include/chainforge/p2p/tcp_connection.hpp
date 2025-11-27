#pragma once

#include "chainforge/core/expected.hpp"
#include "chainforge/core/error.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <atomic>

namespace chainforge {
namespace p2p {

namespace asio = boost::asio;
using tcp = asio::ip::tcp;

/**
 * @brief Error codes for TCP operations
 */
enum class TcpError {
    CONNECTION_FAILED = 1,
    CONNECTION_CLOSED = 2,
    WRITE_FAILED = 3,
    READ_FAILED = 4,
    TIMEOUT = 5,
    INVALID_ADDRESS = 6,
    ALREADY_CONNECTED = 7,
    NOT_CONNECTED = 8
};

/**
 * @brief Result type for TCP operations
 */
template<typename T>
using TcpResult = core::Result<T>;

/**
 * @brief Callback for received data
 */
using ReceiveCallback = std::function<void(const std::vector<uint8_t>&)>;

/**
 * @brief Callback for connection events
 */
using ConnectionCallback = std::function<void(bool connected)>;

/**
 * @brief TCP connection class for client/server communication
 * 
 * This class wraps a Boost.Asio TCP socket and provides:
 * - Async read/write operations
 * - Connection management
 * - Error handling
 * - Callbacks for events
 */
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    /**
     * @brief Create a TCP connection with an existing socket
     */
    static std::shared_ptr<TcpConnection> create(asio::io_context& io_context);
    
    /**
     * @brief Create a TCP connection from an accepted socket
     */
    static std::shared_ptr<TcpConnection> create(tcp::socket socket);

    ~TcpConnection();

    // Non-copyable, movable
    TcpConnection(const TcpConnection&) = delete;
    TcpConnection& operator=(const TcpConnection&) = delete;
    TcpConnection(TcpConnection&&) = default;
    TcpConnection& operator=(TcpConnection&&) = default;

    /**
     * @brief Connect to a remote endpoint
     */
    TcpResult<void> connect(const std::string& host, uint16_t port);

    /**
     * @brief Async connect to a remote endpoint
     */
    void async_connect(
        const std::string& host,
        uint16_t port,
        std::function<void(const boost::system::error_code&)> handler
    );

    /**
     * @brief Send data synchronously
     */
    TcpResult<size_t> send(const std::vector<uint8_t>& data);

    /**
     * @brief Send data asynchronously
     */
    void async_send(
        const std::vector<uint8_t>& data,
        std::function<void(const boost::system::error_code&, size_t)> handler
    );

    /**
     * @brief Receive data synchronously
     */
    TcpResult<std::vector<uint8_t>> receive(size_t max_length = 8192);

    /**
     * @brief Start async receive loop
     */
    void start_receive(ReceiveCallback callback);

    /**
     * @brief Stop async receive loop
     */
    void stop_receive();

    /**
     * @brief Close the connection
     */
    void close();

    /**
     * @brief Check if connected
     */
    bool is_connected() const noexcept;

    /**
     * @brief Get local endpoint
     */
    TcpResult<std::string> local_endpoint() const;

    /**
     * @brief Get remote endpoint
     */
    TcpResult<std::string> remote_endpoint() const;

    /**
     * @brief Get the underlying socket
     */
    tcp::socket& socket() noexcept { return socket_; }

    /**
     * @brief Set connection callback
     */
    void set_connection_callback(ConnectionCallback callback);

    /**
     * @brief Get connection ID
     */
    uint64_t id() const noexcept { return connection_id_; }

private:
    explicit TcpConnection(asio::io_context& io_context);
    explicit TcpConnection(tcp::socket socket);

    void do_receive();
    void handle_receive(
        const boost::system::error_code& error,
        size_t bytes_transferred
    );

    static core::ErrorInfo make_tcp_error(TcpError code, const std::string& message);

    tcp::socket socket_;
    std::atomic<bool> connected_{false};
    std::atomic<bool> receiving_{false};
    std::vector<uint8_t> receive_buffer_;
    ReceiveCallback receive_callback_;
    ConnectionCallback connection_callback_;
    uint64_t connection_id_;
    
    static std::atomic<uint64_t> next_connection_id_;
};

} // namespace p2p
} // namespace chainforge

