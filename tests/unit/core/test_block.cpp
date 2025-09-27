#include <gtest/gtest.h>
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/hash.hpp"
#include <vector>

namespace chainforge::core::test {

class BlockTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test transactions
        tx1 = Transaction::create(
            Address::random(),
            Address::random(),
            Amount(1000),
            Amount(21000), // gas limit
            Amount(20),    // gas price
            std::vector<uint8_t>{0x60, 0x60, 0x60} // data
        ).value();
        
        tx2 = Transaction::create(
            Address::random(),
            Address::random(),
            Amount(2000),
            Amount(21000),
            Amount(25),
            std::vector<uint8_t>{0x61, 0x61, 0x61}
        ).value();
        
        // Create test block header
        header = BlockHeader{
            .parent_hash = Hash::random(),
            .merkle_root = Hash::random(),
            .timestamp = Timestamp::now(),
            .nonce = 12345,
            .difficulty = 1000000,
            .gas_limit = 8000000,
            .gas_used = 21000,
            .miner = Address::random(),
            .extra_data = std::vector<uint8_t>{0x01, 0x02, 0x03}
        };
    }
    
    void TearDown() override {}
    
    Transaction tx1, tx2;
    BlockHeader header;
};

TEST_F(BlockTest, DefaultConstructor) {
    Block block;
    EXPECT_TRUE(block.transactions().empty());
    EXPECT_TRUE(block.header().parent_hash.is_zero());
    EXPECT_EQ(block.header().nonce, 0);
}

TEST_F(BlockTest, ConstructorWithHeader) {
    Block block(header);
    EXPECT_EQ(block.header().parent_hash, header.parent_hash);
    EXPECT_EQ(block.header().nonce, header.nonce);
    EXPECT_EQ(block.header().difficulty, header.difficulty);
    EXPECT_TRUE(block.transactions().empty());
}

TEST_F(BlockTest, ConstructorWithHeaderAndTransactions) {
    std::vector<Transaction> txs = {tx1, tx2};
    Block block(header, txs);
    
    EXPECT_EQ(block.transactions().size(), 2);
    EXPECT_EQ(block.transactions()[0], tx1);
    EXPECT_EQ(block.transactions()[1], tx2);
}

TEST_F(BlockTest, AddTransaction) {
    Block block(header);
    
    EXPECT_TRUE(block.add_transaction(tx1));
    EXPECT_EQ(block.transactions().size(), 1);
    EXPECT_EQ(block.transactions()[0], tx1);
    
    EXPECT_TRUE(block.add_transaction(tx2));
    EXPECT_EQ(block.transactions().size(), 2);
}

TEST_F(BlockTest, AddDuplicateTransaction) {
    Block block(header);
    
    EXPECT_TRUE(block.add_transaction(tx1));
    EXPECT_FALSE(block.add_transaction(tx1)); // Should fail
    EXPECT_EQ(block.transactions().size(), 1);
}

TEST_F(BlockTest, RemoveTransaction) {
    Block block(header);
    block.add_transaction(tx1);
    block.add_transaction(tx2);
    
    EXPECT_TRUE(block.remove_transaction(tx1.hash()));
    EXPECT_EQ(block.transactions().size(), 1);
    EXPECT_EQ(block.transactions()[0], tx2);
    
    EXPECT_FALSE(block.remove_transaction(tx1.hash())); // Already removed
}

TEST_F(BlockTest, CalculateMerkleRoot) {
    Block block(header);
    block.add_transaction(tx1);
    block.add_transaction(tx2);
    
    Hash merkle_root = block.calculate_merkle_root();
    EXPECT_FALSE(merkle_root.is_zero());
    
    // Merkle root should be consistent
    Hash merkle_root2 = block.calculate_merkle_root();
    EXPECT_EQ(merkle_root, merkle_root2);
}

TEST_F(BlockTest, EmptyMerkleRoot) {
    Block block(header);
    Hash merkle_root = block.calculate_merkle_root();
    
    // Empty block should have zero merkle root
    EXPECT_TRUE(merkle_root.is_zero());
}

TEST_F(BlockTest, SingleTransactionMerkleRoot) {
    Block block(header);
    block.add_transaction(tx1);
    
    Hash merkle_root = block.calculate_merkle_root();
    EXPECT_EQ(merkle_root, tx1.hash());
}

TEST_F(BlockTest, CalculateHash) {
    Block block(header);
    block.add_transaction(tx1);
    block.add_transaction(tx2);
    
    Hash block_hash = block.calculate_hash();
    EXPECT_FALSE(block_hash.is_zero());
    
    // Hash should be consistent
    Hash block_hash2 = block.calculate_hash();
    EXPECT_EQ(block_hash, block_hash2);
}

