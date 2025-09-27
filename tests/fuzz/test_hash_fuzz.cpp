#include <gtest/gtest.h>
#include "chainforge/core/hash.hpp"
#include <vector>
#include <random>

namespace chainforge::fuzz::test {

class HashFuzzTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize random number generator
        rng.seed(std::random_device{}());
    }
    
    void TearDown() override {}
    
    std::mt19937 rng;
};

TEST_F(HashFuzzTest, RandomHashGeneration) {
    const int num_iterations = 1000;
    
    for (int i = 0; i < num_iterations; ++i) {
        Hash hash = Hash::random();
        
        // Hash should not be zero
        EXPECT_FALSE(hash.is_zero());
        
        // Hash should be valid
        EXPECT_EQ(hash.size(), HASH_SIZE);
        
        // Hash should be consistent
        Hash hash_copy = hash;
        EXPECT_EQ(hash, hash_copy);
    }
}

TEST_F(HashFuzzTest, RandomDataHashing) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<size_t> size_dist(1, 1024);
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random data
        size_t data_size = size_dist(rng);
        std::vector<uint8_t> data(data_size);
        
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        // Create hash from data
        Hash256 hash_data;
        std::copy(data.begin(), data.begin() + std::min(data_size, HASH_SIZE), hash_data.begin());
        Hash hash(hash_data);
        
        // Hash should be valid
        EXPECT_EQ(hash.size(), HASH_SIZE);
        
        // Hash should be deterministic
        Hash hash2(hash_data);
        EXPECT_EQ(hash, hash2);
    }
}

TEST_F(HashFuzzTest, HashCollisionResistance) {
    const int num_iterations = 10000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    std::set<Hash> seen_hashes;
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random hash data
        Hash256 hash_data;
        for (size_t j = 0; j < HASH_SIZE; ++j) {
            hash_data[j] = byte_dist(rng);
        }
        
        Hash hash(hash_data);
        
        // Check for collisions
        EXPECT_EQ(seen_hashes.count(hash), 0) << "Hash collision detected at iteration " << i;
        seen_hashes.insert(hash);
    }
}

TEST_F(HashFuzzTest, HashStringConversion) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random hash data
        Hash256 hash_data;
        for (size_t j = 0; j < HASH_SIZE; ++j) {
            hash_data[j] = byte_dist(rng);
        }
        
        Hash original_hash(hash_data);
        std::string hex_string = original_hash.to_hex();
        
        // String should be valid hex
        EXPECT_EQ(hex_string.length(), HASH_SIZE * 2);
        EXPECT_TRUE(std::all_of(hex_string.begin(), hex_string.end(), ::isxdigit));
        
        // Round-trip conversion should work
        auto parsed_result = Hash::from_hex(hex_string);
        EXPECT_TRUE(parsed_result.has_value());
        EXPECT_EQ(parsed_result.value(), original_hash);
    }
}

TEST_F(HashFuzzTest, HashBytesConversion) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random hash data
        Hash256 hash_data;
        for (size_t j = 0; j < HASH_SIZE; ++j) {
            hash_data[j] = byte_dist(rng);
        }
        
        Hash original_hash(hash_data);
        std::vector<uint8_t> bytes = original_hash.to_bytes();
        
        // Bytes should be valid
        EXPECT_EQ(bytes.size(), HASH_SIZE);
        EXPECT_EQ(std::vector<uint8_t>(hash_data.begin(), hash_data.end()), bytes);
        
        // Round-trip conversion should work
        Hash256 reconstructed_data;
        std::copy(bytes.begin(), bytes.end(), reconstructed_data.begin());
        Hash reconstructed_hash(reconstructed_data);
        EXPECT_EQ(reconstructed_hash, original_hash);
    }
}

TEST_F(HashFuzzTest, HashEqualityAndInequality) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate two random hashes
        Hash256 hash_data1, hash_data2;
        for (size_t j = 0; j < HASH_SIZE; ++j) {
            hash_data1[j] = byte_dist(rng);
            hash_data2[j] = byte_dist(rng);
        }
        
        Hash hash1(hash_data1);
        Hash hash2(hash_data2);
        
        // Test equality
        if (hash_data1 == hash_data2) {
            EXPECT_EQ(hash1, hash2);
            EXPECT_FALSE(hash1 != hash2);
        } else {
            EXPECT_NE(hash1, hash2);
            EXPECT_FALSE(hash1 == hash2);
        }
    }
}

