#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <variant>
#include <memory>
#include <thread>
#include <chrono>
#include <random>

namespace chainforge::core {

// Forward declaration for error types
enum class ErrorCode : int;
struct ErrorInfo;

// Main error type using std::expected
template<typename T>
using Result = std::expected<T, ErrorInfo>;

// Specialization for void return types
using VoidResult = Result<void>;

// Error code enumeration
enum class ErrorCode : int {
    // Success
    SUCCESS = 0,
    
    // Generic errors
    UNKNOWN_ERROR = 1000,
    INVALID_ARGUMENT = 1001,
    INVALID_STATE = 1002,
    NOT_IMPLEMENTED = 1003,
    TIMEOUT = 1004,
    CANCELLED = 1005,
    
    // I/O errors
    IO_ERROR = 2000,
    FILE_NOT_FOUND = 2001,
    PERMISSION_DENIED = 2002,
    DISK_FULL = 2003,
    NETWORK_ERROR = 2004,
    CONNECTION_REFUSED = 2005,
    CONNECTION_TIMEOUT = 2006,
    
    // Crypto errors
    CRYPTO_ERROR = 3000,
    INVALID_KEY = 3001,
    INVALID_SIGNATURE = 3002,
    INVALID_HASH = 3003,
    INVALID_CURVE = 3004,
    INSUFFICIENT_ENTROPY = 3005,
    
    // Storage errors
    STORAGE_ERROR = 4000,
    DATABASE_ERROR = 4001,
    KEY_NOT_FOUND = 4002,
    CORRUPTED_DATA = 4003,
    TRANSACTION_FAILED = 4004,
    CONCURRENT_MODIFICATION = 4005,
    
    // P2P errors
    P2P_ERROR = 5000,
    PEER_NOT_FOUND = 5001,
    PROTOCOL_ERROR = 5002,
    MESSAGE_TOO_LARGE = 5003,
    INVALID_MESSAGE = 5004,
    PEER_BANNED = 5005,
    
    // Consensus errors
    CONSENSUS_ERROR = 6000,
    INVALID_BLOCK = 6001,
    INVALID_TRANSACTION = 6002,
    FORK_DETECTED = 6003,
    STALE_BLOCK = 6004,
    INSUFFICIENT_STAKE = 6005,
    
    // Execution errors
    EXECUTION_ERROR = 7000,
    OUT_OF_GAS = 7001,
    INVALID_OPCODE = 7002,
    STACK_OVERFLOW = 7003,
    STACK_UNDERFLOW = 7004,
    INVALID_JUMP = 7005,
    
    // Mempool errors
    MEMPOOL_ERROR = 8000,
    TRANSACTION_EXISTS = 8001,
    INSUFFICIENT_FEE = 8002,
    NONCE_TOO_LOW = 8003,
    NONCE_TOO_HIGH = 8004,
    GAS_LIMIT_EXCEEDED = 8005,
    
    // RPC errors
    RPC_ERROR = 9000,
    INVALID_REQUEST = 9001,
    METHOD_NOT_FOUND = 9002,
    INVALID_PARAMS = 9003,
    INTERNAL_ERROR = 9004,
    RATE_LIMITED = 9005,
    
    // Node errors
    NODE_ERROR = 10000,
    NODE_NOT_RUNNING = 10001,
    CONFIG_ERROR = 10002,
    RESOURCE_EXHAUSTED = 10003,
    SERVICE_UNAVAILABLE = 10004
};

// Error information structure
struct ErrorInfo {
    ErrorCode code;
    std::string message;
    std::string context;
    std::string file;
    int line;
    std::shared_ptr<ErrorInfo> cause; // For error chaining
    
    ErrorInfo(ErrorCode c, std::string_view msg, std::string_view ctx = "", 
              std::string_view f = "", int l = 0, std::shared_ptr<ErrorInfo> cau = nullptr)
        : code(c), message(msg), context(ctx), file(f), line(l), cause(std::move(cau)) {}
    
    // Helper to check if this is a success
    bool is_success() const noexcept { return code == ErrorCode::SUCCESS; }
    
    // Helper to check if this is a specific error code
    bool is_error(ErrorCode c) const noexcept { return code == c; }
    
    // Get human-readable error description
    std::string to_string() const;
    
