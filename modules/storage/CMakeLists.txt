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
        set(TOKIWA_HAS_ROCKSDB ON)
        target_compile_definitions(tokiwa-storage PRIVATE TOKIWA_HAS_ROCKSDB=1)
        target_include_directories(tokiwa-storage PRIVATE ${ROCKSDB_INCLUDE_DIR})
        target_link_libraries(tokiwa-storage PRIVATE ${ROCKSDB_LIBRARY})
        message(STATUS "Storage: RocksDB enabled (${ROCKSDB_LIBRARY})")
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
