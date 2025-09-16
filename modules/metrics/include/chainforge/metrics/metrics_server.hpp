#pragma once

#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <prometheus/exposer.h>

namespace chainforge::metrics {

class MetricsRegistry;

/// Configuration for metrics server
struct MetricsServerConfig {
    std::string host = "0.0.0.0";
    uint16_t port = 8080;
    std::string path = "/metrics";
    size_t thread_pool_size = 1;
    bool enable_compression = true;
    
    /// Validate configuration
    bool is_valid() const;
    
    /// Get full bind address
    std::string bind_address() const;
};

/// HTTP server for exposing Prometheus metrics
class MetricsServer {
public:
    /// Constructor
    explicit MetricsServer(const MetricsServerConfig& config);
    
    /// Destructor
    ~MetricsServer();
    
    // Disable copy and move
    MetricsServer(const MetricsServer&) = delete;
    MetricsServer& operator=(const MetricsServer&) = delete;
    MetricsServer(MetricsServer&&) = delete;
    MetricsServer& operator=(MetricsServer&&) = delete;
    
    /// Start the metrics server
    bool start();
    
    /// Stop the metrics server
    void stop();
    
    /// Check if server is running
    bool is_running() const { return running_.load(); }
    
    /// Get server configuration
    const MetricsServerConfig& config() const { return config_; }
    
    /// Get metrics endpoint URL
    std::string metrics_url() const;

private:
    MetricsServerConfig config_;
    std::unique_ptr<prometheus::Exposer> exposer_;
    std::atomic<bool> running_;
    
    /// Initialize exposer
    bool initialize_exposer();
};

/// Global metrics server management
class GlobalMetricsServer {
public:
    /// Get singleton instance
    static GlobalMetricsServer& instance();
    
    /// Initialize and start global metrics server
    bool initialize(const MetricsServerConfig& config);
    
    /// Stop global metrics server
    void shutdown();
    
    /// Get current server (may be nullptr)
    std::shared_ptr<MetricsServer> server() const;
    
    /// Check if global server is running
    bool is_running() const;

private:
    GlobalMetricsServer() = default;
    ~GlobalMetricsServer() = default;
    
    // Disable copy and move
    GlobalMetricsServer(const GlobalMetricsServer&) = delete;
    GlobalMetricsServer& operator=(const GlobalMetricsServer&) = delete;
    GlobalMetricsServer(GlobalMetricsServer&&) = delete;
    GlobalMetricsServer& operator=(GlobalMetricsServer&&) = delete;
    
    mutable std::mutex mutex_;
    std::shared_ptr<MetricsServer> server_;
};

/// Convenience functions
bool start_metrics_server(const MetricsServerConfig& config = {});
void stop_metrics_server();
bool is_metrics_server_running();
std::string get_metrics_url();

} // namespace chainforge::metrics
