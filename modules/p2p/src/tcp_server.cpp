#include "chainforge/p2p/tcp_server.hpp"
#include <spdlog/spdlog.h>

namespace chainforge {
namespace p2p {

TcpServer::TcpServer(asio::io_context& io_context)
    : io_context_(io_context),
      acceptor_(io_context) {
}

TcpServer::~TcpServer() {
    stop();
}

TcpResult<void> TcpServer::start(uint16_t port, const std::string& address) {
    if (running_.load()) {
        return core::ErrorInfo(
            static_cast<int>(TcpError::ALREADY_CONNECTED),
            "Server is already running",
            "tcp_server",
            __FILE__,
            __LINE__
        );
    }

    try {
        tcp::endpoint endpoint(asio::ip::make_address(address), port);
        
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(tcp::acceptor::reuse_address(true));
        
        boost::system::error_code ec;
        acceptor_.bind(endpoint, ec);
        
        if (ec) {
            return core::ErrorInfo(
                static_cast<int>(TcpError::CONNECTION_FAILED),
                "Failed to bind to " + address + ":" + std::to_string(port) + ": " + ec.message(),
                "tcp_server",
                __FILE__,
                __LINE__
            );
        }
        
        acceptor_.listen(asio::socket_base::max_listen_connections, ec);
        
        if (ec) {
            return core::ErrorInfo(
                static_cast<int>(TcpError::CONNECTION_FAILED),
                "Failed to listen: " + ec.message(),
                "tcp_server",
                __FILE__,
                __LINE__
            );
        }
        
        port_ = port;
        running_.store(true);
        
        // Start accepting connections
        do_accept();
        
        spdlog::info("TCP server started on {}:{}", address, port);
        return core::success();
        
    } catch (const std::exception& e) {
        return core::ErrorInfo(
            static_cast<int>(TcpError::CONNECTION_FAILED),
            std::string("Server start exception: ") + e.what(),
            "tcp_server",
            __FILE__,
            __LINE__
        );
    }
}

void TcpServer::stop() {
    if (!running_.exchange(false)) {
        return;  // Already stopped
    }

    boost::system::error_code ec;
    acceptor_.close(ec);
    
    // Close all connections
    std::lock_guard<std::mutex> lock(connections_mutex_);
    for (auto& pair : connections_) {
        pair.second->close();
    }
    connections_.clear();
    
    spdlog::info("TCP server stopped");
}

bool TcpServer::is_running() const noexcept {
    return running_.load();
}

size_t TcpServer::connection_count() const noexcept {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    return connections_.size();
}

std::vector<std::shared_ptr<TcpConnection>> TcpServer::get_connections() const {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    std::vector<std::shared_ptr<TcpConnection>> result;
    result.reserve(connections_.size());
    
    for (const auto& pair : connections_) {
        result.push_back(pair.second);
    }
    
    return result;
}

std::shared_ptr<TcpConnection> TcpServer::get_connection(uint64_t id) const {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    auto it = connections_.find(id);
    if (it != connections_.end()) {
        return it->second;
    }
    
    return nullptr;
}

void TcpServer::remove_connection(uint64_t id) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    auto it = connections_.find(id);
    if (it != connections_.end()) {
        it->second->close();
        connections_.erase(it);
        spdlog::debug("Removed connection {}", id);
    }
}

void TcpServer::set_accept_callback(AcceptCallback callback) {
    accept_callback_ = std::move(callback);
}

void TcpServer::broadcast(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(connections_mutex_);
    
    for (auto& pair : connections_) {
        if (pair.second->is_connected()) {
            pair.second->async_send(data, [](const auto&, size_t) {});
        }
    }
}

void TcpServer::do_accept() {
    if (!running_.load()) {
        return;
    }

    acceptor_.async_accept(
        [this](const boost::system::error_code& ec, tcp::socket socket) {
            handle_accept(ec, std::move(socket));
        }
    );
}

void TcpServer::handle_accept(
    const boost::system::error_code& error,
    tcp::socket socket) {
    
    if (error) {
        spdlog::warn("Accept error: {}", error.message());
        
        // Continue accepting if server is still running
        if (running_.load()) {
            do_accept();
        }
        return;
    }

    // Create connection from accepted socket
    auto connection = TcpConnection::create(std::move(socket));
    
    // Track connection
    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        connections_[connection->id()] = connection;
    }
    
    // Set up connection callback to remove from tracking when closed
    connection->set_connection_callback([this, conn_id = connection->id()](bool connected) {
        if (!connected) {
            remove_connection(conn_id);
        }
    });
    
    spdlog::info("Accepted new connection {}", connection->id());
    
    // Notify callback
    if (accept_callback_) {
        accept_callback_(connection);
    }
    
    // Continue accepting
    do_accept();
}

} // namespace p2p
} // namespace chainforge

