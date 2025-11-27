#include "chainforge/p2p/udp_transport.hpp"
#include <spdlog/spdlog.h>

namespace chainforge {
namespace p2p {

core::ErrorInfo UdpTransport::make_udp_error(UdpError code, const std::string& message) {
    return core::ErrorInfo(
        static_cast<int>(code),
        message,
        "udp_transport",
        __FILE__,
        __LINE__
    );
}

UdpTransport::UdpTransport(asio::io_context& io_context)
    : io_context_(io_context),
      socket_(io_context),
      receive_buffer_(65536) {  // Max UDP datagram size
}

UdpTransport::~UdpTransport() {
    close();
}

UdpResult<void> UdpTransport::bind(uint16_t port, const std::string& address) {
    if (bound_.load()) {
        return make_udp_error(UdpError::BIND_FAILED, "Already bound");
    }

    try {
        udp::endpoint endpoint(asio::ip::make_address(address), port);
        
        boost::system::error_code ec;
        socket_.open(endpoint.protocol(), ec);
        
        if (ec) {
            return make_udp_error(
                UdpError::BIND_FAILED,
                "Failed to open socket: " + ec.message()
            );
        }
        
        // Set socket options
        socket_.set_option(udp::socket::reuse_address(true), ec);
        
        socket_.bind(endpoint, ec);
        
        if (ec) {
            return make_udp_error(
                UdpError::BIND_FAILED,
                "Failed to bind to " + address + ":" + std::to_string(port) + ": " + ec.message()
            );
        }
        
        port_ = port;
        bound_.store(true);
        
        spdlog::info("UDP transport bound to {}:{}", address, port);
        return core::success();
        
    } catch (const std::exception& e) {
        return make_udp_error(
            UdpError::BIND_FAILED,
            std::string("Bind exception: ") + e.what()
        );
    }
}

void UdpTransport::close() {
    if (!bound_.exchange(false)) {
        return;  // Already closed
    }

    receiving_.store(false);
    
    boost::system::error_code ec;
    socket_.close(ec);
    
    spdlog::info("UDP transport closed");
}

bool UdpTransport::is_bound() const noexcept {
    return bound_.load();
}

UdpResult<size_t> UdpTransport::send_to(
    const std::vector<uint8_t>& data,
    const std::string& address,
    uint16_t port) {
    
    if (!bound_.load()) {
        return make_udp_error(UdpError::NOT_BOUND, "Socket not bound");
    }

    try {
        udp::endpoint endpoint(asio::ip::make_address(address), port);
        
        boost::system::error_code ec;
        size_t bytes_sent = socket_.send_to(asio::buffer(data), endpoint, 0, ec);
        
        if (ec) {
            return make_udp_error(
                UdpError::SEND_FAILED,
                "Send failed: " + ec.message()
            );
        }
        
        return bytes_sent;
        
    } catch (const std::exception& e) {
        return make_udp_error(
            UdpError::SEND_FAILED,
            std::string("Send exception: ") + e.what()
        );
    }
}

void UdpTransport::async_send_to(
    const std::vector<uint8_t>& data,
    const std::string& address,
    uint16_t port,
    std::function<void(const boost::system::error_code&, size_t)> handler) {
    
    try {
        udp::endpoint endpoint(asio::ip::make_address(address), port);
        
        socket_.async_send_to(
            asio::buffer(data),
            endpoint,
            [handler](const boost::system::error_code& ec, size_t bytes_transferred) {
                handler(ec, bytes_transferred);
            }
        );
        
    } catch (const std::exception& e) {
        spdlog::error("Async send exception: {}", e.what());
        handler(
            boost::system::errc::make_error_code(boost::system::errc::invalid_argument),
            0
        );
    }
}

UdpResult<size_t> UdpTransport::broadcast(
    const std::vector<uint8_t>& data,
    uint16_t port) {
    
    if (!bound_.load()) {
        return make_udp_error(UdpError::NOT_BOUND, "Socket not bound");
    }

    try {
        // Enable broadcast
        boost::system::error_code ec;
        socket_.set_option(asio::socket_base::broadcast(true), ec);
        
        if (ec) {
            return make_udp_error(
                UdpError::SEND_FAILED,
                "Failed to enable broadcast: " + ec.message()
            );
        }
        
        // Broadcast to 255.255.255.255
        udp::endpoint endpoint(asio::ip::address_v4::broadcast(), port);
        
        size_t bytes_sent = socket_.send_to(asio::buffer(data), endpoint, 0, ec);
        
        if (ec) {
            return make_udp_error(
                UdpError::SEND_FAILED,
                "Broadcast failed: " + ec.message()
            );
        }
        
        return bytes_sent;
        
    } catch (const std::exception& e) {
        return make_udp_error(
            UdpError::SEND_FAILED,
            std::string("Broadcast exception: ") + e.what()
        );
    }
}

void UdpTransport::start_receive(UdpReceiveCallback callback) {
    receive_callback_ = std::move(callback);
    receiving_.store(true);
    do_receive();
}

void UdpTransport::stop_receive() {
    receiving_.store(false);
}

UdpResult<void> UdpTransport::join_multicast(const std::string& multicast_address) {
    if (!bound_.load()) {
        return make_udp_error(UdpError::NOT_BOUND, "Socket not bound");
    }

    try {
        auto multicast_addr = asio::ip::make_address(multicast_address);
        
        boost::system::error_code ec;
        socket_.set_option(
            asio::ip::multicast::join_group(multicast_addr.to_v4()),
            ec
        );
        
        if (ec) {
            return make_udp_error(
                UdpError::BIND_FAILED,
                "Failed to join multicast group: " + ec.message()
            );
        }
        
        spdlog::info("Joined multicast group {}", multicast_address);
        return core::success();
        
    } catch (const std::exception& e) {
        return make_udp_error(
            UdpError::BIND_FAILED,
            std::string("Multicast join exception: ") + e.what()
        );
    }
}

UdpResult<void> UdpTransport::leave_multicast(const std::string& multicast_address) {
    if (!bound_.load()) {
        return make_udp_error(UdpError::NOT_BOUND, "Socket not bound");
    }

    try {
        auto multicast_addr = asio::ip::make_address(multicast_address);
        
        boost::system::error_code ec;
        socket_.set_option(
            asio::ip::multicast::leave_group(multicast_addr.to_v4()),
            ec
        );
        
        if (ec) {
            return make_udp_error(
                UdpError::BIND_FAILED,
                "Failed to leave multicast group: " + ec.message()
            );
        }
        
        spdlog::info("Left multicast group {}", multicast_address);
        return core::success();
        
    } catch (const std::exception& e) {
        return make_udp_error(
            UdpError::BIND_FAILED,
            std::string("Multicast leave exception: ") + e.what()
        );
    }
}

UdpResult<UdpEndpoint> UdpTransport::local_endpoint() const {
    try {
        boost::system::error_code ec;
        auto endpoint = socket_.local_endpoint(ec);
        
        if (ec) {
            return make_udp_error(
                UdpError::NOT_BOUND,
                "Failed to get local endpoint: " + ec.message()
            );
        }
        
        UdpEndpoint result;
        result.address = endpoint.address().to_string();
        result.port = endpoint.port();
        
        return result;
        
    } catch (const std::exception& e) {
        return make_udp_error(
            UdpError::NOT_BOUND,
            std::string("Local endpoint exception: ") + e.what()
        );
    }
}

void UdpTransport::do_receive() {
    if (!receiving_.load() || !bound_.load()) {
        return;
    }

    socket_.async_receive_from(
        asio::buffer(receive_buffer_),
        sender_endpoint_,
        [this](const boost::system::error_code& ec, size_t bytes_transferred) {
            handle_receive(ec, bytes_transferred, sender_endpoint_);
        }
    );
}

void UdpTransport::handle_receive(
    const boost::system::error_code& error,
    size_t bytes_transferred,
    udp::endpoint sender_endpoint) {
    
    if (error) {
        if (error != asio::error::operation_aborted) {
            spdlog::warn("UDP receive error: {}", error.message());
        }
        return;
    }

    // Call user callback with received data
    if (receive_callback_ && bytes_transferred > 0) {
        std::vector<uint8_t> data(
            receive_buffer_.begin(),
            receive_buffer_.begin() + bytes_transferred
        );
        
        UdpEndpoint sender;
        sender.address = sender_endpoint.address().to_string();
        sender.port = sender_endpoint.port();
        
        receive_callback_(data, sender);
    }

    // Continue receiving
    do_receive();
}

} // namespace p2p
} // namespace chainforge

