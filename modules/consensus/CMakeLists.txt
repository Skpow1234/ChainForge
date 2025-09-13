# Consensus module - Proof of Work consensus mechanism
# Provides mining, difficulty adjustment, and block validation

set(CONSENSUS_SOURCES
    src/consensus_impl.cpp
)

set(CONSENSUS_HEADERS
    include/chainforge/consensus/consensus.hpp
)

add_library(chainforge-consensus STATIC ${CONSENSUS_SOURCES} ${CONSENSUS_HEADERS})

target_include_directories(chainforge-consensus
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(chainforge-consensus PRIVATE cxx_std_20)

# Link dependencies
target_link_libraries(chainforge-consensus
    PUBLIC
        chainforge-core
    PRIVATE
        chainforge-crypto
)

# Install
install(TARGETS chainforge-consensus
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
