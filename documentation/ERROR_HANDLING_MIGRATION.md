# Error Handling Migration Guide

This guide explains how to migrate existing ChainForge modules to use the new `std::expected`-based error handling system.

## Overview

The new error handling system provides:

- Type-safe error handling with `std::expected<T, ErrorInfo>`
- Comprehensive error codes for all modules
- Error chaining and context propagation
- Recovery mechanisms (retry, circuit breaker, timeout)
- Error monitoring and rate limiting
- User-friendly error messages

## Migration Steps

### 1. Replace Custom Result Types

**Before (Custom Result):**

```cpp
// Old way
CryptoResult<Hash256> hash_data(const ByteVector& data);
StorageResult<Value> get(const Key& key);
```

**After (std::expected):**

```cpp
// New way
Result<Hash256> hash_data(const ByteVector& data);
Result<Value> get(const Key& key);
```

### 2. Update Error Creation

**Before:**

```cpp
if (data.empty()) {
    return CryptoResult<Hash256>{Hash256{}, CryptoError::INVALID_LENGTH};
}
return CryptoResult<Hash256>{hash, CryptoError::SUCCESS};
```

**After:**

```cpp
if (data.empty()) {
    return errors::error<Hash256>(ErrorCode::INVALID_ARGUMENT, "Data cannot be empty");
}
return errors::success(hash);
```

### 3. Update Error Checking

**Before:**

```cpp
auto result = hash_data(data);
if (!result.success()) {
    std::cerr << "Error: " << result.error << std::endl;
    return;
}
auto hash = result.value;
```

**After:**

```cpp
auto result = hash_data(data);
if (!result.has_value()) {
    std::cerr << "Error: " << result.error().to_string() << std::endl;
    return;
}
auto hash = result.value();
```

### 4. Add Error Context and Chaining

**Before:**

```cpp
auto result = validate_block(block);
if (!result.success()) {
    return StorageResult<void>{result.error};
}
```

**After:**

```cpp
auto result = validate_block(block);
if (!result.has_value()) {
    return propagation::chain_error(
        std::move(result),
        ErrorCode::CONSENSUS_ERROR,
        "Block validation failed during storage"
    );
}
```

### 5. Implement Recovery Mechanisms

**Before:**

```cpp
// Manual retry logic
for (int i = 0; i < 3; ++i) {
    auto result = send_message(peer, message);
    if (result.success()) {
        return result;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100 * (i + 1)));
}
return P2PResult<void>{P2PError::TIMEOUT};
```

**After:**

```cpp
return recovery::retry_with_backoff(
    3,                              // max attempts
    std::chrono::milliseconds(100), // initial delay
    2.0,                            // backoff multiplier
    send_message,
    peer,
    message
);
```

## Module-Specific Migration

### Crypto Module

**Error Codes:**

- `ErrorCode::CRYPTO_ERROR` - General crypto errors
- `ErrorCode::INVALID_KEY` - Invalid cryptographic key
- `ErrorCode::INVALID_SIGNATURE` - Invalid signature
- `ErrorCode::INVALID_HASH` - Invalid hash
- `ErrorCode::INVALID_CURVE` - Invalid elliptic curve
- `ErrorCode::INSUFFICIENT_ENTROPY` - Insufficient entropy

**Example Migration:**

```cpp
// Before
CryptoResult<Secp256k1KeyPair> KeyPair::generate_secp256k1() {
    auto private_key_result = Random::generate_secp256k1_private_key();
    if (!private_key_result.success()) {
        return CryptoResult<Secp256k1KeyPair>{Secp256k1KeyPair{}, private_key_result.error};
    }
    // ...
}

// After
Result<Secp256k1KeyPair> KeyPair::generate_secp256k1() {
    auto private_key_result = Random::generate_secp256k1_private_key();
    if (!private_key_result.has_value()) {
        return propagation::chain_error(
            std::move(private_key_result),
            ErrorCode::CRYPTO_ERROR,
            "Failed to generate secp256k1 keypair"
        );
    }
    // ...
}
```

### Storage Module

**Error Codes:**

