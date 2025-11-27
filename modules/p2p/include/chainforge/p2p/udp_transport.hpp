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
using udp = asio::ip::udp;

/**
 * @brief Error codes for UDP operations
 */
enum class UdpError {
    BIND_FAILED = 1,
    SEND_FAILED = 2,
    RECEIVE_FAILED = 3,
    INVALID_ADDRESS = 4,
    NOT_BOUND = 5
};

/**
 * @brief Result type for UDP operations
 */
template<typename T>
using UdpResult = core::Result<T>;

/**
 * @brief UDP endpoint info
 */
struct UdpEndpoint {
    std::string address;
    uint16_t port;
    
    std::string to_string() const {
        return address + ":" + std::to_string(port);
    }
};

/**
 * @brief Callback for received UDP data
 */
using UdpReceiveCallback = std::function<void(
    const std::vector<uint8_t>& data,
    const UdpEndpoint& sender
)>;

/**
 * @brief UDP transport for peer discovery and broadcast
 * 
 * Features:
 * - Bind to port and receive datagrams
 * - Send datagrams to specific endpoints
 * - Broadcast to subnet
 * - Multicast support
 */
class UdpTransport {
public:
    /**
     * @brief Construct UDP transport
     */
    explicit UdpTransport(asio::io_context& io_context);

    ~UdpTransport();

    // Non-copyable, non-movable (has async operations)
    UdpTransport(const UdpTransport&) = delete;
    UdpTransport& operator=(const UdpTransport&) = delete;
    UdpTransport(UdpTransport&&) = delete;
    UdpTransport& operator=(UdpTransport&&) = delete;

    /**
     * @brief Bind to port
     */
    UdpResult<void> bind(uint16_t port, const std::string& address = "0.0.0.0");

    /**
     * @brief Close the socket
     */
    void close();

    /**
     * @brief Check if bound
     */
    bool is_bound() const noexcept;

    /**
     * @brief Send datagram to endpoint
     */
    UdpResult<size_t> send_to(
        const std::vector<uint8_t>& data,
        const std::string& address,
        uint16_t port
    );

    /**
     * @brief Async send datagram
     */
    void async_send_to(
        const std::vector<uint8_t>& data,
        const std::string& address,
        uint16_t port,
        std::function<void(const boost::system::error_code&, size_t)> handler
    );

    /**
     * @brief Broadcast to subnet
     */
    UdpResult<size_t> broadcast(
        const std::vector<uint8_t>& data,
        uint16_t port
    );

    /**
     * @brief Start receiving datagrams
     */
    void start_receive(UdpReceiveCallback callback);

    /**
     * @brief Stop receiving datagrams
     */
    void stop_receive();

    /**
     * @brief Join multicast group
     */
    UdpResult<void> join_multicast(const std::string& multicast_address);

    /**
     * @brief Leave multicast group
     */
    UdpResult<void> leave_multicast(const std::string& multicast_address);

    /**
     * @brief Get local endpoint
     */
    UdpResult<UdpEndpoint> local_endpoint() const;

    /**
     * @brief Get bound port
     */
    uint16_t port() const noexcept { return port_; }

private:
    void do_receive();
    void handle_receive(
        const boost::system::error_code& error,
        size_t bytes_transferred,
        udp::endpoint sender_endpoint
    );

    static core::ErrorInfo make_udp_error(UdpError code, const std::string& message);

    asio::io_context& io_context_;
    udp::socket socket_;
    std::atomic<bool> bound_{false};
    std::atomic<bool> receiving_{false};
    uint16_t port_{0};
    
    std::vector<uint8_t> receive_buffer_;
    udp::endpoint sender_endpoint_;
    UdpReceiveCallback receive_callback_;
};

} // namespace p2p
} // namespace chainforge

