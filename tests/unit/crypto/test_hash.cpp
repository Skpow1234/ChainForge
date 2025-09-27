#include <gtest/gtest.h>
#include "chainforge/crypto/hash.hpp"
#include "chainforge/crypto/keccak.hpp"
#include <vector>
#include <random>
#include <set>

namespace chainforge::crypto::test {

class CryptoHashTest : public ::testing::Test {
protected:
    void SetUp() override {
        rng.seed(std::random_device{}());
    }
    
    void TearDown() override {}
    
    std::mt19937 rng;
};

TEST_F(CryptoHashTest, Keccak256Basic) {
    std::vector<uint8_t> data = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    
    auto result = keccak256(data);
    EXPECT_TRUE(result.has_value());
    
    std::vector<uint8_t> hash = result.value();
    EXPECT_EQ(hash.size(), 32); // Keccak256 produces 32 bytes
    
    // Hash should be deterministic
    auto result2 = keccak256(data);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), hash);
}

TEST_F(CryptoHashTest, Keccak256EmptyData) {
    std::vector<uint8_t> empty_data;
    
    auto result = keccak256(empty_data);
    EXPECT_TRUE(result.has_value());
    
    std::vector<uint8_t> hash = result.value();
    EXPECT_EQ(hash.size(), 32);
    EXPECT_FALSE(hash.empty());
}

TEST_F(CryptoHashTest, Keccak256LargeData) {
    std::vector<uint8_t> large_data(1024 * 1024, 0x42); // 1MB
    
    auto result = keccak256(large_data);
    EXPECT_TRUE(result.has_value());
    
    std::vector<uint8_t> hash = result.value();
    EXPECT_EQ(hash.size(), 32);
}

TEST_F(CryptoHashTest, Keccak256DifferentData) {
    std::vector<uint8_t> data1 = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    std::vector<uint8_t> data2 = {0x57, 0x6f, 0x72, 0x6c, 0x64}; // "World"
    
    auto result1 = keccak256(data1);
    auto result2 = keccak256(data2);
    
    EXPECT_TRUE(result1.has_value());
    EXPECT_TRUE(result2.has_value());
    EXPECT_NE(result1.value(), result2.value());
}

TEST_F(CryptoHashTest, Keccak256CollisionResistance) {
    const int num_iterations = 10000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    std::uniform_int_distribution<size_t> size_dist(1, 256);
    
    std::set<std::vector<uint8_t>> seen_hashes;
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random data
        size_t data_size = size_dist(rng);
        std::vector<uint8_t> data(data_size);
        
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto result = keccak256(data);
        EXPECT_TRUE(result.has_value());
        
        std::vector<uint8_t> hash = result.value();
        
        // Check for collisions
        EXPECT_EQ(seen_hashes.count(hash), 0) << "Hash collision detected at iteration " << i;
        seen_hashes.insert(hash);
    }
}

TEST_F(CryptoHashTest, Keccak256Performance) {
    const int num_iterations = 1000;
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    std::uniform_int_distribution<size_t> size_dist(1, 1024);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random data
        size_t data_size = size_dist(rng);
        std::vector<uint8_t> data(data_size);
        
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto result = keccak256(data);
        EXPECT_TRUE(result.has_value());
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Performance should be reasonable (less than 1 second for 1k iterations)
    EXPECT_LT(duration.count(), 1000);
}

TEST_F(CryptoHashTest, Keccak256KnownValues) {
    // Test known Keccak256 values
    struct TestCase {
        std::vector<uint8_t> input;
        std::string expected_hex;
    };
    
    std::vector<TestCase> test_cases = {
        {{}, "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470"},
        {{0x48, 0x65, 0x6c, 0x6c, 0x6f}, "1c8aff950685c2ed4bc3174f3472287b56d9517b9c948127319a09a7a36deac8"},
        {{0x00, 0x01, 0x02, 0x03}, "b4c11951957c6f8f642c4af61cd6b24640fec6dc7fc607ee8206a99e92410d30"}
    };
    
    for (const auto& test_case : test_cases) {
        auto result = keccak256(test_case.input);
        EXPECT_TRUE(result.has_value());
        
        std::vector<uint8_t> hash = result.value();
        std::string hash_hex = bytes_to_hex(hash);
        
        EXPECT_EQ(hash_hex, test_case.expected_hex) 
            << "Hash mismatch for input of size " << test_case.input.size();
    }
}

