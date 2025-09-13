# Storage module - Database abstraction layer
# Provides flexible storage backend with RocksDB implementation

set(STORAGE_SOURCES
    src/rocksdb_database.cpp
)

set(STORAGE_HEADERS
    include/chainforge/storage/database.hpp
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

# Link dependencies
find_conan_package(rocksdb)
target_link_libraries(chainforge-storage
    PUBLIC
        rocksdb::rocksdb
    PRIVATE
        chainforge-core
)

# Install
install(TARGETS chainforge-storage
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
