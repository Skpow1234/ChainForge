#include <gtest/gtest.h>
#include "chainforge/core/transaction.hpp"
#include "chainforge/crypto/keypair.hpp"
#include <vector>
#include <random>
#include <set>

namespace chainforge::fuzz::test {

class TransactionFuzzTest : public ::testing::Test {
protected:
    void SetUp() override {
        rng.seed(std::random_device{}());
        
        // Generate test keypairs
        auto keypair1_result = KeyPair::generate();
        auto keypair2_result = KeyPair::generate();
        
        EXPECT_TRUE(keypair1_result.has_value());
        EXPECT_TRUE(keypair2_result.has_value());
        
        keypair1 = keypair1_result.value();
        keypair2 = keypair2_result.value();
    }
    
    void TearDown() override {}
    
    std::mt19937 rng;
    KeyPair keypair1, keypair2;
};

TEST_F(TransactionFuzzTest, RandomTransactionCreation) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint64_t> amount_dist(0, 1000000000000000000ULL);
    std::uniform_int_distribution<uint64_t> gas_dist(21000, 1000000);
    std::uniform_int_distribution<uint64_t> gas_price_dist(1, 1000);
    std::uniform_int_distribution<size_t> data_size_dist(0, 1024);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random transaction parameters
        Address from = Address::from_public_key(keypair1.public_key()).value();
        Address to = Address::from_public_key(keypair2.public_key()).value();
        Amount value = Amount(amount_dist(rng));
        Amount gas_limit = Amount(gas_dist(rng));
        Amount gas_price = Amount(gas_price_dist(rng));
        
        // Generate random data
        size_t data_size = data_size_dist(rng);
        std::vector<uint8_t> data(data_size);
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        // Create transaction
        auto result = Transaction::create(from, to, value, gas_limit, gas_price, data);
        EXPECT_TRUE(result.has_value());
        
        Transaction tx = result.value();
        
        // Verify transaction properties
        EXPECT_EQ(tx.from(), from);
        EXPECT_EQ(tx.to(), to);
        EXPECT_EQ(tx.value(), value);
        EXPECT_EQ(tx.gas_limit(), gas_limit);
        EXPECT_EQ(tx.gas_price(), gas_price);
        EXPECT_EQ(tx.data(), data);
    }
}

TEST_F(TransactionFuzzTest, RandomTransactionSigning) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint64_t> amount_dist(0, 1000000000000000000ULL);
    std::uniform_int_distribution<uint64_t> gas_dist(21000, 1000000);
    std::uniform_int_distribution<uint64_t> gas_price_dist(1, 1000);
    std::uniform_int_distribution<size_t> data_size_dist(0, 1024);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random transaction
        Address from = Address::from_public_key(keypair1.public_key()).value();
        Address to = Address::from_public_key(keypair2.public_key()).value();
        Amount value = Amount(amount_dist(rng));
        Amount gas_limit = Amount(gas_dist(rng));
        Amount gas_price = Amount(gas_price_dist(rng));
        
        size_t data_size = data_size_dist(rng);
        std::vector<uint8_t> data(data_size);
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto tx_result = Transaction::create(from, to, value, gas_limit, gas_price, data);
        EXPECT_TRUE(tx_result.has_value());
        
        Transaction tx = tx_result.value();
        
        // Sign transaction
        auto sign_result = tx.sign(keypair1.private_key());
        EXPECT_TRUE(sign_result.has_value());
        
        // Verify signature
        auto verify_result = tx.verify();
        EXPECT_TRUE(verify_result.has_value());
        EXPECT_TRUE(verify_result.value());
    }
}

TEST_F(TransactionFuzzTest, TransactionHashCollisionResistance) {
    const int num_iterations = 10000;
    std::uniform_int_distribution<uint64_t> amount_dist(0, 1000000000000000000ULL);
    std::uniform_int_distribution<uint64_t> gas_dist(21000, 1000000);
    std::uniform_int_distribution<uint64_t> gas_price_dist(1, 1000);
    std::uniform_int_distribution<size_t> data_size_dist(0, 256);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    std::set<Hash> seen_hashes;
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random transaction
        Address from = Address::from_public_key(keypair1.public_key()).value();
        Address to = Address::from_public_key(keypair2.public_key()).value();
        Amount value = Amount(amount_dist(rng));
        Amount gas_limit = Amount(gas_dist(rng));
        Amount gas_price = Amount(gas_price_dist(rng));
        
        size_t data_size = data_size_dist(rng);
        std::vector<uint8_t> data(data_size);
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto tx_result = Transaction::create(from, to, value, gas_limit, gas_price, data);
        EXPECT_TRUE(tx_result.has_value());
        
        Transaction tx = tx_result.value();
        Hash tx_hash = tx.hash();
        
        // Check for hash collisions
        EXPECT_EQ(seen_hashes.count(tx_hash), 0) << "Transaction hash collision detected at iteration " << i;
        seen_hashes.insert(tx_hash);
    }
}

