# P2P module - Network transport layer
# Provides TCP/UDP communication for peer-to-peer networking

set(P2P_SOURCES
    src/tcp_connection.cpp
    src/tcp_server.cpp
    src/tcp_client.cpp
    src/udp_transport.cpp
    src/peer_address.cpp
    src/peer_discovery.cpp
)

set(P2P_HEADERS
    include/chainforge/p2p/tcp_connection.hpp
    include/chainforge/p2p/tcp_server.hpp
    include/chainforge/p2p/tcp_client.hpp
    include/chainforge/p2p/udp_transport.hpp
    include/chainforge/p2p/peer_address.hpp
    include/chainforge/p2p/peer_discovery.hpp
    include/chainforge/p2p/network.hpp
)

add_library(chainforge-p2p STATIC ${P2P_SOURCES} ${P2P_HEADERS})

target_include_directories(chainforge-p2p
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(chainforge-p2p PRIVATE cxx_std_20)

# Find dependencies
find_conan_package(boost)
find_conan_package(spdlog)

target_link_libraries(chainforge-p2p
    PUBLIC
        chainforge-core
        boost::boost
    PRIVATE
        spdlog::spdlog
)

# Add Boost.Asio specific definitions
target_compile_definitions(chainforge-p2p
    PUBLIC
        BOOST_ASIO_NO_DEPRECATED
        BOOST_ASIO_STANDALONE
)

# Install
install(TARGETS chainforge-p2p
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)

