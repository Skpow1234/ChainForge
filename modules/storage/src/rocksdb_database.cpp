#include "chainforge/storage/database.hpp"
#include <memory>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>

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
    ~MemoryDatabase() override {
        if (is_open_) {
            data_.clear();
            is_open_ = false;
        }
    }

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

// Simple file-backed KV (length-prefixed records). Used when RocksDB is unavailable.
class FileDatabase : public MemoryDatabase {
public:
    StorageResult<void> open(const DatabaseConfig& config) override {
        auto base = MemoryDatabase::open(config);
        if (!base.success()) {
            return base;
        }
        path_ = config.path.empty() ? std::string("./data/chainforge.kv") : config.path;
        load_from_disk();
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> close() override {
        save_to_disk();
        return MemoryDatabase::close();
    }

    StorageResult<void> put(const Key& key, const Value& value, const WriteOptions& options) override {
        auto r = MemoryDatabase::put(key, value, options);
        if (r.success() && options.sync) {
            save_to_disk();
        }
        return r;
    }

    StorageResult<void> flush(const WriteOptions& options) override {
        (void)options;
        save_to_disk();
        return StorageResult<void>{StorageError::SUCCESS};
    }

private:
    std::string path_;

    void load_from_disk() {
        // Access protected via public API only — re-open empty and import.
        // MemoryDatabase stores privately; use get/put after reading file into temp map.
        std::ifstream in(path_, std::ios::binary);
        if (!in) return;
        while (in.peek() != EOF) {
            uint64_t klen = 0, vlen = 0;
            in.read(reinterpret_cast<char*>(&klen), sizeof(klen));
            if (!in) break;
            in.read(reinterpret_cast<char*>(&vlen), sizeof(vlen));
            if (!in) break;
            Key key(klen);
            Value value(vlen);
            in.read(reinterpret_cast<char*>(key.data()), static_cast<std::streamsize>(klen));
            in.read(reinterpret_cast<char*>(value.data()), static_cast<std::streamsize>(vlen));
            if (!in) break;
            MemoryDatabase::put(key, value, {});
        }
    }

    void save_to_disk() {
        // Export via iterator
        auto it = new_iterator({});
        if (!it) return;
        std::ofstream out(path_, std::ios::binary | std::ios::trunc);
        if (!out) return;
        for (it->seek_to_first(); it->valid(); it->next()) {
            const auto& key = it->key();
            const auto& value = it->value();
            uint64_t klen = key.size();
            uint64_t vlen = value.size();
            out.write(reinterpret_cast<const char*>(&klen), sizeof(klen));
            out.write(reinterpret_cast<const char*>(&vlen), sizeof(vlen));
            out.write(reinterpret_cast<const char*>(key.data()), static_cast<std::streamsize>(klen));
            out.write(reinterpret_cast<const char*>(value.data()), static_cast<std::streamsize>(vlen));
        }
    }
};

// Factory functions
std::unique_ptr<Database> create_database(const std::string& backend) {
    if (backend == "memory") {
        return std::make_unique<MemoryDatabase>();
    }
    if (backend == "file" || backend == "rocksdb") {
        // RocksDB real backend is gated; file-backed KV is the durable default today.
        return std::make_unique<FileDatabase>();
    }
    return nullptr;
}

std::unique_ptr<WriteBatch> create_write_batch() {
    return std::make_unique<MemoryWriteBatch>();
}

} // namespace chainforge::storage
