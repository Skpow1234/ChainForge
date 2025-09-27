#include <gtest/gtest.h>
#include "chainforge/p2p/p2p.hpp"
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include <thread>
#include <chrono>
#include <vector>

namespace chainforge::integration::test {

class P2PIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize P2P nodes
        node1_config.port = 30301;
        node1_config.max_peers = 10;
        node1_config.discovery_enabled = true;
        
        node2_config.port = 30302;
        node2_config.max_peers = 10;
        node2_config.discovery_enabled = true;
        
        node3_config.port = 30303;
        node3_config.max_peers = 10;
        node3_config.discovery_enabled = true;
    }
    
    void TearDown() override {
        // Clean up nodes
        if (node1) node1->stop();
        if (node2) node2->stop();
        if (node3) node3->stop();
    }
    
    P2PConfig node1_config, node2_config, node3_config;
    std::unique_ptr<P2PNode> node1, node2, node3;
};

TEST_F(P2PIntegrationTest, BasicNodeStartup) {
    auto result1 = P2PNode::create(node1_config);
    EXPECT_TRUE(result1.has_value());
    node1 = std::move(result1.value());
    
    auto start_result = node1->start();
    EXPECT_TRUE(start_result.has_value());
    
    EXPECT_TRUE(node1->is_running());
    EXPECT_EQ(node1->peer_count(), 0);
}

TEST_F(P2PIntegrationTest, NodeConnection) {
    // Start first node
    auto result1 = P2PNode::create(node1_config);
    EXPECT_TRUE(result1.has_value());
    node1 = std::move(result1.value());
    EXPECT_TRUE(node1->start().has_value());
    
    // Start second node
    auto result2 = P2PNode::create(node2_config);
    EXPECT_TRUE(result2.has_value());
    node2 = std::move(result2.value());
    EXPECT_TRUE(node2->start().has_value());
    
    // Connect nodes
    auto connect_result = node2->connect_to_peer("127.0.0.1", node1_config.port);
    EXPECT_TRUE(connect_result.has_value());
    
    // Wait for connection to establish
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_GT(node1->peer_count(), 0);
    EXPECT_GT(node2->peer_count(), 0);
}

TEST_F(P2PIntegrationTest, MessagePropagation) {
    // Start nodes
    auto result1 = P2PNode::create(node1_config);
    auto result2 = P2PNode::create(node2_config);
    auto result3 = P2PNode::create(node3_config);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result3.has_value());
    
    node1 = std::move(result1.value());
    node2 = std::move(result2.value());
    node3 = std::move(result3.value());
    
    EXPECT_TRUE(node1->start().has_value());
    EXPECT_TRUE(node2->start().has_value());
    EXPECT_TRUE(node3->start().has_value());
    
    // Connect nodes in a chain: node1 -> node2 -> node3
    EXPECT_TRUE(node2->connect_to_peer("127.0.0.1", node1_config.port).has_value());
    EXPECT_TRUE(node3->connect_to_peer("127.0.0.1", node2_config.port).has_value());
    
    // Wait for connections
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Send message from node1
    std::string test_message = "Hello from node1";
    auto send_result = node1->broadcast_message(test_message);
    EXPECT_TRUE(send_result.has_value());
    
    // Wait for message propagation
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check that all nodes received the message
    EXPECT_TRUE(node1->has_received_message(test_message));
    EXPECT_TRUE(node2->has_received_message(test_message));
    EXPECT_TRUE(node3->has_received_message(test_message));
}

TEST_F(P2PIntegrationTest, BlockPropagation) {
    // Start nodes
    auto result1 = P2PNode::create(node1_config);
    auto result2 = P2PNode::create(node2_config);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    node1 = std::move(result1.value());
    node2 = std::move(result2.value());
    
    EXPECT_TRUE(node1->start().has_value());
    EXPECT_TRUE(node2->start().has_value());
    
    // Connect nodes
    EXPECT_TRUE(node2->connect_to_peer("127.0.0.1", node1_config.port).has_value());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Create test block
    BlockHeader header;
    header.parent_hash = Hash::random();
    header.merkle_root = Hash::random();
    header.timestamp = Timestamp::now();
    header.nonce = 12345;
    header.difficulty = 1000000;
    header.gas_limit = 8000000;
    header.gas_used = 0;
    header.miner = Address::random();
    
    Block test_block(header);
    
    // Broadcast block
    auto broadcast_result = node1->broadcast_block(test_block);
    EXPECT_TRUE(broadcast_result.has_value());
    
    // Wait for propagation
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check that node2 received the block
    EXPECT_TRUE(node2->has_received_block(test_block.hash()));
}

TEST_F(P2PIntegrationTest, TransactionPropagation) {
    // Start nodes
    auto result1 = P2PNode::create(node1_config);
    auto result2 = P2PNode::create(node2_config);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    node1 = std::move(result1.value());
    node2 = std::move(result2.value());
    
    EXPECT_TRUE(node1->start().has_value());
    EXPECT_TRUE(node2->start().has_value());
    
    // Connect nodes
    EXPECT_TRUE(node2->connect_to_peer("127.0.0.1", node1_config.port).has_value());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Create test transaction
    auto tx_result = Transaction::create(
        Address::random(),
        Address::random(),
        Amount::from_ether(1.0),
        Amount(21000),
        Amount(20),
        std::vector<uint8_t>{}
    );
    EXPECT_TRUE(tx_result.has_value());
    
    Transaction test_tx = tx_result.value();
    
    // Broadcast transaction
    auto broadcast_result = node1->broadcast_transaction(test_tx);
    EXPECT_TRUE(broadcast_result.has_value());
    
    // Wait for propagation
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check that node2 received the transaction
    EXPECT_TRUE(node2->has_received_transaction(test_tx.hash()));
}

