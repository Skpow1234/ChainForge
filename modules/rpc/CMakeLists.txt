# RPC module - JSON-RPC server for blockchain communication

set(RPC_SOURCES
    src/rpc_server_impl.cpp
    src/http_server.cpp
    src/blockchain_rpc_methods.cpp
    src/chain_state.cpp
)

set(RPC_HEADERS
    include/tokiwa/rpc/rpc_server.hpp
    include/tokiwa/rpc/chain_state.hpp
    src/http_server.hpp
    src/blockchain_rpc_methods.hpp
)

add_library(tokiwa-rpc STATIC ${RPC_SOURCES} ${RPC_HEADERS})

target_include_directories(tokiwa-rpc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(tokiwa-rpc PRIVATE cxx_std_20)

find_conan_package(nlohmann_json)
target_link_libraries(tokiwa-rpc
    PUBLIC
        nlohmann_json::nlohmann_json
        tokiwa-core
        tokiwa-storage
        tokiwa-mempool
)

if(WIN32)
    target_link_libraries(tokiwa-rpc PRIVATE ws2_32)
endif()

install(TARGETS tokiwa-rpc
    EXPORT TokiwaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
