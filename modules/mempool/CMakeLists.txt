# Mempool module - Transaction pool management
# Provides transaction storage, validation, and fee-based ordering

set(MEMPOOL_SOURCES
    src/mempool_impl.cpp
)

set(MEMPOOL_HEADERS
    include/tokiwa/mempool/mempool.hpp
    src/mempool_impl.hpp
)

add_library(tokiwa-mempool STATIC ${MEMPOOL_SOURCES} ${MEMPOOL_HEADERS})

target_include_directories(tokiwa-mempool
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(tokiwa-mempool PRIVATE cxx_std_20)

# Link dependencies
target_link_libraries(tokiwa-mempool
    PUBLIC
        tokiwa-core
    PRIVATE
        tokiwa-crypto
)

# Install
install(TARGETS tokiwa-mempool
    EXPORT TokiwaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
