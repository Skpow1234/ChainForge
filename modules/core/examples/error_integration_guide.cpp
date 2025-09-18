#include "chainforge/core/error.hpp"
#include "chainforge/core/error_utils.hpp"
#include <iostream>
#include <memory>

using namespace chainforge::core;

// Example: Migrating existing storage module to use std::expected
namespace chainforge::storage {

// Old way (using custom Result type)
// StorageResult<Value> get(const Key& key);

// New way (using std::expected)
Result<Value> get_new(const Key& key) {
    // Simulate storage operation
    if (key.empty()) {
        return errors::error<Value>(ErrorCode::INVALID_ARGUMENT, "Key cannot be empty");
    }
    
    // Simulate key not found
    if (key == "nonexistent") {
        return errors::error<Value>(ErrorCode::KEY_NOT_FOUND, "Key not found in storage");
    }
    
    // Simulate success
    Value value = "stored_value";
    return errors::success(std::move(value));
}

// Example: Migrating crypto operations
namespace chainforge::crypto {

Result<Hash256> hash_data_new(const ByteVector& data) {
    if (data.empty()) {
        return errors::error<Hash256>(ErrorCode::INVALID_ARGUMENT, "Data cannot be empty");
    }
    
    // Simulate hash operation
    Hash256 hash{};
    std::fill(hash.begin(), hash.end(), 0x42);
    return errors::success(hash);
}

Result<bool> verify_signature_new(const ByteVector& data, const ByteVector& signature, const ByteVector& public_key) {
    if (data.empty() || signature.empty() || public_key.empty()) {
        return errors::error<bool>(ErrorCode::INVALID_ARGUMENT, "Invalid input parameters");
    }
    
    // Simulate signature verification
    if (signature.size() < 32) {
        return errors::error<bool>(ErrorCode::INVALID_SIGNATURE, "Signature too short");
    }
    
    return errors::success(true);
}

} // namespace chainforge::crypto

// Example: Migrating P2P operations
namespace chainforge::p2p {

Result<void> send_message_new(const std::string& peer_id, const ByteVector& message) {
    if (peer_id.empty()) {
        return errors::error(ErrorCode::INVALID_ARGUMENT, "Peer ID cannot be empty");
    }
    
    if (message.empty()) {
        return errors::error(ErrorCode::INVALID_ARGUMENT, "Message cannot be empty");
    }
    
    if (message.size() > 1024 * 1024) { // 1MB limit
        return errors::error(ErrorCode::MESSAGE_TOO_LARGE, "Message exceeds size limit");
    }
    
    // Simulate network failure
    if (peer_id == "unreachable") {
        return errors::error(ErrorCode::CONNECTION_REFUSED, "Cannot connect to peer");
    }
    
    return errors::success();
}

} // namespace chainforge::p2p

// Example: Migrating consensus operations
namespace chainforge::consensus {

Result<bool> validate_block_new(const Block& block) {
    if (block.height == 0) {
        return errors::error<bool>(ErrorCode::INVALID_BLOCK, "Block height cannot be zero");
    }
    
    if (block.transactions.empty()) {
        return errors::error<bool>(ErrorCode::INVALID_BLOCK, "Block must contain at least one transaction");
    }
    
    // Simulate validation failure
    if (block.height > 1000000) {
        return errors::error<bool>(ErrorCode::STALE_BLOCK, "Block is too old");
    }
    
    return errors::success(true);
}

} // namespace chainforge::consensus

// Example: Error handling patterns in real code
class DatabaseService {
private:
    recovery::CircuitBreaker circuit_breaker_{3, std::chrono::seconds(30)};
    monitoring::ErrorTracker error_tracker_;
    
public:
    Result<std::string> get_user_data(const std::string& user_id) {
        return circuit_breaker_.execute([this, &user_id]() -> Result<std::string> {
            // Simulate database operation
            if (user_id.empty()) {
                auto error = errors::error<std::string>(ErrorCode::INVALID_ARGUMENT, "User ID cannot be empty");
                error_tracker_.record_error(ErrorCode::INVALID_ARGUMENT);
                return error;
            }
            
            if (user_id == "nonexistent") {
                auto error = errors::error<std::string>(ErrorCode::KEY_NOT_FOUND, "User not found");
                error_tracker_.record_error(ErrorCode::KEY_NOT_FOUND);
                return error;
            }
            
            // Simulate database failure
            if (user_id == "db_error") {
                auto error = errors::error<std::string>(ErrorCode::DATABASE_ERROR, "Database connection failed");
                error_tracker_.record_error(ErrorCode::DATABASE_ERROR);
                return error;
            }
            
            error_tracker_.record_success();
            return errors::success("User data for " + user_id);
        });
    }
    
