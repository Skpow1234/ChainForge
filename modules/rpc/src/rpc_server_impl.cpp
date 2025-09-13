#include "rpc_server_impl.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace chainforge::rpc {

// JsonRpcRequest implementation
std::optional<JsonRpcRequest> JsonRpcRequest::from_json(const std::string& json_str) {
    try {
        auto json = nlohmann::json::parse(json_str);

        if (!json.contains("jsonrpc") || !json.contains("method")) {
            return std::nullopt;
        }

        JsonRpcRequest request;
        request.jsonrpc = json["jsonrpc"];
        request.method = json["method"];

        if (json.contains("params")) {
            request.params = json["params"];
        }

        if (json.contains("id")) {
            if (json["id"].is_string()) {
                request.id = json["id"];
            } else if (json["id"].is_number()) {
                request.id = std::to_string(json["id"].get<int>());
            }
        }

        return request;
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

nlohmann::json JsonRpcRequest::to_json() const {
    nlohmann::json json;
    json["jsonrpc"] = jsonrpc;
    json["method"] = method;
    json["params"] = params;
    if (id.has_value()) {
        json["id"] = *id;
    }
    return json;
}

// JsonRpcResponse implementation
nlohmann::json JsonRpcResponse::to_json() const {
    nlohmann::json json;
    json["jsonrpc"] = jsonrpc;

    if (result.has_value()) {
        json["result"] = *result;
    }

    if (error.has_value()) {
        json["error"] = nlohmann::json{
            {"code", error->code},
            {"message", error->message}
        };
        if (error->data.has_value()) {
            json["error"]["data"] = *(error->data);
        }
    }

    if (id.has_value()) {
        json["id"] = *id;
    }

    return json;
}

// JsonRpcError implementation
JsonRpcError JsonRpcError::parse_error(const std::string& message) {
    return {-32700, message};
}

JsonRpcError JsonRpcError::invalid_request(const std::string& message) {
    return {-32600, message};
}

JsonRpcError JsonRpcError::method_not_found(const std::string& message) {
    return {-32601, message};
}

JsonRpcError JsonRpcError::invalid_params(const std::string& message) {
    return {-32602, message};
}

JsonRpcError JsonRpcError::internal_error(const std::string& message) {
    return {-32603, message};
}

JsonRpcError JsonRpcError::server_error(int code, const std::string& message) {
    return {code, message};
}

// HttpResponse implementation
HttpResponse HttpResponse::ok(const std::string& body, const std::string& content_type) {
    HttpResponse response;
    response.status_code = 200;
    response.status_message = "OK";
    response.body = body;
    response.content_type = content_type;
    response.headers["Content-Type"] = content_type;
    response.headers["Content-Length"] = std::to_string(body.size());
    return response;
}

HttpResponse HttpResponse::bad_request(const std::string& message) {
    HttpResponse response;
    response.status_code = 400;
    response.status_message = "Bad Request";
    nlohmann::json error = {
        {"error", {
            {"code", -32600},
            {"message", message}
        }}
    };
    response.body = error.dump();
    response.content_type = "application/json";
    response.headers["Content-Type"] = "application/json";
    response.headers["Content-Length"] = std::to_string(response.body.size());
    return response;
}

HttpResponse HttpResponse::not_found(const std::string& message) {
    HttpResponse response;
    response.status_code = 404;
    response.status_message = "Not Found";
    response.body = message;
    response.content_type = "text/plain";
    response.headers["Content-Type"] = "text/plain";
    response.headers["Content-Length"] = std::to_string(response.body.size());
    return response;
}

HttpResponse HttpResponse::internal_error(const std::string& message) {
    HttpResponse response;
    response.status_code = 500;
    response.status_message = "Internal Server Error";
    nlohmann::json error = {
        {"error", {
            {"code", -32603},
            {"message", message}
        }}
    };
    response.body = error.dump();
    response.content_type = "application/json";
    response.headers["Content-Type"] = "application/json";
    response.headers["Content-Length"] = std::to_string(response.body.size());
    return response;
}

// RpcServerImpl implementation
RpcServerImpl::RpcServerImpl() = default;

RpcServerImpl::~RpcServerImpl() {
    stop();
}

bool RpcServerImpl::start(const RpcServerConfig& config) {
    config_ = config;
    http_server_ = std::make_unique<HttpServer>(config_);

    http_server_->set_request_handler([this](const HttpRequest& request) {
        return this->handle_http_request(request);
    });

    return http_server_->start();
}

void RpcServerImpl::stop() {
    if (http_server_) {
        http_server_->stop();
        http_server_.reset();
    }
}

bool RpcServerImpl::is_running() const {
    return http_server_ && http_server_->is_running();
}

void RpcServerImpl::register_method(const std::string& method_name, RpcMethodHandler handler) {
    std::lock_guard<std::mutex> lock(methods_mutex_);
    methods_[method_name] = std::move(handler);
}

void RpcServerImpl::unregister_method(const std::string& method_name) {
    std::lock_guard<std::mutex> lock(methods_mutex_);
    methods_.erase(method_name);
}

bool RpcServerImpl::has_method(const std::string& method_name) const {
    std::lock_guard<std::mutex> lock(methods_mutex_);
    return methods_.find(method_name) != methods_.end();
}

RpcServerConfig RpcServerImpl::get_config() const {
    return config_;
}

std::string RpcServerImpl::get_server_info() const {
    std::stringstream ss;
    ss << "ChainForge RPC Server v0.1.0\n";
    ss << "Host: " << config_.host << "\n";
    ss << "Port: " << config_.port << "\n";
    ss << "Methods registered: " << methods_.size() << "\n";
    return ss.str();
}

HttpResponse RpcServerImpl::handle_http_request(const HttpRequest& request) {
    // Handle CORS preflight requests
    if (request.method == "OPTIONS") {
        HttpResponse response = HttpResponse::ok("");
        add_cors_headers(response);
        return response;
    }

    // Only accept POST requests for JSON-RPC
    if (request.method != "POST") {
        return HttpResponse::bad_request("Only POST requests are supported");
    }

    // Parse JSON-RPC request
    auto jsonrpc_request = JsonRpcRequest::from_json(request.body);
    if (!jsonrpc_request) {
        JsonRpcResponse error_response;
        error_response.error = JsonRpcError::parse_error();
        return HttpResponse::ok(error_response.to_json().dump());
    }

    // Process the request
    JsonRpcResponse response = process_jsonrpc_request(*jsonrpc_request);
    if (jsonrpc_request->id.has_value()) {
        response.id = jsonrpc_request->id;
    }

    HttpResponse http_response = HttpResponse::ok(response.to_json().dump());
    add_cors_headers(http_response);
    return http_response;
}

JsonRpcResponse RpcServerImpl::process_jsonrpc_request(const JsonRpcRequest& request) {
    // Validate request
    if (request.jsonrpc != "2.0" || request.method.empty()) {
        JsonRpcResponse response;
        response.error = JsonRpcError::invalid_request();
        return response;
    }

    // Check if method exists
    {
        std::lock_guard<std::mutex> lock(methods_mutex_);
        if (methods_.find(request.method) == methods_.end()) {
            JsonRpcResponse response;
            response.error = JsonRpcError::method_not_found();
            return response;
        }
    }

    // Execute method
    try {
        std::lock_guard<std::mutex> lock(methods_mutex_);
        return methods_[request.method](request.params);
    } catch (const std::exception& e) {
        JsonRpcResponse response;
        response.error = JsonRpcError::internal_error(e.what());
        return response;
    }
}

void RpcServerImpl::add_cors_headers(HttpResponse& response) const {
    if (!config_.enable_cors) {
        return;
    }

    response.headers["Access-Control-Allow-Origin"] = config_.allowed_origins.empty() ? "*" : config_.allowed_origins[0];
    response.headers["Access-Control-Allow-Methods"] = "POST, OPTIONS";
    response.headers["Access-Control-Allow-Headers"] = "Content-Type";
    response.headers["Access-Control-Max-Age"] = "86400";
}

// Factory functions
std::unique_ptr<RpcServer> create_rpc_server() {
    return std::make_unique<RpcServerImpl>();
}

} // namespace chainforge::rpc
