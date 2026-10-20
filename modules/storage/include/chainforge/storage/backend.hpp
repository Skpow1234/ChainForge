#pragma once

#include "chainforge/storage/database.hpp"
#include <memory>

namespace chainforge::storage {

/// True when this build was linked against RocksDB.
bool rocksdb_backend_available();

/// Create a RocksDB-backed database, or nullptr if unavailable.
std::unique_ptr<Database> create_rocksdb_database();

} // namespace chainforge::storage
