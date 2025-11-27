/**
 * @file test_peer_discovery.cpp
 * @brief Comprehensive tests for peer discovery mechanisms
 * 
 * Tests bootstrap nodes, DNS seeds, peer exchange, and address validation
 */

#include <gtest/gtest.h>
#include "chainforge/p2p/peer_address.hpp"
#include "chainforge/p2p/peer_discovery.hpp"
#include <thread>
#include <chrono>

using namespace chainforge::p2p;
using namespace std::chrono_literals;

// ============================================================================
// PeerAddress Tests
// ============================================================================

TEST(PeerAddressTest, Construction) {
    PeerAddress addr("192.168.1.100", 8333);
    
    EXPECT_EQ(addr.ip, "192.168.1.100");
    EXPECT_EQ(addr.port, 8333);
    EXPECT_EQ(addr.services, 0);
}

TEST(PeerAddressTest, ToStringFormat) {
    PeerAddress addr("127.0.0.1", 8080);
    EXPECT_EQ(addr.to_string(), "127.0.0.1:8080");
}

TEST(PeerAddressTest, Comparison) {
    PeerAddress addr1("192.168.1.100", 8333);
    PeerAddress addr2("192.168.1.100", 8333);
    PeerAddress addr3("192.168.1.101", 8333);
    
    EXPECT_EQ(addr1, addr2);
    EXPECT_NE(addr1, addr3);
}

TEST(PeerAddressTest, LastSeen) {
    PeerAddress addr("127.0.0.1", 8080);
    
    auto before = std::chrono::system_clock::now();
    addr.update_last_seen();
    auto after = std::chrono::system_clock::now();
    
    EXPECT_GE(addr.last_seen, before);
    EXPECT_LE(addr.last_seen, after);
}

TEST(PeerAddressTest, AgeCalculation) {
    PeerAddress addr("127.0.0.1", 8080);
    addr.last_seen = std::chrono::system_clock::now() - std::chrono::seconds(60);
    
    auto age = addr.age_seconds();
    EXPECT_GE(age, 59);
    EXPECT_LE(age, 61);
}

// ============================================================================
// Address Validation Tests
// ============================================================================

TEST(AddressValidationTest, ValidIPv4) {
    EXPECT_TRUE(address_validation::is_valid_ip("192.168.1.1"));
    EXPECT_TRUE(address_validation::is_valid_ip("8.8.8.8"));
    EXPECT_TRUE(address_validation::is_valid_ip("127.0.0.1"));
}

TEST(AddressValidationTest, InvalidIPv4) {
    EXPECT_FALSE(address_validation::is_valid_ip("256.1.1.1"));
    EXPECT_FALSE(address_validation::is_valid_ip("192.168.1"));
    EXPECT_FALSE(address_validation::is_valid_ip("not.an.ip"));
}

TEST(AddressValidationTest, ValidIPv6) {
    EXPECT_TRUE(address_validation::is_valid_ip("::1"));
    EXPECT_TRUE(address_validation::is_valid_ip("2001:0db8:85a3:0000:0000:8a2e:0370:7334"));
    EXPECT_TRUE(address_validation::is_valid_ip("fe80::1"));
}

TEST(AddressValidationTest, Localhost) {
    EXPECT_TRUE(address_validation::is_localhost("127.0.0.1"));
    EXPECT_TRUE(address_validation::is_localhost("::1"));
    EXPECT_FALSE(address_validation::is_localhost("192.168.1.1"));
}

TEST(AddressValidationTest, PrivateIP) {
    // 10.0.0.0/8
    EXPECT_TRUE(address_validation::is_private_ip("10.0.0.1"));
    EXPECT_TRUE(address_validation::is_private_ip("10.255.255.255"));
    
    // 172.16.0.0/12
    EXPECT_TRUE(address_validation::is_private_ip("172.16.0.1"));
    EXPECT_TRUE(address_validation::is_private_ip("172.31.255.255"));
    
    // 192.168.0.0/16
    EXPECT_TRUE(address_validation::is_private_ip("192.168.0.1"));
    EXPECT_TRUE(address_validation::is_private_ip("192.168.255.255"));
    
    // Public IPs
    EXPECT_FALSE(address_validation::is_private_ip("8.8.8.8"));
    EXPECT_FALSE(address_validation::is_private_ip("1.1.1.1"));
}

TEST(AddressValidationTest, RoutableIP) {
    // Routable public IPs
    EXPECT_TRUE(address_validation::is_routable_ip("8.8.8.8"));
    EXPECT_TRUE(address_validation::is_routable_ip("1.1.1.1"));
    
    // Non-routable
    EXPECT_FALSE(address_validation::is_routable_ip("127.0.0.1"));
    EXPECT_FALSE(address_validation::is_routable_ip("192.168.1.1"));
    EXPECT_FALSE(address_validation::is_routable_ip("10.0.0.1"));
}

