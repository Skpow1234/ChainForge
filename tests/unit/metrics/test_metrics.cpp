#include <gtest/gtest.h>
#include "chainforge/metrics/metrics.hpp"
#include <thread>
#include <chrono>

using namespace chainforge::metrics;

class MetricsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Clear metrics registry for clean tests
        get_metrics_registry().clear();
    }
    
    void TearDown() override {
        // Stop metrics server if running
        stop_metrics_server();
        
        // Clear metrics registry
        get_metrics_registry().clear();
    }
};

TEST_F(MetricsTest, CounterBasicOperations) {
    auto& registry = get_metrics_registry();
    
    auto counter = registry.create_counter("test_counter", "Test counter");
    ASSERT_NE(counter, nullptr);
    
    // Initial value should be 0
    EXPECT_EQ(counter->value(), 0.0);
    
    // Test increment
    counter->increment();
    EXPECT_EQ(counter->value(), 1.0);
    
    counter->increment(5.0);
    EXPECT_EQ(counter->value(), 6.0);
    
    // Test that negative increments throw
    EXPECT_THROW(counter->increment(-1.0), std::invalid_argument);
}

TEST_F(MetricsTest, CounterWithLabels) {
    auto& registry = get_metrics_registry();
    
    std::map<std::string, std::string> labels1 = {{"method", "GET"}, {"status", "200"}};
    std::map<std::string, std::string> labels2 = {{"method", "POST"}, {"status", "404"}};
    
    auto counter1 = registry.create_counter("http_requests", "HTTP requests", labels1);
    auto counter2 = registry.create_counter("http_requests", "HTTP requests", labels2);
    
    ASSERT_NE(counter1, nullptr);
    ASSERT_NE(counter2, nullptr);
    EXPECT_NE(counter1, counter2);
    
    counter1->increment(10);
    counter2->increment(5);
    
    EXPECT_EQ(counter1->value(), 10.0);
    EXPECT_EQ(counter2->value(), 5.0);
}

TEST_F(MetricsTest, GaugeBasicOperations) {
    auto& registry = get_metrics_registry();
    
    auto gauge = registry.create_gauge("test_gauge", "Test gauge");
    ASSERT_NE(gauge, nullptr);
    
    // Initial value should be 0
    EXPECT_EQ(gauge->value(), 0.0);
    
    // Test set
    gauge->set(42.5);
    EXPECT_EQ(gauge->value(), 42.5);
    
    // Test increment/decrement
    gauge->increment(7.5);
    EXPECT_EQ(gauge->value(), 50.0);
    
    gauge->decrement(10.0);
    EXPECT_EQ(gauge->value(), 40.0);
}

TEST_F(MetricsTest, GaugeTracker) {
    auto& registry = get_metrics_registry();
    auto gauge = registry.create_gauge("resource_usage", "Resource usage");
    
    EXPECT_EQ(gauge->value(), 0.0);
    
    {
        GaugeTracker tracker(gauge, 5.0);
        EXPECT_EQ(gauge->value(), 5.0);
        
        tracker.update(8.0);
        EXPECT_EQ(gauge->value(), 8.0);
    }
    
    // After tracker is destroyed, gauge should be decremented
    EXPECT_EQ(gauge->value(), 0.0);
}

TEST_F(MetricsTest, HistogramBasicOperations) {
    auto& registry = get_metrics_registry();
    
    auto histogram = registry.create_histogram("test_histogram", "Test histogram");
    ASSERT_NE(histogram, nullptr);
    
    // Observe some values
    histogram->observe(0.1);
    histogram->observe(0.5);
    histogram->observe(1.0);
    histogram->observe(2.5);
    
    // Note: We can't easily test the internal state of Prometheus histograms
    // In a real scenario, these would be exposed via the metrics endpoint
}

TEST_F(MetricsTest, HistogramTimer) {
    auto& registry = get_metrics_registry();
    auto histogram = registry.create_histogram("request_duration", "Request duration");
    
    {
        HistogramTimer timer(histogram);
        
        // Simulate some work
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        auto elapsed = timer.elapsed();
        EXPECT_GT(elapsed.count(), 5000); // At least 5ms (allowing for variance)
    }
    // Timer destructor should record the elapsed time
}

TEST_F(MetricsTest, CustomHistogramBuckets) {
    auto& registry = get_metrics_registry();
    
    std::vector<double> custom_buckets = {0.001, 0.01, 0.1, 1.0, 10.0};
    auto histogram = registry.create_histogram(
        "custom_histogram", 
        "Custom histogram", 
        custom_buckets
    );
    
    ASSERT_NE(histogram, nullptr);
    
    // Test observations in different buckets
    histogram->observe(0.005);  // Should go in 0.01 bucket
    histogram->observe(0.5);    // Should go in 1.0 bucket
    histogram->observe(5.0);    // Should go in 10.0 bucket
}