    // Get error chain as string
    std::string chain_to_string() const;
};

// Error creation helpers
namespace errors {

// Create error with just code and message
inline ErrorInfo make_error(ErrorCode code, std::string_view message) {
    return ErrorInfo(code, message);
}

// Create error with context
inline ErrorInfo make_error(ErrorCode code, std::string_view message, std::string_view context) {
    return ErrorInfo(code, message, context);
}

// Create error with source location
#define MAKE_ERROR(code, message) \
    chainforge::core::errors::make_error(code, message, "", __FILE__, __LINE__)

#define MAKE_ERROR_WITH_CONTEXT(code, message, context) \
    chainforge::core::errors::make_error(code, message, context, __FILE__, __LINE__)

// Create chained error
inline ErrorInfo make_chained_error(ErrorCode code, std::string_view message, 
                                   std::shared_ptr<ErrorInfo> cause) {
    return ErrorInfo(code, message, "", "", 0, std::move(cause));
}

// Create chained error with context
inline ErrorInfo make_chained_error(ErrorCode code, std::string_view message, 
                                   std::string_view context, std::shared_ptr<ErrorInfo> cause) {
    return ErrorInfo(code, message, context, "", 0, std::move(cause));
}

// Success result helpers
template<typename T>
inline Result<T> success(T&& value) {
    return std::expected<T, ErrorInfo>(std::forward<T>(value));
}

inline VoidResult success() {
    return std::expected<void, ErrorInfo>();
}

// Error result helpers
template<typename T>
inline Result<T> error(ErrorCode code, std::string_view message) {
    return std::unexpected(ErrorInfo(code, message));
}

template<typename T>
inline Result<T> error(ErrorCode code, std::string_view message, std::string_view context) {
    return std::unexpected(ErrorInfo(code, message, context));
}

template<typename T>
inline Result<T> error(const ErrorInfo& error_info) {
    return std::unexpected(error_info);
}

inline VoidResult error(ErrorCode code, std::string_view message) {
    return std::unexpected(ErrorInfo(code, message));
}

inline VoidResult error(ErrorCode code, std::string_view message, std::string_view context) {
    return std::unexpected(ErrorInfo(code, message, context));
}

inline VoidResult error(const ErrorInfo& error_info) {
    return std::unexpected(error_info);
}

} // namespace errors

// Error recovery utilities
namespace recovery {

// Retry mechanism
template<typename F, typename... Args>
auto retry(int max_attempts, std::chrono::milliseconds delay, F&& func, Args&&... args) 
    -> Result<std::invoke_result_t<F, Args...>> {
    
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        auto result = func(std::forward<Args>(args)...);
        if (result.has_value()) {
            return result;
        }
        
        if (attempt < max_attempts - 1) {
            std::this_thread::sleep_for(delay);
        }
    }
    
    return errors::error(ErrorCode::TIMEOUT, "Operation failed after maximum retry attempts");
}

// Fallback mechanism
template<typename F1, typename F2, typename... Args>
auto fallback(F1&& primary, F2&& fallback_func, Args&&... args) 
    -> Result<std::invoke_result_t<F1, Args...>> {
    
    auto result = primary(std::forward<Args>(args)...);
    if (result.has_value()) {
        return result;
    }
    
    return fallback_func(std::forward<Args>(args)...);
}

} // namespace recovery

// Error conversion utilities
namespace conversion {

// Convert std::error_code to ErrorInfo
inline ErrorInfo from_std_error(const std::error_code& ec, std::string_view context = "") {
    ErrorCode code = ErrorCode::UNKNOWN_ERROR;
    
    // Map common std::error_codes to our ErrorCodes
    if (ec.category() == std::system_category()) {
        switch (ec.value()) {
            case ENOENT: code = ErrorCode::FILE_NOT_FOUND; break;
            case EACCES: code = ErrorCode::PERMISSION_DENIED; break;
            case ENOSPC: code = ErrorCode::DISK_FULL; break;
            case ECONNREFUSED: code = ErrorCode::CONNECTION_REFUSED; break;
            case ETIMEDOUT: code = ErrorCode::CONNECTION_TIMEOUT; break;
            default: code = ErrorCode::IO_ERROR; break;
        }
    }
    
    return ErrorInfo(code, ec.message(), context);
}

// Convert exception to ErrorInfo
inline ErrorInfo from_exception(const std::exception& e, std::string_view context = "") {
    return ErrorInfo(ErrorCode::UNKNOWN_ERROR, e.what(), context);
}

} // namespace conversion

// Additional utility functions
std::string_view error_code_to_string(ErrorCode code);
std::string get_user_friendly_message(ErrorCode code);

} // namespace chainforge::core
