#include <gtest/gtest.h>
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/crypto/keypair.hpp"
#include <vector>

namespace chainforge::integration::test {

class BlockchainIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Generate test accounts
        auto miner_result = KeyPair::generate();
        auto user1_result = KeyPair::generate();
        auto user2_result = KeyPair::generate();
        
        EXPECT_TRUE(miner_result.has_value());
        EXPECT_TRUE(user1_result.has_value());
        EXPECT_TRUE(user2_result.has_value());
        
        miner = miner_result.value();
        user1 = user1_result.value();
        user2 = user2_result.value();
        
        // Create genesis block
        genesis_header = BlockHeader{
            .parent_hash = Hash::zero(),
            .merkle_root = Hash::zero(),
            .timestamp = Timestamp::now(),
            .nonce = 0,
            .difficulty = 1,
            .gas_limit = 8000000,
            .gas_used = 0,
            .miner = Address::from_public_key(miner.public_key()).value(),
            .extra_data = std::vector<uint8_t>{0x47, 0x65, 0x6e, 0x65, 0x73, 0x69, 0x73} // "Genesis"
        };
        
        genesis_block = Block(genesis_header);
    }
    
    void TearDown() override {}
    
    KeyPair miner, user1, user2;
    BlockHeader genesis_header;
    Block genesis_block;
};

TEST_F(BlockchainIntegrationTest, GenesisBlockCreation) {
    EXPECT_TRUE(genesis_block.transactions().empty());
    EXPECT_EQ(genesis_block.header().parent_hash, Hash::zero());
    EXPECT_EQ(genesis_block.header().nonce, 0);
    EXPECT_EQ(genesis_block.header().difficulty, 1);
    EXPECT_EQ(genesis_block.header().gas_used, 0);
}

TEST_F(BlockchainIntegrationTest, GenesisBlockValidation) {
    auto validation_result = genesis_block.validate();
    EXPECT_TRUE(validation_result.has_value());
}

TEST_F(BlockchainIntegrationTest, CreateAndMineBlock) {
    // Create transactions
    auto tx1_result = Transaction::create(
        Address::from_public_key(user1.public_key()).value(),
        Address::from_public_key(user2.public_key()).value(),
        Amount::from_ether(1.0),
        Amount(21000),
        Amount(20),
        std::vector<uint8_t>{}
    );
    EXPECT_TRUE(tx1_result.has_value());
    
    Transaction tx1 = tx1_result.value();
    tx1.sign(user1.private_key());
    
    auto tx2_result = Transaction::create(
        Address::from_public_key(user2.public_key()).value(),
        Address::from_public_key(user1.public_key()).value(),
        Amount::from_ether(0.5),
        Amount(21000),
        Amount(25),
        std::vector<uint8_t>{}
    );
    EXPECT_TRUE(tx2_result.has_value());
    
    Transaction tx2 = tx2_result.value();
    tx2.sign(user2.private_key());
    
    // Create new block
    BlockHeader new_header = {
        .parent_hash = genesis_block.calculate_hash(),
        .merkle_root = Hash::zero(), // Will be calculated
        .timestamp = Timestamp::now(),
        .nonce = 0,
        .difficulty = 1000000,
        .gas_limit = 8000000,
        .gas_used = 0, // Will be calculated
        .miner = Address::from_public_key(miner.public_key()).value(),
        .extra_data = std::vector<uint8_t>{}
    };
    
    Block new_block(new_header);
    new_block.add_transaction(tx1);
    new_block.add_transaction(tx2);
    
    // Calculate merkle root and gas used
    new_block.header().merkle_root = new_block.calculate_merkle_root();
    new_block.header().gas_used = new_block.calculate_total_gas();
    
    // Validate block
    auto validation_result = new_block.validate();
    EXPECT_TRUE(validation_result.has_value());
}

TEST_F(BlockchainIntegrationTest, BlockChainValidation) {
    std::vector<Block> blockchain;
    blockchain.push_back(genesis_block);
    
    // Create and add multiple blocks
    for (int i = 1; i <= 5; ++i) {
        BlockHeader header = {
            .parent_hash = blockchain.back().calculate_hash(),
            .merkle_root = Hash::zero(),
            .timestamp = Timestamp::now(),
            .nonce = i * 1000,
            .difficulty = 1000000 + (i * 100000),
            .gas_limit = 8000000,
            .gas_used = 0,
            .miner = Address::from_public_key(miner.public_key()).value(),
            .extra_data = std::vector<uint8_t>{}
        };
        
        Block block(header);
        
        // Add some transactions
        if (i % 2 == 0) {
            auto tx_result = Transaction::create(
                Address::from_public_key(user1.public_key()).value(),
                Address::from_public_key(user2.public_key()).value(),
                Amount::from_ether(0.1 * i),
                Amount(21000),
                Amount(20 + i),
                std::vector<uint8_t>{}
            );
            EXPECT_TRUE(tx_result.has_value());
            
            Transaction tx = tx_result.value();
            tx.sign(user1.private_key());
            block.add_transaction(tx);
        }
        
        block.header().merkle_root = block.calculate_merkle_root();
        block.header().gas_used = block.calculate_total_gas();
        
        blockchain.push_back(block);
    }
    
    // Validate entire blockchain
    for (size_t i = 0; i < blockchain.size(); ++i) {
        auto validation_result = blockchain[i].validate();
        EXPECT_TRUE(validation_result.has_value()) << "Block " << i << " validation failed";
        
        // Check parent hash linkage (except genesis)
        if (i > 0) {
            EXPECT_EQ(blockchain[i].header().parent_hash, blockchain[i-1].calculate_hash())
                << "Block " << i << " parent hash mismatch";
        }
    }
}

