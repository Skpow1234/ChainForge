/**
 * @file test_framework_demo.cpp
 * @brief Demonstration of the ChainForge testing framework features
 * 
 * This file showcases all the testing capabilities:
 * - Test fixtures
 * - Test helpers
 * - Mock objects
 * - Custom matchers
 * - Parameterized tests
 * - Performance tests
 */

#include <gtest/gtest.h>
#include "test_fixtures.hpp"
#include "test_helpers.hpp"
#include "mock_objects.hpp"

using namespace chainforge;
using namespace chainforge::testing;
using ::testing::Return;
using ::testing::_;

// ============================================================================
// Basic Fixture Tests
// ============================================================================

TEST_F(BlockchainTestFixture, CreateBasicObjects) {
    // Test that fixture provides valid test objects
    EXPECT_TRUE(test_address_from.is_valid());
    EXPECT_TRUE(test_address_to.is_valid());
    EXPECT_GT(test_amount.wei(), 0);
    EXPECT_GT(test_timestamp.seconds(), 0);
    EXPECT_FALSE(test_hash.is_zero());
}

TEST_F(TransactionTestFixture, DefaultTransactionIsValid) {
    // Test that the fixture creates a valid transaction
    ASSERT_NE(default_tx, nullptr);
    EXPECT_TRUE(default_tx->is_valid());
    EXPECT_EQ(default_tx->gas_limit(), 21000);
    EXPECT_EQ(default_tx->nonce(), 0);
}

TEST_F(BlockTestFixture, DefaultBlockIsValid) {
    // Test that the fixture creates a valid block
    ASSERT_NE(default_block, nullptr);
    EXPECT_TRUE(default_block->is_valid());
    EXPECT_EQ(default_block->height(), 1);
    EXPECT_EQ(default_block->transaction_count(), 0);
}

// ============================================================================
// Test Helpers Tests
// ============================================================================

TEST(TestHelpersTest, CreateRandomTransaction) {
    auto tx = TestHelpers::CreateRandomTransaction();
    
    EXPECT_TRUE(tx.is_valid());
    EXPECT_GT(tx.gas_limit(), 0);
    EXPECT_GT(tx.value().wei(), 0);
}

TEST(TestHelpersTest, CreateTransactionWithSpecificParameters) {
    auto from = core::Address::random();
    auto to = core::Address::random();
    
    auto tx = TestHelpers::CreateTransaction(from, to, 1000000, 50000, 2000000000, 5);
    
    EXPECT_EQ(tx.from(), from);
    EXPECT_EQ(tx.to(), to);
    EXPECT_EQ(tx.value().wei(), 1000000);
    EXPECT_EQ(tx.gas_limit(), 50000);
    EXPECT_EQ(tx.gas_price(), 2000000000);
    EXPECT_EQ(tx.nonce(), 5);
}

TEST(TestHelpersTest, CreateTransactionWithData) {
    auto from = core::Address::random();
    auto to = core::Address::random();
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04};
    
    auto tx = TestHelpers::CreateTransactionWithData(from, to, data);
    
    EXPECT_EQ(tx.payload().size(), data.size());
    EXPECT_EQ(tx.payload(), data);
}

TEST(TestHelpersTest, CreateGenesisBlock) {
    auto genesis = TestHelpers::CreateGenesisBlock(1);
    
    EXPECT_EQ(genesis.height(), 0);
    EXPECT_TRUE(genesis.parent_hash().is_zero());
    EXPECT_EQ(genesis.chain_id(), 1);
}

TEST(TestHelpersTest, CreateBlockchain) {
    auto chain = TestHelpers::CreateBlockchain(5, 3);
    
    ASSERT_EQ(chain.size(), 5);
    EXPECT_EQ(chain[0].height(), 0);  // Genesis
    EXPECT_EQ(chain[4].height(), 4);  // Last block
    
    // Verify chain linkage
    for (size_t i = 1; i < chain.size(); ++i) {
        EXPECT_EQ(chain[i].parent_hash(), chain[i-1].calculate_hash());
    }
    
    // Verify transactions
    for (size_t i = 1; i < chain.size(); ++i) {
        EXPECT_EQ(chain[i].transaction_count(), 3);
    }
}

