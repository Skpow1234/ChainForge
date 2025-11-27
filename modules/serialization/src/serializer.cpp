#include "chainforge/serialization/serializer.hpp"
#include "chainforge/core/block.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/core/timestamp.hpp"
#include "chainforge/core/hash.hpp"

// Protobuf generated headers (will be generated from .proto files)
#include "types.pb.h"
#include "transaction.pb.h"
#include "block.pb.h"

// Include protobuf runtime
#include <google/protobuf/message.h>
#include <google/protobuf/util/json_util.h>

namespace chainforge {
namespace serialization {

// Error handling helper
inline core::ErrorInfo make_serialization_error(SerializationError code, const std::string& message) {
    return core::ErrorInfo(
        static_cast<int>(code),
        message,
        "serialization",
        __FILE__,
        __LINE__
    );
}

ProtobufSerializer::ProtobufSerializer() {
    // Initialize protobuf if needed
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

ProtobufSerializer::~ProtobufSerializer() {
    // Shutdown protobuf
    google::protobuf::ShutdownProtobufLibrary();
}

SerializationResult<std::vector<uint8_t>> ProtobufSerializer::serialize_to_bytes(
    const google::protobuf::Message& message) {

    std::vector<uint8_t> result;
    size_t size = message.ByteSizeLong();
    result.resize(size);

    if (!message.SerializeToArray(result.data(), static_cast<int>(size))) {
        return make_serialization_error(
            SerializationError::ENCODING_ERROR,
            "Failed to serialize protobuf message"
        );
    }

    return result;
}

SerializationResult<std::unique_ptr<google::protobuf::Message>> ProtobufSerializer::deserialize_from_bytes(
    const std::vector<uint8_t>& data, google::protobuf::Message* prototype) {

    if (!prototype->ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        return make_serialization_error(
            SerializationError::CORRUPTED_DATA,
            "Failed to parse protobuf message"
        );
    }

    // Create a copy of the parsed message
    auto result = std::unique_ptr<google::protobuf::Message>(prototype->New());
    result->CopyFrom(*prototype);

    return result;
}

// Block serialization implementation
SerializationResult<std::vector<uint8_t>> ProtobufSerializer::serialize_block(const Block& block) {
    chainforge::block::Block proto_block;
    
    // Serialize header
    auto* proto_header = proto_block.mutable_header();
    proto_header->set_version(1);
    proto_header->set_height(block.height());
    proto_header->set_difficulty(0); // Not stored in Block currently
    proto_header->set_nonce(block.nonce());
    
    // Set parent hash
    auto parent_hash_bytes = block.parent_hash().to_bytes();
    auto* proto_parent_hash = proto_header->mutable_prev_block_hash();
    proto_parent_hash->set_data(parent_hash_bytes.data(), parent_hash_bytes.size());
    
    // Set merkle root
    auto merkle_root_bytes = block.merkle_root().to_bytes();
    auto* proto_merkle_root = proto_header->mutable_merkle_root();
    proto_merkle_root->set_data(merkle_root_bytes.data(), merkle_root_bytes.size());
    
    // Set timestamp
    auto* proto_timestamp = proto_header->mutable_timestamp();
    proto_timestamp->set_seconds(block.timestamp().seconds());
    proto_timestamp->set_nanoseconds(0);
    
    // Set gas parameters
    proto_header->set_extra_data(""); // Future use
    
    // Serialize transactions
    for (const auto& tx : block.transactions()) {
        auto tx_result = serialize_transaction(tx);
        if (!tx_result.has_value()) {
            return core::ErrorInfo(tx_result.error());
        }
        
        // Parse transaction back to protobuf
        chainforge::transaction::Transaction proto_tx;
        if (!proto_tx.ParseFromArray(tx_result.value().data(), 
                                      static_cast<int>(tx_result.value().size()))) {
            return make_serialization_error(
                SerializationError::ENCODING_ERROR,
                "Failed to parse transaction for block"
            );
        }
        
        *proto_block.add_transactions() = proto_tx;
    }
    
    // Set block hash
    auto block_hash_bytes = block.calculate_hash().to_bytes();
    auto* proto_block_hash = proto_block.mutable_block_hash();
    proto_block_hash->set_data(block_hash_bytes.data(), block_hash_bytes.size());
    
    // Set transaction count
    proto_block.set_tx_count(static_cast<uint32_t>(block.transactions().size()));
    
    return serialize_to_bytes(proto_block);
}

SerializationResult<std::unique_ptr<Block>> ProtobufSerializer::deserialize_block(const std::vector<uint8_t>& data) {
    chainforge::block::Block proto_block;
    
    if (!proto_block.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        return make_serialization_error(
            SerializationError::CORRUPTED_DATA,
            "Failed to parse block protobuf"
        );
    }
    
    // Extract header information
    const auto& proto_header = proto_block.header();
    
    // Create parent hash
    Hash parent_hash(std::vector<uint8_t>(
        proto_header.prev_block_hash().data().begin(),
        proto_header.prev_block_hash().data().end()
    ));
    
    // Create timestamp
    Timestamp timestamp(proto_header.timestamp().seconds());
    
    // Create block
    auto block = std::make_unique<Block>(proto_header.height(), parent_hash, timestamp);
    
    // Set other header fields
    block->set_nonce(proto_header.nonce());
    block->set_gas_limit(21000); // Default gas limit
    block->set_gas_price(1000000000); // Default gas price (1 gwei)
    block->set_chain_id(1); // Default chain ID
    
    // Deserialize transactions
    for (const auto& proto_tx : proto_block.transactions()) {
        std::vector<uint8_t> tx_data(proto_tx.ByteSizeLong());
        if (!proto_tx.SerializeToArray(tx_data.data(), static_cast<int>(tx_data.size()))) {
            return make_serialization_error(
                SerializationError::ENCODING_ERROR,
                "Failed to serialize transaction in block"
            );
        }
        
        auto tx_result = deserialize_transaction(tx_data);
        if (!tx_result.has_value()) {
            return core::ErrorInfo(tx_result.error());
        }
        
        block->add_transaction(*tx_result.value());
    }
    
    return block;
}

// Transaction serialization implementation
SerializationResult<std::vector<uint8_t>> ProtobufSerializer::serialize_transaction(const Transaction& tx) {
    chainforge::transaction::Transaction proto_tx;
    
    // Set version
    proto_tx.set_version(1);
    
    // Set nonce
    proto_tx.set_nonce(tx.nonce());
    
    // Set gas parameters
    proto_tx.set_gas_limit(tx.gas_limit());
    
    // Set gas price as Amount
    auto* proto_gas_price = proto_tx.mutable_gas_price();
    auto gas_price_str = std::to_string(tx.gas_price());
    proto_gas_price->set_value(gas_price_str);
    
    // Set data
    const auto& payload = tx.payload();
    proto_tx.set_data(payload.data(), payload.size());
    
    // Set timestamp
    auto* proto_timestamp = proto_tx.mutable_timestamp();
    proto_timestamp->set_seconds(Timestamp::now().seconds());
    proto_timestamp->set_nanoseconds(0);
    
    // Create single input (from address)
    auto* proto_input = proto_tx.add_inputs();
    proto_input->set_prev_tx_hash("");  // Empty for now
    proto_input->set_output_index(0);
    
    auto from_bytes = tx.from().to_bytes();
    auto* proto_from_pubkey = proto_input->mutable_pubkey();
    proto_from_pubkey->set_data(from_bytes.data(), from_bytes.size());
    
    // Signature placeholder
    auto* proto_signature = proto_input->mutable_signature();
    proto_signature->set_data("", 0);
    
    // Create single output (to address + amount)
    auto* proto_output = proto_tx.add_outputs();
    
    auto* proto_amount = proto_output->mutable_amount();
    auto amount_str = std::to_string(tx.value().wei());
    proto_amount->set_value(amount_str);
    
    auto to_bytes = tx.to().to_bytes();
    auto* proto_recipient = proto_output->mutable_recipient();
    proto_recipient->set_data(to_bytes.data(), to_bytes.size());
    
    // Set transaction hash
    auto tx_hash_bytes = tx.calculate_hash().to_bytes();
    auto* proto_tx_hash = proto_tx.mutable_tx_hash();
    proto_tx_hash->set_data(tx_hash_bytes.data(), tx_hash_bytes.size());
    
    return serialize_to_bytes(proto_tx);
}

SerializationResult<std::unique_ptr<Transaction>> ProtobufSerializer::deserialize_transaction(const std::vector<uint8_t>& data) {
    chainforge::transaction::Transaction proto_tx;
    
    if (!proto_tx.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        return make_serialization_error(
            SerializationError::CORRUPTED_DATA,
            "Failed to parse transaction protobuf"
        );
    }
    
    // Extract from address (from first input's pubkey)
    if (proto_tx.inputs().empty()) {
        return make_serialization_error(
            SerializationError::INVALID_DATA,
            "Transaction must have at least one input"
        );
    }
    
    Address from(std::vector<uint8_t>(
        proto_tx.inputs(0).pubkey().data().begin(),
        proto_tx.inputs(0).pubkey().data().end()
    ));
    
    // Extract to address and amount (from first output)
    if (proto_tx.outputs().empty()) {
        return make_serialization_error(
            SerializationError::INVALID_DATA,
            "Transaction must have at least one output"
        );
    }
    
    Address to(std::vector<uint8_t>(
        proto_tx.outputs(0).recipient().data().begin(),
        proto_tx.outputs(0).recipient().data().end()
    ));
    
    // Parse amount
    Amount amount = Amount::from_string(proto_tx.outputs(0).amount().value());
    
    // Create transaction
    auto tx = std::make_unique<Transaction>(from, to, amount);
    
    // Set other fields
    tx->set_nonce(proto_tx.nonce());
    tx->set_gas_limit(proto_tx.gas_limit());
    tx->set_gas_price(std::stoull(proto_tx.gas_price().value()));
    
    // Set data if present
    if (!proto_tx.data().empty()) {
        std::vector<uint8_t> data_vec(proto_tx.data().begin(), proto_tx.data().end());
        tx->set_data(data_vec);
    }
    
    return tx;
}

// Address serialization implementation
SerializationResult<std::vector<uint8_t>> ProtobufSerializer::serialize_address(const Address& addr) {
    chainforge::types::Address proto_addr;

    // Convert Address to protobuf bytes
    std::vector<uint8_t> addr_bytes = addr.to_bytes();
    proto_addr.set_data(addr_bytes.data(), addr_bytes.size());

    return serialize_to_bytes(proto_addr);
}

SerializationResult<std::unique_ptr<Address>> ProtobufSerializer::deserialize_address(const std::vector<uint8_t>& data) {
    chainforge::types::Address proto_addr;

    if (!proto_addr.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        return make_serialization_error(
            SerializationError::CORRUPTED_DATA,
            "Failed to parse address protobuf"
        );
    }

    // Convert protobuf back to Address
    std::vector<uint8_t> addr_bytes(proto_addr.data().begin(), proto_addr.data().end());
    
    if (addr_bytes.size() != Address::size()) {
        return make_serialization_error(
            SerializationError::INVALID_DATA,
            "Invalid address size"
        );
    }
    
    return std::make_unique<Address>(addr_bytes);
}

// Amount serialization implementation
SerializationResult<std::vector<uint8_t>> ProtobufSerializer::serialize_amount(const Amount& amount) {
    chainforge::types::Amount proto_amount;

    // Convert Amount to protobuf as string
    std::string amount_str = std::to_string(amount.wei());
    proto_amount.set_value(amount_str);

    return serialize_to_bytes(proto_amount);
}

SerializationResult<std::unique_ptr<Amount>> ProtobufSerializer::deserialize_amount(const std::vector<uint8_t>& data) {
    chainforge::types::Amount proto_amount;

    if (!proto_amount.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        return make_serialization_error(
            SerializationError::CORRUPTED_DATA,
            "Failed to parse amount protobuf"
        );
    }

    // Convert protobuf back to Amount
    try {
        uint64_t wei = std::stoull(proto_amount.value());
        return std::make_unique<Amount>(Amount::from_wei(wei));
    } catch (const std::exception& e) {
        return make_serialization_error(
            SerializationError::INVALID_DATA,
            std::string("Failed to parse amount: ") + e.what()
        );
    }
}

// Timestamp serialization implementation
SerializationResult<std::vector<uint8_t>> ProtobufSerializer::serialize_timestamp(const Timestamp& ts) {
    chainforge::types::Timestamp proto_ts;

    // Convert Timestamp to protobuf
    proto_ts.set_seconds(ts.seconds());
    proto_ts.set_nanoseconds(0);  // We don't store nanoseconds currently

    return serialize_to_bytes(proto_ts);
}

SerializationResult<std::unique_ptr<Timestamp>> ProtobufSerializer::deserialize_timestamp(const std::vector<uint8_t>& data) {
    chainforge::types::Timestamp proto_ts;

    if (!proto_ts.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        return make_serialization_error(
            SerializationError::CORRUPTED_DATA,
            "Failed to parse timestamp protobuf"
        );
    }

    // Convert protobuf back to Timestamp
    return std::make_unique<Timestamp>(Timestamp::from_seconds(proto_ts.seconds()));
}

// Hash serialization implementation
SerializationResult<std::vector<uint8_t>> ProtobufSerializer::serialize_hash(const Hash& hash) {
    chainforge::types::Hash proto_hash;

    // Convert Hash to protobuf bytes
    std::vector<uint8_t> hash_bytes = hash.to_bytes();
    proto_hash.set_data(hash_bytes.data(), hash_bytes.size());

    return serialize_to_bytes(proto_hash);
}

SerializationResult<std::unique_ptr<Hash>> ProtobufSerializer::deserialize_hash(const std::vector<uint8_t>& data) {
    chainforge::types::Hash proto_hash;

    if (!proto_hash.ParseFromArray(data.data(), static_cast<int>(data.size()))) {
        return make_serialization_error(
            SerializationError::CORRUPTED_DATA,
            "Failed to parse hash protobuf"
        );
    }

    // Convert protobuf back to Hash
    std::vector<uint8_t> hash_bytes(proto_hash.data().begin(), proto_hash.data().end());
    
    if (hash_bytes.size() != Hash::size()) {
        return make_serialization_error(
            SerializationError::INVALID_DATA,
            "Invalid hash size"
        );
    }
    
    return std::make_unique<Hash>(hash_bytes);
}

// Factory function
std::unique_ptr<Serializer> create_serializer() {
    return std::make_unique<ProtobufSerializer>();
}

} // namespace serialization
} // namespace chainforge