TEST_F(BlockchainIntegrationTest, TransactionPoolIntegration) {
    // Create multiple transactions
    std::vector<Transaction> transactions;
    
    for (int i = 0; i < 10; ++i) {
        auto tx_result = Transaction::create(
            Address::from_public_key(user1.public_key()).value(),
            Address::from_public_key(user2.public_key()).value(),
            Amount::from_ether(0.1),
            Amount(21000),
            Amount(20 + i), // Different gas prices
            std::vector<uint8_t>{static_cast<uint8_t>(i)}
        );
        EXPECT_TRUE(tx_result.has_value());
        
        Transaction tx = tx_result.value();
        tx.sign(user1.private_key());
        transactions.push_back(tx);
    }
    
    // Create block with transactions
    BlockHeader header = {
        .parent_hash = genesis_block.calculate_hash(),
        .merkle_root = Hash::zero(),
        .timestamp = Timestamp::now(),
        .nonce = 12345,
        .difficulty = 1000000,
        .gas_limit = 8000000,
        .gas_used = 0,
        .miner = Address::from_public_key(miner.public_key()).value(),
        .extra_data = std::vector<uint8_t>{}
    };
    
    Block block(header);
    
    // Add transactions to block
    for (const auto& tx : transactions) {
        EXPECT_TRUE(block.add_transaction(tx));
    }
    
    EXPECT_EQ(block.transactions().size(), 10);
    
    // Calculate and validate
    block.header().merkle_root = block.calculate_merkle_root();
    block.header().gas_used = block.calculate_total_gas();
    
    auto validation_result = block.validate();
    EXPECT_TRUE(validation_result.has_value());
}

TEST_F(BlockchainIntegrationTest, GasLimitEnforcement) {
    // Create transaction that exceeds gas limit
    auto tx_result = Transaction::create(
        Address::from_public_key(user1.public_key()).value(),
        Address::from_public_key(user2.public_key()).value(),
        Amount::from_ether(1.0),
        Amount(10000000), // Exceeds gas limit
        Amount(20),
        std::vector<uint8_t>{}
    );
    EXPECT_TRUE(tx_result.has_value());
    
    Transaction tx = tx_result.value();
    tx.sign(user1.private_key());
    
    // Create block with high gas limit
    BlockHeader header = {
        .parent_hash = genesis_block.calculate_hash(),
        .merkle_root = Hash::zero(),
        .timestamp = Timestamp::now(),
        .nonce = 12345,
        .difficulty = 1000000,
        .gas_limit = 15000000, // Higher limit
        .gas_used = 0,
        .miner = Address::from_public_key(miner.public_key()).value(),
        .extra_data = std::vector<uint8_t>{}
    };
    
    Block block(header);
    EXPECT_TRUE(block.add_transaction(tx));
    
    block.header().merkle_root = block.calculate_merkle_root();
    block.header().gas_used = block.calculate_total_gas();
    
    // Should be valid with higher gas limit
    auto validation_result = block.validate();
    EXPECT_TRUE(validation_result.has_value());
}

