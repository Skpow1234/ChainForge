#pragma once

// Main network transport header - includes all transport components

#include "tcp_connection.hpp"
#include "tcp_server.hpp"
#include "tcp_client.hpp"
#include "udp_transport.hpp"

/**
 * @file network.hpp
 * @brief Main header for ChainForge network transport layer
 * 
 * This header provides convenient access to all network transport components:
 * - TcpConnection: Low-level TCP socket wrapper
 * - TcpServer: Multi-client TCP server
 * - TcpClient: TCP client with auto-reconnect
 * - UdpTransport: UDP datagram transport
 * 
 * Usage:
 * @code
 * #include "chainforge/p2p/network.hpp"
 * 
 * using namespace chainforge::p2p;
 * 
 * asio::io_context io_context;
 * 
 * // TCP Server
 * TcpServer server(io_context);
 * server.start(8080);
 * 
 * // TCP Client
 * TcpClient client(io_context);
 * client.connect("127.0.0.1", 8080);
 * 
 * // UDP Transport
 * UdpTransport udp(io_context);
 * udp.bind(9999);
 * @endcode
 */

namespace chainforge {
namespace p2p {

/**
 * @brief Network transport layer version
 */
constexpr int NETWORK_VERSION_MAJOR = 1;
constexpr int NETWORK_VERSION_MINOR = 0;
constexpr int NETWORK_VERSION_PATCH = 0;

} // namespace p2p
} // namespace chainforge

