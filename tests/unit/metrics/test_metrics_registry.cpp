#include <gtest/gtest.h>
#include "chainforge/metrics/metrics.hpp"
#include <thread>
#include <chrono>
#include <vector>

using namespace chainforge::metrics;

class MetricsRegistryTest : public ::testing::Test {
protected:
    void SetUp() override {
        get_metrics_registry().clear();
    }
    
    void TearDown() override {
        get_metrics_registry().clear();
    }
};

TEST_F(MetricsRegistryTest, CreateCounter) {
    auto& registry = get_metrics_registry();
    
    auto counter = registry.create_counter("test_counter", "Test counter");
    ASSERT_NE(counter, nullptr);
    
    EXPECT_EQ(counter->name(), "test_counter");
    EXPECT_EQ(counter->help(), "Test counter");
    EXPECT_EQ(counter->value(), 0.0);
}

TEST_F(MetricsRegistryTest, CreateGauge) {
    auto& registry = get_metrics_registry();
    
    auto gauge = registry.create_gauge("test_gauge", "Test gauge");
    ASSERT_NE(gauge, nullptr);
    
    EXPECT_EQ(gauge->name(), "test_gauge");
    EXPECT_EQ(gauge->help(), "Test gauge");
    EXPECT_EQ(gauge->value(), 0.0);
}

TEST_F(MetricsRegistryTest, CreateHistogram) {
    auto& registry = get_metrics_registry();
    
    auto histogram = registry.create_histogram("test_histogram", "Test histogram");
    ASSERT_NE(histogram, nullptr);
    
    EXPECT_EQ(histogram->name(), "test_histogram");
    EXPECT_EQ(histogram->help(), "Test histogram");
    EXPECT_EQ(histogram->count(), 0);
}

TEST_F(MetricsRegistryTest, DuplicateMetricNames) {
    auto& registry = get_metrics_registry();
    
    auto counter1 = registry.create_counter("duplicate", "First counter");
    auto counter2 = registry.create_counter("duplicate", "Second counter");
    
    EXPECT_NE(counter1, nullptr);
    EXPECT_EQ(counter2, counter1); // Should return same instance
}

TEST_F(MetricsRegistryTest, GetExistingMetric) {
    auto& registry = get_metrics_registry();
    
    auto counter = registry.create_counter("existing", "Existing counter");
    ASSERT_NE(counter, nullptr);
    
    auto retrieved = registry.get_counter("existing");
    EXPECT_EQ(retrieved, counter);
}

TEST_F(MetricsRegistryTest, GetNonExistentMetric) {
    auto& registry = get_metrics_registry();
    
    auto counter = registry.get_counter("non_existent");
    EXPECT_EQ(counter, nullptr);
}

TEST_F(MetricsRegistryTest, ClearRegistry) {
    auto& registry = get_metrics_registry();
    
    auto counter = registry.create_counter("test", "Test");
    ASSERT_NE(counter, nullptr);
    
    registry.clear();
    
    auto retrieved = registry.get_counter("test");
    EXPECT_EQ(retrieved, nullptr);
}

TEST_F(MetricsRegistryTest, RegistrySize) {
    auto& registry = get_metrics_registry();
    
    EXPECT_EQ(registry.size(), 0);
    
    registry.create_counter("counter1", "Counter 1");
    EXPECT_EQ(registry.size(), 1);
    
    registry.create_gauge("gauge1", "Gauge 1");
    EXPECT_EQ(registry.size(), 2);
    
    registry.create_histogram("histogram1", "Histogram 1");
    EXPECT_EQ(registry.size(), 3);
}

TEST_F(MetricsRegistryTest, RegistryThreadSafety) {
    auto& registry = get_metrics_registry();
    
    const int num_threads = 4;
    const int metrics_per_thread = 100;
    
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&registry, t, metrics_per_thread]() {
            for (int i = 0; i < metrics_per_thread; ++i) {
                std::string name = "thread_" + std::to_string(t) + "_metric_" + std::to_string(i);
                registry.create_counter(name, "Test counter");
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(registry.size(), num_threads * metrics_per_thread);
}

TEST_F(MetricsRegistryTest, RegistryIteration) {
    auto& registry = get_metrics_registry();
    
    registry.create_counter("counter1", "Counter 1");
    registry.create_gauge("gauge1", "Gauge 1");
    registry.create_histogram("histogram1", "Histogram 1");
    
    std::set<std::string> names;
    for (const auto& [name, metric] : registry) {
        names.insert(name);
    }
    
    EXPECT_EQ(names.size(), 3);
    EXPECT_NE(names.find("counter1"), names.end());
    EXPECT_NE(names.find("gauge1"), names.end());
    EXPECT_NE(names.find("histogram1"), names.end());
}

TEST_F(MetricsRegistryTest, RegistryPerformance) {
    auto& registry = get_metrics_registry();
    
    const int num_metrics = 10000;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_metrics; ++i) {
        std::string name = "metric_" + std::to_string(i);
        registry.create_counter(name, "Test counter");
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    EXPECT_LT(duration.count(), 1000); // Should complete in less than 1 second
    EXPECT_EQ(registry.size(), num_metrics);
}

} // namespace chainforge::metrics::test