TEST(AddressValidationTest, ValidPort) {
    EXPECT_TRUE(address_validation::is_valid_port(8333));
    EXPECT_TRUE(address_validation::is_valid_port(1));
    EXPECT_TRUE(address_validation::is_valid_port(65535));
    EXPECT_FALSE(address_validation::is_valid_port(0));
}

TEST(AddressValidationTest, ValidPeerAddress) {
    PeerAddress valid("8.8.8.8", 8333);
    EXPECT_TRUE(address_validation::is_valid_peer_address(valid, false));
    
    PeerAddress private_addr("192.168.1.1", 8333);
    EXPECT_FALSE(address_validation::is_valid_peer_address(private_addr, false));
    EXPECT_TRUE(address_validation::is_valid_peer_address(private_addr, true));
    
    PeerAddress invalid_ip("invalid", 8333);
    EXPECT_FALSE(address_validation::is_valid_peer_address(invalid_ip, false));
    
    PeerAddress invalid_port("8.8.8.8", 0);
    EXPECT_FALSE(address_validation::is_valid_peer_address(invalid_port, false));
}

TEST(AddressValidationTest, PeerAddressValidation) {
    PeerAddress valid("8.8.8.8", 8333);
    EXPECT_TRUE(valid.is_valid());
    EXPECT_FALSE(valid.is_local());
    EXPECT_TRUE(valid.is_routable());
    
    PeerAddress local("192.168.1.1", 8333);
    EXPECT_FALSE(local.is_valid());  // Not routable
    EXPECT_TRUE(local.is_local());
    EXPECT_FALSE(local.is_routable());
}

// ============================================================================
// PeerInfo Tests
// ============================================================================

TEST(PeerInfoTest, Construction) {
    PeerAddress addr("127.0.0.1", 8333);
    PeerInfo info(addr);
    
    EXPECT_EQ(info.address, addr);
    EXPECT_FALSE(info.connected);
    EXPECT_EQ(info.connection_attempts, 0);
}

TEST(PeerInfoTest, ConnectionTracking) {
    PeerInfo info(PeerAddress("127.0.0.1", 8333));
    
    EXPECT_FALSE(info.connected);
    
    info.mark_connected();
    EXPECT_TRUE(info.connected);
    
    info.mark_disconnected();
    EXPECT_FALSE(info.connected);
}

TEST(PeerInfoTest, ConnectionAttempts) {
    PeerInfo info(PeerAddress("127.0.0.1", 8333));
    
    EXPECT_EQ(info.connection_attempts, 0);
    
    info.record_attempt();
    EXPECT_EQ(info.connection_attempts, 1);
    
    info.record_attempt();
    EXPECT_EQ(info.connection_attempts, 2);
}

TEST(PeerInfoTest, ShouldRetry) {
    PeerInfo info(PeerAddress("127.0.0.1", 8333));
    
    // Should retry initially (no attempt yet)
    EXPECT_TRUE(info.should_retry(std::chrono::seconds(1)));
    
    // Record attempt
    info.record_attempt();
    
    // Should not retry immediately
    EXPECT_FALSE(info.should_retry(std::chrono::seconds(10)));
    
    // Wait a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Should retry after short interval
    EXPECT_TRUE(info.should_retry(std::chrono::milliseconds(50)));
    
    // Should not retry if connected
    info.mark_connected();
    EXPECT_FALSE(info.should_retry(std::chrono::seconds(1)));
}

// ============================================================================
// PeerExchange Tests
// ============================================================================

TEST(PeerExchangeTest, SerializeDeserialize) {
    std::vector<PeerAddress> original = {
        PeerAddress("192.168.1.1", 8333, ServiceFlags::NODE_NETWORK),
        PeerAddress("192.168.1.2", 8334, ServiceFlags::NODE_BLOOM),
        PeerAddress("10.0.0.1", 8335, ServiceFlags::NODE_WITNESS)
    };
    
    auto serialized = PeerExchange::serialize_peers(original);
    EXPECT_GT(serialized.size(), 0);
    
    auto deserialized = PeerExchange::deserialize_peers(serialized);
    
    ASSERT_EQ(deserialized.size(), original.size());
    
    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(deserialized[i].ip, original[i].ip);
        EXPECT_EQ(deserialized[i].port, original[i].port);
        EXPECT_EQ(deserialized[i].services, original[i].services);
    }
}

TEST(PeerExchangeTest, EmptyList) {
    std::vector<PeerAddress> empty;
    
    auto serialized = PeerExchange::serialize_peers(empty);
    EXPECT_EQ(serialized.size(), 4);  // Just count field
    
    auto deserialized = PeerExchange::deserialize_peers(serialized);
    EXPECT_EQ(deserialized.size(), 0);
}

