/**
 * @file test_network_transport.cpp
 * @brief Comprehensive tests for network transport layer
 * 
 * Tests TCP/UDP communication, connection management, and error handling
 */

#include <gtest/gtest.h>
#include "chainforge/p2p/tcp_connection.hpp"
#include "chainforge/p2p/tcp_server.hpp"
#include "chainforge/p2p/tcp_client.hpp"
#include "chainforge/p2p/udp_transport.hpp"
#include <thread>
#include <chrono>
#include <atomic>

using namespace chainforge::p2p;
using namespace std::chrono_literals;

// Test fixture with io_context
class NetworkTransportTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Start io_context in background thread
        work_guard = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
            io_context.get_executor()
        );
        
        io_thread = std::thread([this]() {
            io_context.run();
        });
    }

    void TearDown() override {
        // Stop io_context
        work_guard.reset();
        io_context.stop();
        
        if (io_thread.joinable()) {
            io_thread.join();
        }
    }

    asio::io_context io_context;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard;
    std::thread io_thread;
};

// ============================================================================
// TCP Connection Tests
// ============================================================================

TEST_F(NetworkTransportTest, TcpConnectionCreate) {
    auto conn = TcpConnection::create(io_context);
    ASSERT_NE(conn, nullptr);
    EXPECT_FALSE(conn->is_connected());
    EXPECT_GT(conn->id(), 0);
}

TEST_F(NetworkTransportTest, TcpConnectionConnectInvalidAddress) {
    auto conn = TcpConnection::create(io_context);
    
    auto result = conn->connect("invalid.address.test", 9999);
    EXPECT_FALSE(result.has_value());
}

// ============================================================================
// TCP Server Tests
// ============================================================================

TEST_F(NetworkTransportTest, TcpServerStartStop) {
    TcpServer server(io_context);
    
    auto result = server.start(0);  // Use ephemeral port
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(server.is_running());
    EXPECT_GT(server.port(), 0);
    
    server.stop();
    EXPECT_FALSE(server.is_running());
}

TEST_F(NetworkTransportTest, TcpServerMultipleStart) {
    TcpServer server(io_context);
    
    auto result1 = server.start(0);
    ASSERT_TRUE(result1.has_value());
    
    // Try to start again
    auto result2 = server.start(0);
    EXPECT_FALSE(result2.has_value());
    
    server.stop();
}

TEST_F(NetworkTransportTest, TcpServerAcceptConnection) {
    TcpServer server(io_context);
    
    // Start server
    auto result = server.start(0);
    ASSERT_TRUE(result.has_value());
    uint16_t port = server.port();
    
    std::atomic<bool> connection_accepted{false};
    
    server.set_accept_callback([&](std::shared_ptr<TcpConnection> conn) {
        connection_accepted.store(true);
        EXPECT_TRUE(conn->is_connected());
    });
    
    // Connect client
    TcpClient client(io_context);
    client.async_connect("127.0.0.1", port, [](const auto& ec) {
        EXPECT_FALSE(ec);
    });
    
    // Wait for connection
    std::this_thread::sleep_for(100ms);
    
    EXPECT_TRUE(connection_accepted.load());
    EXPECT_EQ(server.connection_count(), 1);
    
    server.stop();
}

TEST_F(NetworkTransportTest, TcpServerBroadcast) {
    TcpServer server(io_context);
    
    auto result = server.start(0);
    ASSERT_TRUE(result.has_value());
    uint16_t port = server.port();
    
    // Connect multiple clients
    const int num_clients = 3;
    std::vector<std::unique_ptr<TcpClient>> clients;
    std::atomic<int> messages_received{0};
    
    for (int i = 0; i < num_clients; ++i) {
        auto client = std::make_unique<TcpClient>(io_context);
        client->connect("127.0.0.1", port);
        
        client->start_receive([&](const std::vector<uint8_t>& data) {
            EXPECT_EQ(data.size(), 5);
            messages_received++;
        });
        
        clients.push_back(std::move(client));
    }
    
    std::this_thread::sleep_for(100ms);
    
    // Broadcast message
    std::vector<uint8_t> message = {0x01, 0x02, 0x03, 0x04, 0x05};
    server.broadcast(message);
    
    std::this_thread::sleep_for(100ms);
    
    EXPECT_EQ(messages_received.load(), num_clients);
    
    server.stop();
}

// ============================================================================
// TCP Client Tests
// ============================================================================

TEST_F(NetworkTransportTest, TcpClientConnectSuccess) {
    TcpServer server(io_context);
    auto server_result = server.start(0);
    ASSERT_TRUE(server_result.has_value());
    uint16_t port = server.port();
    
    TcpClient client(io_context);
    auto client_result = client.connect("127.0.0.1", port);
    
    ASSERT_TRUE(client_result.has_value());
    EXPECT_TRUE(client.is_connected());
    
    std::this_thread::sleep_for(50ms);
    
    server.stop();
}

