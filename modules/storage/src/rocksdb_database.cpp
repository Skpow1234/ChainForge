#include "chainforge/storage/database.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <algorithm>
#include <vector>

namespace chainforge::storage {

// In-memory iterator implementation
class MemoryIterator : public Iterator {
public:
    explicit MemoryIterator(const std::unordered_map<Key, Value>& data, const ReadOptions& options)
        : data_(data), options_(options) {
        // Create a sorted vector of keys for iteration
        keys_.reserve(data.size());
        for (const auto& [key, value] : data) {
            keys_.push_back(key);
        }
        std::sort(keys_.begin(), keys_.end());
        current_index_ = -1;
    }

    ~MemoryIterator() override = default;

    bool valid() const override {
        return current_index_ >= 0 && static_cast<size_t>(current_index_) < keys_.size();
    }

    void seek_to_first() override {
        current_index_ = keys_.empty() ? -1 : 0;
    }

    void seek_to_last() override {
        current_index_ = keys_.empty() ? -1 : static_cast<int>(keys_.size() - 1);
    }

    void seek(const Key& key) override {
        // Binary search for the key
        auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
        if (it != keys_.end() && *it == key) {
            current_index_ = static_cast<int>(it - keys_.begin());
        } else {
            current_index_ = -1;
        }
    }

    void next() override {
        if (current_index_ >= 0 && static_cast<size_t>(current_index_) < keys_.size() - 1) {
            ++current_index_;
        } else {
            current_index_ = -1;
        }
    }

    void prev() override {
        if (current_index_ > 0) {
            --current_index_;
        } else {
            current_index_ = -1;
        }
    }

    const Key& key() const override {
        static const Key empty_key;
        if (!valid()) return empty_key;
        return keys_[current_index_];
    }

    const Value& value() const override {
        static const Value empty_value;
        if (!valid()) return empty_value;
        auto it = data_.find(keys_[current_index_]);
        if (it != data_.end()) {
            return it->second;
        }
        return empty_value;
    }

    StorageError status() const override {
        return StorageError::SUCCESS;
    }

private:
    const std::unordered_map<Key, Value>& data_;
    ReadOptions options_;
    std::vector<Key> keys_;
    int current_index_;
};

// In-memory write batch implementation
class MemoryWriteBatch : public WriteBatch {
public:
    MemoryWriteBatch() = default;

    void put(const Key& key, const Value& value) override {
        operations_.emplace_back(OperationType::PUT, key, value);
    }

    void remove(const Key& key) override {
        operations_.emplace_back(OperationType::REMOVE, key, Value{});
    }

    void clear() override {
        operations_.clear();
    }

    size_t size() const override {
        return operations_.size();
    }

    const std::vector<Operation>& get_operations() const {
        return operations_;
    }

private:
    enum class OperationType { PUT, REMOVE };

    struct Operation {
        OperationType type;
        Key key;
        Value value;

        Operation(OperationType t, const Key& k, const Value& v)
            : type(t), key(k), value(v) {}
    };

    std::vector<Operation> operations_;
};

// In-memory database implementation
class MemoryDatabase : public Database {
public:
    MemoryDatabase() = default;
    ~MemoryDatabase() override = default;

    StorageResult<void> open(const DatabaseConfig& config) override {
        if (is_open_) {
            return StorageResult<void>{StorageError::ALREADY_EXISTS};
        }

        if (config.error_if_exists && !data_.empty()) {
            return StorageResult<void>{StorageError::ALREADY_EXISTS};
        }

        config_ = config;
        is_open_ = true;
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> close() override {
        if (!is_open_) {
            return StorageResult<void>{StorageError::SUCCESS};
        }

        data_.clear();
        is_open_ = false;
        return StorageResult<void>{StorageError::SUCCESS};
    }

    bool is_open() const override {
        return is_open_;
    }

    StorageResult<Value> get(const Key& key, const ReadOptions& options) override {
        if (!is_open_) {
            return StorageResult<Value>{Value{}, StorageError::IO_ERROR};
        }

        std::shared_lock lock(mutex_);
        auto it = data_.find(key);
        if (it == data_.end()) {
            return StorageResult<Value>{Value{}, StorageError::NOT_FOUND};
        }

        return StorageResult<Value>{it->second, StorageError::SUCCESS};
    }

    StorageResult<void> put(const Key& key, const Value& value, const WriteOptions& options) override {
        if (!is_open_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        std::unique_lock lock(mutex_);
        data_[key] = value;
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> remove(const Key& key, const WriteOptions& options) override {
        if (!is_open_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        std::unique_lock lock(mutex_);
        auto it = data_.find(key);
        if (it == data_.end()) {
            return StorageResult<void>{StorageError::NOT_FOUND};
        }

        data_.erase(it);
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> write(WriteBatch& batch, const WriteOptions& options) override {
        if (!is_open_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        auto* memory_batch = dynamic_cast<MemoryWriteBatch*>(&batch);
        if (!memory_batch) {
            return StorageResult<void>{StorageError::INVALID_ARGUMENT};
        }

        std::unique_lock lock(mutex_);
        for (const auto& op : memory_batch->get_operations()) {
            if (op.type == MemoryWriteBatch::OperationType::PUT) {
                data_[op.key] = op.value;
            } else if (op.type == MemoryWriteBatch::OperationType::REMOVE) {
                data_.erase(op.key);
            }
        }

        return StorageResult<void>{StorageError::SUCCESS};
    }

    std::unique_ptr<Iterator> new_iterator(const ReadOptions& options) override {
        if (!is_open_) {
            return nullptr;
        }

        std::shared_lock lock(mutex_);
        return std::make_unique<MemoryIterator>(data_, options);
    }

    StorageResult<void> flush(const WriteOptions& options) override {
        if (!is_open_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }
        // In-memory database, nothing to flush
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> compact_range(const Key* begin, const Key* end) override {
        if (!is_open_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }
        // In-memory database, nothing to compact
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<std::string> get_property(const std::string& property) override {
        if (!is_open_) {
            return StorageResult<std::string>{"", StorageError::IO_ERROR};
        }

        if (property == "rocksdb.stats") {
            std::string stats = "In-memory database stats:\n";
            stats += "Keys: " + std::to_string(data_.size()) + "\n";
            return StorageResult<std::string>{stats, StorageError::SUCCESS};
        }

        return StorageResult<std::string>{"", StorageError::NOT_FOUND};
    }

    StorageResult<uint64_t> get_approximate_size(const Key& start, const Key& limit) override {
        if (!is_open_) {
            return StorageResult<uint64_t>{0, StorageError::IO_ERROR};
        }

        std::shared_lock lock(mutex_);
        uint64_t size = 0;

        for (const auto& [key, value] : data_) {
            if (key >= start && key <= limit) {
                size += key.size() + value.size();
            }
        }

        return StorageResult<uint64_t>{size, StorageError::SUCCESS};
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_map<Key, Value> data_;
    DatabaseConfig config_;
    bool is_open_ = false;
};

// Factory functions
std::unique_ptr<Database> create_database(const std::string& backend) {
    if (backend == "memory" || backend == "rocksdb") {
        // Provide in-memory implementation for both backends for now
        return std::make_unique<MemoryDatabase>();
    }
    return nullptr;  // Unknown backend
}

std::unique_ptr<WriteBatch> create_write_batch() {
    return std::make_unique<MemoryWriteBatch>();
}

} // namespace chainforge::storage
