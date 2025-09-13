#include "http_server.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif

namespace chainforge::rpc {

class HttpServer::SocketImpl {
public:
    SocketImpl() {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
#endif
    }

    ~SocketImpl() {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    int create_socket() {
#ifdef _WIN32
        return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#else
        return socket(AF_INET, SOCK_STREAM, 0);
#endif
    }

    void close_socket(int socket) {
#ifdef _WIN32
        closesocket(socket);
#else
        close(socket);
#endif
    }

    bool set_non_blocking(int socket) {
#ifdef _WIN32
        u_long mode = 1;
        return ioctlsocket(socket, FIONBIO, &mode) == 0;
#else
        int flags = fcntl(socket, F_GETFL, 0);
        return fcntl(socket, F_SETFL, flags | O_NONBLOCK) == 0;
#endif
    }
};

HttpServer::HttpServer(const RpcServerConfig& config)
    : config_(config), running_(false), socket_impl_(std::make_unique<SocketImpl>()) {
}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start() {
    if (running_) {
        return true;
    }

    try {
        server_thread_ = std::thread(&HttpServer::server_loop, this);
        // Give the thread a moment to start
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return running_;
    } catch (const std::exception& e) {
        std::cerr << "Failed to start HTTP server: " << e.what() << std::endl;
        return false;
    }
}

void HttpServer::stop() {
    running_ = false;
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
}

bool HttpServer::is_running() const {
    return running_;
}

void HttpServer::set_request_handler(RequestHandler handler) {
    request_handler_ = std::move(handler);
}

void HttpServer::remove_request_handler() {
    request_handler_ = nullptr;
}

void HttpServer::server_loop() {
    int server_socket = socket_impl_->create_socket();
    if (server_socket < 0) {
        std::cerr << "Failed to create server socket" << std::endl;
        return;
    }

    // Set up server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config_.port);
    server_addr.sin_addr.s_addr = inet_addr(config_.host.c_str());

    // Bind socket
    if (bind(server_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind server socket" << std::endl;
        socket_impl_->close_socket(server_socket);
        return;
    }

    // Listen
    if (listen(server_socket, config_.max_connections) < 0) {
        std::cerr << "Failed to listen on server socket" << std::endl;
        socket_impl_->close_socket(server_socket);
        return;
    }

    running_ = true;
    std::cout << "RPC server listening on " << config_.host << ":" << config_.port << std::endl;

    while (running_) {
        sockaddr_in client_addr{};
        socklen_t client_addr_len = sizeof(client_addr);

        int client_socket = accept(server_socket,
                                 reinterpret_cast<sockaddr*>(&client_addr),
                                 &client_addr_len);

        if (client_socket >= 0) {
            // Handle connection in a separate thread or asynchronously
            std::thread(&HttpServer::handle_connection, this, client_socket).detach();
        } else {
            // Check if we should continue running
            if (!running_) break;

#ifdef _WIN32
            int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                std::cerr << "Accept failed with error: " << error << std::endl;
            }
#else
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "Accept failed with error: " << errno << std::endl;
            }
#endif
        }

        // Small delay to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    socket_impl_->close_socket(server_socket);
    running_ = false;
}

void HttpServer::handle_connection(int client_socket) {
    try {
        // Set socket timeout
        struct timeval timeout;
        timeout.tv_sec = config_.timeout_seconds;
        timeout.tv_usec = 0;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
        setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));

        // Read request
        const size_t buffer_size = 8192;
        char buffer[buffer_size];
        std::string request_data;

        ssize_t bytes_read;
        while ((bytes_read = recv(client_socket, buffer, buffer_size - 1, 0)) > 0) {
            buffer[bytes_read] = '\0';
            request_data += buffer;

            // Check if we have a complete HTTP request
            if (request_data.find("\r\n\r\n") != std::string::npos) {
                break;
            }
        }

        if (bytes_read < 0) {
            socket_impl_->close_socket(client_socket);
            return;
        }

        // Parse HTTP request
        HttpRequest request = parse_http_request(request_data);

        // Handle request
        HttpResponse response;
        if (request_handler_) {
            response = request_handler_(request);
        } else {
            response = HttpResponse::not_found("No request handler configured");
        }

        // Send response
        std::string response_str = format_http_response(response);
        send(client_socket, response_str.c_str(), response_str.size(), 0);

    } catch (const std::exception& e) {
        std::cerr << "Error handling connection: " << e.what() << std::endl;
    }

    socket_impl_->close_socket(client_socket);
}

HttpRequest HttpServer::parse_http_request(const std::string& request_data) const {
    HttpRequest request;
    std::istringstream stream(request_data);
    std::string line;

    // Parse request line
    if (std::getline(stream, line)) {
        std::istringstream line_stream(line);
        line_stream >> request.method >> request.path;

        // Parse query parameters
        size_t query_pos = request.path.find('?');
        if (query_pos != std::string::npos) {
            std::string query = request.path.substr(query_pos + 1);
            parse_query_string(query, request.query_params);
            request.path = request.path.substr(0, query_pos);
        }
    }

    // Parse headers
    std::vector<std::string> header_lines;
    while (std::getline(stream, line) && !line.empty()) {
        // Remove trailing \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (!line.empty()) {
            header_lines.push_back(line);
        }
    }
    parse_headers(header_lines, request.headers);

    // Parse body (for POST requests)
    if (request.method == "POST") {
        std::string content_length_str = request.headers["Content-Length"];
        if (!content_length_str.empty()) {
            try {
                size_t content_length = std::stoul(content_length_str);
                if (content_length > 0) {
                    request.body.resize(content_length);
                    stream.read(&request.body[0], content_length);
                }
            } catch (const std::exception&) {
                // Invalid content length, body will be empty
            }
        }
    }

    return request;
}

std::string HttpServer::format_http_response(const HttpResponse& response) const {
    std::ostringstream stream;

    // Status line
    stream << "HTTP/1.1 " << response.status_code << " " << response.status_message << "\r\n";

    // Headers
    for (const auto& [key, value] : response.headers) {
        stream << key << ": " << value << "\r\n";
    }

    // Content-Type and Content-Length if not already set
    if (response.headers.find("Content-Type") == response.headers.end()) {
        stream << "Content-Type: " << response.content_type << "\r\n";
    }
    if (response.headers.find("Content-Length") == response.headers.end()) {
        stream << "Content-Length: " << response.body.size() << "\r\n";
    }

    // End headers
    stream << "\r\n";

    // Body
    stream << response.body;

    return stream.str();
}

void HttpServer::parse_query_string(const std::string& query, std::unordered_map<std::string, std::string>& params) const {
    std::istringstream stream(query);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        size_t equals_pos = pair.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = pair.substr(0, equals_pos);
            std::string value = pair.substr(equals_pos + 1);
            params[key] = value;
        }
    }
}

void HttpServer::parse_headers(const std::vector<std::string>& header_lines,
                              std::unordered_map<std::string, std::string>& headers) const {
    for (const std::string& line : header_lines) {
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);

            // Trim whitespace
            key.erase(key.begin(), std::find_if(key.begin(), key.end(), [](int ch) { return !std::isspace(ch); }));
            key.erase(std::find_if(key.rbegin(), key.rend(), [](int ch) { return !std::isspace(ch); }).base(), key.end());

            value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](int ch) { return !std::isspace(ch); }));
            value.erase(std::find_if(value.rbegin(), value.rend(), [](int ch) { return !std::isspace(ch); }).base(), value.end());

            headers[key] = value;
        }
    }
}

} // namespace chainforge::rpc
