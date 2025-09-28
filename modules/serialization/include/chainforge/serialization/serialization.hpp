#pragma once

// Main serialization module header
// Includes all serialization functionality

#include "serializer.hpp"
#include "validator.hpp"

namespace chainforge {
namespace serialization {

/**
 * @brief Serialization module for ChainForge blockchain data
 *
 * This module provides:
 * - Protocol buffer-based serialization/deserialization
 * - Data validation
 * - Forward compatibility
 * - Type safety
 *
 * Usage:
 * @code
 * auto serializer = create_serializer();
 * auto validator = create_validator();
 *
 * // Serialize a block
 * auto result = serializer->serialize_block(block);
 * if (result.has_value()) {
 *     // Use serialized data
 *     auto data = result.value();
 * }
 *
 * // Deserialize and validate
 * auto deserialize_result = serializer->deserialize_block(data);
 * if (deserialize_result.has_value()) {
 *     auto block = std::move(deserialize_result.value());
 *     auto validation = validator->validate_block(*block);
 *     if (validation.has_value()) {
 *         // Block is valid
 *     }
 * }
 * @endcode
 */

} // namespace serialization
} // namespace chainforge