- `ErrorCode::STORAGE_ERROR` - General storage errors
- `ErrorCode::DATABASE_ERROR` - Database operation failed
- `ErrorCode::KEY_NOT_FOUND` - Key not found in storage
- `ErrorCode::CORRUPTED_DATA` - Data corruption detected
- `ErrorCode::TRANSACTION_FAILED` - Database transaction failed
- `ErrorCode::CONCURRENT_MODIFICATION` - Concurrent modification detected

**Example Migration:**

```cpp
// Before
StorageResult<Value> Database::get(const Key& key, const ReadOptions& options) {
    if (!is_open_) {
        return StorageResult<Value>{Value{}, StorageError::IO_ERROR};
    }
    // ...
}

// After
Result<Value> Database::get(const Key& key, const ReadOptions& options) {
    if (!is_open_) {
        return errors::error<Value>(ErrorCode::IO_ERROR, "Database is not open");
    }
    // ...
}
```

### P2P Module

**Error Codes:**

- `ErrorCode::P2P_ERROR` - General P2P errors
- `ErrorCode::PEER_NOT_FOUND` - Peer not found
- `ErrorCode::PROTOCOL_ERROR` - Protocol error
- `ErrorCode::MESSAGE_TOO_LARGE` - Message too large
- `ErrorCode::INVALID_MESSAGE` - Invalid message format
- `ErrorCode::PEER_BANNED` - Peer is banned

**Example Migration:**

```cpp
// Before
P2PResult<void> NetworkManager::send_message(const std::string& peer_id, const Message& message) {
    if (peer_id.empty()) {
        return P2PResult<void>{P2PError::INVALID_PEER_ID};
    }
    // ...
}

// After
Result<void> NetworkManager::send_message(const std::string& peer_id, const Message& message) {
    if (peer_id.empty()) {
        return errors::error(ErrorCode::INVALID_ARGUMENT, "Peer ID cannot be empty");
    }
    // ...
}
```

### Consensus Module

**Error Codes:**

- `ErrorCode::CONSENSUS_ERROR` - General consensus errors
- `ErrorCode::INVALID_BLOCK` - Invalid block
- `ErrorCode::INVALID_TRANSACTION` - Invalid transaction
- `ErrorCode::FORK_DETECTED` - Blockchain fork detected
- `ErrorCode::STALE_BLOCK` - Block is stale
- `ErrorCode::INSUFFICIENT_STAKE` - Insufficient stake for operation

**Example Migration:**

```cpp
// Before
ConsensusResult<bool> ConsensusEngine::validate_block(const Block& block) {
    if (block.height == 0) {
        return ConsensusResult<bool>{false, ConsensusError::INVALID_BLOCK};
    }
    // ...
}

// After
Result<bool> ConsensusEngine::validate_block(const Block& block) {
    if (block.height == 0) {
        return errors::error<bool>(ErrorCode::INVALID_BLOCK, "Block height cannot be zero");
    }
    // ...
}
```

### RPC Module

**Error Codes:**

- `ErrorCode::RPC_ERROR` - General RPC errors
- `ErrorCode::INVALID_REQUEST` - Invalid RPC request
- `ErrorCode::METHOD_NOT_FOUND` - RPC method not found
- `ErrorCode::INVALID_PARAMS` - Invalid RPC parameters
- `ErrorCode::INTERNAL_ERROR` - Internal server error
- `ErrorCode::RATE_LIMITED` - Rate limit exceeded

**Example Migration:**

```cpp
// Before
JsonRpcResponse RpcServer::process_request(const JsonRpcRequest& request) {
    if (request.method.empty()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_request("Method cannot be empty");
        return response;
    }
    // ...
}

// After
Result<JsonRpcResponse> RpcServer::process_request(const JsonRpcRequest& request) {
    if (request.method.empty()) {
        return errors::error<JsonRpcResponse>(
            ErrorCode::INVALID_REQUEST,
            "Method cannot be empty"
        );
    }
    // ...
}
```

## Best Practices

### 1. Error Context

Always provide meaningful context when creating errors:

```cpp
// Good
return errors::error<Value>(ErrorCode::KEY_NOT_FOUND, "User profile not found", "user_id: " + user_id);

// Bad
return errors::error<Value>(ErrorCode::KEY_NOT_FOUND, "Not found");
```

