# Consensus module - Proof of Work consensus mechanism
# Provides mining, difficulty adjustment, and block validation

set(CONSENSUS_SOURCES
    src/consensus_impl.cpp
)

set(CONSENSUS_HEADERS
    include/tokiwa/consensus/consensus.hpp
)

add_library(tokiwa-consensus STATIC ${CONSENSUS_SOURCES} ${CONSENSUS_HEADERS})

target_include_directories(tokiwa-consensus
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(tokiwa-consensus PRIVATE cxx_std_20)

# Link dependencies
target_link_libraries(tokiwa-consensus
    PUBLIC
        tokiwa-core
    PRIVATE
        tokiwa-crypto
)

# Install
install(TARGETS tokiwa-consensus
    EXPORT TokiwaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
