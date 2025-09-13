# Mempool module - Transaction pool management
# Provides transaction storage, validation, and fee-based ordering

set(MEMPOOL_SOURCES
    src/mempool_impl.cpp
)

set(MEMPOOL_HEADERS
    include/chainforge/mempool/mempool.hpp
    src/mempool_impl.hpp
)

add_library(chainforge-mempool STATIC ${MEMPOOL_SOURCES} ${MEMPOOL_HEADERS})

target_include_directories(chainforge-mempool
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(chainforge-mempool PRIVATE cxx_std_20)

# Link dependencies
target_link_libraries(chainforge-mempool
    PUBLIC
        chainforge-core
    PRIVATE
        chainforge-crypto
)

# Install
install(TARGETS chainforge-mempool
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