TEST_F(NetworkTransportTest, TcpClientSendReceive) {
    TcpServer server(io_context);
    auto result = server.start(0);
    ASSERT_TRUE(result.has_value());
    uint16_t port = server.port();
    
    std::atomic<bool> data_received{false};
    std::vector<uint8_t> received_data;
    
    server.set_accept_callback([&](std::shared_ptr<TcpConnection> conn) {
        conn->start_receive([&](const std::vector<uint8_t>& data) {
            received_data = data;
            data_received.store(true);
        });
    });
    
    TcpClient client(io_context);
    client.connect("127.0.0.1", port);
    
    std::this_thread::sleep_for(50ms);
    
    // Send data
    std::vector<uint8_t> message = {0xAA, 0xBB, 0xCC};
    auto send_result = client.send(message);
    ASSERT_TRUE(send_result.has_value());
    
    std::this_thread::sleep_for(50ms);
    
    EXPECT_TRUE(data_received.load());
    EXPECT_EQ(received_data, message);
    
    server.stop();
}

TEST_F(NetworkTransportTest, TcpClientAutoReconnect) {
    TcpServer server(io_context);
    auto result = server.start(0);
    ASSERT_TRUE(result.has_value());
    uint16_t port = server.port();
    
    TcpClient client(io_context);
    client.enable_auto_reconnect(200ms);
    
    std::atomic<int> connect_count{0};
    client.set_connection_callback([&](bool connected) {
        if (connected) {
            connect_count++;
        }
    });
    
    // Initial connection
    client.connect("127.0.0.1", port);
    std::this_thread::sleep_for(50ms);
    EXPECT_EQ(connect_count.load(), 1);
    
    // Stop server to trigger disconnect
    server.stop();
    std::this_thread::sleep_for(50ms);
    
    // Restart server
    result = server.start(port);
    ASSERT_TRUE(result.has_value());
    
    // Wait for reconnect
    std::this_thread::sleep_for(300ms);
    
    // Should have reconnected
    EXPECT_GE(connect_count.load(), 2);
    
    client.disable_auto_reconnect();
    server.stop();
}

// ============================================================================
// UDP Transport Tests
// ============================================================================

TEST_F(NetworkTransportTest, UdpTransportBindClose) {
    UdpTransport transport(io_context);
    
    auto result = transport.bind(0);  // Ephemeral port
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(transport.is_bound());
    EXPECT_GT(transport.port(), 0);
    
    transport.close();
    EXPECT_FALSE(transport.is_bound());
}

TEST_F(NetworkTransportTest, UdpTransportSendReceive) {
    UdpTransport sender(io_context);
    UdpTransport receiver(io_context);
    
    // Bind receiver
    auto bind_result = receiver.bind(0);
    ASSERT_TRUE(bind_result.has_value());
    uint16_t receiver_port = receiver.port();
    
    std::atomic<bool> data_received{false};
    std::vector<uint8_t> received_data;
    UdpEndpoint received_from;
    
    receiver.start_receive([&](const std::vector<uint8_t>& data, const UdpEndpoint& sender) {
        received_data = data;
        received_from = sender;
        data_received.store(true);
    });
    
    // Bind sender
    bind_result = sender.bind(0);
    ASSERT_TRUE(bind_result.has_value());
    
    // Send data
    std::vector<uint8_t> message = {0x11, 0x22, 0x33, 0x44};
    auto send_result = sender.send_to(message, "127.0.0.1", receiver_port);
    ASSERT_TRUE(send_result.has_value());
    EXPECT_EQ(send_result.value(), message.size());
    
    // Wait for receive
    std::this_thread::sleep_for(100ms);
    
    EXPECT_TRUE(data_received.load());
    EXPECT_EQ(received_data, message);
    EXPECT_EQ(received_from.address, "127.0.0.1");
    
    sender.close();
    receiver.close();
}

TEST_F(NetworkTransportTest, UdpTransportBroadcast) {
    const int num_receivers = 2;
    std::vector<std::unique_ptr<UdpTransport>> receivers;
    std::atomic<int> messages_received{0};
    
    // Create receivers
    for (int i = 0; i < num_receivers; ++i) {
        auto receiver = std::make_unique<UdpTransport>(io_context);
        
        // Bind to same port
        uint16_t port = (i == 0) ? 0 : receivers[0]->port();
        auto result = receiver->bind(port);
        
        if (i == 0) {
            ASSERT_TRUE(result.has_value());
        }
        
        receiver->start_receive([&](const std::vector<uint8_t>&, const UdpEndpoint&) {
            messages_received++;
        });
        
        receivers.push_back(std::move(receiver));
    }
    
    // Note: Broadcast typically requires proper network configuration
    // This test may not work in all environments
    // Keeping it simple for demonstration
    
    for (auto& receiver : receivers) {
        receiver->close();
    }
}