    double get_error_rate() const {
        return error_tracker_.get_error_rate();
    }
};

// Example: Comprehensive error handling in a service
class BlockchainService {
private:
    DatabaseService db_service_;
    recovery::CircuitBreaker consensus_breaker_{5, std::chrono::seconds(60)};
    
public:
    Result<Block> process_block(const Block& block) {
        // Step 1: Validate block with retry
        auto validation_result = recovery::retry_with_backoff(
            3,
            std::chrono::milliseconds(100),
            2.0,
            [&block]() -> Result<bool> {
                return consensus::validate_block_new(block);
            }
        );
        
        if (!validation_result.has_value()) {
            return propagation::chain_error(
                std::move(validation_result),
                ErrorCode::CONSENSUS_ERROR,
                "Block validation failed"
            );
        }
        
        // Step 2: Get user data with circuit breaker
        auto user_data_result = db_service_.get_user_data("user123");
        if (!user_data_result.has_value()) {
            return propagation::add_context(
                std::move(user_data_result),
                "while processing block"
            );
        }
        
        // Step 3: Process with timeout
        auto processing_result = recovery::with_timeout(
            std::chrono::seconds(5),
            [&block, &user_data_result]() -> Result<Block> {
                // Simulate processing
                Block processed_block = block;
                processed_block.height += 1;
                return errors::success(processed_block);
            }
        );
        
        if (!processing_result.has_value()) {
            return propagation::chain_error(
                std::move(processing_result),
                ErrorCode::EXECUTION_ERROR,
                "Block processing failed"
            );
        }
        
        return processing_result;
    }
    
    Result<void> handle_rpc_request(const std::string& method, const nlohmann::json& params) {
        // Validate request
        if (method.empty()) {
            return errors::error(ErrorCode::INVALID_REQUEST, "Method cannot be empty");
        }
        
        // Rate limiting simulation
        static monitoring::ErrorRateLimiter rate_limiter(0.1, std::chrono::seconds(1));
        if (!rate_limiter.should_allow_operation()) {
            return errors::error(ErrorCode::RATE_LIMITED, "Rate limit exceeded");
        }
        
        // Process request with fallback
        auto result = recovery::fallback_chain(
            [&method, &params]() -> Result<void> {
                if (method == "get_block") {
                    return errors::success();
                }
                return errors::error(ErrorCode::METHOD_NOT_FOUND, "Method not found");
            },
            [&method]() -> Result<void> {
                // Fallback: return generic error
                return errors::error(ErrorCode::INTERNAL_ERROR, "Fallback handler");
            },
            []() -> Result<void> {
                // Final fallback
                return errors::error(ErrorCode::SERVICE_UNAVAILABLE, "All handlers failed");
            }
        );
        
        if (!result.has_value()) {
            rate_limiter.record_error();
        }
        
        return result;
    }
};

// Example: Error handling in main application
void demonstrate_error_handling() {
    std::cout << "=== Error Handling Integration Examples ===\n\n";
    
    // 1. Basic error handling
    std::cout << "1. Basic Error Handling:\n";
    auto storage_result = storage::get_new("test_key");
    if (storage_result.has_value()) {
        std::cout << "Storage success: " << storage_result.value() << "\n";
    } else {
        std::cout << "Storage error: " << storage_result.error().to_string() << "\n";
    }
    
    // 2. Error chaining
    std::cout << "\n2. Error Chaining:\n";
    auto chained_result = storage::get_new("nonexistent");
    if (!chained_result.has_value()) {
        auto chained = propagation::chain_error(
            std::move(chained_result),
            ErrorCode::EXECUTION_ERROR,
            "Failed to retrieve data"
        );
        std::cout << "Chained error: " << chained.error().chain_to_string() << "\n";
    }
    
    // 3. Service with circuit breaker
    std::cout << "\n3. Service with Circuit Breaker:\n";
    DatabaseService db_service;
    
    // Simulate some failures
    for (int i = 0; i < 5; ++i) {
        auto result = db_service.get_user_data("db_error");
        if (result.has_value()) {
            std::cout << "Success: " << result.value() << "\n";
        } else {
            std::cout << "Error: " << result.error().to_string() << "\n";
        }
    }
    
    std::cout << "Error rate: " << (db_service.get_error_rate() * 100) << "%\n";
    
    // 4. Comprehensive service
    std::cout << "\n4. Comprehensive Service:\n";
    BlockchainService blockchain_service;
    
    Block test_block;
    test_block.height = 100;
    test_block.transactions = {"tx1", "tx2"};
    
    auto block_result = blockchain_service.process_block(test_block);
    if (block_result.has_value()) {
        std::cout << "Block processed successfully, new height: " << block_result.value().height << "\n";
    } else {
        std::cout << "Block processing failed: " << block_result.error().chain_to_string() << "\n";
    }
    
    // 5. RPC error handling
    std::cout << "\n5. RPC Error Handling:\n";
    auto rpc_result = blockchain_service.handle_rpc_request("get_block", nlohmann::json{});
    if (rpc_result.has_value()) {
        std::cout << "RPC request handled successfully\n";
    } else {
        std::cout << "RPC error: " << rpc_result.error().to_string() << "\n";
    }
}

} // namespace chainforge::storage

int main() {
    chainforge::storage::demonstrate_error_handling();
    return 0;
}