TEST_F(CryptoHashTest, Keccak512Basic) {
    std::vector<uint8_t> data = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    
    auto result = keccak512(data);
    EXPECT_TRUE(result.has_value());
    
    std::vector<uint8_t> hash = result.value();
    EXPECT_EQ(hash.size(), 64); // Keccak512 produces 64 bytes
    
    // Hash should be deterministic
    auto result2 = keccak512(data);
    EXPECT_TRUE(result2.has_value());
    EXPECT_EQ(result2.value(), hash);
}

TEST_F(CryptoHashTest, Keccak512EmptyData) {
    std::vector<uint8_t> empty_data;
    
    auto result = keccak512(empty_data);
    EXPECT_TRUE(result.has_value());
    
    std::vector<uint8_t> hash = result.value();
    EXPECT_EQ(hash.size(), 64);
    EXPECT_FALSE(hash.empty());
}

TEST_F(CryptoHashTest, Keccak512LargeData) {
    std::vector<uint8_t> large_data(1024 * 1024, 0x42); // 1MB
    
    auto result = keccak512(large_data);
    EXPECT_TRUE(result.has_value());
    
    std::vector<uint8_t> hash = result.value();
    EXPECT_EQ(hash.size(), 64);
}

TEST_F(CryptoHashTest, Keccak512CollisionResistance) {
    const int num_iterations = 5000; // Fewer iterations for 512-bit
    std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
    std::uniform_int_distribution<size_t> size_dist(1, 256);
    
    std::set<std::vector<uint8_t>> seen_hashes;
    
    for (int i = 0; i < num_iterations; ++i) {
        // Generate random data
        size_t data_size = size_dist(rng);
        std::vector<uint8_t> data(data_size);
        
        for (size_t j = 0; j < data_size; ++j) {
            data[j] = byte_dist(rng);
        }
        
        auto result = keccak512(data);
        EXPECT_TRUE(result.has_value());
        
        std::vector<uint8_t> hash = result.value();
        
        // Check for collisions
        EXPECT_EQ(seen_hashes.count(hash), 0) << "Hash collision detected at iteration " << i;
        seen_hashes.insert(hash);
    }
}

TEST_F(CryptoHashTest, Keccak512KnownValues) {
    // Test known Keccak512 values
    struct TestCase {
        std::vector<uint8_t> input;
        std::string expected_hex;
    };
    
    std::vector<TestCase> test_cases = {
        {{}, "0eab42de4c3ceb9235fc91acffe746b29c29a8c366b7c60e4e67c466f36a4304c00fa9caf9d87976ba469bcbe06713b435f091ef2769fb160cdab33d3670680e"},
        {{0x48, 0x65, 0x6c, 0x6c, 0x6f}, "52fa80662e64c128f8389c9ea6c73d4c02368004bf4463491900d11aaabca6d3c2cbf15cb458a1869dcfcb4b4f44770a5eb3bb2fa1b4ee8b7591f2ccf2f4c67c"}
    };
    
    for (const auto& test_case : test_cases) {
        auto result = keccak512(test_case.input);
        EXPECT_TRUE(result.has_value());
        
        std::vector<uint8_t> hash = result.value();
        std::string hash_hex = bytes_to_hex(hash);
        
        EXPECT_EQ(hash_hex, test_case.expected_hex) 
            << "Hash mismatch for input of size " << test_case.input.size();
    }
}

TEST_F(CryptoHashTest, HashComparison) {
    std::vector<uint8_t> data1 = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    std::vector<uint8_t> data2 = {0x57, 0x6f, 0x72, 0x6c, 0x64}; // "World"
    
    auto result1_256 = keccak256(data1);
    auto result2_256 = keccak256(data2);
    auto result1_512 = keccak512(data1);
    auto result2_512 = keccak512(data2);
    
    EXPECT_TRUE(result1_256.has_value());
    EXPECT_TRUE(result2_256.has_value());
    EXPECT_TRUE(result1_512.has_value());
    EXPECT_TRUE(result2_512.has_value());
    
    // Different data should produce different hashes
    EXPECT_NE(result1_256.value(), result2_256.value());
    EXPECT_NE(result1_512.value(), result2_512.value());
    
    // 256 and 512 bit hashes should be different
    EXPECT_NE(result1_256.value().size(), result1_512.value().size());
}

