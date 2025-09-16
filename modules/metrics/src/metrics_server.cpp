#include "chainforge/metrics/metrics_server.hpp"
#include "chainforge/metrics/metrics_registry.hpp"
#include <sstream>
#include <stdexcept>
#include <mutex>

namespace chainforge::metrics {

bool MetricsServerConfig::is_valid() const {
    return port > 0 && port <= 65535 && !host.empty() && !path.empty();
}

std::string MetricsServerConfig::bind_address() const {
    std::ostringstream oss;
    oss << host << ":" << port;
    return oss.str();
}

MetricsServer::MetricsServer(const MetricsServerConfig& config)
    : config_(config), running_(false) {
    
    if (!config_.is_valid()) {
        throw std::invalid_argument("Invalid metrics server configuration");
    }
}

MetricsServer::~MetricsServer() {
    stop();
}

bool MetricsServer::start() {
    if (running_.load()) {
        return true; // Already running
    }
    
    try {
        if (!initialize_exposer()) {
            return false;
        }
        
        running_.store(true);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void MetricsServer::stop() {
    if (!running_.load()) {
        return; // Already stopped
    }
    
    exposer_.reset();
    running_.store(false);
}

std::string MetricsServer::metrics_url() const {
    std::ostringstream oss;
    oss << "http://" << config_.bind_address() << config_.path;
    return oss.str();
}

bool MetricsServer::initialize_exposer() {
    try {
        // Create Prometheus exposer
        exposer_ = std::make_unique<prometheus::Exposer>(config_.bind_address());
        
        // Register metrics registry
        auto& registry = get_metrics_registry();
        exposer_->RegisterCollectable(registry.prometheus_registry());
        
        return true;
    } catch (const std::exception&) {
        exposer_.reset();
        return false;
    }
}

GlobalMetricsServer& GlobalMetricsServer::instance() {
    static GlobalMetricsServer instance;
    return instance;
}

bool GlobalMetricsServer::initialize(const MetricsServerConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Stop existing server
    if (server_ && server_->is_running()) {
        server_->stop();
    }
    
    try {
        server_ = std::make_shared<MetricsServer>(config);
        return server_->start();
    } catch (const std::exception&) {
        server_.reset();
        return false;
    }
}

void GlobalMetricsServer::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (server_) {
        server_->stop();
        server_.reset();
    }
}

std::shared_ptr<MetricsServer> GlobalMetricsServer::server() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return server_;
}

bool GlobalMetricsServer::is_running() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return server_ && server_->is_running();
}

// Convenience functions
bool start_metrics_server(const MetricsServerConfig& config) {
    return GlobalMetricsServer::instance().initialize(config);
}

void stop_metrics_server() {
    GlobalMetricsServer::instance().shutdown();
}

bool is_metrics_server_running() {
    return GlobalMetricsServer::instance().is_running();
}

std::string get_metrics_url() {
    auto server = GlobalMetricsServer::instance().server();
    if (server && server->is_running()) {
        return server->metrics_url();
    }
    return "";
}

} // namespace chainforge::metrics