TEST_F(TransactionFuzzTest, TransactionSerialization) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint64_t> amount_dist(0, 1000000000000000000ULL);
    std::uniform_int_distribution<uint64_t> gas_dist(21000, 1000000);
    std::uniform_int_distribution<uint64_t> gas_price_dist(1, 1000);
    std::uniform_int_distribution<size_t> data_size_dist(0, 1024);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random transaction
        Address from = Address::from_public_key(keypair1.public_key()).value();
        Address to = Address::from_public_key(keypair2.public_key()).value();
        Amount value = Amount(amount_dist(rng));
        Amount gas_limit = Amount(gas_dist(rng));
        Amount gas_price = Amount(gas_price_dist(rng));
        
        size_t data_size = data_size_dist(rng);
        std::vector<uint8_t> data(data_size);
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto tx_result = Transaction::create(from, to, value, gas_limit, gas_price, data);
        EXPECT_TRUE(tx_result.has_value());
        
        Transaction original_tx = tx_result.value();
        original_tx.sign(keypair1.private_key());
        
        // Serialize transaction
        std::vector<uint8_t> serialized = original_tx.serialize();
        EXPECT_FALSE(serialized.empty());
        
        // Deserialize transaction
        auto deserialized_result = Transaction::deserialize(serialized);
        EXPECT_TRUE(deserialized_result.has_value());
        
        Transaction deserialized_tx = deserialized_result.value();
        
        // Verify round-trip
        EXPECT_EQ(deserialized_tx.from(), original_tx.from());
        EXPECT_EQ(deserialized_tx.to(), original_tx.to());
        EXPECT_EQ(deserialized_tx.value(), original_tx.value());
        EXPECT_EQ(deserialized_tx.gas_limit(), original_tx.gas_limit());
        EXPECT_EQ(deserialized_tx.gas_price(), original_tx.gas_price());
        EXPECT_EQ(deserialized_tx.data(), original_tx.data());
        EXPECT_EQ(deserialized_tx.signature(), original_tx.signature());
    }
}

TEST_F(TransactionFuzzTest, TransactionValidation) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint64_t> amount_dist(0, 1000000000000000000ULL);
    std::uniform_int_distribution<uint64_t> gas_dist(21000, 1000000);
    std::uniform_int_distribution<uint64_t> gas_price_dist(1, 1000);
    std::uniform_int_distribution<size_t> data_size_dist(0, 1024);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random transaction
        Address from = Address::from_public_key(keypair1.public_key()).value();
        Address to = Address::from_public_key(keypair2.public_key()).value();
        Amount value = Amount(amount_dist(rng));
        Amount gas_limit = Amount(gas_dist(rng));
        Amount gas_price = Amount(gas_price_dist(rng));
        
        size_t data_size = data_size_dist(rng);
        std::vector<uint8_t> data(data_size);
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto tx_result = Transaction::create(from, to, value, gas_limit, gas_price, data);
        EXPECT_TRUE(tx_result.has_value());
        
        Transaction tx = tx_result.value();
        
        // Validate unsigned transaction
        auto validation_result = tx.validate();
        EXPECT_TRUE(validation_result.has_value());
        
        // Sign and validate again
        tx.sign(keypair1.private_key());
        validation_result = tx.validate();
        EXPECT_TRUE(validation_result.has_value());
    }
}

TEST_F(TransactionFuzzTest, TransactionGasCalculation) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint64_t> amount_dist(0, 1000000000000000000ULL);
    std::uniform_int_distribution<uint64_t> gas_dist(21000, 1000000);
    std::uniform_int_distribution<uint64_t> gas_price_dist(1, 1000);
    std::uniform_int_distribution<size_t> data_size_dist(0, 1024);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random transaction
        Address from = Address::from_public_key(keypair1.public_key()).value();
        Address to = Address::from_public_key(keypair2.public_key()).value();
        Amount value = Amount(amount_dist(rng));
        Amount gas_limit = Amount(gas_dist(rng));
        Amount gas_price = Amount(gas_price_dist(rng));
        
        size_t data_size = data_size_dist(rng);
        std::vector<uint8_t> data(data_size);
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto tx_result = Transaction::create(from, to, value, gas_limit, gas_price, data);
        EXPECT_TRUE(tx_result.has_value());
        
        Transaction tx = tx_result.value();
        
        // Calculate gas cost
        Amount gas_cost = tx.calculate_gas_cost();
        EXPECT_EQ(gas_cost, gas_limit * gas_price);
        
        // Calculate total cost
        Amount total_cost = tx.calculate_total_cost();
        EXPECT_EQ(total_cost, value + gas_cost);
    }
}

