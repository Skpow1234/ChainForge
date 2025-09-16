#pragma once

/// @file logging.hpp
/// @brief Main include file for ChainForge logging system
/// 
/// This file provides a convenient way to include all logging functionality.
/// Usage example:
/// 
/// @code
/// #include "chainforge/logging/logging.hpp"
/// 
/// using namespace chainforge::logging;
/// 
/// int main() {
///     // Initialize logging
///     LogConfig config;
///     config.console_level = LogLevel::INFO;
///     config.file_level = LogLevel::DEBUG;
///     config.global_fields["service"] = "my-service";
///     
///     LogManager::instance().initialize(config);
///     
///     // Get logger
///     auto& logger = get_logger("main");
///     
///     // Simple logging
///     logger.info("Application started");
///     
///     // Structured logging
///     LogContext ctx;
///     ctx.with("user_id", 12345)
///        .with("action", "login");
///     logger.info("User action", ctx);
///     
///     // Performance logging
///     {
///         CHAINFORGE_PERF_TIMER(logger, "database_query");
///         // ... do work ...
///     }
///     
///     // Shutdown
///     LogManager::instance().shutdown();
///     return 0;
/// }
/// @endcode

#include "logger.hpp"
#include "json_formatter.hpp"
#include "performance_logger.hpp"
#include "log_manager.hpp"
#include "config_loader.hpp"
