#include "chainforge/storage/database.hpp"

#ifdef CHAINFORGE_HAS_ROCKSDB
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/write_batch.h>
#include <rocksdb/iterator.h>
#endif

#include <memory>
#include <string>

namespace chainforge::storage {

#ifdef CHAINFORGE_HAS_ROCKSDB

namespace {

rocksdb::Slice to_slice(const Key& key) {
    return rocksdb::Slice(reinterpret_cast<const char*>(key.data()), key.size());
}

Key from_slice_key(const rocksdb::Slice& s) {
    return Key(reinterpret_cast<const uint8_t*>(s.data()),
               reinterpret_cast<const uint8_t*>(s.data()) + s.size());
}

Value from_slice_value(const rocksdb::Slice& s) {
    return Value(reinterpret_cast<const uint8_t*>(s.data()),
                 reinterpret_cast<const uint8_t*>(s.data()) + s.size());
}

StorageError map_status(const rocksdb::Status& st) {
    if (st.ok()) return StorageError::SUCCESS;
    if (st.IsNotFound()) return StorageError::NOT_FOUND;
    if (st.IsCorruption()) return StorageError::CORRUPTION;
    if (st.IsInvalidArgument()) return StorageError::INVALID_ARGUMENT;
    if (st.IsTimedOut()) return StorageError::TIMEOUT;
    if (st.IsBusy() || st.IsLockLimit()) return StorageError::LOCK_CONFLICT;
    return StorageError::IO_ERROR;
}

} // namespace

class RocksDBIterator : public Iterator {
public:
    explicit RocksDBIterator(std::unique_ptr<rocksdb::Iterator> it) : it_(std::move(it)) {}

    bool valid() const override {
        return it_ && it_->Valid();
    }

    void seek_to_first() override {
        if (it_) it_->SeekToFirst();
    }

    void seek_to_last() override {
        if (it_) it_->SeekToLast();
    }

    void seek(const Key& key) override {
        if (it_) it_->Seek(to_slice(key));
    }

    void next() override {
        if (it_ && it_->Valid()) it_->Next();
    }

    void prev() override {
        if (it_ && it_->Valid()) it_->Prev();
    }

    const Key& key() const override {
        static const Key empty;
        if (!valid()) return empty;
        key_buf_ = from_slice_key(it_->key());
        return key_buf_;
    }

    const Value& value() const override {
        static const Value empty;
        if (!valid()) return empty;
        val_buf_ = from_slice_value(it_->value());
        return val_buf_;
    }

    StorageError status() const override {
        if (!it_) return StorageError::IO_ERROR;
        return map_status(it_->status());
    }

private:
    std::unique_ptr<rocksdb::Iterator> it_;
    mutable Key key_buf_;
    mutable Value val_buf_;
};

class RocksDBDatabase : public Database {
public:
    RocksDBDatabase() = default;
    ~RocksDBDatabase() override {
        close();
    }

    StorageResult<void> open(const DatabaseConfig& config) override {
        if (db_) {
            return StorageResult<void>{StorageError::ALREADY_EXISTS};
        }

        rocksdb::Options options;
        options.create_if_missing = config.create_if_missing;
        options.error_if_exists = config.error_if_exists;
        options.write_buffer_size = config.write_buffer_size;
        options.max_open_files = static_cast<int>(config.max_open_files);
        options.max_background_jobs = static_cast<int>(config.max_background_jobs);
        // Prefer no compression to avoid optional codec deps (snappy/lz4) at link time.
        options.compression = rocksdb::kNoCompression;
        (void)config.compression;

        rocksdb::DB* raw = nullptr;
        const auto path = config.path.empty() ? std::string("./data/chainforge.rdb") : config.path;
        auto st = rocksdb::DB::Open(options, path, &raw);
        if (!st.ok()) {
            return StorageResult<void>{map_status(st)};
        }
        db_.reset(raw);
        path_ = path;
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<void> close() override {
        db_.reset();
        return StorageResult<void>{StorageError::SUCCESS};
    }

    bool is_open() const override {
        return static_cast<bool>(db_);
    }

    StorageResult<Value> get(const Key& key, const ReadOptions& options) override {
        if (!db_) {
            return StorageResult<Value>{Value{}, StorageError::IO_ERROR};
        }
        rocksdb::ReadOptions ro;
        ro.verify_checksums = options.verify_checksums;
        std::string value;
        auto st = db_->Get(ro, to_slice(key), &value);
        if (st.IsNotFound()) {
            return StorageResult<Value>{Value{}, StorageError::NOT_FOUND};
        }
        if (!st.ok()) {
            return StorageResult<Value>{Value{}, map_status(st)};
        }
        return StorageResult<Value>{
            Value(value.begin(), value.end()),
            StorageError::SUCCESS};
    }

    StorageResult<void> put(const Key& key, const Value& value, const WriteOptions& options) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }
        rocksdb::WriteOptions wo;
        wo.sync = options.sync;
        auto st = db_->Put(wo, to_slice(key),
                           rocksdb::Slice(reinterpret_cast<const char*>(value.data()), value.size()));
        return StorageResult<void>{map_status(st)};
    }

