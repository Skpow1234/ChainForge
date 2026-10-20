# ChainForge Testing Framework

## Overview

The ChainForge testing framework provides a comprehensive, production-grade testing infrastructure built on Google Test (gtest/gmock). It includes test fixtures, helpers, mock objects, custom matchers, parameterized tests, and code coverage reporting.

## Table of Contents

1. [Features](#features)
2. [Quick Start](#quick-start)
3. [Test Fixtures](#test-fixtures)
4. [Test Helpers](#test-helpers)
5. [Mock Objects](#mock-objects)
6. [Custom Matchers](#custom-matchers)
7. [Parameterized Tests](#parameterized-tests)
8. [Performance Testing](#performance-testing)
9. [Code Coverage](#code-coverage)
10. [Best Practices](#best-practices)
11. [Examples](#examples)

## Features

✅ **Test Fixtures** - Pre-configured test environments for common scenarios
✅ **Test Helpers** - Utility functions for creating blockchain objects
✅ **Mock Objects** - Mock implementations for testing without dependencies
✅ **Custom Matchers** - Domain-specific assertion helpers
✅ **Parameterized Tests** - Data-driven testing support
✅ **Performance Testing** - Built-in performance measurement utilities
✅ **Code Coverage** - Integrated coverage reporting (lcov/gcov)
✅ **Call Tracking** - Spy objects for monitoring method calls

## Quick Start

### Including the Framework

```cpp
#include <gtest/gtest.h>
#include "test_fixtures.hpp"
#include "test_helpers.hpp"
#include "mock_objects.hpp"
```

### Writing a Simple Test

```cpp
#include <gtest/gtest.h>
#include "test_fixtures.hpp"

using namespace chainforge::testing;

TEST_F(BlockchainTestFixture, CreateTransaction) {
    // Fixture provides: test_address_from, test_address_to, test_amount
    auto tx = core::Transaction(test_address_from, test_address_to, test_amount);
    
    EXPECT_TRUE(tx.is_valid());
    EXPECT_EQ(tx.from(), test_address_from);
}
```

### Running Tests

```bash
# Build tests
cmake --build build --target framework_tests

# Run all tests
cd build && ctest --output-on-failure

# Run specific test suite
./bin/framework_tests --gtest_filter=BlockchainTestFixture.*

# Run with verbose output
./bin/framework_tests --gtest_verbose

# Generate coverage report
cmake --build build --target coverage
# View: build/coverage/index.html
```

## Test Fixtures

### Available Fixtures

#### `BlockchainTestFixture`
Base fixture providing common blockchain objects:

```cpp
class MyTest : public BlockchainTestFixture {
protected:
    void SetUp() override {
        BlockchainTestFixture::SetUp();
        // Your additional setup
    }
};

TEST_F(MyTest, UseFixtureObjects) {
    // Available objects:
    // - test_address_from: core::Address
    // - test_address_to: core::Address
    // - test_amount: core::Amount
    // - test_timestamp: core::Timestamp
    // - test_hash: core::Hash
    // - test_parent_hash: core::Hash
}
```

#### `TransactionTestFixture`
Extends `BlockchainTestFixture` with a default transaction:

```cpp
TEST_F(TransactionTestFixture, ModifyTransaction) {
    // Available: default_tx (std::unique_ptr<core::Transaction>)
    default_tx->set_nonce(5);
    EXPECT_EQ(default_tx->nonce(), 5);
}
```

#### `BlockTestFixture`
Extends `BlockchainTestFixture` with a default block:

```cpp
TEST_F(BlockTestFixture, AddTransactions) {
    // Available: default_block (std::unique_ptr<core::Block>)
    auto tx = TestHelpers::CreateRandomTransaction();
    default_block->add_transaction(tx);
    EXPECT_EQ(default_block->transaction_count(), 1);
}
```

#### `BlockchainChainTestFixture`
Provides a genesis block and chain building utilities:

```cpp
TEST_F(BlockchainChainTestFixture, BuildChain) {
    // Available: genesis_block, chain
    AddBlockToChain(5);  // Add block with 5 transactions
    AddBlockToChain(3);  // Add another with 3 transactions
    
    EXPECT_EQ(chain.size(), 3);  // Genesis + 2 blocks
}
```

#### `PerformanceTestFixture`
Includes performance measurement utilities:

```cpp
TEST_F(PerformanceTestFixture, MeasureSpeed) {
    auto time_us = MeasureMicroseconds([&]() {
        // Code to measure
        auto hash = test_hash.to_hex();
    });
    
    EXPECT_LT(time_us, 100);  // Should be < 100 microseconds
}
```

## Test Helpers

### `TestHelpers` Class

Static utility functions for creating blockchain objects:

#### Creating Transactions

```cpp
// Random transaction
auto tx = TestHelpers::CreateRandomTransaction();

// Transaction with specific parameters
auto tx = TestHelpers::CreateTransaction(
    from_addr,
    to_addr,
    1000000,      // wei amount
    21000,        // gas limit
    1000000000,   // gas price
    0             // nonce
);

// Transaction with data payload
std::vector<uint8_t> data = {0x01, 0x02, 0x03};
auto tx = TestHelpers::CreateTransactionWithData(from_addr, to_addr, data);
```

#### Creating Blocks

```cpp
// Random block
auto block = TestHelpers::CreateRandomBlock(height);

// Block with specific parameters
auto block = TestHelpers::CreateBlock(height, parent_hash, timestamp, nonce);

// Block with transactions
auto block = TestHelpers::CreateBlockWithTransactions(
    height,
    parent_hash,
    10  // number of transactions
);

// Genesis block
auto genesis = TestHelpers::CreateGenesisBlock(chain_id);
```

#### Creating Blockchains

```cpp
// Create a chain of 10 blocks, each with 5 transactions
auto chain = TestHelpers::CreateBlockchain(10, 5);

// Verify chain linkage
for (size_t i = 1; i < chain.size(); ++i) {
    EXPECT_EQ(chain[i].parent_hash(), chain[i-1].calculate_hash());
}
```

#### Deterministic Objects

```cpp
// Same seed produces same objects (useful for reproducible tests)
auto addr1 = TestHelpers::CreateDeterministicAddress(42);
auto addr2 = TestHelpers::CreateDeterministicAddress(42);
EXPECT_EQ(addr1, addr2);

auto hash1 = TestHelpers::CreateDeterministicHash(42);
```

#### Comparison Helpers

```cpp
// Compare transactions (excluding hash)
bool equal = TestHelpers::TransactionsEqual(tx1, tx2);

// Compare blocks (excluding hash)
bool equal = TestHelpers::BlocksEqual(block1, block2);
```

## Mock Objects

### Using Google Mock

#### MockSerializer

```cpp
#include "mock_objects.hpp"
using ::testing::Return;
using ::testing::_;

TEST(MyTest, UseMockSerializer) {
    MockSerializer mock;
    
    // Set expectation
    std::vector<uint8_t> expected = {0x01, 0x02};
    EXPECT_CALL(mock, serialize_hash(_))
        .WillOnce(Return(serialization::SerializationResult<std::vector<uint8_t>>(expected)));
    
    // Use mock
    auto result = mock.serialize_hash(test_hash);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), expected);
}
```

#### MockValidator

```cpp
TEST(MyTest, UseMockValidator) {
    MockValidator mock;
    
    // Expect validation to succeed
    EXPECT_CALL(mock, validate_transaction(_))
        .WillOnce(Return(core::success()));
    
    auto tx = TestHelpers::CreateRandomTransaction();
    auto result = mock.validate_transaction(tx);
    EXPECT_TRUE(result.has_value());
}
```

### Stub Implementations

Stubs always succeed with fake data (no expectations needed):

```cpp
TEST(MyTest, UseStubs) {
    stubs::StubSerializer serializer;
    stubs::StubValidator validator;
    
    // Always succeeds
    auto ser_result = serializer.serialize_block(block);
    EXPECT_TRUE(ser_result.has_value());
    
    auto val_result = validator.validate_block(block);
    EXPECT_TRUE(val_result.has_value());
}
```

### Call Tracker

Track method calls without mocking:

```cpp
TEST(MyTest, TrackCalls) {
    CallTracker<int> tracker;
    
    tracker.Record(42);
    tracker.Record(43);
    
    EXPECT_EQ(tracker.CallCount(), 2);
    EXPECT_TRUE(tracker.WasCalledWith(42));
    EXPECT_FALSE(tracker.WasCalledWith(99));
}
```

## Custom Matchers

Domain-specific assertion helpers:

```cpp
using namespace chainforge::testing::matchers;

// Amount in range
EXPECT_TRUE(AmountInRange(amount, 1000, 10000));

// Timestamp is recent (within N seconds)
EXPECT_TRUE(TimestampIsRecent(timestamp, 60));

// Hash is non-zero
EXPECT_TRUE(HashIsNonZero(hash));

// Transaction is valid
EXPECT_TRUE(TransactionIsValid(tx));

// Block is valid
EXPECT_TRUE(BlockIsValid(block));
```

## Parameterized Tests

Test same logic with different data:

```cpp
class MyParamTest : public ParameterizedBlockchainTest {};

TEST_P(MyParamTest, TestWithDifferentGasSettings) {
    // gas_limit and gas_price are available from GetParam()
    auto tx = TestHelpers::CreateTransaction(
        test_address_from, test_address_to, 1000, 
        gas_limit, gas_price, 0
    );
    
    EXPECT_EQ(tx.gas_limit(), gas_limit);
    EXPECT_EQ(tx.gas_price(), gas_price);
}

// Define parameter values
INSTANTIATE_TEST_SUITE_P(
    GasVariations,
    MyParamTest,
    ::testing::Values(
        std::make_tuple(21000, 1000000000),
        std::make_tuple(50000, 2000000000),
        std::make_tuple(100000, 5000000000)
    )
);
```

## Performance Testing

### Measuring Execution Time

```cpp
TEST_F(PerformanceTestFixture, MeasureHashCalculation) {
    auto block = TestHelpers::CreateBlockWithTransactions(1, test_hash, 100);
    
    // Measure single execution
    auto time_us = MeasureMicroseconds([&]() {
        auto hash = block.calculate_hash();
    });
    
    std::cout << "Hash calculation: " << time_us << " μs" << std::endl;
    EXPECT_LT(time_us, 1000);  // Should be < 1ms
}
```

### Average Performance

```cpp
TEST_F(PerformanceTestFixture, AveragePerformance) {
    const int iterations = 1000;
    
    auto avg_time = MeasureAverageMicroseconds([&]() {
        auto tx = TestHelpers::CreateRandomTransaction();
    }, iterations);
    
    std::cout << "Average: " << avg_time << " μs" << std::endl;
    EXPECT_LT(avg_time, 10);
}
```

### Automatic Timing

```cpp
TEST_F(PerformanceTestFixture, AutoTiming) {
    // Entire test duration is measured and logged automatically
    for (int i = 0; i < 1000; ++i) {
        auto tx = TestHelpers::CreateRandomTransaction();
    }
    // Output: [PERF] Test duration: XXX μs
}
```

## Code Coverage

### Enabling Coverage

```bash
# Configure with coverage enabled
cmake -B build -DENABLE_COVERAGE=ON

# Build and run tests
cmake --build build
cd build && ctest

# Generate coverage report
cmake --build build --target coverage

# View HTML report
open build/coverage/index.html  # macOS
xdg-open build/coverage/index.html  # Linux
```

### Coverage Targets

```bash
# Generate coverage report
make coverage

# Clean coverage data
make coverage-clean
```

### Coverage Configuration

Located in `cmake/TestCoverage.cmake`:
- Supports GCC and Clang
- Filters out system headers and test code
- Generates HTML reports with lcov/genhtml
- Provides summary statistics

### Coverage Thresholds

Set minimum coverage requirements in your CI:

```yaml
# .github/workflows/ci.yml
- name: Check Coverage
  run: |
    coverage_percent=$(lcov --summary coverage.info.cleaned | grep lines | awk '{print $2}' | sed 's/%//')
    if (( $(echo "$coverage_percent < 80" | bc -l) )); then
      echo "Coverage $coverage_percent% is below 80%"
      exit 1
    fi
```

## Best Practices

### 1. Use Appropriate Fixtures

```cpp
// ✅ Good: Use specific fixture
TEST_F(TransactionTestFixture, ModifyTransaction) {
    default_tx->set_nonce(5);
}

// ❌ Bad: Set up everything manually
TEST(TransactionTest, ModifyTransaction) {
    auto tx = core::Transaction(...);  // Repetitive setup
}
```

### 2. Use Test Helpers

```cpp
// ✅ Good: Use helper
auto chain = TestHelpers::CreateBlockchain(10, 5);

// ❌ Bad: Manual creation
std::vector<core::Block> chain;
for (int i = 0; i < 10; ++i) {
    // 20+ lines of boilerplate...
}
```

### 3. Name Tests Descriptively

```cpp
// ✅ Good: Clear intent
TEST_F(BlockTest, AddingTransactionIncreasesCount)

// ❌ Bad: Unclear
TEST_F(BlockTest, Test1)
```

### 4. One Assertion Per Concept

```cpp
// ✅ Good: Focused test
TEST(AddressTest, RandomAddressIsValid) {
    auto addr = core::Address::random();
    EXPECT_TRUE(addr.is_valid());
}

TEST(AddressTest, RandomAddressIsNonZero) {
    auto addr = core::Address::random();
    EXPECT_FALSE(addr.is_zero());
}

// ❌ Bad: Testing multiple things
TEST(AddressTest, RandomAddress) {
    auto addr = core::Address::random();
    EXPECT_TRUE(addr.is_valid());
    EXPECT_FALSE(addr.is_zero());
    EXPECT_EQ(addr.data().size(), 20);
    // ... 10 more assertions
}
```

### 5. Use Custom Matchers

```cpp
// ✅ Good: Expressive
EXPECT_TRUE(matchers::AmountInRange(amount, 1000, 10000));

// ❌ Bad: Manual check
EXPECT_GE(amount.wei(), 1000);
EXPECT_LE(amount.wei(), 10000);
```

### 6. Mock External Dependencies

```cpp
// ✅ Good: Mock external service
TEST(BlockProcessorTest, ProcessWithMockValidator) {
    MockValidator mock_validator;
    EXPECT_CALL(mock_validator, validate_block(_))
        .WillOnce(Return(core::success()));
    
    BlockProcessor processor(&mock_validator);
    // Test processor logic without real validator
}

// ❌ Bad: Use real validator (slow, brittle)
TEST(BlockProcessorTest, ProcessWithRealValidator) {
    auto validator = create_validator();  // Real implementation
    BlockProcessor processor(validator.get());
    // Test may fail for unrelated validation reasons
}
```

### 7. Test Edge Cases

```cpp
TEST(AmountTest, ZeroAmount) { /* ... */ }
TEST(AmountTest, MaxAmount) { /* ... */ }
TEST(AmountTest, Overflow) { /* ... */ }
TEST(AddressTest, ZeroAddress) { /* ... */ }
TEST(BlockTest, EmptyBlock) { /* ... */ }
TEST(BlockTest, BlockWithMaxTransactions) { /* ... */ }
```

### 8. Use Performance Tests Wisely

```cpp
// ✅ Good: Performance regression test
TEST_F(PerformanceTestFixture, HashingIsStillFast) {
    auto time = MeasureMicroseconds([&]() {
        // Critical path
    });
    EXPECT_LT(time, 100);  // Catches regressions
}

// ❌ Bad: Flaky performance test
TEST(Performance, HashingSpeed) {
    // No fixture, timing may be inconsistent
    auto start = std::chrono::now();
    // ...
}
```

## Examples

See `tests/unit/test_framework_demo.cpp` for comprehensive examples of all framework features.

### Example: Complete Test Suite

```cpp
#include <gtest/gtest.h>
#include "test_fixtures.hpp"
#include "test_helpers.hpp"
#include "mock_objects.hpp"

using namespace chainforge::testing;

class TransactionPoolTest : public BlockchainTestFixture {
protected:
    void SetUp() override {
        BlockchainTestFixture::SetUp();
        pool = std::make_unique<TransactionPool>();
    }
    
    std::unique_ptr<TransactionPool> pool;
};

TEST_F(TransactionPoolTest, AddTransaction) {
    auto tx = TestHelpers::CreateRandomTransaction();
    EXPECT_TRUE(pool->add(tx));
    EXPECT_EQ(pool->size(), 1);
}

TEST_F(TransactionPoolTest, RejectDuplicateTransaction) {
    auto tx = TestHelpers::CreateRandomTransaction();
    EXPECT_TRUE(pool->add(tx));
    EXPECT_FALSE(pool->add(tx));  // Duplicate
    EXPECT_EQ(pool->size(), 1);
}

TEST_F(TransactionPoolTest, RemoveTransaction) {
    auto tx = TestHelpers::CreateRandomTransaction();
    auto hash = tx.calculate_hash();
    
    pool->add(tx);
    EXPECT_TRUE(pool->remove(hash));
    EXPECT_EQ(pool->size(), 0);
}

TEST_F(PerformanceTestFixture, PoolAddPerformance) {
    TransactionPool pool;
    
    auto avg_time = MeasureAverageMicroseconds([&]() {
        auto tx = TestHelpers::CreateRandomTransaction();
        pool.add(tx);
    }, 1000);
    
    EXPECT_LT(avg_time, 50);  // < 50μs per add
}
```

## Summary

The ChainForge testing framework provides:

✅ **Comprehensive** - All testing needs covered
✅ **Easy to Use** - Intuitive API with minimal boilerplate
✅ **Fast** - Optimized fixtures and helpers
✅ **Flexible** - Supports unit, integration, and performance tests
✅ **Production-Grade** - Mock objects, coverage, parameterized tests
✅ **Well-Documented** - Clear examples and best practices

Start with the fixtures and helpers, then add mocks and coverage as needed. Follow the best practices for maintainable, reliable tests.

For more examples, see:
- `tests/unit/test_framework_demo.cpp` - Framework feature demonstrations
- `tests/unit/test_serialization.cpp` - Real-world serialization tests
- `tests/unit/core/test_*.cpp` - Core module tests

