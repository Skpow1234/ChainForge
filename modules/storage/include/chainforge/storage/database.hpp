#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

namespace chainforge::storage {

/**
 * Storage error codes
 */
enum class StorageError {
    SUCCESS = 0,
    NOT_FOUND = 1,
    ALREADY_EXISTS = 2,
    INVALID_ARGUMENT = 3,
    IO_ERROR = 4,
    CORRUPTION = 5,
    NOT_SUPPORTED = 6,
    TIMEOUT = 7,
    LOCK_CONFLICT = 8
};

/**
 * Storage operation result
 */
template<typename T>
struct StorageResult {
    T value;
    StorageError error;

    bool success() const { return error == StorageError::SUCCESS; }
    explicit operator bool() const { return success(); }
};

// Specialization for void operations
template<>
struct StorageResult<void> {
    StorageError error;

    bool success() const { return error == StorageError::SUCCESS; }
    explicit operator bool() const { return success(); }
};

/**
 * Key-value pair type aliases
 */
using Key = std::vector<uint8_t>;
using Value = std::vector<uint8_t>;
using KeyValuePair = std::pair<Key, Value>;

/**
 * Database configuration
 */
struct DatabaseConfig {
    std::string path;                    // Database path
    bool create_if_missing = true;       // Create database if it doesn't exist
    bool error_if_exists = false;        // Error if database already exists
    size_t write_buffer_size = 64 * 1024 * 1024;  // 64MB write buffer
    size_t max_open_files = 1000;        // Maximum open files
    size_t block_cache_size = 8 * 1024 * 1024;   // 8MB block cache
    bool compression = true;             // Enable compression
    int compression_level = 6;           // Compression level (1-9)
    size_t max_background_jobs = 2;      // Background compaction jobs
};

/**
 * Write options
 */
struct WriteOptions {
    bool sync = false;  // Sync to disk immediately
};

/**
 * Read options
 */
struct ReadOptions {
    bool verify_checksums = false;  // Verify data integrity
};

/**
 * Iterator for range queries
 */
class Iterator {
public:
    virtual ~Iterator() = default;

    virtual bool valid() const = 0;
    virtual void seek_to_first() = 0;
    virtual void seek_to_last() = 0;
    virtual void seek(const Key& key) = 0;
    virtual void next() = 0;
    virtual void prev() = 0;

    virtual const Key& key() const = 0;
    virtual const Value& value() const = 0;

    virtual StorageError status() const = 0;
};

/**
 * Batch write operations
 */
class WriteBatch {
public:
    virtual ~WriteBatch() = default;

    virtual void put(const Key& key, const Value& value) = 0;
    virtual void remove(const Key& key) = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
};

/**
 * Database interface
 */
class Database {
public:
    virtual ~Database() = default;

    // Database management
    virtual StorageResult<void> open(const DatabaseConfig& config) = 0;
    virtual StorageResult<void> close() = 0;
    virtual bool is_open() const = 0;

    // Basic operations
    virtual StorageResult<Value> get(const Key& key, const ReadOptions& options = {}) = 0;
    virtual StorageResult<void> put(const Key& key, const Value& value, const WriteOptions& options = {}) = 0;
    virtual StorageResult<void> remove(const Key& key, const WriteOptions& options = {}) = 0;

    // Batch operations
    virtual StorageResult<void> write(WriteBatch& batch, const WriteOptions& options = {}) = 0;

    // Range operations
    virtual std::unique_ptr<Iterator> new_iterator(const ReadOptions& options = {}) = 0;

    // Utility operations
    virtual StorageResult<void> flush(const WriteOptions& options = {}) = 0;
    virtual StorageResult<void> compact_range(const Key* begin = nullptr, const Key* end = nullptr) = 0;

    // Statistics and info
    virtual StorageResult<std::string> get_property(const std::string& property) = 0;
    virtual StorageResult<uint64_t> get_approximate_size(const Key& start, const Key& limit) = 0;
};

/**
 * Database factory function
 */
std::unique_ptr<Database> create_database(const std::string& backend = "rocksdb");

/**
 * Write batch factory function
 */
std::unique_ptr<WriteBatch> create_write_batch();

} // namespace chainforge::storage
