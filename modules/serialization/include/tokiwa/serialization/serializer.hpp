#pragma once

#include <vector>
#include <string>
#include <memory>
#include <google/protobuf/message.h>
#include "tokiwa/core/expected.hpp"
#include "tokiwa/core/error.hpp"
#include "tokiwa/core/block.hpp"
#include "tokiwa/core/transaction.hpp"
#include "tokiwa/core/address.hpp"
#include "tokiwa/core/amount.hpp"
#include "tokiwa/core/timestamp.hpp"
#include "tokiwa/core/hash.hpp"

namespace tokiwa {
namespace serialization {

/**
 * @brief Serialization error codes
 */
enum class SerializationError {
    INVALID_DATA = 1,
    BUFFER_TOO_SMALL = 2,
    UNSUPPORTED_VERSION = 3,
    CORRUPTED_DATA = 4,
    ENCODING_ERROR = 5
};

/**
 * @brief Result type for serialization operations
 */
template<typename T>
using SerializationResult = core::Result<T>;

/**
 * @brief Interface for data serialization
 */
class Serializer {
public:
    virtual ~Serializer() = default;

    /**
     * @brief Serialize a block to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_block(const core::Block& block) = 0;

    /**
     * @brief Deserialize bytes to a block
     */
    virtual SerializationResult<std::unique_ptr<core::Block>> deserialize_block(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize a transaction to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_transaction(const core::Transaction& tx) = 0;

    /**
     * @brief Deserialize bytes to a transaction
     */
    virtual SerializationResult<std::unique_ptr<core::Transaction>> deserialize_transaction(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize an address to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_address(const core::Address& addr) = 0;

    /**
     * @brief Deserialize bytes to an address
     */
    virtual SerializationResult<std::unique_ptr<core::Address>> deserialize_address(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize an amount to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_amount(const core::Amount& amount) = 0;

    /**
     * @brief Deserialize bytes to an amount
     */
    virtual SerializationResult<std::unique_ptr<core::Amount>> deserialize_amount(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize a timestamp to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_timestamp(const core::Timestamp& ts) = 0;

    /**
     * @brief Deserialize bytes to a timestamp
     */
    virtual SerializationResult<std::unique_ptr<core::Timestamp>> deserialize_timestamp(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize a hash to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_hash(const core::Hash& hash) = 0;

    /**
     * @brief Deserialize bytes to a hash
     */
    virtual SerializationResult<std::unique_ptr<core::Hash>> deserialize_hash(const std::vector<uint8_t>& data) = 0;
};

/**
 * @brief Protobuf-based serializer implementation
 */
class ProtobufSerializer : public Serializer {
public:
    ProtobufSerializer();
    ~ProtobufSerializer() override;

    SerializationResult<std::vector<uint8_t>> serialize_block(const core::Block& block) override;
    SerializationResult<std::unique_ptr<core::Block>> deserialize_block(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_transaction(const core::Transaction& tx) override;
    SerializationResult<std::unique_ptr<core::Transaction>> deserialize_transaction(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_address(const core::Address& addr) override;
    SerializationResult<std::unique_ptr<core::Address>> deserialize_address(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_amount(const core::Amount& amount) override;
    SerializationResult<std::unique_ptr<core::Amount>> deserialize_amount(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_timestamp(const core::Timestamp& ts) override;
    SerializationResult<std::unique_ptr<core::Timestamp>> deserialize_timestamp(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_hash(const core::Hash& hash) override;
    SerializationResult<std::unique_ptr<core::Hash>> deserialize_hash(const std::vector<uint8_t>& data) override;

private:
    // Protobuf helper methods
    SerializationResult<std::vector<uint8_t>> serialize_to_bytes(const google::protobuf::Message& message);
    SerializationResult<std::unique_ptr<google::protobuf::Message>> deserialize_from_bytes(
        const std::vector<uint8_t>& data, google::protobuf::Message* prototype);
};

/**
 * @brief Create a default serializer instance
 */
std::unique_ptr<Serializer> create_serializer();

} // namespace serialization
} // namespace tokiwa
