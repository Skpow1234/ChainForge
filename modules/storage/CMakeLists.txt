# Storage module - Database abstraction layer
# Provides memory, file-KV, and optional RocksDB backends

set(STORAGE_SOURCES
    src/rocksdb_database.cpp
    src/rocksdb_impl.cpp
)

set(STORAGE_HEADERS
    include/chainforge/storage/database.hpp
    include/chainforge/storage/backend.hpp
)

add_library(chainforge-storage STATIC ${STORAGE_SOURCES} ${STORAGE_HEADERS})

target_include_directories(chainforge-storage
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(chainforge-storage PRIVATE cxx_std_20)

option(CHAINFORGE_USE_ROCKSDB "Enable RocksDB storage backend when available" ON)

set(CHAINFORGE_HAS_ROCKSDB OFF)
if(CHAINFORGE_USE_ROCKSDB)
    find_path(ROCKSDB_INCLUDE_DIR NAMES rocksdb/db.h)
    find_library(ROCKSDB_LIBRARY NAMES rocksdb)
    if(ROCKSDB_INCLUDE_DIR AND ROCKSDB_LIBRARY)
        set(CHAINFORGE_HAS_ROCKSDB ON)
        target_compile_definitions(chainforge-storage PRIVATE CHAINFORGE_HAS_ROCKSDB=1)
        target_include_directories(chainforge-storage PRIVATE ${ROCKSDB_INCLUDE_DIR})
        target_link_libraries(chainforge-storage PRIVATE ${ROCKSDB_LIBRARY})
        message(STATUS "Storage: RocksDB enabled (${ROCKSDB_LIBRARY})")
    else()
        message(STATUS "Storage: RocksDB not found; file-KV used for rocksdb backend")
    endif()
endif()

target_link_libraries(chainforge-storage
    PRIVATE
        chainforge-core
)

install(TARGETS chainforge-storage
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
