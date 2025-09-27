#include <gtest/gtest.h>
#include "chainforge/core/timestamp.hpp"
#include <chrono>
#include <thread>

namespace chainforge::core::test {

class TimestampTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TimestampTest, DefaultConstructor) {
    Timestamp ts;
    EXPECT_EQ(ts.value(), 0);
    EXPECT_TRUE(ts.is_zero());
}

TEST_F(TimestampTest, ValueConstructor) {
    uint64_t now = 1640995200; // 2022-01-01 00:00:00 UTC
    Timestamp ts(now);
    EXPECT_EQ(ts.value(), now);
    EXPECT_FALSE(ts.is_zero());
}

TEST_F(TimestampTest, Now) {
    Timestamp ts = Timestamp::now();
    EXPECT_GT(ts.value(), 0);
    EXPECT_FALSE(ts.is_zero());
}

TEST_F(TimestampTest, FromUnixTime) {
    uint64_t unix_time = 1640995200; // 2022-01-01 00:00:00 UTC
    Timestamp ts = Timestamp::from_unix_time(unix_time);
    EXPECT_EQ(ts.value(), unix_time);
}

TEST_F(TimestampTest, ToUnixTime) {
    uint64_t unix_time = 1640995200;
    Timestamp ts(unix_time);
    EXPECT_EQ(ts.to_unix_time(), unix_time);
}

TEST_F(TimestampTest, FromString) {
    std::string time_str = "2022-01-01T00:00:00Z";
    auto result = Timestamp::from_string(time_str);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value().value(), 1640995200);
}

TEST_F(TimestampTest, ToString) {
    Timestamp ts(1640995200); // 2022-01-01 00:00:00 UTC
    std::string str = ts.to_string();
    EXPECT_EQ(str, "2022-01-01T00:00:00Z");
}

TEST_F(TimestampTest, InvalidString) {
    auto result1 = Timestamp::from_string("invalid");
    EXPECT_FALSE(result1.has_value());
    
    auto result2 = Timestamp::from_string("2022-13-01T00:00:00Z"); // Invalid month
    EXPECT_FALSE(result2.has_value());
    
    auto result3 = Timestamp::from_string("2022-01-32T00:00:00Z"); // Invalid day
    EXPECT_FALSE(result3.has_value());
}

TEST_F(TimestampTest, ComparisonOperators) {
    Timestamp ts1(1000);
    Timestamp ts2(2000);
    Timestamp ts3(1000);
    
    EXPECT_TRUE(ts1 < ts2);
    EXPECT_TRUE(ts2 > ts1);
    EXPECT_TRUE(ts1 <= ts3);
    EXPECT_TRUE(ts2 >= ts1);
    EXPECT_TRUE(ts1 == ts3);
    EXPECT_TRUE(ts1 != ts2);
}

TEST_F(TimestampTest, ArithmeticOperations) {
    Timestamp ts1(1000);
    Timestamp ts2(500);
    
    // Addition with duration
    Timestamp sum = ts1 + std::chrono::seconds(100);
    EXPECT_EQ(sum.value(), 1100);
    
    // Subtraction with duration
    Timestamp diff = ts1 - std::chrono::seconds(100);
    EXPECT_EQ(diff.value(), 900);
    
    // Difference between timestamps
    auto duration = ts1 - ts2;
    EXPECT_EQ(duration.count(), 500);
}

TEST_F(TimestampTest, DurationOperations) {
    Timestamp ts(1000);
    
    // Add various durations
    Timestamp ts_plus_sec = ts + std::chrono::seconds(60);
    EXPECT_EQ(ts_plus_sec.value(), 1060);
    
    Timestamp ts_plus_min = ts + std::chrono::minutes(1);
    EXPECT_EQ(ts_plus_min.value(), 1060);
    
    Timestamp ts_plus_hour = ts + std::chrono::hours(1);
    EXPECT_EQ(ts_plus_hour.value(), 4600);
}

TEST_F(TimestampTest, TimeZoneHandling) {
    // Test UTC timestamp
    Timestamp utc_ts(1640995200); // 2022-01-01 00:00:00 UTC
    std::string utc_str = utc_ts.to_string();
    EXPECT_EQ(utc_str, "2022-01-01T00:00:00Z");
    
    // Test with timezone offset
    std::string local_str = utc_ts.to_string_with_timezone(3600); // +1 hour
    EXPECT_EQ(local_str, "2022-01-01T01:00:00+01:00");
}

TEST_F(TimestampTest, PrecisionHandling) {
    // Test millisecond precision
    Timestamp ts_ms = Timestamp::from_unix_time_ms(1640995200000);
    EXPECT_EQ(ts_ms.value(), 1640995200);
    
    // Test microsecond precision
    Timestamp ts_us = Timestamp::from_unix_time_us(1640995200000000);
    EXPECT_EQ(ts_us.value(), 1640995200);
}

TEST_F(TimestampTest, ValidityChecks) {
    // Test valid timestamp
    Timestamp valid_ts(1640995200);
    EXPECT_TRUE(valid_ts.is_valid());
    
    // Test future timestamp (should be valid)
    uint64_t future_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count() + 86400; // 1 day in future
    Timestamp future_ts(future_time);
    EXPECT_TRUE(future_ts.is_valid());
    
    // Test very old timestamp (should be valid but might be flagged)
    Timestamp old_ts(0);
    EXPECT_TRUE(old_ts.is_valid());
}

TEST_F(TimestampTest, Hash) {
    Timestamp ts1(1000);
    Timestamp ts2(1000);
    Timestamp ts3(2000);
    
    EXPECT_EQ(std::hash<Timestamp>{}(ts1), std::hash<Timestamp>{}(ts2));
    EXPECT_NE(std::hash<Timestamp>{}(ts1), std::hash<Timestamp>{}(ts3));
}

TEST_F(TimestampTest, CopyAndMove) {
    Timestamp original(12345);
    Timestamp copy = original;
    Timestamp moved = std::move(original);
    
    EXPECT_EQ(copy, moved);
    EXPECT_TRUE(original.is_zero()); // Moved-from state
}

TEST_F(TimestampTest, ThreadSafety) {
    // Test that timestamp generation is thread-safe
    std::vector<std::thread> threads;
    std::vector<Timestamp> results(10);
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&results, i]() {
            results[i] = Timestamp::now();
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    // All timestamps should be different (or very close)
    for (size_t i = 0; i < results.size(); ++i) {
        for (size_t j = i + 1; j < results.size(); ++j) {
            // Timestamps should be within reasonable range
            auto diff = std::abs(static_cast<int64_t>(results[i].value()) - 
                                static_cast<int64_t>(results[j].value()));
            EXPECT_LT(diff, 10); // Within 10 seconds
        }
    }
}

} // namespace chainforge::core::test