TEST_F(MetricsTest, MetricsRegistry) {
    auto& registry = get_metrics_registry();
    
    EXPECT_EQ(registry.metrics_count(), 0);
    
    auto counter = registry.create_counter("counter1", "Counter 1");
    auto gauge = registry.create_gauge("gauge1", "Gauge 1");
    auto histogram = registry.create_histogram("histogram1", "Histogram 1");
    
    EXPECT_EQ(registry.metrics_count(), 3);
    
    // Creating same metrics should return existing instances
    auto counter2 = registry.create_counter("counter1", "Counter 1");
    EXPECT_EQ(counter, counter2);
    EXPECT_EQ(registry.metrics_count(), 3);
    
    registry.clear();
    EXPECT_EQ(registry.metrics_count(), 0);
}

TEST_F(MetricsTest, MetricsServerConfig) {
    MetricsServerConfig config;
    EXPECT_TRUE(config.is_valid());
    
    config.port = 0;
    EXPECT_FALSE(config.is_valid());
    
    config.port = 8080;
    config.host = "";
    EXPECT_FALSE(config.is_valid());
    
    config.host = "localhost";
    config.path = "";
    EXPECT_FALSE(config.is_valid());
    
    config.path = "/metrics";
    EXPECT_TRUE(config.is_valid());
    
    EXPECT_EQ(config.bind_address(), "localhost:8080");
}

TEST_F(MetricsTest, MetricsServer) {
    MetricsServerConfig config;
    config.host = "127.0.0.1";
    config.port = 18080; // Use different port to avoid conflicts
    
    // Start server
    EXPECT_TRUE(start_metrics_server(config));
    EXPECT_TRUE(is_metrics_server_running());
    
    std::string expected_url = "http://127.0.0.1:18080/metrics";
    EXPECT_EQ(get_metrics_url(), expected_url);
    
    // Stop server
    stop_metrics_server();
    EXPECT_FALSE(is_metrics_server_running());
    EXPECT_EQ(get_metrics_url(), "");
}

TEST_F(MetricsTest, ChainForgeMetrics) {
    // Initialize ChainForge metrics
    initialize_chainforge_metrics();
    
    auto& metrics = get_chainforge_metrics();
    
    // Test block metrics
    auto blocks_counter = metrics.blocks_processed_total();
    ASSERT_NE(blocks_counter, nullptr);
    blocks_counter->increment();
    EXPECT_EQ(blocks_counter->value(), 1.0);
    
    auto block_height = metrics.current_block_height();
    ASSERT_NE(block_height, nullptr);
    block_height->set(12345);
    EXPECT_EQ(block_height->value(), 12345.0);
    
    // Test transaction metrics
    auto tx_counter = metrics.transactions_processed_total();
    ASSERT_NE(tx_counter, nullptr);
    tx_counter->increment(10);
    EXPECT_EQ(tx_counter->value(), 10.0);
    
    // Test timing metrics
    auto block_duration = metrics.block_processing_duration();
    ASSERT_NE(block_duration, nullptr);
    block_duration->observe(1.5); // 1.5 seconds
    
    // Test network metrics
    auto peers = metrics.connected_peers();
    ASSERT_NE(peers, nullptr);
    peers->set(25);
    EXPECT_EQ(peers->value(), 25.0);
}

TEST_F(MetricsTest, ConvenienceMacros) {
    // Initialize so macros work
    initialize_chainforge_metrics();
    
    // Test counter macros
    CHAINFORGE_COUNTER_INC("test_macro_counter");
    CHAINFORGE_COUNTER_ADD("test_macro_counter", 5);
    
    // Test gauge macros  
    CHAINFORGE_GAUGE_SET("test_macro_gauge", 42);
    CHAINFORGE_GAUGE_INC("test_macro_gauge");
    CHAINFORGE_GAUGE_DEC("test_macro_gauge");
    
    // Test histogram timer macro
    {
        CHAINFORGE_HISTOGRAM_TIMER("test_macro_histogram");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Verify metrics were created (basic smoke test)
    auto& registry = get_metrics_registry();
    EXPECT_GT(registry.metrics_count(), 0);
}

TEST_F(MetricsTest, BucketDefinitions) {
    // Test that bucket definitions are reasonable
    EXPECT_FALSE(buckets::DEFAULT_TIMING.empty());
    EXPECT_FALSE(buckets::HTTP_REQUEST_DURATION.empty());
    EXPECT_FALSE(buckets::DB_QUERY_DURATION.empty());
    EXPECT_FALSE(buckets::SIZE_BYTES.empty());
    EXPECT_FALSE(buckets::BLOCK_PROCESSING.empty());
    EXPECT_FALSE(buckets::TRANSACTION_PROCESSING.empty());
    
    // Buckets should be in ascending order
    auto is_ascending = [](const std::vector<double>& buckets) {
        for (size_t i = 1; i < buckets.size(); ++i) {
            if (buckets[i] <= buckets[i-1]) return false;
        }
        return true;
    };
    
    EXPECT_TRUE(is_ascending(buckets::DEFAULT_TIMING));
    EXPECT_TRUE(is_ascending(buckets::HTTP_REQUEST_DURATION));
    EXPECT_TRUE(is_ascending(buckets::DB_QUERY_DURATION));
    EXPECT_TRUE(is_ascending(buckets::SIZE_BYTES));
    EXPECT_TRUE(is_ascending(buckets::BLOCK_PROCESSING));
    EXPECT_TRUE(is_ascending(buckets::TRANSACTION_PROCESSING));
}
