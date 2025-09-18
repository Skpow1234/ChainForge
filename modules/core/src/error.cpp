#include "chainforge/core/error.hpp"
#include <sstream>
#include <iomanip>
#include <cerrno>

namespace chainforge::core {

std::string ErrorInfo::to_string() const {
    std::ostringstream oss;
    oss << "Error " << static_cast<int>(code) << ": " << message;
    
    if (!context.empty()) {
        oss << " (Context: " << context << ")";
    }
    
    if (!file.empty() && line > 0) {
        oss << " [at " << file << ":" << line << "]";
    }
    
    return oss.str();
}

std::string ErrorInfo::chain_to_string() const {
    std::ostringstream oss;
    oss << to_string();
    
    if (cause) {
        oss << "\nCaused by: " << cause->chain_to_string();
    }
    
    return oss.str();
}

// Error code to string conversion
std::string_view error_code_to_string(ErrorCode code) {
    switch (code) {
        case ErrorCode::SUCCESS: return "SUCCESS";
        
        // Generic errors
        case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        case ErrorCode::INVALID_ARGUMENT: return "INVALID_ARGUMENT";
        case ErrorCode::INVALID_STATE: return "INVALID_STATE";
        case ErrorCode::NOT_IMPLEMENTED: return "NOT_IMPLEMENTED";
        case ErrorCode::TIMEOUT: return "TIMEOUT";
        case ErrorCode::CANCELLED: return "CANCELLED";
        
        // I/O errors
        case ErrorCode::IO_ERROR: return "IO_ERROR";
        case ErrorCode::FILE_NOT_FOUND: return "FILE_NOT_FOUND";
        case ErrorCode::PERMISSION_DENIED: return "PERMISSION_DENIED";
        case ErrorCode::DISK_FULL: return "DISK_FULL";
        case ErrorCode::NETWORK_ERROR: return "NETWORK_ERROR";
        case ErrorCode::CONNECTION_REFUSED: return "CONNECTION_REFUSED";
        case ErrorCode::CONNECTION_TIMEOUT: return "CONNECTION_TIMEOUT";
        
        // Crypto errors
        case ErrorCode::CRYPTO_ERROR: return "CRYPTO_ERROR";
        case ErrorCode::INVALID_KEY: return "INVALID_KEY";
        case ErrorCode::INVALID_SIGNATURE: return "INVALID_SIGNATURE";
        case ErrorCode::INVALID_HASH: return "INVALID_HASH";
        case ErrorCode::INVALID_CURVE: return "INVALID_CURVE";
        case ErrorCode::INSUFFICIENT_ENTROPY: return "INSUFFICIENT_ENTROPY";
        
        // Storage errors
        case ErrorCode::STORAGE_ERROR: return "STORAGE_ERROR";
        case ErrorCode::DATABASE_ERROR: return "DATABASE_ERROR";
        case ErrorCode::KEY_NOT_FOUND: return "KEY_NOT_FOUND";
        case ErrorCode::CORRUPTED_DATA: return "CORRUPTED_DATA";
        case ErrorCode::TRANSACTION_FAILED: return "TRANSACTION_FAILED";
        case ErrorCode::CONCURRENT_MODIFICATION: return "CONCURRENT_MODIFICATION";
        
        // P2P errors
        case ErrorCode::P2P_ERROR: return "P2P_ERROR";
        case ErrorCode::PEER_NOT_FOUND: return "PEER_NOT_FOUND";
        case ErrorCode::PROTOCOL_ERROR: return "PROTOCOL_ERROR";
        case ErrorCode::MESSAGE_TOO_LARGE: return "MESSAGE_TOO_LARGE";
        case ErrorCode::INVALID_MESSAGE: return "INVALID_MESSAGE";
        case ErrorCode::PEER_BANNED: return "PEER_BANNED";
        
        // Consensus errors
        case ErrorCode::CONSENSUS_ERROR: return "CONSENSUS_ERROR";
        case ErrorCode::INVALID_BLOCK: return "INVALID_BLOCK";
        case ErrorCode::INVALID_TRANSACTION: return "INVALID_TRANSACTION";
        case ErrorCode::FORK_DETECTED: return "FORK_DETECTED";
        case ErrorCode::STALE_BLOCK: return "STALE_BLOCK";
        case ErrorCode::INSUFFICIENT_STAKE: return "INSUFFICIENT_STAKE";
        
        // Execution errors
        case ErrorCode::EXECUTION_ERROR: return "EXECUTION_ERROR";
        case ErrorCode::OUT_OF_GAS: return "OUT_OF_GAS";
        case ErrorCode::INVALID_OPCODE: return "INVALID_OPCODE";
        case ErrorCode::STACK_OVERFLOW: return "STACK_OVERFLOW";
        case ErrorCode::STACK_UNDERFLOW: return "STACK_UNDERFLOW";
        case ErrorCode::INVALID_JUMP: return "INVALID_JUMP";
        
        // Mempool errors
        case ErrorCode::MEMPOOL_ERROR: return "MEMPOOL_ERROR";
        case ErrorCode::TRANSACTION_EXISTS: return "TRANSACTION_EXISTS";
        case ErrorCode::INSUFFICIENT_FEE: return "INSUFFICIENT_FEE";
        case ErrorCode::NONCE_TOO_LOW: return "NONCE_TOO_LOW";
        case ErrorCode::NONCE_TOO_HIGH: return "NONCE_TOO_HIGH";
        case ErrorCode::GAS_LIMIT_EXCEEDED: return "GAS_LIMIT_EXCEEDED";
        
        // RPC errors
        case ErrorCode::RPC_ERROR: return "RPC_ERROR";
        case ErrorCode::INVALID_REQUEST: return "INVALID_REQUEST";
        case ErrorCode::METHOD_NOT_FOUND: return "METHOD_NOT_FOUND";
        case ErrorCode::INVALID_PARAMS: return "INVALID_PARAMS";
        case ErrorCode::INTERNAL_ERROR: return "INTERNAL_ERROR";
        case ErrorCode::RATE_LIMITED: return "RATE_LIMITED";
        
        // Node errors
        case ErrorCode::NODE_ERROR: return "NODE_ERROR";
        case ErrorCode::NODE_NOT_RUNNING: return "NODE_NOT_RUNNING";
        case ErrorCode::CONFIG_ERROR: return "CONFIG_ERROR";
        case ErrorCode::RESOURCE_EXHAUSTED: return "RESOURCE_EXHAUSTED";
        case ErrorCode::SERVICE_UNAVAILABLE: return "SERVICE_UNAVAILABLE";
        
        default: return "UNKNOWN_ERROR_CODE";
    }
}

// User-friendly error messages
std::string get_user_friendly_message(ErrorCode code) {
    switch (code) {
        case ErrorCode::SUCCESS: return "Operation completed successfully";
        
        // Generic errors
        case ErrorCode::UNKNOWN_ERROR: return "An unexpected error occurred";
        case ErrorCode::INVALID_ARGUMENT: return "Invalid argument provided";
        case ErrorCode::INVALID_STATE: return "System is in an invalid state";
        case ErrorCode::NOT_IMPLEMENTED: return "This feature is not yet implemented";
        case ErrorCode::TIMEOUT: return "Operation timed out";
        case ErrorCode::CANCELLED: return "Operation was cancelled";
        
        // I/O errors
        case ErrorCode::IO_ERROR: return "Input/output error occurred";
        case ErrorCode::FILE_NOT_FOUND: return "File not found";
        case ErrorCode::PERMISSION_DENIED: return "Permission denied";
        case ErrorCode::DISK_FULL: return "Disk is full";
        case ErrorCode::NETWORK_ERROR: return "Network error occurred";
        case ErrorCode::CONNECTION_REFUSED: return "Connection was refused";
        case ErrorCode::CONNECTION_TIMEOUT: return "Connection timed out";
        
        // Crypto errors
        case ErrorCode::CRYPTO_ERROR: return "Cryptographic operation failed";
        case ErrorCode::INVALID_KEY: return "Invalid cryptographic key";
        case ErrorCode::INVALID_SIGNATURE: return "Invalid signature";
        case ErrorCode::INVALID_HASH: return "Invalid hash";
        case ErrorCode::INVALID_CURVE: return "Invalid elliptic curve";
        case ErrorCode::INSUFFICIENT_ENTROPY: return "Insufficient entropy for secure operation";
        
        // Storage errors
        case ErrorCode::STORAGE_ERROR: return "Storage operation failed";
        case ErrorCode::DATABASE_ERROR: return "Database operation failed";
        case ErrorCode::KEY_NOT_FOUND: return "Key not found in storage";
        case ErrorCode::CORRUPTED_DATA: return "Data corruption detected";
        case ErrorCode::TRANSACTION_FAILED: return "Database transaction failed";
        case ErrorCode::CONCURRENT_MODIFICATION: return "Concurrent modification detected";
        
        // P2P errors
        case ErrorCode::P2P_ERROR: return "Peer-to-peer communication error";
        case ErrorCode::PEER_NOT_FOUND: return "Peer not found";
        case ErrorCode::PROTOCOL_ERROR: return "Protocol error";
        case ErrorCode::MESSAGE_TOO_LARGE: return "Message too large";
        case ErrorCode::INVALID_MESSAGE: return "Invalid message format";
        case ErrorCode::PEER_BANNED: return "Peer is banned";
        
        // Consensus errors
        case ErrorCode::CONSENSUS_ERROR: return "Consensus mechanism error";
        case ErrorCode::INVALID_BLOCK: return "Invalid block";
        case ErrorCode::INVALID_TRANSACTION: return "Invalid transaction";
        case ErrorCode::FORK_DETECTED: return "Blockchain fork detected";
        case ErrorCode::STALE_BLOCK: return "Block is stale";
        case ErrorCode::INSUFFICIENT_STAKE: return "Insufficient stake for operation";
        
        // Execution errors
        case ErrorCode::EXECUTION_ERROR: return "Transaction execution error";
        case ErrorCode::OUT_OF_GAS: return "Out of gas during execution";
        case ErrorCode::INVALID_OPCODE: return "Invalid opcode";
        case ErrorCode::STACK_OVERFLOW: return "Stack overflow";
        case ErrorCode::STACK_UNDERFLOW: return "Stack underflow";
        case ErrorCode::INVALID_JUMP: return "Invalid jump destination";
        
        // Mempool errors
        case ErrorCode::MEMPOOL_ERROR: return "Mempool operation failed";
        case ErrorCode::TRANSACTION_EXISTS: return "Transaction already exists";
        case ErrorCode::INSUFFICIENT_FEE: return "Insufficient transaction fee";
        case ErrorCode::NONCE_TOO_LOW: return "Transaction nonce too low";
        case ErrorCode::NONCE_TOO_HIGH: return "Transaction nonce too high";
        case ErrorCode::GAS_LIMIT_EXCEEDED: return "Gas limit exceeded";
        
        // RPC errors
        case ErrorCode::RPC_ERROR: return "RPC operation failed";
        case ErrorCode::INVALID_REQUEST: return "Invalid RPC request";
        case ErrorCode::METHOD_NOT_FOUND: return "RPC method not found";
        case ErrorCode::INVALID_PARAMS: return "Invalid RPC parameters";
        case ErrorCode::INTERNAL_ERROR: return "Internal server error";
        case ErrorCode::RATE_LIMITED: return "Rate limit exceeded";
        
        // Node errors
        case ErrorCode::NODE_ERROR: return "Node operation failed";
        case ErrorCode::NODE_NOT_RUNNING: return "Node is not running";
        case ErrorCode::CONFIG_ERROR: return "Configuration error";
        case ErrorCode::RESOURCE_EXHAUSTED: return "System resources exhausted";
        case ErrorCode::SERVICE_UNAVAILABLE: return "Service unavailable";
        
        default: return "Unknown error occurred";
    }
}

} // namespace chainforge::core
