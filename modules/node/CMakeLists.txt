# Node composition root

set(NODE_SOURCES
    src/main.cpp
    src/config.cpp
)

add_executable(chainforge-node ${NODE_SOURCES})

target_include_directories(chainforge-node
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(chainforge-node PRIVATE cxx_std_20)

target_link_libraries(chainforge-node
    PRIVATE
        chainforge-core
        chainforge-crypto
        chainforge-storage
        chainforge-consensus
        chainforge-mempool
        chainforge-rpc
        chainforge-p2p
)

install(TARGETS chainforge-node
    EXPORT ChainForgeTargets
    RUNTIME DESTINATION bin
)
