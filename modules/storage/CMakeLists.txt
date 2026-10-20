# Storage module - Database abstraction layer
# Provides memory, file-KV, and optional RocksDB backends

set(STORAGE_SOURCES
    src/rocksdb_database.cpp
    src/rocksdb_impl.cpp
)

set(STORAGE_HEADERS
    include/tokiwa/storage/database.hpp
    include/tokiwa/storage/backend.hpp
)

add_library(tokiwa-storage STATIC ${STORAGE_SOURCES} ${STORAGE_HEADERS})

target_include_directories(tokiwa-storage
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(tokiwa-storage PRIVATE cxx_std_20)

option(TOKIWA_USE_ROCKSDB "Enable RocksDB storage backend when available" ON)

set(TOKIWA_HAS_ROCKSDB OFF)
if(TOKIWA_USE_ROCKSDB)
    find_path(ROCKSDB_INCLUDE_DIR NAMES rocksdb/db.h)
    find_library(ROCKSDB_LIBRARY NAMES rocksdb)
    if(ROCKSDB_INCLUDE_DIR AND ROCKSDB_LIBRARY)
        # Distro RocksDB headers (esp. Ubuntu) can be incompatible with C++20
        # libstdc++ (incomplete types in rocksdb/metadata.h). Probe before enabling.
        file(WRITE "${CMAKE_BINARY_DIR}/tokiwa_rocksdb_probe.cpp"
             "#include <rocksdb/db.h>\n"
             "#include <rocksdb/options.h>\n"
             "#include <rocksdb/write_batch.h>\n"
             "#include <rocksdb/iterator.h>\n"
             "int main() { rocksdb::Options opts; (void)opts; return 0; }\n")
        try_compile(TOKIWA_ROCKSDB_HEADERS_OK
            "${CMAKE_BINARY_DIR}/tokiwa_rocksdb_probe"
            "${CMAKE_BINARY_DIR}/tokiwa_rocksdb_probe.cpp"
            CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${ROCKSDB_INCLUDE_DIR}"
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
        )

        if(TOKIWA_ROCKSDB_HEADERS_OK)
            set(TOKIWA_HAS_ROCKSDB ON)
            target_compile_definitions(tokiwa-storage PRIVATE TOKIWA_HAS_ROCKSDB=1)
            target_include_directories(tokiwa-storage PRIVATE ${ROCKSDB_INCLUDE_DIR})
            target_link_libraries(tokiwa-storage PRIVATE ${ROCKSDB_LIBRARY})
            message(STATUS "Storage: RocksDB enabled (${ROCKSDB_LIBRARY})")
        else()
            message(WARNING "Storage: RocksDB found but headers incompatible with C++20; using file-KV fallback")
        endif()
    else()
        message(STATUS "Storage: RocksDB not found; file-KV used for rocksdb backend")
    endif()
endif()

target_link_libraries(tokiwa-storage
    PRIVATE
        tokiwa-core
)

install(TARGETS tokiwa-storage
    EXPORT TokiwaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