TEST_F(P2PIntegrationTest, PeerDiscovery) {
    // Start nodes with discovery enabled
    auto result1 = P2PNode::create(node1_config);
    auto result2 = P2PNode::create(node2_config);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    node1 = std::move(result1.value());
    node2 = std::move(result2.value());
    
    EXPECT_TRUE(node1->start().has_value());
    EXPECT_TRUE(node2->start().has_value());
    
    // Add node1 as a bootstrap peer for node2
    node2->add_bootstrap_peer("127.0.0.1", node1_config.port);
    
    // Start discovery
    EXPECT_TRUE(node2->start_discovery().has_value());
    
    // Wait for discovery
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Check that nodes discovered each other
    EXPECT_GT(node1->peer_count(), 0);
    EXPECT_GT(node2->peer_count(), 0);
}

TEST_F(P2PIntegrationTest, NetworkPartition) {
    // Start three nodes
    auto result1 = P2PNode::create(node1_config);
    auto result2 = P2PNode::create(node2_config);
    auto result3 = P2PNode::create(node3_config);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result3.has_value());
    
    node1 = std::move(result1.value());
    node2 = std::move(result2.value());
    node3 = std::move(result3.value());
    
    EXPECT_TRUE(node1->start().has_value());
    EXPECT_TRUE(node2->start().has_value());
    EXPECT_TRUE(node3->start().has_value());
    
    // Connect all nodes
    EXPECT_TRUE(node2->connect_to_peer("127.0.0.1", node1_config.port).has_value());
    EXPECT_TRUE(node3->connect_to_peer("127.0.0.1", node1_config.port).has_value());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Verify all nodes are connected
    EXPECT_GT(node1->peer_count(), 0);
    EXPECT_GT(node2->peer_count(), 0);
    EXPECT_GT(node3->peer_count(), 0);
    
    // Disconnect node2
    node2->disconnect_all_peers();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Check that node2 is disconnected
    EXPECT_EQ(node2->peer_count(), 0);
    
    // Send message from node1
    std::string test_message = "Message after partition";
    EXPECT_TRUE(node1->broadcast_message(test_message).has_value());
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Node3 should receive the message, node2 should not
    EXPECT_TRUE(node1->has_received_message(test_message));
    EXPECT_TRUE(node3->has_received_message(test_message));
    EXPECT_FALSE(node2->has_received_message(test_message));
}

TEST_F(P2PIntegrationTest, MessageOrdering) {
    // Start nodes
    auto result1 = P2PNode::create(node1_config);
    auto result2 = P2PNode::create(node2_config);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    node1 = std::move(result1.value());
    node2 = std::move(result2.value());
    
    EXPECT_TRUE(node1->start().has_value());
    EXPECT_TRUE(node2->start().has_value());
    
    // Connect nodes
    EXPECT_TRUE(node2->connect_to_peer("127.0.0.1", node1_config.port).has_value());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Send multiple messages in sequence
    std::vector<std::string> messages = {
        "Message 1",
        "Message 2", 
        "Message 3",
        "Message 4",
        "Message 5"
    };
    
    for (const auto& message : messages) {
        EXPECT_TRUE(node1->broadcast_message(message).has_value());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Wait for all messages to propagate
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Check that all messages were received in order
    auto received_messages = node2->get_received_messages();
    EXPECT_EQ(received_messages.size(), messages.size());
    
    for (size_t i = 0; i < messages.size(); ++i) {
        EXPECT_EQ(received_messages[i], messages[i]);
    }
}

TEST_F(P2PIntegrationTest, ConcurrentConnections) {
    // Start nodes
    auto result1 = P2PNode::create(node1_config);
    auto result2 = P2PNode::create(node2_config);
    auto result3 = P2PNode::create(node3_config);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    EXPECT_TRUE(result3.has_value());
    
    node1 = std::move(result1.value());
    node2 = std::move(result2.value());
    node3 = std::move(result3.value());
    
    EXPECT_TRUE(node1->start().has_value());
    EXPECT_TRUE(node2->start().has_value());
    EXPECT_TRUE(node3->start().has_value());
    
    // Connect nodes concurrently
    std::vector<std::thread> threads;
    
    threads.emplace_back([this]() {
        node2->connect_to_peer("127.0.0.1", node1_config.port);
    });
    
    threads.emplace_back([this]() {
        node3->connect_to_peer("127.0.0.1", node1_config.port);
    });
    
    threads.emplace_back([this]() {
        node3->connect_to_peer("127.0.0.1", node2_config.port);
    });
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Wait for connections to establish
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // All nodes should have peers
    EXPECT_GT(node1->peer_count(), 0);
    EXPECT_GT(node2->peer_count(), 0);
    EXPECT_GT(node3->peer_count(), 0);
}

TEST_F(P2PIntegrationTest, NetworkStress) {
    // Start nodes
    auto result1 = P2PNode::create(node1_config);
    auto result2 = P2PNode::create(node2_config);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    node1 = std::move(result1.value());
    node2 = std::move(result2.value());
    
    EXPECT_TRUE(node1->start().has_value());
    EXPECT_TRUE(node2->start().has_value());
    
    // Connect nodes
    EXPECT_TRUE(node2->connect_to_peer("127.0.0.1", node1_config.port).has_value());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Send many messages concurrently
    const int num_messages = 1000;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_messages; ++i) {
        threads.emplace_back([this, i]() {
            std::string message = "Stress test message " + std::to_string(i);
            node1->broadcast_message(message);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Wait for all messages to propagate
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Check that most messages were received
    auto received_messages = node2->get_received_messages();
    EXPECT_GT(received_messages.size(), num_messages * 0.9); // At least 90% should be received
}

} // namespace chainforge::integration::test