TEST_F(TransactionFuzzTest, TransactionEdgeCases) {
    // Test with zero values
    Address from = Address::from_public_key(keypair1.public_key()).value();
    Address to = Address::from_public_key(keypair2.public_key()).value();
    
    auto zero_tx_result = Transaction::create(from, to, Amount(0), Amount(21000), Amount(1), std::vector<uint8_t>{});
    EXPECT_TRUE(zero_tx_result.has_value());
    
    Transaction zero_tx = zero_tx_result.value();
    EXPECT_TRUE(zero_tx.sign(keypair1.private_key()).has_value());
    EXPECT_TRUE(zero_tx.verify().has_value());
    
    // Test with maximum values
    auto max_tx_result = Transaction::create(
        from, to, 
        Amount(std::numeric_limits<uint64_t>::max()),
        Amount(std::numeric_limits<uint64_t>::max()),
        Amount(std::numeric_limits<uint64_t>::max()),
        std::vector<uint8_t>(1024, 0xFF)
    );
    EXPECT_TRUE(max_tx_result.has_value());
    
    Transaction max_tx = max_tx_result.value();
    EXPECT_TRUE(max_tx.sign(keypair1.private_key()).has_value());
    EXPECT_TRUE(max_tx.verify().has_value());
    
    // Test with empty data
    auto empty_data_tx_result = Transaction::create(from, to, Amount(1000), Amount(21000), Amount(20), std::vector<uint8_t>{});
    EXPECT_TRUE(empty_data_tx_result.has_value());
    
    Transaction empty_data_tx = empty_data_tx_result.value();
    EXPECT_TRUE(empty_data_tx.sign(keypair1.private_key()).has_value());
    EXPECT_TRUE(empty_data_tx.verify().has_value());
}

TEST_F(TransactionFuzzTest, TransactionPerformance) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint64_t> amount_dist(0, 1000000000000000000ULL);
    std::uniform_int_distribution<uint64_t> gas_dist(21000, 1000000);
    std::uniform_int_distribution<uint64_t> gas_price_dist(1, 1000);
    std::uniform_int_distribution<size_t> data_size_dist(0, 1024);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random transaction
        Address from = Address::from_public_key(keypair1.public_key()).value();
        Address to = Address::from_public_key(keypair2.public_key()).value();
        Amount value = Amount(amount_dist(rng));
        Amount gas_limit = Amount(gas_dist(rng));
        Amount gas_price = Amount(gas_price_dist(rng));
        
        size_t data_size = data_size_dist(rng);
        std::vector<uint8_t> data(data_size);
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto tx_result = Transaction::create(from, to, value, gas_limit, gas_price, data);
        EXPECT_TRUE(tx_result.has_value());
        
        Transaction tx = tx_result.value();
        
        // Perform various operations
        Hash hash = tx.hash();
        Amount gas_cost = tx.calculate_gas_cost();
        Amount total_cost = tx.calculate_total_cost();
        size_t size = tx.size();
        
        // Use variables to prevent optimization
        (void)hash;
        (void)gas_cost;
        (void)total_cost;
        (void)size;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Performance should be reasonable (less than 1 second for 1k iterations)
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(TransactionFuzzTest, TransactionThreadSafety) {
    const int num_threads = 4;
    const int iterations_per_thread = 250;
    
    std::vector<std::thread> threads;
    std::vector<bool> results(num_threads, true);
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&results, t, iterations_per_thread, this]() {
            std::mt19937 local_rng(t);
            std::uniform_int_distribution<uint64_t> amount_dist(0, 1000000000000000000ULL);
            std::uniform_int_distribution<uint64_t> gas_dist(21000, 1000000);
            std::uniform_int_distribution<uint64_t> gas_price_dist(1, 1000);
            std::uniform_int_distribution<size_t> data_size_dist(0, 1024);
            std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
            
            try {
                for (int i = 0; i < iterations_per_thread; ++i) {
                    // Generate random transaction
                    Address from = Address::from_public_key(keypair1.public_key()).value();
                    Address to = Address::from_public_key(keypair2.public_key()).value();
                    Amount value = Amount(amount_dist(local_rng));
                    Amount gas_limit = Amount(gas_dist(local_rng));
                    Amount gas_price = Amount(gas_price_dist(local_rng));
                    
                    size_t data_size = data_size_dist(local_rng);
                    std::vector<uint8_t> data(data_size);
                    for (size_t j = 0; j < data_size; ++j) {
                        data[j] = byte_dist(local_rng);
                    }
                    
                    auto tx_result = Transaction::create(from, to, value, gas_limit, gas_price, data);
                    if (!tx_result.has_value()) {
                        results[t] = false;
                        return;
                    }
                    
                    Transaction tx = tx_result.value();
                    
                    // Sign and verify
                    if (!tx.sign(keypair1.private_key()).has_value()) {
                        results[t] = false;
                        return;
                    }
                    
                    auto verify_result = tx.verify();
                    if (!verify_result.has_value() || !verify_result.value()) {
                        results[t] = false;
                        return;
                    }
                }
            } catch (...) {
                results[t] = false;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    for (bool result : results) {
        EXPECT_TRUE(result);
    }
}

} // namespace chainforge::fuzz::test