TEST(PeerExchangeTest, SelectPeersToShare) {
    std::vector<PeerAddress> peers;
    for (int i = 0; i < 20; ++i) {
        peers.emplace_back("192.168.1." + std::to_string(i), 8333);
    }
    
    auto selected = PeerExchange::select_peers_to_share(peers, 10);
    
    EXPECT_EQ(selected.size(), 10);
    
    // Should return all if less than max
    auto selected_all = PeerExchange::select_peers_to_share(peers, 30);
    EXPECT_EQ(selected_all.size(), 20);
}

// ============================================================================
// PeerDiscovery Tests
// ============================================================================

class PeerDiscoveryTest : public ::testing::Test {
protected:
    void SetUp() override {
        work_guard = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(
            io_context.get_executor()
        );
        
        io_thread = std::thread([this]() {
            io_context.run();
        });
        
        // Create config with test settings
        config.enable_mdns = false;  // Disable for most tests
        config.enable_peer_exchange = true;
        config.discovery_port = 0;  // Use ephemeral port
        config.max_peers = 100;
    }

    void TearDown() override {
        work_guard.reset();
        io_context.stop();
        
        if (io_thread.joinable()) {
            io_thread.join();
        }
    }

    asio::io_context io_context;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> work_guard;
    std::thread io_thread;
    BootstrapConfig config;
};

TEST_F(PeerDiscoveryTest, Construction) {
    PeerDiscovery discovery(io_context, config);
    
    EXPECT_FALSE(discovery.is_running());
    EXPECT_EQ(discovery.peer_count(), 0);
}

TEST_F(PeerDiscoveryTest, BootstrapNodes) {
    config.static_nodes = {
        PeerAddress("127.0.0.1", 8333),
        PeerAddress("127.0.0.1", 8334)
    };
    
    PeerDiscovery discovery(io_context, config);
    discovery.start();
    
    EXPECT_TRUE(discovery.is_running());
    EXPECT_EQ(discovery.peer_count(), 2);
    
    discovery.stop();
    EXPECT_FALSE(discovery.is_running());
}

TEST_F(PeerDiscoveryTest, AddRemovePeer) {
    PeerDiscovery discovery(io_context, config);
    
    PeerAddress addr("127.0.0.1", 8333);
    
    EXPECT_FALSE(discovery.has_peer(addr));
    
    EXPECT_TRUE(discovery.add_peer(addr));
    EXPECT_TRUE(discovery.has_peer(addr));
    EXPECT_EQ(discovery.peer_count(), 1);
    
    EXPECT_TRUE(discovery.remove_peer(addr));
    EXPECT_FALSE(discovery.has_peer(addr));
    EXPECT_EQ(discovery.peer_count(), 0);
}

TEST_F(PeerDiscoveryTest, AddInvalidPeer) {
    PeerDiscovery discovery(io_context, config);
    
    // Invalid IP
    PeerAddress invalid("invalid.ip", 8333);
    EXPECT_FALSE(discovery.add_peer(invalid));
    
    // Invalid port
    PeerAddress invalid_port("127.0.0.1", 0);
    EXPECT_FALSE(discovery.add_peer(invalid_port));
}

TEST_F(PeerDiscoveryTest, MaxPeers) {
    config.max_peers = 5;
    PeerDiscovery discovery(io_context, config);
    
    // Add max_peers
    for (int i = 0; i < 5; ++i) {
        PeerAddress addr("127.0.0." + std::to_string(i + 1), 8333);
        EXPECT_TRUE(discovery.add_peer(addr));
    }
    
    EXPECT_EQ(discovery.peer_count(), 5);
    
    // Try to add one more (should fail)
    PeerAddress extra("127.0.0.100", 8333);
    EXPECT_FALSE(discovery.add_peer(extra));
    EXPECT_EQ(discovery.peer_count(), 5);
}

TEST_F(PeerDiscoveryTest, GetPeers) {
    PeerDiscovery discovery(io_context, config);
    
    std::vector<PeerAddress> added = {
        PeerAddress("127.0.0.1", 8333),
        PeerAddress("127.0.0.2", 8333),
        PeerAddress("127.0.0.3", 8333)
    };
    
    for (const auto& addr : added) {
        discovery.add_peer(addr);
    }
    
    auto peers = discovery.get_peers();
    
    EXPECT_EQ(peers.size(), 3);
    
    // Check all peers are present (order may differ)
    for (const auto& addr : added) {
        EXPECT_TRUE(std::find(peers.begin(), peers.end(), addr) != peers.end());
    }
}

TEST_F(PeerDiscoveryTest, GetRandomPeers) {
    PeerDiscovery discovery(io_context, config);
    
    // Add 10 peers
    for (int i = 0; i < 10; ++i) {
        discovery.add_peer(PeerAddress("127.0.0." + std::to_string(i + 1), 8333));
    }
    
    // Get 5 random peers
    auto random = discovery.get_random_peers(5);
    EXPECT_EQ(random.size(), 5);
    
    // Get more than available
    auto all = discovery.get_random_peers(20);
    EXPECT_EQ(all.size(), 10);
}

