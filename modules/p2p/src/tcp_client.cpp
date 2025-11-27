#include "chainforge/p2p/tcp_client.hpp"
#include <spdlog/spdlog.h>

namespace chainforge {
namespace p2p {

TcpClient::TcpClient(asio::io_context& io_context)
    : io_context_(io_context),
      reconnect_timer_(std::make_unique<asio::steady_timer>(io_context)) {
}

TcpClient::~TcpClient() {
    disconnect();
}

TcpResult<void> TcpClient::connect(const std::string& host, uint16_t port) {
    if (is_connected()) {
        return core::ErrorInfo(
            static_cast<int>(TcpError::ALREADY_CONNECTED),
            "Already connected",
            "tcp_client",
            __FILE__,
            __LINE__
        );
    }

    host_ = host;
    port_ = port;
    
    connection_ = TcpConnection::create(io_context_);
    
    // Set connection callback
    connection_->set_connection_callback([this](bool connected) {
        if (connection_callback_) {
            connection_callback_(connected);
        }
        
        // Handle reconnection
        if (!connected && auto_reconnect_) {
            schedule_reconnect();
        }
    });
    
    auto result = connection_->connect(host, port);
    
    if (!result.has_value()) {
        connection_.reset();
        return result;
    }
    
    spdlog::info("TCP client connected to {}:{}", host, port);
    return core::success();
}

TcpResult<void> TcpClient::connect(
    const std::string& host,
    uint16_t port,
    std::chrono::milliseconds timeout) {
    
    // For timeout support, we'd need to use async_connect with a timer
    // For now, just use regular connect
    // TODO: Implement timeout properly with async operations
    return connect(host, port);
}

void TcpClient::async_connect(
    const std::string& host,
    uint16_t port,
    std::function<void(const boost::system::error_code&)> handler) {
    
    if (is_connected()) {
        handler(boost::system::errc::make_error_code(boost::system::errc::already_connected));
        return;
    }

    host_ = host;
    port_ = port;
    
    connection_ = TcpConnection::create(io_context_);
    
    // Set connection callback
    connection_->set_connection_callback([this](bool connected) {
        if (connection_callback_) {
            connection_callback_(connected);
        }
        
        if (!connected && auto_reconnect_) {
            schedule_reconnect();
        }
    });
    
    connection_->async_connect(host, port, handler);
}

void TcpClient::disconnect() {
    auto_reconnect_ = false;
    
    if (reconnect_timer_) {
        boost::system::error_code ec;
        reconnect_timer_->cancel(ec);
    }
    
    if (connection_) {
        connection_->close();
        connection_.reset();
    }
}

bool TcpClient::is_connected() const noexcept {
    return connection_ && connection_->is_connected();
}

std::shared_ptr<TcpConnection> TcpClient::connection() const noexcept {
    return connection_;
}

void TcpClient::enable_auto_reconnect(std::chrono::milliseconds retry_interval) {
    auto_reconnect_ = true;
    reconnect_interval_ = retry_interval;
}

void TcpClient::disable_auto_reconnect() {
    auto_reconnect_ = false;
    
    if (reconnect_timer_) {
        boost::system::error_code ec;
        reconnect_timer_->cancel(ec);
    }
}

TcpResult<size_t> TcpClient::send(const std::vector<uint8_t>& data) {
    if (!is_connected()) {
        return core::ErrorInfo(
            static_cast<int>(TcpError::NOT_CONNECTED),
            "Not connected",
            "tcp_client",
            __FILE__,
            __LINE__
        );
    }

    return connection_->send(data);
}

void TcpClient::start_receive(ReceiveCallback callback) {
    if (is_connected()) {
        connection_->start_receive(std::move(callback));
    }
}

void TcpClient::stop_receive() {
    if (connection_) {
        connection_->stop_receive();
    }
}

void TcpClient::set_connection_callback(ConnectionCallback callback) {
    connection_callback_ = std::move(callback);
}

void TcpClient::do_reconnect() {
    spdlog::info("Attempting to reconnect to {}:{}...", host_, port_);
    
    connection_ = TcpConnection::create(io_context_);
    
    connection_->set_connection_callback([this](bool connected) {
        if (connection_callback_) {
            connection_callback_(connected);
        }
        
        if (!connected && auto_reconnect_) {
            schedule_reconnect();
        }
    });
    
    connection_->async_connect(host_, port_, [this](const boost::system::error_code& ec) {
        if (ec) {
            spdlog::warn("Reconnect failed: {}", ec.message());
            connection_.reset();
            
            if (auto_reconnect_) {
                schedule_reconnect();
            }
        } else {
            spdlog::info("Reconnected successfully to {}:{}", host_, port_);
        }
    });
}

void TcpClient::schedule_reconnect() {
    if (!auto_reconnect_ || !reconnect_timer_) {
        return;
    }

    reconnect_timer_->expires_after(reconnect_interval_);
    reconnect_timer_->async_wait([this](const boost::system::error_code& ec) {
        if (!ec && auto_reconnect_) {
            do_reconnect();
        }
    });
}

} // namespace p2p
} // namespace chainforge

