#pragma once

#include <gtest/gtest.h>
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/core/timestamp.hpp"
#include "chainforge/core/hash.hpp"
#include <memory>
#include <vector>

namespace chainforge {
namespace testing {

/**
 * @brief Base test fixture providing common blockchain objects
 */
class BlockchainTestFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // Create common test objects
        test_address_from = core::Address::random();
        test_address_to = core::Address::random();
        test_amount = core::Amount::from_wei(1000000000000000000ULL); // 1 ETH
        test_timestamp = core::Timestamp::now();
        test_hash = core::Hash::random();
        test_parent_hash = core::Hash::random();
    }

    void TearDown() override {
        // Cleanup if needed
    }

    // Common test objects
    core::Address test_address_from;
    core::Address test_address_to;
    core::Amount test_amount;
    core::Timestamp test_timestamp;
    core::Hash test_hash;
    core::Hash test_parent_hash;
};

/**
 * @brief Test fixture for transaction-related tests
 */
class TransactionTestFixture : public BlockchainTestFixture {
protected:
    void SetUp() override {
        BlockchainTestFixture::SetUp();
        
        // Create a default transaction
        default_tx = std::make_unique<core::Transaction>(
            test_address_from,
            test_address_to,
            test_amount
        );
        default_tx->set_gas_limit(21000);
        default_tx->set_gas_price(1000000000); // 1 gwei
        default_tx->set_nonce(0);
    }

    std::unique_ptr<core::Transaction> default_tx;
};

/**
 * @brief Test fixture for block-related tests
 */
class BlockTestFixture : public BlockchainTestFixture {
protected:
    void SetUp() override {
        BlockchainTestFixture::SetUp();
        
        // Create a default block
        default_block = std::make_unique<core::Block>(
            1,  // height
            test_parent_hash,
            test_timestamp
        );
        default_block->set_nonce(123456);
        default_block->set_gas_limit(8000000);
        default_block->set_gas_price(1000000000);
        default_block->set_chain_id(1);
    }

    std::unique_ptr<core::Block> default_block;
};

/**
 * @brief Test fixture for blockchain (chain of blocks) tests
 */
class BlockchainChainTestFixture : public BlockTestFixture {
protected:
    void SetUp() override {
        BlockTestFixture::SetUp();
        
        // Create a genesis block
        genesis_block = std::make_unique<core::Block>(
            0,
            core::Hash::zero(),
            core::Timestamp::from_seconds(1609459200) // 2021-01-01
        );
        genesis_block->set_gas_limit(8000000);
        genesis_block->set_chain_id(1);
        
        // Create a chain of blocks
        chain.clear();
        chain.push_back(std::make_unique<core::Block>(*genesis_block));
    }

    void AddBlockToChain(size_t num_transactions = 0) {
        auto parent_hash = chain.back()->calculate_hash();
        auto height = chain.back()->height() + 1;
        auto timestamp = core::Timestamp::now();
        
        auto block = std::make_unique<core::Block>(height, parent_hash, timestamp);
        block->set_gas_limit(8000000);
        block->set_chain_id(1);
        
        // Add transactions if requested
        for (size_t i = 0; i < num_transactions; ++i) {
            core::Transaction tx(
                core::Address::random(),
                core::Address::random(),
                core::Amount::from_wei(1000000)
            );
            tx.set_gas_limit(21000);
            tx.set_nonce(i);
            block->add_transaction(tx);
        }
        
        chain.push_back(std::move(block));
    }

    std::unique_ptr<core::Block> genesis_block;
    std::vector<std::unique_ptr<core::Block>> chain;
};

/**
 * @brief Parameterized test fixture for testing multiple configurations
 */
class ParameterizedBlockchainTest : public BlockchainTestFixture,
                                     public ::testing::WithParamInterface<std::tuple<uint64_t, uint64_t>> {
protected:
    void SetUp() override {
        BlockchainTestFixture::SetUp();
        
        // Get parameters: (gas_limit, gas_price)
        std::tie(gas_limit, gas_price) = GetParam();
    }

    uint64_t gas_limit;
    uint64_t gas_price;
};

/**
 * @brief Test fixture with performance monitoring
 */
class PerformanceTestFixture : public BlockchainTestFixture {
protected:
    void SetUp() override {
        BlockchainTestFixture::SetUp();
        start_time = std::chrono::high_resolution_clock::now();
    }

    void TearDown() override {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - start_time);
        
        // Log performance if test passed
        if (!HasFailure()) {
            std::cout << "[PERF] Test duration: " << duration.count() << " μs" << std::endl;
        }
        
        BlockchainTestFixture::TearDown();
    }

    template<typename Func>
    int64_t MeasureMicroseconds(Func&& func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    template<typename Func>
    int64_t MeasureAverageMicroseconds(Func&& func, int iterations) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto total = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        return total / iterations;
    }

private:
    std::chrono::high_resolution_clock::time_point start_time;
};

} // namespace testing
} // namespace chainforge

