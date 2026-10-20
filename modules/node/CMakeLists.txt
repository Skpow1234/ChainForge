# Node composition root

set(NODE_SOURCES
    src/main.cpp
    src/config.cpp
)

add_executable(tokiwa-node ${NODE_SOURCES})

target_include_directories(tokiwa-node
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(tokiwa-node PRIVATE cxx_std_20)

target_link_libraries(tokiwa-node
    PRIVATE
        tokiwa-core
        tokiwa-crypto
        tokiwa-storage
        tokiwa-consensus
        tokiwa-mempool
        tokiwa-rpc
        tokiwa-p2p
)

install(TARGETS tokiwa-node
    EXPORT TokiwaTargets
    RUNTIME DESTINATION bin
)