TEST_F(PeerDiscoveryTest, DiscoveryCallback) {
    PeerDiscovery discovery(io_context, config);
    
    std::atomic<int> callback_count{0};
    PeerAddress last_discovered;
    
    discovery.set_discovery_callback([&](const PeerAddress& addr) {
        callback_count++;
        last_discovered = addr;
    });
    
    PeerAddress addr("127.0.0.1", 8333);
    discovery.add_peer(addr);
    
    EXPECT_EQ(callback_count.load(), 1);
    EXPECT_EQ(last_discovered, addr);
}

TEST_F(PeerDiscoveryTest, PeerExchange) {
    PeerDiscovery discovery(io_context, config);
    
    // Add some initial peers
    discovery.add_peer(PeerAddress("127.0.0.1", 8333));
    discovery.add_peer(PeerAddress("127.0.0.2", 8333));
    
    // Peers from another node
    std::vector<PeerAddress> their_peers = {
        PeerAddress("127.0.0.3", 8333),
        PeerAddress("127.0.0.4", 8333)
    };
    
    std::vector<PeerAddress> our_peers;
    
    auto result = discovery.exchange_peers(their_peers, our_peers);
    
    ASSERT_TRUE(result.has_value());
    
    // Should have added their peers
    EXPECT_EQ(discovery.peer_count(), 4);
    
    // Should share our peers
    EXPECT_GT(our_peers.size(), 0);
}

TEST_F(PeerDiscoveryTest, CleanupStalePeers) {
    PeerDiscovery discovery(io_context, config);
    
    // Add peer with old timestamp
    PeerAddress stale("127.0.0.1", 8333);
    stale.last_seen = std::chrono::system_clock::now() - std::chrono::hours(48);
    discovery.add_peer(stale);
    
    // Add recent peer
    PeerAddress recent("127.0.0.2", 8333);
    discovery.add_peer(recent);
    
    EXPECT_EQ(discovery.peer_count(), 2);
    
    // Cleanup stale peers (max age 24 hours)
    discovery.cleanup_stale_peers(std::chrono::hours(24));
    
    EXPECT_EQ(discovery.peer_count(), 1);
    EXPECT_TRUE(discovery.has_peer(recent));
    EXPECT_FALSE(discovery.has_peer(stale));
}

// ============================================================================
// DnsSeedResolver Tests
// ============================================================================

TEST(DnsSeedResolverTest, ResolveLocalhost) {
    auto peers = DnsSeedResolver::resolve("localhost", 8333);
    
    // Should resolve to at least one address (127.0.0.1 and/or ::1)
    EXPECT_GT(peers.size(), 0);
    
    // All should have port 8333
    for (const auto& peer : peers) {
        EXPECT_EQ(peer.port, 8333);
    }
}

TEST(DnsSeedResolverTest, ResolveInvalidDomain) {
    auto peers = DnsSeedResolver::resolve("invalid.domain.that.does.not.exist.xyz", 8333);
    
    // Should return empty vector on failure
    EXPECT_EQ(peers.size(), 0);
}

TEST(DnsSeedResolverTest, ResolveMultiple) {
    std::vector<std::string> seeds = {"localhost", "127.0.0.1"};
    
    auto peers = DnsSeedResolver::resolve_multiple(seeds, 8333);
    
    EXPECT_GT(peers.size(), 0);
    
    // Should remove duplicates
    std::set<PeerAddress> unique_peers(peers.begin(), peers.end());
    EXPECT_EQ(unique_peers.size(), peers.size());
}

// ============================================================================
// Service Flags Tests
// ============================================================================

TEST(ServiceFlagsTest, FlagValues) {
    EXPECT_EQ(ServiceFlags::NODE_NETWORK, 1);
    EXPECT_EQ(ServiceFlags::NODE_BLOOM, 2);
    EXPECT_EQ(ServiceFlags::NODE_WITNESS, 4);
    EXPECT_EQ(ServiceFlags::NODE_COMPACT, 8);
    EXPECT_EQ(ServiceFlags::NODE_NETWORK_LIMITED, 16);
}

TEST(ServiceFlagsTest, CombineFlags) {
    uint32_t flags = ServiceFlags::NODE_NETWORK | ServiceFlags::NODE_WITNESS;
    
    EXPECT_EQ(flags, 5);
    EXPECT_TRUE(flags & ServiceFlags::NODE_NETWORK);
    EXPECT_FALSE(flags & ServiceFlags::NODE_BLOOM);
    EXPECT_TRUE(flags & ServiceFlags::NODE_WITNESS);
}