TEST_F(CryptoHashTest, HashConsistency) {
    std::vector<uint8_t> data = {0x48, 0x65, 0x6c, 0x6c, 0x6f}; // "Hello"
    
    // Hash the same data multiple times
    std::vector<std::vector<uint8_t>> hashes_256;
    std::vector<std::vector<uint8_t>> hashes_512;
    
    for (int i = 0; i < 100; ++i) {
        auto result_256 = keccak256(data);
        auto result_512 = keccak512(data);
        
        EXPECT_TRUE(result_256.has_value());
        EXPECT_TRUE(result_512.has_value());
        
        hashes_256.push_back(result_256.value());
        hashes_512.push_back(result_512.value());
    }
    
    // All hashes should be identical
    for (size_t i = 1; i < hashes_256.size(); ++i) {
        EXPECT_EQ(hashes_256[0], hashes_256[i]);
        EXPECT_EQ(hashes_512[0], hashes_512[i]);
    }
}

TEST_F(CryptoHashTest, HashWithDifferentSizes) {
    std::vector<size_t> sizes = {0, 1, 16, 32, 64, 128, 256, 512, 1024, 4096};
    
    for (size_t size : sizes) {
        std::vector<uint8_t> data(size, 0x42);
        
        auto result_256 = keccak256(data);
        auto result_512 = keccak512(data);
        
        EXPECT_TRUE(result_256.has_value());
        EXPECT_TRUE(result_512.has_value());
        
        EXPECT_EQ(result_256.value().size(), 32);
        EXPECT_EQ(result_512.value().size(), 64);
    }
}

TEST_F(CryptoHashTest, HashErrorHandling) {
    // Test with extremely large data (should still work)
    std::vector<uint8_t> huge_data(100 * 1024 * 1024, 0x42); // 100MB
    
    auto result_256 = keccak256(huge_data);
    auto result_512 = keccak512(huge_data);
    
    EXPECT_TRUE(result_256.has_value());
    EXPECT_TRUE(result_512.has_value());
    
    EXPECT_EQ(result_256.value().size(), 32);
    EXPECT_EQ(result_512.value().size(), 64);
}

TEST_F(CryptoHashTest, HashThreadSafety) {
    const int num_threads = 4;
    const int iterations_per_thread = 100;
    
    std::vector<std::thread> threads;
    std::vector<bool> results(num_threads, true);
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&results, t, iterations_per_thread]() {
            std::mt19937 local_rng(t);
            std::uniform_int_distribution<uint8_t> byte_dist(0, 255);
            std::uniform_int_distribution<size_t> size_dist(1, 1024);
            
            for (int i = 0; i < iterations_per_thread; ++i) {
                // Generate random data
                size_t data_size = size_dist(local_rng);
                std::vector<uint8_t> data(data_size);
                
                for (size_t j = 0; j < data_size; ++j) {
                    data[j] = byte_dist(local_rng);
                }
                
                auto result_256 = keccak256(data);
                auto result_512 = keccak512(data);
                
                if (!result_256.has_value() || !result_512.has_value()) {
                    results[t] = false;
                    return;
                }
                
                if (result_256.value().size() != 32 || result_512.value().size() != 64) {
                    results[t] = false;
                    return;
                }
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

TEST_F(CryptoHashTest, HashEdgeCases) {
    // Test with all zeros
    std::vector<uint8_t> all_zeros(256, 0x00);
    auto result_zeros_256 = keccak256(all_zeros);
    auto result_zeros_512 = keccak512(all_zeros);
    EXPECT_TRUE(result_zeros_256.has_value());
    EXPECT_TRUE(result_zeros_512.has_value());
    
    // Test with all ones
    std::vector<uint8_t> all_ones(256, 0xFF);
    auto result_ones_256 = keccak256(all_ones);
    auto result_ones_512 = keccak512(all_ones);
    EXPECT_TRUE(result_ones_256.has_value());
    EXPECT_TRUE(result_ones_512.has_value());
    
    // Test with alternating pattern
    std::vector<uint8_t> alternating(256);
    for (size_t i = 0; i < 256; ++i) {
        alternating[i] = (i % 2) ? 0xAA : 0x55;
    }
    auto result_alt_256 = keccak256(alternating);
    auto result_alt_512 = keccak512(alternating);
    EXPECT_TRUE(result_alt_256.has_value());
    EXPECT_TRUE(result_alt_512.has_value());
    
    // All should be different
    EXPECT_NE(result_zeros_256.value(), result_ones_256.value());
    EXPECT_NE(result_zeros_256.value(), result_alt_256.value());
    EXPECT_NE(result_ones_256.value(), result_alt_256.value());
}

} // namespace chainforge::crypto::test
