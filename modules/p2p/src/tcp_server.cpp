#include "tokiwa/p2p/tcp_server.hpp"
#include <spdlog/spdlog.h>
#include <future>

namespace tokiwa {
namespace p2p {

TcpServer::TcpServer(asio::io_context& io_context)
    : io_context_(io_context),
      acceptor_(io_context) {
}

TcpServer::~TcpServer() {
    alive_->store(false);
    stop();
}

TcpResult<void> TcpServer::start(uint16_t port, const std::string& address) {
    if (running_.load()) {
        return core::ErrorInfo(
            core::ErrorCode::INVALID_STATE,
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
                core::ErrorCode::NETWORK_ERROR,
                "Failed to bind to " + address + ":" + std::to_string(port) + ": " + ec.message(),
                "tcp_server",
                __FILE__,
                __LINE__
            );
        }
        
        acceptor_.listen(asio::socket_base::max_listen_connections, ec);
        
        if (ec) {
            return core::ErrorInfo(
                core::ErrorCode::NETWORK_ERROR,
                "Failed to listen: " + ec.message(),
                "tcp_server",
                __FILE__,
                __LINE__
            );
        }
        
        // Port 0 means ephemeral — read the OS-assigned port after bind
        port_ = acceptor_.local_endpoint().port();
        running_.store(true);
        
        // Start accepting connections
        do_accept();
        
        spdlog::info("TCP server started on {}:{}", address, port_);
        return core::errors::success();
        
    } catch (const std::exception& e) {
        return core::ErrorInfo(
            core::ErrorCode::NETWORK_ERROR,
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
    acceptor_.cancel(ec);
    acceptor_.close(ec);
    
    // Snapshot connections then release the lock before close().
    // close() may invoke connection callbacks that call remove_connection()
    // and would otherwise deadlock on connections_mutex_.
    std::vector<std::shared_ptr<TcpConnection>> to_close;
    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        to_close.reserve(connections_.size());
        for (auto& pair : connections_) {
            to_close.push_back(pair.second);
        }
        connections_.clear();
    }
    for (auto& conn : to_close) {
        conn->set_connection_callback(nullptr);
        conn->close();
    }

    // Drain accept completion handlers so they do not race with ~TcpServer
    if (!io_context_.stopped()) {
        std::promise<void> barrier;
        auto future = barrier.get_future();
        asio::post(io_context_, [&barrier]() { barrier.set_value(); });
        future.wait_for(std::chrono::seconds(2));
    }
    
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
    std::shared_ptr<TcpConnection> conn;
    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        auto it = connections_.find(id);
        if (it == connections_.end()) {
            return;
        }
        conn = it->second;
        connections_.erase(it);
    }
    // Clear callback before close to avoid re-entrant remove_connection deadlock
    conn->set_connection_callback(nullptr);
    conn->close();
    spdlog::debug("Removed connection {}", id);
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

    auto alive = alive_;
    acceptor_.async_accept(
        [this, alive](const boost::system::error_code& ec, tcp::socket socket) {
            if (!alive->load()) {
                return;
            }
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

    // Keep a receive loop so peer disconnect (EOF) is observed and tracked
    connection->start_receive([](const std::vector<uint8_t>&) {});
    
    // Notify callback
    if (accept_callback_) {
        accept_callback_(connection);
    }
    
    // Continue accepting
    do_accept();
}

} // namespace p2p
} // namespace tokiwa