TEST(TestHelpersTest, DeterministicObjects) {
    // Same seed should produce same objects
    auto addr1 = TestHelpers::CreateDeterministicAddress(42);
    auto addr2 = TestHelpers::CreateDeterministicAddress(42);
    EXPECT_EQ(addr1, addr2);
    
    auto hash1 = TestHelpers::CreateDeterministicHash(42);
    auto hash2 = TestHelpers::CreateDeterministicHash(42);
    EXPECT_EQ(hash1, hash2);
    
    // Different seeds should produce different objects
    auto addr3 = TestHelpers::CreateDeterministicAddress(43);
    EXPECT_NE(addr1, addr3);
}

TEST(TestHelpersTest, CompareTransactions) {
    auto from = core::Address::random();
    auto to = core::Address::random();
    
    auto tx1 = TestHelpers::CreateTransaction(from, to, 1000);
    auto tx2 = TestHelpers::CreateTransaction(from, to, 1000);
    
    EXPECT_TRUE(TestHelpers::TransactionsEqual(tx1, tx2));
    
    auto tx3 = TestHelpers::CreateTransaction(from, to, 2000);
    EXPECT_FALSE(TestHelpers::TransactionsEqual(tx1, tx3));
}

// ============================================================================
// Custom Matchers Tests
// ============================================================================

TEST(MatchersTest, AmountInRange) {
    core::Amount amount = core::Amount::from_wei(5000);
    
    EXPECT_TRUE(matchers::AmountInRange(amount, 1000, 10000));
    EXPECT_FALSE(matchers::AmountInRange(amount, 6000, 10000));
    EXPECT_FALSE(matchers::AmountInRange(amount, 1000, 4000));
}

TEST(MatchersTest, TimestampIsRecent) {
    auto now = core::Timestamp::now();
    EXPECT_TRUE(matchers::TimestampIsRecent(now, 60));
    
    auto old_ts = core::Timestamp::from_seconds(now.seconds() - 120);
    EXPECT_FALSE(matchers::TimestampIsRecent(old_ts, 60));
}

TEST(MatchersTest, HashIsNonZero) {
    auto random_hash = core::Hash::random();
    EXPECT_TRUE(matchers::HashIsNonZero(random_hash));
    
    auto zero_hash = core::Hash::zero();
    EXPECT_FALSE(matchers::HashIsNonZero(zero_hash));
}

TEST(MatchersTest, TransactionIsValid) {
    auto tx = TestHelpers::CreateRandomTransaction();
    EXPECT_TRUE(matchers::TransactionIsValid(tx));
}

TEST(MatchersTest, BlockIsValid) {
    auto block = TestHelpers::CreateRandomBlock(1);
    EXPECT_TRUE(matchers::BlockIsValid(block));
}

// ============================================================================
// Mock Objects Tests
// ============================================================================

TEST(MockObjectsTest, MockSerializerBasicUsage) {
    MockSerializer mock_serializer;
    
    // Set up expectation
    std::vector<uint8_t> expected_data = {0x01, 0x02, 0x03};
    EXPECT_CALL(mock_serializer, serialize_hash(_))
        .WillOnce(Return(serialization::SerializationResult<std::vector<uint8_t>>(expected_data)));
    
    // Use the mock
    auto hash = core::Hash::random();
    auto result = mock_serializer.serialize_hash(hash);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), expected_data);
}

TEST(MockObjectsTest, MockValidatorBasicUsage) {
    MockValidator mock_validator;
    
    // Set up expectation
    EXPECT_CALL(mock_validator, validate_transaction(_))
        .WillOnce(Return(core::success()));
    
    // Use the mock
    auto tx = TestHelpers::CreateRandomTransaction();
    auto result = mock_validator.validate_transaction(tx);
    
    EXPECT_TRUE(result.has_value());
}

TEST(MockObjectsTest, StubSerializerAlwaysSucceeds) {
    stubs::StubSerializer stub;
    
    auto block = TestHelpers::CreateRandomBlock(1);
    auto result = stub.serialize_block(block);
    
    ASSERT_TRUE(result.has_value());
    EXPECT_GT(result.value().size(), 0);
}

TEST(MockObjectsTest, StubValidatorAlwaysSucceeds) {
    stubs::StubValidator stub;
    
    auto tx = TestHelpers::CreateRandomTransaction();
    auto result = stub.validate_transaction(tx);
    
    EXPECT_TRUE(result.has_value());
}

// ============================================================================
// Blockchain Chain Tests
// ============================================================================

TEST_F(BlockchainChainTestFixture, GenesisBlockCreated) {
    ASSERT_NE(genesis_block, nullptr);
    EXPECT_EQ(genesis_block->height(), 0);
    EXPECT_TRUE(genesis_block->parent_hash().is_zero());
}

