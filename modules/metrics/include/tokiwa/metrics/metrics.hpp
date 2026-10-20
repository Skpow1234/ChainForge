#pragma once

/// @file metrics.hpp
/// @brief Main include file for Tokiwa metrics system
/// 
/// This file provides a convenient way to include all metrics functionality.
/// Usage example:
/// 
/// @code
/// #include "tokiwa/metrics/metrics.hpp"
/// 
/// using namespace tokiwa::metrics;
/// 
/// int main() {
///     // Start metrics server
///     MetricsServerConfig config;
///     config.port = 8080;
///     start_metrics_server(config);
///     
///     // Create custom metrics
///     auto counter = get_metrics_registry().create_counter(
///         "my_counter", "Description of my counter");
///     
///     auto gauge = get_metrics_registry().create_gauge(
///         "my_gauge", "Description of my gauge");
///     
///     auto histogram = get_metrics_registry().create_histogram(
///         "my_histogram", "Description of my histogram");
///     
///     // Use metrics
///     counter->increment();
///     gauge->set(42.0);
///     histogram->observe(1.5);
///     
///     // Use Tokiwa-specific metrics
///     initialize_tokiwa_metrics();
///     auto& tokiwa_metrics = get_tokiwa_metrics();
///     tokiwa_metrics.blocks_processed_total()->increment();
///     
///     // Use convenience macros
///     TOKIWA_COUNTER_INC("api_requests");
///     TOKIWA_GAUGE_SET("active_connections", 10);
///     
///     {
///         TOKIWA_HISTOGRAM_TIMER("request_duration");
///         // ... do work ...
///     }
///     
///     // Stop metrics server
///     stop_metrics_server();
///     return 0;
/// }
/// @endcode

#include "metrics_registry.hpp"
#include "counter.hpp"
#include "gauge.hpp"
#include "histogram.hpp"
#include "metrics_server.hpp"
#include "tokiwa_metrics.hpp"
#include "logging_metrics.hpp"
