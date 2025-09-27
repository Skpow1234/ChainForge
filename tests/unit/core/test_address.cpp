#include <gtest/gtest.h>
#include "chainforge/core/address.hpp"
#include <vector>
#include <string>

namespace chainforge::core::test {

class AddressTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AddressTest, DefaultConstructor) {
    Address addr;
    EXPECT_TRUE(addr.is_zero());
    EXPECT_EQ(addr.to_string(), "0x0000000000000000000000000000000000000000");
}

TEST_F(AddressTest, FromBytes) {
    std::vector<uint8_t> bytes(20, 0x42);
    Address addr(bytes);
    
    EXPECT_FALSE(addr.is_zero());
    EXPECT_EQ(addr.to_string(), "0x4242424242424242424242424242424242424242");
}

TEST_F(AddressTest, FromString) {
    std::string addr_str = "0x1234567890123456789012345678901234567890";
    auto addr_result = Address::from_string(addr_str);
    
    EXPECT_TRUE(addr_result.has_value());
    EXPECT_EQ(addr_result.value().to_string(), addr_str);
}

TEST_F(AddressTest, InvalidString) {
    auto result1 = Address::from_string("invalid");
    EXPECT_FALSE(result1.has_value());
    
    auto result2 = Address::from_string("0x123"); // Too short
    EXPECT_FALSE(result2.has_value());
    
    auto result3 = Address::from_string("0x" + std::string(41, '1')); // Too long
    EXPECT_FALSE(result3.has_value());
}

TEST_F(AddressTest, Equality) {
    Address addr1 = Address::from_string("0x1234567890123456789012345678901234567890").value();
    Address addr2 = Address::from_string("0x1234567890123456789012345678901234567890").value();
    Address addr3 = Address::from_string("0x1234567890123456789012345678901234567891").value();
    
    EXPECT_EQ(addr1, addr2);
    EXPECT_NE(addr1, addr3);
}

TEST_F(AddressTest, Hash) {
    Address addr1 = Address::from_string("0x1234567890123456789012345678901234567890").value();
    Address addr2 = Address::from_string("0x1234567890123456789012345678901234567890").value();
    Address addr3 = Address::from_string("0x1234567890123456789012345678901234567891").value();
    
    EXPECT_EQ(std::hash<Address>{}(addr1), std::hash<Address>{}(addr2));
    EXPECT_NE(std::hash<Address>{}(addr1), std::hash<Address>{}(addr3));
}

TEST_F(AddressTest, ToBytes) {
    std::string addr_str = "0x1234567890123456789012345678901234567890";
    Address addr = Address::from_string(addr_str).value();
    std::vector<uint8_t> bytes = addr.to_bytes();
    
    EXPECT_EQ(bytes.size(), 20);
    EXPECT_EQ(bytes[0], 0x12);
    EXPECT_EQ(bytes[1], 0x34);
    EXPECT_EQ(bytes[19], 0x90);
}

TEST_F(AddressTest, Checksum) {
    // Test EIP-55 checksum validation
    std::string checksum_addr = "0x5aAeb6053F3E94C9b9A09f33669435E7Ef1BeAed";
    auto result = Address::from_string(checksum_addr);
    EXPECT_TRUE(result.has_value());
    
    std::string lowercase_addr = "0x5aaeb6053f3e94c9b9a09f33669435e7ef1beaed";
    auto result2 = Address::from_string(lowercase_addr);
    EXPECT_TRUE(result2.has_value());
    
    EXPECT_EQ(result.value(), result2.value());
}

TEST_F(AddressTest, RandomGeneration) {
    Address addr1 = Address::random();
    Address addr2 = Address::random();
    
    EXPECT_FALSE(addr1.is_zero());
    EXPECT_FALSE(addr2.is_zero());
    EXPECT_NE(addr1, addr2);
}

TEST_F(AddressTest, CopyAndMove) {
    Address original = Address::random();
    Address copy = original;
    Address moved = std::move(original);
    
    EXPECT_EQ(copy, moved);
    EXPECT_TRUE(original.is_zero()); // Moved-from state
}

} // namespace chainforge::core::test
