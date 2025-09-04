#include <gtest/gtest.h>
#include "chainforge/core/hash.hpp"
#include <vector>

namespace chainforge::core::test {

class HashTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test fixtures
    }
    
    void TearDown() override {
        // Clean up test fixtures
    }
};

TEST_F(HashTest, DefaultConstructor) {
    Hash hash;
    EXPECT_TRUE(hash.is_zero());
}

TEST_F(HashTest, ZeroHash) {
    Hash zero = Hash::zero();
    EXPECT_TRUE(zero.is_zero());
}

TEST_F(HashTest, RandomHash) {
    Hash random1 = Hash::random();
    Hash random2 = Hash::random();
    
    EXPECT_FALSE(random1.is_zero());
    EXPECT_FALSE(random2.is_zero());
    EXPECT_NE(random1, random2);
}

TEST_F(HashTest, HexConversion) {
    Hash256 data{};
    data[0] = 0x12;
    data[1] = 0x34;
    data[2] = 0x56;
    data[3] = 0x78;
    
    Hash hash(data);
    std::string hex = hash.to_hex();
    
    EXPECT_EQ(hex.substr(0, 8), "12345678");
}

TEST_F(HashTest, Equality) {
    Hash256 data1{};
    Hash256 data2{};
    data1[0] = 0x01;
    data2[0] = 0x01;
    
    Hash hash1(data1);
    Hash hash2(data2);
    
    EXPECT_EQ(hash1, hash2);
}

TEST_F(HashTest, Inequality) {
    Hash256 data1{};
    Hash256 data2{};
    data1[0] = 0x01;
    data2[0] = 0x02;
    
    Hash hash1(data1);
    Hash hash2(data2);
    
    EXPECT_NE(hash1, hash2);
}

TEST_F(HashTest, Size) {
    Hash hash;
    EXPECT_EQ(hash.size(), HASH_SIZE);
}

TEST_F(HashTest, BytesConversion) {
    Hash256 data{};
    data[0] = 0xAA;
    data[1] = 0xBB;
    
    Hash hash(data);
    std::vector<uint8_t> bytes = hash.to_bytes();
    
    EXPECT_EQ(bytes.size(), HASH_SIZE);
    EXPECT_EQ(bytes[0], 0xAA);
    EXPECT_EQ(bytes[1], 0xBB);
}

} // namespace chainforge::core::test