TEST_F(BlockchainIntegrationTest, DifficultyAdjustment) {
    std::vector<Block> blockchain;
    blockchain.push_back(genesis_block);
    
    uint64_t base_difficulty = 1000000;
    uint64_t target_block_time = 15; // 15 seconds
    
    // Simulate blocks with different mining times
    std::vector<uint64_t> mining_times = {10, 20, 5, 25, 12, 18, 8, 22};
    
    for (size_t i = 0; i < mining_times.size(); ++i) {
        uint64_t mining_time = mining_times[i];
        
        // Calculate difficulty adjustment
        uint64_t difficulty_adjustment = 1;
        if (mining_time < target_block_time) {
            difficulty_adjustment = target_block_time * 100 / mining_time;
        } else if (mining_time > target_block_time) {
            difficulty_adjustment = mining_time * 100 / target_block_time;
        }
        
        uint64_t new_difficulty = base_difficulty * difficulty_adjustment / 100;
        
        BlockHeader header = {
            .parent_hash = blockchain.back().calculate_hash(),
            .merkle_root = Hash::zero(),
            .timestamp = blockchain.back().header().timestamp + std::chrono::seconds(mining_time),
            .nonce = i * 1000,
            .difficulty = new_difficulty,
            .gas_limit = 8000000,
            .gas_used = 0,
            .miner = Address::from_public_key(miner.public_key()).value(),
            .extra_data = std::vector<uint8_t>{}
        };
        
        Block block(header);
        block.header().merkle_root = block.calculate_merkle_root();
        block.header().gas_used = block.calculate_total_gas();
        
        blockchain.push_back(block);
        
        // Update base difficulty for next block
        base_difficulty = new_difficulty;
    }
    
    // Validate all blocks
    for (size_t i = 0; i < blockchain.size(); ++i) {
        auto validation_result = blockchain[i].validate();
        EXPECT_TRUE(validation_result.has_value()) << "Block " << i << " validation failed";
    }
}

TEST_F(BlockchainIntegrationTest, ForkHandling) {
    // Create main chain
    std::vector<Block> main_chain;
    main_chain.push_back(genesis_block);
    
    // Create fork chain
    std::vector<Block> fork_chain;
    fork_chain.push_back(genesis_block);
    
    // Add blocks to main chain
    for (int i = 1; i <= 3; ++i) {
        BlockHeader header = {
            .parent_hash = main_chain.back().calculate_hash(),
            .merkle_root = Hash::zero(),
            .timestamp = Timestamp::now(),
            .nonce = i * 1000,
            .difficulty = 1000000,
            .gas_limit = 8000000,
            .gas_used = 0,
            .miner = Address::from_public_key(miner.public_key()).value(),
            .extra_data = std::vector<uint8_t>{}
        };
        
        Block block(header);
        block.header().merkle_root = block.calculate_merkle_root();
        block.header().gas_used = block.calculate_total_gas();
        
        main_chain.push_back(block);
    }
    
    // Create fork from second block
    BlockHeader fork_header = {
        .parent_hash = main_chain[1].calculate_hash(), // Fork from block 1
        .merkle_root = Hash::zero(),
        .timestamp = Timestamp::now(),
        .nonce = 9999,
        .difficulty = 1000000,
        .gas_limit = 8000000,
        .gas_used = 0,
        .miner = Address::from_public_key(user1.public_key()).value(), // Different miner
        .extra_data = std::vector<uint8_t>{0x46, 0x6f, 0x72, 0x6b} // "Fork"
    };
    
    Block fork_block(fork_header);
    fork_block.header().merkle_root = fork_block.calculate_merkle_root();
    fork_block.header().gas_used = fork_block.calculate_total_gas();
    
    fork_chain.push_back(fork_block);
    
    // Both chains should be valid
    for (const auto& block : main_chain) {
        auto validation_result = block.validate();
        EXPECT_TRUE(validation_result.has_value());
    }
    
    for (const auto& block : fork_chain) {
        auto validation_result = block.validate();
        EXPECT_TRUE(validation_result.has_value());
    }
    
    // Fork should have different hash
    EXPECT_NE(main_chain[2].calculate_hash(), fork_block.calculate_hash());
}

TEST_F(BlockchainIntegrationTest, LargeBlockHandling) {
    BlockHeader header = {
        .parent_hash = genesis_block.calculate_hash(),
        .merkle_root = Hash::zero(),
        .timestamp = Timestamp::now(),
        .nonce = 12345,
        .difficulty = 1000000,
        .gas_limit = 8000000,
        .gas_used = 0,
        .miner = Address::from_public_key(miner.public_key()).value(),
        .extra_data = std::vector<uint8_t>{}
    };
    
    Block block(header);
    
    // Add many small transactions
    for (int i = 0; i < 100; ++i) {
        auto tx_result = Transaction::create(
            Address::from_public_key(user1.public_key()).value(),
            Address::from_public_key(user2.public_key()).value(),
            Amount::from_ether(0.001),
            Amount(21000),
            Amount(20),
            std::vector<uint8_t>{static_cast<uint8_t>(i)}
        );
        EXPECT_TRUE(tx_result.has_value());
        
        Transaction tx = tx_result.value();
        tx.sign(user1.private_key());
        
        if (!block.add_transaction(tx)) {
            break; // Block is full
        }
    }
    
    EXPECT_GT(block.transactions().size(), 0);
    
    block.header().merkle_root = block.calculate_merkle_root();
    block.header().gas_used = block.calculate_total_gas();
    
    auto validation_result = block.validate();
    EXPECT_TRUE(validation_result.has_value());
}

} // namespace chainforge::integration::test
