# RPC module - JSON-RPC server for blockchain communication
# Provides HTTP server with JSON-RPC protocol and blockchain API endpoints

set(RPC_SOURCES
    src/rpc_server_impl.cpp
    src/http_server.cpp
    src/blockchain_rpc_methods.cpp
)

set(RPC_HEADERS
    include/chainforge/rpc/rpc_server.hpp
    src/http_server.hpp
    src/blockchain_rpc_methods.hpp
)

add_library(chainforge-rpc STATIC ${RPC_SOURCES} ${RPC_HEADERS})

target_include_directories(chainforge-rpc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(chainforge-rpc PRIVATE cxx_std_20)

# Link dependencies
find_conan_package(nlohmann_json)
target_link_libraries(chainforge-rpc
    PUBLIC
        nlohmann_json::nlohmann_json
    PRIVATE
        chainforge-core
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(chainforge-rpc PRIVATE ws2_32)
endif()

# Install
install(TARGETS chainforge-rpc
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
