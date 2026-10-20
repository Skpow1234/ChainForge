# P2P module - Network transport layer
# Provides TCP/UDP communication for peer-to-peer networking

set(P2P_SOURCES
    src/tcp_connection.cpp
    src/tcp_server.cpp
    src/tcp_client.cpp
    src/udp_transport.cpp
    src/peer_address.cpp
    src/peer_discovery.cpp
    src/peer_manager.cpp
    src/message.cpp
    src/sync.cpp
)

set(P2P_HEADERS
    include/tokiwa/p2p/tcp_connection.hpp
    include/tokiwa/p2p/tcp_server.hpp
    include/tokiwa/p2p/tcp_client.hpp
    include/tokiwa/p2p/udp_transport.hpp
    include/tokiwa/p2p/peer_address.hpp
    include/tokiwa/p2p/peer_discovery.hpp
    include/tokiwa/p2p/peer_manager.hpp
    include/tokiwa/p2p/message.hpp
    include/tokiwa/p2p/sync.hpp
    include/tokiwa/p2p/network.hpp
)

add_library(tokiwa-p2p STATIC ${P2P_SOURCES} ${P2P_HEADERS})

target_include_directories(tokiwa-p2p
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(tokiwa-p2p PRIVATE cxx_std_20)

# Find dependencies
# Conan CMakeDeps exposes Boost as package name "Boost" (capital B)
find_conan_package(Boost)
find_conan_package(spdlog)

target_link_libraries(tokiwa-p2p
    PUBLIC
        tokiwa-core
        Boost::headers
    PRIVATE
        spdlog::spdlog
)

# Install
install(TARGETS tokiwa-p2p
    EXPORT TokiwaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

