#pragma once

#include <gtest/gtest.h>
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/core/timestamp.hpp"
#include "chainforge/core/hash.hpp"
#include <random>
#include <string>
#include <vector>

namespace chainforge {
namespace testing {

/**
 * @brief Test helper functions for creating blockchain objects
 */
class TestHelpers {
public:
    /**
     * @brief Create a random transaction
     */
    static core::Transaction CreateRandomTransaction() {
        core::Transaction tx(
            core::Address::random(),
            core::Address::random(),
            core::Amount::from_wei(RandomAmount())
        );
        tx.set_gas_limit(21000 + (rand() % 100000));
        tx.set_gas_price(1000000000 + (rand() % 1000000000));
        tx.set_nonce(rand() % 1000);
        return tx;
    }

    /**
     * @brief Create a transaction with specific parameters
     */
    static core::Transaction CreateTransaction(
        const core::Address& from,
        const core::Address& to,
        uint64_t wei_amount,
        uint64_t gas_limit = 21000,
        uint64_t gas_price = 1000000000,
        uint64_t nonce = 0
    ) {
        core::Transaction tx(from, to, core::Amount::from_wei(wei_amount));
        tx.set_gas_limit(gas_limit);
        tx.set_gas_price(gas_price);
        tx.set_nonce(nonce);
        return tx;
    }

    /**
     * @brief Create a transaction with data payload
     */
    static core::Transaction CreateTransactionWithData(
        const core::Address& from,
        const core::Address& to,
        const std::vector<uint8_t>& data
    ) {
        core::Transaction tx(from, to, core::Amount::zero());
        tx.set_gas_limit(100000);
        tx.set_gas_price(1000000000);
        tx.set_nonce(0);
        tx.set_data(data);
        return tx;
    }

    /**
     * @brief Create a random block
     */
    static core::Block CreateRandomBlock(uint32_t height = 1) {
        core::Block block(
            height,
            core::Hash::random(),
            core::Timestamp::now()
        );
        block.set_nonce(rand() % 1000000);
        block.set_gas_limit(8000000);
        block.set_gas_price(1000000000);
        block.set_chain_id(1);
        return block;
    }

    /**
     * @brief Create a block with specific parameters
     */
    static core::Block CreateBlock(
        uint32_t height,
        const core::Hash& parent_hash,
        const core::Timestamp& timestamp,
        uint64_t nonce = 0
    ) {
        core::Block block(height, parent_hash, timestamp);
        block.set_nonce(nonce);
        block.set_gas_limit(8000000);
        block.set_gas_price(1000000000);
        block.set_chain_id(1);
        return block;
    }

    /**
     * @brief Create a block with transactions
     */
    static core::Block CreateBlockWithTransactions(
        uint32_t height,
        const core::Hash& parent_hash,
        size_t num_transactions
    ) {
        auto block = CreateBlock(height, parent_hash, core::Timestamp::now());
        
        for (size_t i = 0; i < num_transactions; ++i) {
            auto tx = CreateRandomTransaction();
            tx.set_nonce(i);
            block.add_transaction(tx);
        }
        
        return block;
    }

    /**
     * @brief Create a genesis block
     */
    static core::Block CreateGenesisBlock(uint32_t chain_id = 1) {
        core::Block genesis(
            0,
            core::Hash::zero(),
            core::Timestamp::from_seconds(1609459200) // 2021-01-01
        );
        genesis.set_gas_limit(8000000);
        genesis.set_chain_id(chain_id);
        return genesis;
    }

    /**
     * @brief Create a deterministic address from seed
     */
    static core::Address CreateDeterministicAddress(uint32_t seed) {
        std::vector<uint8_t> data(20);
        for (size_t i = 0; i < 20; ++i) {
            data[i] = static_cast<uint8_t>((seed + i) % 256);
        }
        return core::Address(data);
    }

    /**
     * @brief Create a deterministic hash from seed
     */
    static core::Hash CreateDeterministicHash(uint32_t seed) {
        std::vector<uint8_t> data(32);
        for (size_t i = 0; i < 32; ++i) {
            data[i] = static_cast<uint8_t>((seed + i) % 256);
        }
        return core::Hash(data);
    }

