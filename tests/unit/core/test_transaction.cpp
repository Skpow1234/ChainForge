#include <gtest/gtest.h>
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include <vector>

namespace chainforge::core::test {

class TransactionTest : public ::testing::Test {
protected:
    void SetUp() override {
        from_addr = Address::random();
        to_addr = Address::random();
        value = Amount(1000);
        gas_limit = Amount(21000);
        gas_price = Amount(20);
        data = std::vector<uint8_t>{0x60, 0x60, 0x60};
    }
    
    void TearDown() override {}
    
    Address from_addr, to_addr;
    Amount value, gas_limit, gas_price;
    std::vector<uint8_t> data;
};

TEST_F(TransactionTest, CreateTransaction) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    EXPECT_EQ(tx.from(), from_addr);
    EXPECT_EQ(tx.to(), to_addr);
    EXPECT_EQ(tx.value(), value);
    EXPECT_EQ(tx.gas_limit(), gas_limit);
    EXPECT_EQ(tx.gas_price(), gas_price);
    EXPECT_EQ(tx.data(), data);
}

TEST_F(TransactionTest, CreateTransactionWithEmptyData) {
    std::vector<uint8_t> empty_data;
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, empty_data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    EXPECT_TRUE(tx.data().empty());
}

TEST_F(TransactionTest, CreateTransactionWithInvalidGasLimit) {
    Amount invalid_gas_limit(0);
    auto result = Transaction::create(from_addr, to_addr, value, invalid_gas_limit, gas_price, data);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::INVALID_GAS_LIMIT);
}

TEST_F(TransactionTest, CreateTransactionWithInvalidGasPrice) {
    Amount invalid_gas_price(0);
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, invalid_gas_price, data);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::INVALID_GAS_PRICE);
}

TEST_F(TransactionTest, TransactionHash) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    Hash hash = tx.hash();
    EXPECT_FALSE(hash.is_zero());
    
    // Hash should be consistent
    Hash hash2 = tx.hash();
    EXPECT_EQ(hash, hash2);
}

TEST_F(TransactionTest, DifferentTransactionsDifferentHashes) {
    auto result1 = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    auto result2 = Transaction::create(from_addr, to_addr, Amount(2000), gas_limit, gas_price, data);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    EXPECT_NE(result1.value().hash(), result2.value().hash());
}

TEST_F(TransactionTest, TransactionSignature) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    
    // Initially unsigned
    EXPECT_FALSE(tx.is_signed());
    EXPECT_TRUE(tx.signature().empty());
    
    // Sign transaction
    auto sign_result = tx.sign(from_addr);
    EXPECT_TRUE(sign_result.has_value());
    EXPECT_TRUE(tx.is_signed());
    EXPECT_FALSE(tx.signature().empty());
}

TEST_F(TransactionTest, TransactionVerification) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    
    // Sign transaction
    auto sign_result = tx.sign(from_addr);
    EXPECT_TRUE(sign_result.has_value());
    
    // Verify signature
    auto verify_result = tx.verify();
    EXPECT_TRUE(verify_result.has_value());
    EXPECT_TRUE(verify_result.value());
}

TEST_F(TransactionTest, TransactionVerificationWithWrongSigner) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    
    // Sign with wrong address
    Address wrong_addr = Address::random();
    auto sign_result = tx.sign(wrong_addr);
    EXPECT_TRUE(sign_result.has_value());
    
    // Verification should fail
    auto verify_result = tx.verify();
    EXPECT_TRUE(verify_result.has_value());
    EXPECT_FALSE(verify_result.value());
}

TEST_F(TransactionTest, TransactionVerificationWithoutSignature) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    
    // Verify unsigned transaction
    auto verify_result = tx.verify();
    EXPECT_TRUE(verify_result.has_value());
    EXPECT_FALSE(verify_result.value());
}

TEST_F(TransactionTest, GasCostCalculation) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    
    Amount gas_cost = tx.calculate_gas_cost();
    EXPECT_EQ(gas_cost, gas_limit * gas_price);
}

TEST_F(TransactionTest, TotalCostCalculation) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    
    Amount total_cost = tx.calculate_total_cost();
    Amount expected_cost = value + (gas_limit * gas_price);
    EXPECT_EQ(total_cost, expected_cost);
}

