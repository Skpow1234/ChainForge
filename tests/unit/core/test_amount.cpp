#include <gtest/gtest.h>
#include "chainforge/core/amount.hpp"
#include <limits>

namespace chainforge::core::test {

class AmountTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(AmountTest, DefaultConstructor) {
    Amount amount;
    EXPECT_EQ(amount.value(), 0);
    EXPECT_TRUE(amount.is_zero());
}

TEST_F(AmountTest, ValueConstructor) {
    Amount amount(1000);
    EXPECT_EQ(amount.value(), 1000);
    EXPECT_FALSE(amount.is_zero());
}

TEST_F(AmountTest, FromWei) {
    Amount amount = Amount::from_wei(1000000000000000000ULL);
    EXPECT_EQ(amount.value(), 1000000000000000000ULL);
}

TEST_F(AmountTest, ToWei) {
    Amount amount(1);
    EXPECT_EQ(amount.to_wei(), 1000000000000000000ULL);
}

TEST_F(AmountTest, FromEther) {
    Amount amount = Amount::from_ether(1.5);
    EXPECT_EQ(amount.value(), 1500000000000000000ULL);
}

TEST_F(AmountTest, ToEther) {
    Amount amount(1500000000000000000ULL);
    EXPECT_DOUBLE_EQ(amount.to_ether(), 1.5);
}

TEST_F(AmountTest, ArithmeticOperations) {
    Amount a(1000);
    Amount b(500);
    
    // Addition
    Amount sum = a + b;
    EXPECT_EQ(sum.value(), 1500);
    
    // Subtraction
    Amount diff = a - b;
    EXPECT_EQ(diff.value(), 500);
    
    // Multiplication
    Amount product = a * 2;
    EXPECT_EQ(product.value(), 2000);
    
    // Division
    Amount quotient = a / 2;
    EXPECT_EQ(quotient.value(), 500);
}

TEST_F(AmountTest, ComparisonOperators) {
    Amount a(1000);
    Amount b(500);
    Amount c(1000);
    
    EXPECT_TRUE(a > b);
    EXPECT_TRUE(b < a);
    EXPECT_TRUE(a >= c);
    EXPECT_TRUE(b <= a);
    EXPECT_TRUE(a == c);
    EXPECT_TRUE(a != b);
}

TEST_F(AmountTest, OverflowProtection) {
    Amount max_amount(std::numeric_limits<uint64_t>::max());
    
    // Test overflow in addition
    EXPECT_THROW({
        Amount result = max_amount + Amount(1);
    }, std::overflow_error);
    
    // Test overflow in multiplication
    EXPECT_THROW({
        Amount result = max_amount * 2;
    }, std::overflow_error);
}

TEST_F(AmountTest, UnderflowProtection) {
    Amount small_amount(100);
    Amount large_amount(200);
    
    // Test underflow in subtraction
    EXPECT_THROW({
        Amount result = small_amount - large_amount;
    }, std::underflow_error);
}

TEST_F(AmountTest, StringConversion) {
    Amount amount(1234567890123456789ULL);
    std::string str = amount.to_string();
    EXPECT_EQ(str, "1234567890123456789");
    
    auto parsed = Amount::from_string("9876543210987654321");
    EXPECT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed.value().value(), 9876543210987654321ULL);
}

TEST_F(AmountTest, InvalidStringConversion) {
    auto result1 = Amount::from_string("invalid");
    EXPECT_FALSE(result1.has_value());
    
    auto result2 = Amount::from_string("-100");
    EXPECT_FALSE(result2.has_value());
    
    auto result3 = Amount::from_string("1.5.2");
    EXPECT_FALSE(result3.has_value());
}

TEST_F(AmountTest, PrecisionHandling) {
    // Test precision in ether conversion
    Amount amount = Amount::from_ether(0.123456789012345678);
    double back_to_ether = amount.to_ether();
    
    // Should maintain reasonable precision
    EXPECT_NEAR(back_to_ether, 0.123456789012345678, 1e-15);
}

TEST_F(AmountTest, ZeroHandling) {
    Amount zero;
    Amount non_zero(100);
    
    EXPECT_TRUE(zero.is_zero());
    EXPECT_FALSE(non_zero.is_zero());
    
    Amount result = zero + non_zero;
    EXPECT_EQ(result, non_zero);
    
    result = non_zero - non_zero;
    EXPECT_TRUE(result.is_zero());
}

TEST_F(AmountTest, Hash) {
    Amount a(1000);
    Amount b(1000);
    Amount c(2000);
    
    EXPECT_EQ(std::hash<Amount>{}(a), std::hash<Amount>{}(b));
    EXPECT_NE(std::hash<Amount>{}(a), std::hash<Amount>{}(c));
}

TEST_F(AmountTest, CopyAndMove) {
    Amount original(12345);
    Amount copy = original;
    Amount moved = std::move(original);
    
    EXPECT_EQ(copy, moved);
    EXPECT_TRUE(original.is_zero()); // Moved-from state
}

} // namespace chainforge::core::test