    /**
     * @brief Generate random amount (in wei)
     */
    static uint64_t RandomAmount() {
        return 1000000 + (rand() % 1000000000);
    }

    /**
     * @brief Generate random data payload
     */
    static std::vector<uint8_t> RandomData(size_t size) {
        std::vector<uint8_t> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = static_cast<uint8_t>(rand() % 256);
        }
        return data;
    }

    /**
     * @brief Create a chain of blocks
     */
    static std::vector<core::Block> CreateBlockchain(
        size_t num_blocks,
        size_t transactions_per_block = 5
    ) {
        std::vector<core::Block> chain;
        
        // Genesis block
        chain.push_back(CreateGenesisBlock());
        
        // Add subsequent blocks
        for (size_t i = 1; i < num_blocks; ++i) {
            auto parent_hash = chain.back().calculate_hash();
            auto block = CreateBlockWithTransactions(
                static_cast<uint32_t>(i),
                parent_hash,
                transactions_per_block
            );
            chain.push_back(std::move(block));
        }
        
        return chain;
    }

    /**
     * @brief Compare two transactions for equality (excluding hash)
     */
    static bool TransactionsEqual(
        const core::Transaction& tx1,
        const core::Transaction& tx2
    ) {
        return tx1.from() == tx2.from() &&
               tx1.to() == tx2.to() &&
               tx1.value() == tx2.value() &&
               tx1.gas_limit() == tx2.gas_limit() &&
               tx1.gas_price() == tx2.gas_price() &&
               tx1.nonce() == tx2.nonce() &&
               tx1.payload() == tx2.payload();
    }

    /**
     * @brief Compare two blocks for equality (excluding hash)
     */
    static bool BlocksEqual(
        const core::Block& b1,
        const core::Block& b2
    ) {
        if (b1.height() != b2.height() ||
            b1.parent_hash() != b2.parent_hash() ||
            b1.timestamp() != b2.timestamp() ||
            b1.nonce() != b2.nonce() ||
            b1.gas_limit() != b2.gas_limit() ||
            b1.transaction_count() != b2.transaction_count()) {
            return false;
        }

        // Compare transactions
        for (size_t i = 0; i < b1.transaction_count(); ++i) {
            if (!TransactionsEqual(b1.transactions()[i], b2.transactions()[i])) {
                return false;
            }
        }

        return true;
    }
};

/**
 * @brief Custom GTest matchers for blockchain types
 */
namespace matchers {

// Matcher for testing if an amount is in a specific range
inline ::testing::AssertionResult AmountInRange(
    const core::Amount& amount,
    uint64_t min_wei,
    uint64_t max_wei
) {
    if (amount.wei() >= min_wei && amount.wei() <= max_wei) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure()
        << "Amount " << amount.wei() << " wei is not in range ["
        << min_wei << ", " << max_wei << "]";
}

// Matcher for testing if a timestamp is recent (within N seconds)
inline ::testing::AssertionResult TimestampIsRecent(
    const core::Timestamp& ts,
    uint64_t max_age_seconds = 60
) {
    auto now = core::Timestamp::now();
    auto age = now.seconds() - ts.seconds();
    
    if (age <= max_age_seconds) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure()
        << "Timestamp is " << age << " seconds old (max: "
        << max_age_seconds << ")";
}

// Matcher for testing if a hash is non-zero
inline ::testing::AssertionResult HashIsNonZero(const core::Hash& hash) {
    if (!hash.is_zero()) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure() << "Hash is zero";
}

// Matcher for testing if a transaction is valid
inline ::testing::AssertionResult TransactionIsValid(const core::Transaction& tx) {
    if (tx.is_valid()) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure() << "Transaction is invalid";
}

// Matcher for testing if a block is valid
inline ::testing::AssertionResult BlockIsValid(const core::Block& block) {
    if (block.is_valid()) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure() << "Block is invalid";
}

} // namespace matchers

} // namespace testing
} // namespace chainforge