TEST_F(TransactionTest, SerializeAndDeserialize) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction original_tx = result.value();
    original_tx.sign(from_addr);
    
    std::vector<uint8_t> serialized = original_tx.serialize();
    EXPECT_FALSE(serialized.empty());
    
    auto deserialized_result = Transaction::deserialize(serialized);
    EXPECT_TRUE(deserialized_result.has_value());
    
    Transaction deserialized_tx = deserialized_result.value();
    EXPECT_EQ(deserialized_tx.from(), original_tx.from());
    EXPECT_EQ(deserialized_tx.to(), original_tx.to());
    EXPECT_EQ(deserialized_tx.value(), original_tx.value());
    EXPECT_EQ(deserialized_tx.gas_limit(), original_tx.gas_limit());
    EXPECT_EQ(deserialized_tx.gas_price(), original_tx.gas_price());
    EXPECT_EQ(deserialized_tx.data(), original_tx.data());
    EXPECT_EQ(deserialized_tx.signature(), original_tx.signature());
}

TEST_F(TransactionTest, DeserializeInvalidData) {
    std::vector<uint8_t> invalid_data = {0x01, 0x02, 0x03};
    
    auto result = Transaction::deserialize(invalid_data);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::DESERIALIZATION_ERROR);
}

TEST_F(TransactionTest, SizeCalculation) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    size_t size = tx.size();
    EXPECT_GT(size, 0);
    
    // Size should include all fields
    EXPECT_GT(size, sizeof(Address) * 2); // from + to
    EXPECT_GT(size, sizeof(Amount) * 3);  // value + gas_limit + gas_price
    EXPECT_GE(size, data.size());         // data
}

TEST_F(TransactionTest, TransactionType) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    EXPECT_EQ(tx.type(), TransactionType::LEGACY);
}

TEST_F(TransactionTest, CreateEIP1559Transaction) {
    Amount max_fee_per_gas(30);
    Amount max_priority_fee_per_gas(2);
    
    auto result = Transaction::create_eip1559(
        from_addr, to_addr, value, gas_limit, 
        max_fee_per_gas, max_priority_fee_per_gas, data
    );
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    EXPECT_EQ(tx.type(), TransactionType::EIP1559);
    EXPECT_EQ(tx.max_fee_per_gas(), max_fee_per_gas);
    EXPECT_EQ(tx.max_priority_fee_per_gas(), max_priority_fee_per_gas);
}

TEST_F(TransactionTest, EIP1559GasCostCalculation) {
    Amount max_fee_per_gas(30);
    Amount max_priority_fee_per_gas(2);
    
    auto result = Transaction::create_eip1559(
        from_addr, to_addr, value, gas_limit, 
        max_fee_per_gas, max_priority_fee_per_gas, data
    );
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    Amount gas_cost = tx.calculate_gas_cost();
    EXPECT_EQ(gas_cost, gas_limit * max_fee_per_gas);
}

TEST_F(TransactionTest, CopyAndMove) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction original_tx = result.value();
    original_tx.sign(from_addr);
    
    Transaction copy = original_tx;
    Transaction moved = std::move(original_tx);
    
    EXPECT_EQ(copy.from(), moved.from());
    EXPECT_EQ(copy.to(), moved.to());
    EXPECT_EQ(copy.value(), moved.value());
    EXPECT_EQ(copy.signature(), moved.signature());
    EXPECT_TRUE(original_tx.signature().empty()); // Moved-from state
}

TEST_F(TransactionTest, Equality) {
    auto result1 = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    auto result2 = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    Transaction tx1 = result1.value();
    Transaction tx2 = result2.value();
    
    EXPECT_EQ(tx1, tx2);
    
    // Sign one transaction
    tx1.sign(from_addr);
    EXPECT_NE(tx1, tx2);
}

TEST_F(TransactionTest, Hash) {
    auto result1 = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    auto result2 = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    
    Transaction tx1 = result1.value();
    Transaction tx2 = result2.value();
    
    EXPECT_EQ(std::hash<Transaction>{}(tx1), std::hash<Transaction>{}(tx2));
    
    // Sign one transaction
    tx1.sign(from_addr);
    EXPECT_NE(std::hash<Transaction>{}(tx1), std::hash<Transaction>{}(tx2));
}

TEST_F(TransactionTest, TransactionValidation) {
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, data);
    EXPECT_TRUE(result.has_value());
    
    Transaction tx = result.value();
    
    // Validate unsigned transaction
    auto validation_result = tx.validate();
    EXPECT_TRUE(validation_result.has_value());
    
    // Sign and validate
    tx.sign(from_addr);
    validation_result = tx.validate();
    EXPECT_TRUE(validation_result.has_value());
}

TEST_F(TransactionTest, TransactionValidationWithInvalidData) {
    // Create transaction with excessive data size
    std::vector<uint8_t> large_data(1024 * 1024, 0x01); // 1MB
    
    auto result = Transaction::create(from_addr, to_addr, value, gas_limit, gas_price, large_data);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::TRANSACTION_TOO_LARGE);
}

} // namespace chainforge::core::test