TEST_F(BlockchainChainTestFixture, AddBlockToChain) {
    ASSERT_EQ(chain.size(), 1);  // Genesis block
    
    AddBlockToChain(5);  // Add block with 5 transactions
    
    ASSERT_EQ(chain.size(), 2);
    EXPECT_EQ(chain[1]->height(), 1);
    EXPECT_EQ(chain[1]->transaction_count(), 5);
    EXPECT_EQ(chain[1]->parent_hash(), chain[0]->calculate_hash());
}

TEST_F(BlockchainChainTestFixture, BuildLongChain) {
    for (size_t i = 0; i < 10; ++i) {
        AddBlockToChain(i % 5);  // Varying number of transactions
    }
    
    ASSERT_EQ(chain.size(), 11);  // Genesis + 10 blocks
    
    // Verify chain integrity
    for (size_t i = 1; i < chain.size(); ++i) {
        EXPECT_EQ(chain[i]->height(), i);
        EXPECT_EQ(chain[i]->parent_hash(), chain[i-1]->calculate_hash());
    }
}

// ============================================================================
// Parameterized Tests
// ============================================================================

TEST_P(ParameterizedBlockchainTest, TransactionWithDifferentGasSettings) {
    auto tx = TestHelpers::CreateTransaction(
        test_address_from,
        test_address_to,
        1000000,
        gas_limit,
        gas_price,
        0
    );
    
    EXPECT_TRUE(tx.is_valid());
    EXPECT_EQ(tx.gas_limit(), gas_limit);
    EXPECT_EQ(tx.gas_price(), gas_price);
    
    // Calculate expected fee
    auto expected_fee = gas_limit * gas_price;
    auto actual_fee = tx.gas_limit() * tx.gas_price();
    EXPECT_EQ(actual_fee, expected_fee);
}

// Parameterized test values: (gas_limit, gas_price)
INSTANTIATE_TEST_SUITE_P(
    GasParameterVariations,
    ParameterizedBlockchainTest,
    ::testing::Values(
        std::make_tuple(21000, 1000000000),      // Standard
        std::make_tuple(50000, 2000000000),      // High gas limit
        std::make_tuple(21000, 10000000000),     // High gas price
        std::make_tuple(100000, 5000000000),     // Both high
        std::make_tuple(21000, 1)                // Minimal gas price
    )
);

// ============================================================================
// Performance Tests
// ============================================================================

TEST_F(PerformanceTestFixture, TransactionCreationPerformance) {
    const int iterations = 1000;
    
    auto avg_time = MeasureAverageMicroseconds([this]() {
        auto tx = TestHelpers::CreateTransaction(
            test_address_from,
            test_address_to,
            1000000
        );
    }, iterations);
    
    std::cout << "Average transaction creation time: " << avg_time << " μs" << std::endl;
    EXPECT_LT(avg_time, 10);  // Should be < 10 microseconds
}

TEST_F(PerformanceTestFixture, BlockCreationPerformance) {
    const int iterations = 100;
    
    auto avg_time = MeasureAverageMicroseconds([&]() {
        auto block = TestHelpers::CreateBlockWithTransactions(1, test_hash, 10);
    }, iterations);
    
    std::cout << "Average block creation time (10 txs): " << avg_time << " μs" << std::endl;
    EXPECT_LT(avg_time, 1000);  // Should be < 1ms
}

TEST_F(PerformanceTestFixture, HashCalculationPerformance) {
    auto block = TestHelpers::CreateBlockWithTransactions(1, test_hash, 50);
    const int iterations = 100;
    
    auto avg_time = MeasureAverageMicroseconds([&]() {
        auto hash = block.calculate_hash();
    }, iterations);
    
    std::cout << "Average hash calculation time: " << avg_time << " μs" << std::endl;
    EXPECT_LT(avg_time, 100);  // Should be < 100 microseconds
}

// ============================================================================
// Call Tracker Tests
// ============================================================================

TEST(CallTrackerTest, BasicTracking) {
    CallTracker<int> tracker;
    
    EXPECT_EQ(tracker.CallCount(), 0);
    
    tracker.Record(42);
    tracker.Record(43);
    tracker.Record(42);
    
    EXPECT_EQ(tracker.CallCount(), 3);
    EXPECT_TRUE(tracker.WasCalledWith(42));
    EXPECT_TRUE(tracker.WasCalledWith(43));
    EXPECT_FALSE(tracker.WasCalledWith(44));
    
    tracker.Clear();
    EXPECT_EQ(tracker.CallCount(), 0);
}

