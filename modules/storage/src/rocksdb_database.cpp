#include "chainforge/storage/database.hpp"
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/iterator.h>
#include <memory>
#include <mutex>

namespace chainforge::storage {

// RocksDB iterator implementation
class RocksDBIterator : public Iterator {
public:
    explicit RocksDBIterator(rocksdb::DB* db, const ReadOptions& options) {
        rocksdb::ReadOptions rocksdb_options;
        rocksdb_options.verify_checksums = options.verify_checksums;
        iterator_ = db->NewIterator(rocksdb_options);
    }

    ~RocksDBIterator() override = default;

    bool valid() const override {
        return iterator_->Valid();
    }

    void seek_to_first() override {
        iterator_->SeekToFirst();
    }

    void seek_to_last() override {
        iterator_->SeekToLast();
    }

    void seek(const Key& key) override {
        rocksdb::Slice key_slice(reinterpret_cast<const char*>(key.data()), key.size());
        iterator_->Seek(key_slice);
    }

    void next() override {
        iterator_->Next();
    }

    void prev() override {
        iterator_->Prev();
    }

    const Key& key() const override {
        const rocksdb::Slice& slice = iterator_->key();
        // Create a temporary Key from the slice
        static thread_local Key temp_key;
        temp_key.assign(slice.data(), slice.data() + slice.size());
        return temp_key;
    }

    const Value& value() const override {
        const rocksdb::Slice& slice = iterator_->value();
        // Create a temporary Value from the slice
        static thread_local Value temp_value;
        temp_value.assign(slice.data(), slice.data() + slice.size());
        return temp_value;
    }

    StorageError status() const override {
        if (!iterator_->status().ok()) {
            if (iterator_->status().IsNotFound()) {
                return StorageError::NOT_FOUND;
            } else if (iterator_->status().IsIOError()) {
                return StorageError::IO_ERROR;
            } else if (iterator_->status().IsCorruption()) {
                return StorageError::CORRUPTION;
            } else {
                return StorageError::IO_ERROR;
            }
        }
        return StorageError::SUCCESS;
    }

private:
    std::unique_ptr<rocksdb::Iterator> iterator_;
};

// RocksDB write batch implementation
class RocksDBWriteBatch : public WriteBatch {
public:
    RocksDBWriteBatch() : batch_(std::make_unique<rocksdb::WriteBatch>()) {}

    void put(const Key& key, const Value& value) override {
        rocksdb::Slice key_slice(reinterpret_cast<const char*>(key.data()), key.size());
        rocksdb::Slice value_slice(reinterpret_cast<const char*>(value.data()), value.size());
        batch_->Put(key_slice, value_slice);
    }

    void remove(const Key& key) override {
        rocksdb::Slice key_slice(reinterpret_cast<const char*>(key.data()), key.size());
        batch_->Delete(key_slice);
    }

    void clear() override {
        batch_ = std::make_unique<rocksdb::WriteBatch>();
    }

    size_t size() const override {
        return batch_->Count();
    }

    rocksdb::WriteBatch* get_batch() {
        return batch_.get();
    }

private:
    std::unique_ptr<rocksdb::WriteBatch> batch_;
};

// RocksDB database implementation
class RocksDBDatabase : public Database {
public:
    RocksDBDatabase() = default;
    ~RocksDBDatabase() override {
        if (db_) {
            close();
        }
    }