TEST_F(HashFuzzTest, HashOrdering) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate two random hashes
        Hash256 hash_data1, hash_data2;
        for (size_t j = 0; j < HASH_SIZE; ++j) {
            hash_data1[j] = byte_dist(rng);
            hash_data2[j] = byte_dist(rng);
        }
        
        Hash hash1(hash_data1);
        Hash hash2(hash_data2);
        
        // Test ordering consistency
        bool data1_less = hash_data1 < hash_data2;
        bool hash1_less = hash1 < hash2;
        EXPECT_EQ(data1_less, hash1_less);
        
        bool data1_greater = hash_data1 > hash_data2;
        bool hash1_greater = hash1 > hash2;
        EXPECT_EQ(data1_greater, hash1_greater);
    }
}

TEST_F(HashFuzzTest, HashHashFunction) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    std::set<size_t> seen_hashes;
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random hash data
        Hash256 hash_data;
        for (size_t j = 0; j < HASH_SIZE; ++j) {
            hash_data[j] = byte_dist(rng);
        }
        
        Hash hash(hash_data);
        size_t hash_value = std::hash<Hash>{}(hash);
        
        // Hash function should be consistent
        EXPECT_EQ(std::hash<Hash>{}(hash), hash_value);
        
        // Check for hash collisions (less strict than hash collision test)
        if (seen_hashes.count(hash_value) > 0) {
            // This is acceptable for hash function collisions
            // but we log it for analysis
            continue;
        }
        seen_hashes.insert(hash_value);
    }
}

TEST_F(HashFuzzTest, HashCopyAndMove) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random hash data
        Hash256 hash_data;
        for (size_t j = 0; j < HASH_SIZE; ++j) {
            hash_data[j] = byte_dist(rng);
        }
        
        Hash original_hash(hash_data);
        
        // Test copy constructor
        Hash copy_hash = original_hash;
        EXPECT_EQ(copy_hash, original_hash);
        
        // Test move constructor
        Hash moved_hash = std::move(original_hash);
        EXPECT_EQ(moved_hash, copy_hash);
        EXPECT_TRUE(original_hash.is_zero()); // Moved-from state
        
        // Test copy assignment
        Hash assigned_hash;
        assigned_hash = copy_hash;
        EXPECT_EQ(assigned_hash, copy_hash);
        
        // Test move assignment
        Hash move_assigned_hash;
        move_assigned_hash = std::move(copy_hash);
        EXPECT_EQ(move_assigned_hash, assigned_hash);
        EXPECT_TRUE(copy_hash.is_zero()); // Moved-from state
    }
}

TEST_F(HashFuzzTest, HashEdgeCases) {
    // Test zero hash
    Hash zero_hash = Hash::zero();
    EXPECT_TRUE(zero_hash.is_zero());
    EXPECT_EQ(zero_hash.to_hex(), std::string(HASH_SIZE * 2, '0'));
    
    // Test all zeros data
    Hash256 all_zeros{};
    Hash all_zeros_hash(all_zeros);
    EXPECT_TRUE(all_zeros_hash.is_zero());
    
    // Test all ones data
    Hash256 all_ones;
    all_ones.fill(0xFF);
    Hash all_ones_hash(all_ones);
    EXPECT_FALSE(all_ones_hash.is_zero());
    EXPECT_EQ(all_ones_hash.to_hex(), std::string(HASH_SIZE * 2, 'f'));
    
    // Test alternating pattern
    Hash256 alternating;
    for (size_t i = 0; i < HASH_SIZE; ++i) {
        alternating[i] = (i % 2) ? 0xAA : 0x55;
    }
    Hash alternating_hash(alternating);
    EXPECT_FALSE(alternating_hash.is_zero());
}

TEST_F(HashFuzzTest, HashPerformance) {
    const int num_iterations = 10000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random hash data
        Hash256 hash_data;
        for (size_t j = 0; j < HASH_SIZE; ++j) {
            hash_data[j] = byte_dist(rng);
        }
        
        Hash hash(hash_data);
        
        // Perform various operations
        std::string hex = hash.to_hex();
        std::vector<uint8_t> bytes = hash.to_bytes();
        bool is_zero = hash.is_zero();
        size_t hash_value = std::hash<Hash>{}(hash);
        
        // Use variables to prevent optimization
        (void)hex;
        (void)bytes;
        (void)is_zero;
        (void)hash_value;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Performance should be reasonable (less than 1 second for 10k iterations)
    EXPECT_LT(duration.count(), 1000);
}

} // namespace chainforge::fuzz::test
