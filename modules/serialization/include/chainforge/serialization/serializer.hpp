#pragma once

#include <vector>
#include <string>
#include <memory>
#include "chainforge/core/expected.hpp"
#include "chainforge/core/error.hpp"

namespace chainforge {

class Block;
class Transaction;
class Address;
class Amount;
class Timestamp;
class Hash;

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
    virtual SerializationResult<std::vector<uint8_t>> serialize_block(const Block& block) = 0;

    /**
     * @brief Deserialize bytes to a block
     */
    virtual SerializationResult<std::unique_ptr<Block>> deserialize_block(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize a transaction to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_transaction(const Transaction& tx) = 0;

    /**
     * @brief Deserialize bytes to a transaction
     */
    virtual SerializationResult<std::unique_ptr<Transaction>> deserialize_transaction(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize an address to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_address(const Address& addr) = 0;

    /**
     * @brief Deserialize bytes to an address
     */
    virtual SerializationResult<std::unique_ptr<Address>> deserialize_address(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize an amount to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_amount(const Amount& amount) = 0;

    /**
     * @brief Deserialize bytes to an amount
     */
    virtual SerializationResult<std::unique_ptr<Amount>> deserialize_amount(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize a timestamp to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_timestamp(const Timestamp& ts) = 0;

    /**
     * @brief Deserialize bytes to a timestamp
     */
    virtual SerializationResult<std::unique_ptr<Timestamp>> deserialize_timestamp(const std::vector<uint8_t>& data) = 0;

    /**
     * @brief Serialize a hash to bytes
     */
    virtual SerializationResult<std::vector<uint8_t>> serialize_hash(const Hash& hash) = 0;

    /**
     * @brief Deserialize bytes to a hash
     */
    virtual SerializationResult<std::unique_ptr<Hash>> deserialize_hash(const std::vector<uint8_t>& data) = 0;
};

/**
 * @brief Protobuf-based serializer implementation
 */
class ProtobufSerializer : public Serializer {
public:
    ProtobufSerializer();
    ~ProtobufSerializer() override;

    SerializationResult<std::vector<uint8_t>> serialize_block(const Block& block) override;
    SerializationResult<std::unique_ptr<Block>> deserialize_block(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_transaction(const Transaction& tx) override;
    SerializationResult<std::unique_ptr<Transaction>> deserialize_transaction(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_address(const Address& addr) override;
    SerializationResult<std::unique_ptr<Address>> deserialize_address(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_amount(const Amount& amount) override;
    SerializationResult<std::unique_ptr<Amount>> deserialize_amount(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_timestamp(const Timestamp& ts) override;
    SerializationResult<std::unique_ptr<Timestamp>> deserialize_timestamp(const std::vector<uint8_t>& data) override;

    SerializationResult<std::vector<uint8_t>> serialize_hash(const Hash& hash) override;
    SerializationResult<std::unique_ptr<Hash>> deserialize_hash(const std::vector<uint8_t>& data) override;

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
} // namespace chainforge
