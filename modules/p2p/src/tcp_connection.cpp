#include "chainforge/p2p/tcp_connection.hpp"
#include <spdlog/spdlog.h>

namespace chainforge {
namespace p2p {

std::atomic<uint64_t> TcpConnection::next_connection_id_{1};

core::ErrorInfo TcpConnection::make_tcp_error(TcpError code, const std::string& message) {
    return core::ErrorInfo(
        static_cast<int>(code),
        message,
        "tcp_connection",
        __FILE__,
        __LINE__
    );
}

std::shared_ptr<TcpConnection> TcpConnection::create(asio::io_context& io_context) {
    return std::shared_ptr<TcpConnection>(new TcpConnection(io_context));
}

std::shared_ptr<TcpConnection> TcpConnection::create(tcp::socket socket) {
    return std::shared_ptr<TcpConnection>(new TcpConnection(std::move(socket)));
}

TcpConnection::TcpConnection(asio::io_context& io_context)
    : socket_(io_context),
      receive_buffer_(8192),
      connection_id_(next_connection_id_++) {
}

TcpConnection::TcpConnection(tcp::socket socket)
    : socket_(std::move(socket)),
      connected_(true),
      receive_buffer_(8192),
      connection_id_(next_connection_id_++) {
}

TcpConnection::~TcpConnection() {
    close();
}

TcpResult<void> TcpConnection::connect(const std::string& host, uint16_t port) {
    if (connected_.load()) {
        return make_tcp_error(TcpError::ALREADY_CONNECTED, "Already connected");
    }

    try {
        tcp::resolver resolver(socket_.get_executor());
        auto endpoints = resolver.resolve(host, std::to_string(port));
        
        boost::system::error_code ec;
        asio::connect(socket_, endpoints, ec);
        
        if (ec) {
            return make_tcp_error(
                TcpError::CONNECTION_FAILED,
                "Failed to connect: " + ec.message()
            );
        }
        
        connected_.store(true);
        
        if (connection_callback_) {
            connection_callback_(true);
        }
        
        spdlog::info("TCP connection {} established to {}:{}", connection_id_, host, port);
        return core::success();
        
    } catch (const std::exception& e) {
        return make_tcp_error(
            TcpError::CONNECTION_FAILED,
            std::string("Connection exception: ") + e.what()
        );
    }
}

void TcpConnection::async_connect(
    const std::string& host,
    uint16_t port,
    std::function<void(const boost::system::error_code&)> handler) {
    
    auto self = shared_from_this();
    
    auto resolver = std::make_shared<tcp::resolver>(socket_.get_executor());
    resolver->async_resolve(
        host,
        std::to_string(port),
        [this, self, resolver, handler](
            const boost::system::error_code& ec,
            tcp::resolver::results_type endpoints) {
            
            if (ec) {
                handler(ec);
                return;
            }
            
            asio::async_connect(
                socket_,
                endpoints,
                [this, self, handler](
                    const boost::system::error_code& ec,
                    const tcp::endpoint&) {
                    
                    if (!ec) {
                        connected_.store(true);
                        if (connection_callback_) {
                            connection_callback_(true);
                        }
                    }
                    handler(ec);
                }
            );
        }
    );
}

TcpResult<size_t> TcpConnection::send(const std::vector<uint8_t>& data) {
    if (!connected_.load()) {
        return make_tcp_error(TcpError::NOT_CONNECTED, "Not connected");
    }

    try {
        boost::system::error_code ec;
        size_t bytes_written = asio::write(socket_, asio::buffer(data), ec);
        
        if (ec) {
            return make_tcp_error(
                TcpError::WRITE_FAILED,
                "Write failed: " + ec.message()
            );
        }
        
        return bytes_written;
        
    } catch (const std::exception& e) {
        return make_tcp_error(
            TcpError::WRITE_FAILED,
            std::string("Write exception: ") + e.what()
        );
    }
}

void TcpConnection::async_send(
    const std::vector<uint8_t>& data,
    std::function<void(const boost::system::error_code&, size_t)> handler) {
    
    auto self = shared_from_this();
    
    asio::async_write(
        socket_,
        asio::buffer(data),
        [this, self, handler](
            const boost::system::error_code& ec,
            size_t bytes_transferred) {
            handler(ec, bytes_transferred);
        }
    );
}

TcpResult<std::vector<uint8_t>> TcpConnection::receive(size_t max_length) {
    if (!connected_.load()) {
        return make_tcp_error(TcpError::NOT_CONNECTED, "Not connected");
    }

    try {
        std::vector<uint8_t> buffer(max_length);
        boost::system::error_code ec;
        
        size_t bytes_read = socket_.read_some(asio::buffer(buffer), ec);
        
        if (ec == asio::error::eof) {
            connected_.store(false);
            return make_tcp_error(TcpError::CONNECTION_CLOSED, "Connection closed by peer");
        }
        
        if (ec) {
            return make_tcp_error(
                TcpError::READ_FAILED,
                "Read failed: " + ec.message()
            );
        }
        
        buffer.resize(bytes_read);
        return buffer;
        
    } catch (const std::exception& e) {
        return make_tcp_error(
            TcpError::READ_FAILED,
            std::string("Read exception: ") + e.what()
        );
    }
}

void TcpConnection::start_receive(ReceiveCallback callback) {
    receive_callback_ = std::move(callback);
    receiving_.store(true);
    do_receive();
}

void TcpConnection::stop_receive() {
    receiving_.store(false);
}

void TcpConnection::do_receive() {
    if (!receiving_.load() || !connected_.load()) {
        return;
    }

    auto self = shared_from_this();
    
    socket_.async_read_some(
        asio::buffer(receive_buffer_),
        [this, self](const boost::system::error_code& ec, size_t bytes_transferred) {
            handle_receive(ec, bytes_transferred);
        }
    );
}

void TcpConnection::handle_receive(
    const boost::system::error_code& error,
    size_t bytes_transferred) {
    
    if (error) {
        if (error == asio::error::eof || error == asio::error::connection_reset) {
            spdlog::info("TCP connection {} closed by peer", connection_id_);
            connected_.store(false);
            if (connection_callback_) {
                connection_callback_(false);
            }
        } else {
            spdlog::warn("TCP connection {} receive error: {}", connection_id_, error.message());
        }
        return;
    }

    // Call user callback with received data
    if (receive_callback_ && bytes_transferred > 0) {
        std::vector<uint8_t> data(
            receive_buffer_.begin(),
            receive_buffer_.begin() + bytes_transferred
        );
        receive_callback_(data);
    }

    // Continue receiving
    do_receive();
}

void TcpConnection::close() {
    if (!connected_.exchange(false)) {
        return;  // Already closed
    }

    receiving_.store(false);
    
    boost::system::error_code ec;
    socket_.shutdown(tcp::socket::shutdown_both, ec);
    socket_.close(ec);
    
    if (connection_callback_) {
        connection_callback_(false);
    }
    
    spdlog::info("TCP connection {} closed", connection_id_);
}

bool TcpConnection::is_connected() const noexcept {
    return connected_.load();
}

TcpResult<std::string> TcpConnection::local_endpoint() const {
    try {
        boost::system::error_code ec;
        auto endpoint = socket_.local_endpoint(ec);
        
        if (ec) {
            return make_tcp_error(
                TcpError::NOT_CONNECTED,
                "Failed to get local endpoint: " + ec.message()
            );
        }
        
        return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
        
    } catch (const std::exception& e) {
        return make_tcp_error(
            TcpError::NOT_CONNECTED,
            std::string("Local endpoint exception: ") + e.what()
        );
    }
}

TcpResult<std::string> TcpConnection::remote_endpoint() const {
    try {
        boost::system::error_code ec;
        auto endpoint = socket_.remote_endpoint(ec);
        
        if (ec) {
            return make_tcp_error(
                TcpError::NOT_CONNECTED,
                "Failed to get remote endpoint: " + ec.message()
            );
        }
        
        return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
        
    } catch (const std::exception& e) {
        return make_tcp_error(
            TcpError::NOT_CONNECTED,
            std::string("Remote endpoint exception: ") + e.what()
        );
    }
}

void TcpConnection::set_connection_callback(ConnectionCallback callback) {
    connection_callback_ = std::move(callback);
}

} // namespace p2p
} // namespace chainforge