TEST_F(BlockTest, HashChangesWithNonce) {
    Block block1(header);
    Block block2(header);
    
    block1.add_transaction(tx1);
    block2.add_transaction(tx1);
    
    // Change nonce
    block2.header().nonce = 54321;
    
    Hash hash1 = block1.calculate_hash();
    Hash hash2 = block2.calculate_hash();
    
    EXPECT_NE(hash1, hash2);
}

TEST_F(BlockTest, ValidateBlock) {
    Block block(header);
    block.add_transaction(tx1);
    block.add_transaction(tx2);
    
    // Update merkle root in header
    block.header().merkle_root = block.calculate_merkle_root();
    
    auto validation_result = block.validate();
    EXPECT_TRUE(validation_result.has_value());
}

TEST_F(BlockTest, ValidateBlockWithInvalidMerkleRoot) {
    Block block(header);
    block.add_transaction(tx1);
    
    // Set incorrect merkle root
    block.header().merkle_root = Hash::random();
    
    auto validation_result = block.validate();
    EXPECT_FALSE(validation_result.has_value());
    EXPECT_EQ(validation_result.error().code, ErrorCode::INVALID_MERKLE_ROOT);
}

TEST_F(BlockTest, ValidateBlockWithInvalidTimestamp) {
    Block block(header);
    block.add_transaction(tx1);
    
    // Set future timestamp
    block.header().timestamp = Timestamp::now() + std::chrono::hours(1);
    
    auto validation_result = block.validate();
    EXPECT_FALSE(validation_result.has_value());
    EXPECT_EQ(validation_result.error().code, ErrorCode::INVALID_TIMESTAMP);
}

TEST_F(BlockTest, ValidateBlockWithInvalidGasUsed) {
    Block block(header);
    block.add_transaction(tx1);
    
    // Set gas used > gas limit
    block.header().gas_used = block.header().gas_limit + 1;
    
    auto validation_result = block.validate();
    EXPECT_FALSE(validation_result.has_value());
    EXPECT_EQ(validation_result.error().code, ErrorCode::INVALID_GAS_USAGE);
}

TEST_F(BlockTest, SerializeAndDeserialize) {
    Block original_block(header);
    original_block.add_transaction(tx1);
    original_block.add_transaction(tx2);
    
    std::vector<uint8_t> serialized = original_block.serialize();
    EXPECT_FALSE(serialized.empty());
    
    auto deserialized_result = Block::deserialize(serialized);
    EXPECT_TRUE(deserialized_result.has_value());
    
    Block deserialized_block = deserialized_result.value();
    EXPECT_EQ(deserialized_block.header().parent_hash, original_block.header().parent_hash);
    EXPECT_EQ(deserialized_block.header().nonce, original_block.header().nonce);
    EXPECT_EQ(deserialized_block.transactions().size(), original_block.transactions().size());
}

TEST_F(BlockTest, DeserializeInvalidData) {
    std::vector<uint8_t> invalid_data = {0x01, 0x02, 0x03};
    
    auto result = Block::deserialize(invalid_data);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::DESERIALIZATION_ERROR);
}

TEST_F(BlockTest, SizeCalculation) {
    Block block(header);
    block.add_transaction(tx1);
    block.add_transaction(tx2);
    
    size_t size = block.size();
    EXPECT_GT(size, 0);
    
    // Size should include header + transactions
    EXPECT_GT(size, sizeof(BlockHeader));
    EXPECT_GT(size, tx1.size() + tx2.size());
}

TEST_F(BlockTest, GasCalculation) {
    Block block(header);
    block.add_transaction(tx1);
    block.add_transaction(tx2);
    
    uint64_t total_gas = block.calculate_total_gas();
    EXPECT_EQ(total_gas, tx1.gas_limit() + tx2.gas_limit());
}

TEST_F(BlockTest, CopyAndMove) {
    Block original_block(header);
    original_block.add_transaction(tx1);
    
    Block copy = original_block;
    Block moved = std::move(original_block);
    
    EXPECT_EQ(copy.header().nonce, moved.header().nonce);
    EXPECT_EQ(copy.transactions().size(), moved.transactions().size());
    EXPECT_TRUE(original_block.transactions().empty()); // Moved-from state
}

TEST_F(BlockTest, Equality) {
    Block block1(header);
    Block block2(header);
    
    block1.add_transaction(tx1);
    block2.add_transaction(tx1);
    
    EXPECT_EQ(block1, block2);
    
    block2.add_transaction(tx2);
    EXPECT_NE(block1, block2);
}

TEST_F(BlockTest, Hash) {
    Block block1(header);
    Block block2(header);
    
    block1.add_transaction(tx1);
    block2.add_transaction(tx1);
    
    EXPECT_EQ(std::hash<Block>{}(block1), std::hash<Block>{}(block2));
    
    block2.add_transaction(tx2);
    EXPECT_NE(std::hash<Block>{}(block1), std::hash<Block>{}(block2));
}

} // namespace chainforge::core::test