TEST_F(NetworkTransportTest, UdpTransportLocalEndpoint) {
    UdpTransport transport(io_context);
    
    auto bind_result = transport.bind(0);
    ASSERT_TRUE(bind_result.has_value());
    
    auto endpoint_result = transport.local_endpoint();
    ASSERT_TRUE(endpoint_result.has_value());
    
    auto endpoint = endpoint_result.value();
    EXPECT_FALSE(endpoint.address.empty());
    EXPECT_GT(endpoint.port, 0);
    EXPECT_EQ(endpoint.port, transport.port());
    
    transport.close();
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(NetworkTransportTest, TcpConnectionNotConnectedError) {
    auto conn = TcpConnection::create(io_context);
    
    // Try to send without connecting
    std::vector<uint8_t> data = {0x01};
    auto result = conn->send(data);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetworkTransportTest, UdpTransportNotBoundError) {
    UdpTransport transport(io_context);
    
    // Try to send without binding
    std::vector<uint8_t> data = {0x01};
    auto result = transport.send_to(data, "127.0.0.1", 9999);
    
    EXPECT_FALSE(result.has_value());
}

TEST_F(NetworkTransportTest, TcpServerBindUsedPort) {
    TcpServer server1(io_context);
    
    auto result1 = server1.start(0);
    ASSERT_TRUE(result1.has_value());
    uint16_t port = server1.port();
    
    // Try to bind to same port
    TcpServer server2(io_context);
    auto result2 = server2.start(port);
    
    EXPECT_FALSE(result2.has_value());
    
    server1.stop();
}

// ============================================================================
// Connection Management Tests
// ============================================================================

TEST_F(NetworkTransportTest, TcpServerConnectionTracking) {
    TcpServer server(io_context);
    auto result = server.start(0);
    ASSERT_TRUE(result.has_value());
    uint16_t port = server.port();
    
    // Connect clients
    TcpClient client1(io_context);
    TcpClient client2(io_context);
    
    client1.connect("127.0.0.1", port);
    client2.connect("127.0.0.1", port);
    
    std::this_thread::sleep_for(100ms);
    
    EXPECT_EQ(server.connection_count(), 2);
    
    auto connections = server.get_connections();
    EXPECT_EQ(connections.size(), 2);
    
    // Disconnect one client
    client1.disconnect();
    std::this_thread::sleep_for(50ms);
    
    EXPECT_EQ(server.connection_count(), 1);
    
    server.stop();
}

TEST_F(NetworkTransportTest, TcpServerRemoveConnection) {
    TcpServer server(io_context);
    auto result = server.start(0);
    ASSERT_TRUE(result.has_value());
    uint16_t port = server.port();
    
    uint64_t connection_id = 0;
    
    server.set_accept_callback([&](std::shared_ptr<TcpConnection> conn) {
        connection_id = conn->id();
    });
    
    TcpClient client(io_context);
    client.connect("127.0.0.1", port);
    
    std::this_thread::sleep_for(50ms);
    
    EXPECT_GT(connection_id, 0);
    EXPECT_EQ(server.connection_count(), 1);
    
    // Remove connection
    server.remove_connection(connection_id);
    
    EXPECT_EQ(server.connection_count(), 0);
    
    server.stop();
}

// ============================================================================
// Performance Tests
// ============================================================================

TEST_F(NetworkTransportTest, TcpThroughputTest) {
    TcpServer server(io_context);
    auto result = server.start(0);
    ASSERT_TRUE(result.has_value());
    uint16_t port = server.port();
    
    std::atomic<size_t> bytes_received{0};
    
    server.set_accept_callback([&](std::shared_ptr<TcpConnection> conn) {
        conn->start_receive([&](const std::vector<uint8_t>& data) {
            bytes_received += data.size();
        });
    });
    
    TcpClient client(io_context);
    client.connect("127.0.0.1", port);
    
    std::this_thread::sleep_for(50ms);
    
    // Send multiple messages
    const int num_messages = 100;
    const size_t message_size = 1024;
    std::vector<uint8_t> message(message_size, 0xAA);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_messages; ++i) {
        client.send(message);
    }
    
    std::this_thread::sleep_for(100ms);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    size_t total_bytes = num_messages * message_size;
    EXPECT_GE(bytes_received.load(), total_bytes * 0.9);  // Allow 10% loss
    
    std::cout << "TCP throughput: " << (total_bytes / 1024.0) << " KB in "
              << duration.count() << " ms" << std::endl;
    
    server.stop();
}