    StorageResult<void> remove(const Key& key, const WriteOptions& options) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }
        rocksdb::WriteOptions wo;
        wo.sync = options.sync;
        auto st = db_->Delete(wo, to_slice(key));
        return StorageResult<void>{map_status(st)};
    }

    StorageResult<void> write(WriteBatch& batch, const WriteOptions& options) override {
        (void)batch;
        (void)options;
        // Callers currently use put/remove directly; batch path reserved.
        return StorageResult<void>{StorageError::NOT_SUPPORTED};
    }

    std::unique_ptr<Iterator> new_iterator(const ReadOptions& options) override {
        if (!db_) {
            return nullptr;
        }
        rocksdb::ReadOptions ro;
        ro.verify_checksums = options.verify_checksums;
        return std::make_unique<RocksDBIterator>(
            std::unique_ptr<rocksdb::Iterator>(db_->NewIterator(ro)));
    }

    StorageResult<void> flush(const WriteOptions& options) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }
        rocksdb::FlushOptions fo;
        fo.wait = options.sync;
        return StorageResult<void>{map_status(db_->Flush(fo))};
    }

    StorageResult<void> compact_range(const Key* begin, const Key* end) override {
        if (!db_) {
            return StorageResult<void>{StorageError::IO_ERROR};
        }
        rocksdb::Slice b, e;
        const rocksdb::Slice* begin_ptr = nullptr;
        const rocksdb::Slice* end_ptr = nullptr;
        if (begin) {
            b = to_slice(*begin);
            begin_ptr = &b;
        }
        if (end) {
            e = to_slice(*end);
            end_ptr = &e;
        }
        db_->CompactRange(rocksdb::CompactRangeOptions(), begin_ptr, end_ptr);
        return StorageResult<void>{StorageError::SUCCESS};
    }

    StorageResult<std::string> get_property(const std::string& property) override {
        if (!db_) {
            return StorageResult<std::string>{"", StorageError::IO_ERROR};
        }
        std::string value;
        if (!db_->GetProperty(property, &value)) {
            return StorageResult<std::string>{"", StorageError::NOT_FOUND};
        }
        return StorageResult<std::string>{value, StorageError::SUCCESS};
    }

    StorageResult<uint64_t> get_approximate_size(const Key& start, const Key& limit) override {
        if (!db_) {
            return StorageResult<uint64_t>{0, StorageError::IO_ERROR};
        }
        rocksdb::Range range(to_slice(start), to_slice(limit));
        uint64_t size = 0;
        db_->GetApproximateSizes(&range, 1, &size);
        return StorageResult<uint64_t>{size, StorageError::SUCCESS};
    }

private:
    std::unique_ptr<rocksdb::DB> db_;
    std::string path_;
};

std::unique_ptr<Database> create_rocksdb_database() {
    return std::make_unique<RocksDBDatabase>();
}

bool rocksdb_backend_available() {
    return true;
}

#else // !CHAINFORGE_HAS_ROCKSDB

std::unique_ptr<Database> create_rocksdb_database() {
    return nullptr;
}

bool rocksdb_backend_available() {
    return false;
}

#endif

} // namespace chainforge::storage