    StorageResult<void> open(const DatabaseConfig& config) override {
        if (db_) {
            return StorageResult<void>{StorageError::ALREADY_EXISTS};
        }

        rocksdb::Options options;
        options.create_if_missing = config.create_if_missing;
        options.error_if_exists = config.error_if_exists;
        options.write_buffer_size = config.write_buffer_size;
        options.max_open_files = config.max_open_files;
        options.max_background_jobs = config.max_background_jobs;

        if (config.compression) {
            options.compression = rocksdb::CompressionType::kLZ4Compression;
            options.compression_opts.level = config.compression_level;
        }

        // Block cache
        rocksdb::BlockBasedTableOptions table_options;
        table_options.block_cache = rocksdb::NewLRUCache(config.block_cache_size);
        options.table_factory.reset(rocksdb::NewBlockBasedTableFactory(table_options));

        rocksdb::Status status = rocksdb::DB::Open(options, config.path, &db_);
        if (!status.ok()) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        config_ = config;
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> close() override {
        if (!db_) {
            return StorageResult<void>{StorageError::SUCCESS};
        }

        rocksdb::Status status = db_->Close();
        delete db_;
        db_ = nullptr;

        if (!status.ok()) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        return StorageResult<void>{StorageError::SUCCESS};
    }

    bool is_open() const override {
        return db_ != nullptr;
    }

    StorageResult<Value> get(const Key& key, const ReadOptions& options) override {
        if (!db_) {
            return StorageResult<Value>{Value{}, StorageError::IO_ERROR};
        }

        rocksdb::ReadOptions rocksdb_options;
        rocksdb_options.verify_checksums = options.verify_checksums;

        rocksdb::Slice key_slice(reinterpret_cast<const char*>(key.data()), key.size());
        std::string value_str;

        rocksdb::Status status = db_->Get(rocksdb_options, key_slice, &value_str);
        if (status.IsNotFound()) {
            return StorageResult<Value>{Value{}, StorageError::NOT_FOUND};
        } else if (!status.ok()) {
            return StorageResult<Value>{Value{}, StorageError::IO_ERROR};
        }

        Value value(value_str.begin(), value_str.end());
        return StorageResult<Value>{value, StorageError::SUCCESS};
    }

    StorageResult<void> put(const Key& key, const Value& value, const WriteOptions& options) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        rocksdb::WriteOptions rocksdb_options;
        rocksdb_options.sync = options.sync;

        rocksdb::Slice key_slice(reinterpret_cast<const char*>(key.data()), key.size());
        rocksdb::Slice value_slice(reinterpret_cast<const char*>(value.data()), value.size());

        rocksdb::Status status = db_->Put(rocksdb_options, key_slice, value_slice);
        if (!status.ok()) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> remove(const Key& key, const WriteOptions& options) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        rocksdb::WriteOptions rocksdb_options;
        rocksdb_options.sync = options.sync;

        rocksdb::Slice key_slice(reinterpret_cast<const char*>(key.data()), key.size());

        rocksdb::Status status = db_->Delete(rocksdb_options, key_slice);
        if (status.IsNotFound()) {
            return StorageResult<void>{StorageError::NOT_FOUND};
        } else if (!status.ok()) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> write(WriteBatch& batch, const WriteOptions& options) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        auto* rocksdb_batch = dynamic_cast<RocksDBWriteBatch*>(&batch);
        if (!rocksdb_batch) {
            return StorageResult<void>{StorageError::INVALID_ARGUMENT};
        }

        rocksdb::WriteOptions rocksdb_options;
        rocksdb_options.sync = options.sync;

        rocksdb::Status status = db_->Write(rocksdb_options, rocksdb_batch->get_batch());
        if (!status.ok()) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        return StorageResult<void>{StorageError::SUCCESS};
    }

    std::unique_ptr<Iterator> new_iterator(const ReadOptions& options) override {
        if (!db_) {
            return nullptr;
        }
        return std::make_unique<RocksDBIterator>(db_, options);
    }

    StorageResult<void> flush(const WriteOptions& options) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        rocksdb::FlushOptions flush_options;
        flush_options.wait = options.sync;

        rocksdb::Status status = db_->Flush(flush_options);
        if (!status.ok()) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> compact_range(const Key* begin, const Key* end) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        rocksdb::CompactRangeOptions options;
        rocksdb::Slice* begin_slice = nullptr;
        rocksdb::Slice* end_slice = nullptr;

        rocksdb::Slice begin_slice_storage, end_slice_storage;
        if (begin) {
            begin_slice_storage = rocksdb::Slice(reinterpret_cast<const char*>(begin->data()), begin->size());
            begin_slice = &begin_slice_storage;
        }
        if (end) {
            end_slice_storage = rocksdb::Slice(reinterpret_cast<const char*>(end->data()), end->size());
            end_slice = &end_slice_storage;
        }

        rocksdb::Status status = db_->CompactRange(options, begin_slice, end_slice);
        if (!status.ok()) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }

        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<std::string> get_property(const std::string& property) override {
        if (!db_) {
            return StorageResult<std::string>{"", StorageError::IO_ERROR};
        }

        std::string value;
        bool found = db_->GetProperty(property, &value);
        if (!found) {
            return StorageResult<std::string>{"", StorageError::NOT_FOUND};
        }

        return StorageResult<std::string>{value, StorageError::SUCCESS};
    }

    StorageResult<uint64_t> get_approximate_size(const Key& start, const Key& limit) override {
        if (!db_) {
            return StorageResult<uint64_t>{0, StorageError::IO_ERROR};
        }

        rocksdb::Range range;
        range.start = rocksdb::Slice(reinterpret_cast<const char*>(start.data()), start.size());
        range.limit = rocksdb::Slice(reinterpret_cast<const char*>(limit.data()), limit.size());

        uint64_t size = 0;
        db_->GetApproximateSizes(&range, 1, &size);

        return StorageResult<uint64_t>{size, StorageError::SUCCESS};
    }

private:
    rocksdb::DB* db_ = nullptr;
    DatabaseConfig config_;
};

// Factory functions
std::unique_ptr<Database> create_database(const std::string& backend) {
    if (backend == "rocksdb") {
        return std::make_unique<RocksDBDatabase>();
    }
    return nullptr;  // Unknown backend
}

std::unique_ptr<WriteBatch> create_write_batch() {
    return std::make_unique<RocksDBWriteBatch>();
}

} // namespace chainforge::storage