### 2. Error Chaining

Chain errors when propagating through layers:

```cpp
auto result = storage.get(key);
if (!result.has_value()) {
    return propagation::chain_error(
        std::move(result),
        ErrorCode::EXECUTION_ERROR,
        "Failed to retrieve data for processing"
    );
}
```

### 3. Recovery Mechanisms

Use appropriate recovery mechanisms:

```cpp
// For network operations
auto result = recovery::retry_with_backoff(3, std::chrono::milliseconds(100), 2.0, send_message, peer, message);

// For service calls
auto result = circuit_breaker.execute(service_call);

// For time-sensitive operations
auto result = recovery::with_timeout(std::chrono::seconds(5), slow_operation);
```

### 4. Error Monitoring

Track errors for observability:

```cpp
class Service {
private:
    monitoring::ErrorTracker error_tracker_;
    
public:
    Result<void> operation() {
        auto result = do_work();
        if (result.has_value()) {
            error_tracker_.record_success();
        } else {
            error_tracker_.record_error(result.error().code);
        }
        return result;
    }
};
```

### 5. User-Friendly Messages

Provide user-friendly error messages:

```cpp
// For logging/debugging
std::cerr << "Error: " << result.error().to_string() << std::endl;

// For user display
std::cout << "Error: " << get_user_friendly_message(result.error().code) << std::endl;
```

## Testing Error Handling

### 1. Error Scenario Tests

```cpp
TEST(ErrorHandlingTest, InvalidInput) {
    auto result = hash_data(ByteVector{});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::INVALID_ARGUMENT);
}
```

### 2. Recovery Tests

```cpp
TEST(ErrorHandlingTest, RetryMechanism) {
    int attempts = 0;
    auto failing_function = [&attempts]() -> Result<int> {
        attempts++;
        if (attempts < 3) {
            return errors::error<int>(ErrorCode::TIMEOUT, "Temporary failure");
        }
        return errors::success(42);
    };
    
    auto result = recovery::retry_with_backoff(5, std::chrono::milliseconds(10), 1.5, failing_function);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 42);
    EXPECT_EQ(attempts, 3);
}
```

### 3. Circuit Breaker Tests

```cpp
TEST(ErrorHandlingTest, CircuitBreaker) {
    recovery::CircuitBreaker breaker(2, std::chrono::milliseconds(100));
    
    auto failing_function = []() -> Result<bool> {
        return errors::error<bool>(ErrorCode::SERVICE_UNAVAILABLE, "Service down");
    };
    
    // First two failures should open circuit
    breaker.execute(failing_function);
    breaker.execute(failing_function);
    
    // Third call should fail immediately
    auto result = breaker.execute(failing_function);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::SERVICE_UNAVAILABLE);
}
```

## Migration Checklist

- [ ] Replace custom Result types with `Result<T>`
- [ ] Update error creation to use `errors::error()` and `errors::success()`
- [ ] Update error checking to use `has_value()` and `value()`
- [ ] Add error context where appropriate
- [ ] Implement error chaining for layered operations
- [ ] Add recovery mechanisms for retryable operations
- [ ] Implement error monitoring for observability
- [ ] Update tests to cover error scenarios
- [ ] Update documentation with new error codes
- [ ] Verify user-friendly error messages

## Common Pitfalls

1. **Forgetting to check `has_value()`**: Always check before accessing `value()`
2. **Not providing context**: Include relevant context in error messages
3. **Overusing exceptions**: Use `std::expected` for recoverable errors, exceptions for truly exceptional cases
4. **Ignoring error chains**: Don't lose error context when propagating errors
5. **Not implementing recovery**: Add appropriate recovery mechanisms for retryable operations

## Performance Considerations

- `std::expected` has minimal overhead compared to exceptions
- Error chaining uses `shared_ptr` for memory efficiency
- Recovery mechanisms are designed to be efficient and configurable
- Error monitoring uses atomic operations for thread safety

This migration guide should help you successfully transition to the new error handling system while maintaining code quality and performance.
