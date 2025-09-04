#pragma once

#include "types.hpp"
#include "hash.hpp"
#include "timestamp.hpp"
#include <vector>
#include <memory>
#include <optional>

namespace chainforge::core {

class Transaction;

/**
 * Block class representing a blockchain block
 * Contains header information and a list of transactions
 */
class Block {
public:
    // Constructors
    Block() = default;
    Block(BlockHeight height, const Hash& parent_hash, const Timestamp& timestamp);
    Block(const BlockHeader& header, std::vector<Transaction> transactions);
    
    // Copy and move
    Block(const Block&) = default;
    Block(Block&&) = default;
    Block& operator=(const Block&) = default;
    Block& operator=(Block&&) = default;
    
    // Destructor
    ~Block() = default;
    
    // Accessors
    const BlockHeader& header() const noexcept { return header_; }
    BlockHeader& header() noexcept { return header_; }
    
    const std::vector<Transaction>& transactions() const noexcept { return transactions_; }
    std::vector<Transaction>& transactions() noexcept { return transactions_; }
    
    // Header field accessors
    BlockHeight height() const noexcept { return header_.height; }
    const Hash& parent_hash() const noexcept { return header_.parent_hash; }
    const Hash& merkle_root() const noexcept { return header_.merkle_root; }
    const Timestamp& timestamp() const noexcept { return header_.timestamp; }
    BlockNonce nonce() const noexcept { return header_.nonce; }
    GasLimit gas_limit() const noexcept { return header_.gas_limit; }
    GasPrice gas_price() const noexcept { return header_.gas_price; }
    ChainId chain_id() const noexcept { return header_.chain_id; }
    
    // Setters
    void set_height(BlockHeight height);
    void set_parent_hash(const Hash& parent_hash);
    void set_timestamp(const Timestamp& timestamp);
    void set_nonce(BlockNonce nonce);
    void set_gas_limit(GasLimit gas_limit);
    void set_gas_price(GasPrice gas_price);
    void set_chain_id(ChainId chain_id);
    
    // Transaction management
    void add_transaction(const Transaction& transaction);
    void remove_transaction(size_t index);
    void clear_transactions();
    
    // Block operations
    Hash calculate_hash() const;
    Hash calculate_merkle_root() const;
    bool is_genesis() const noexcept;
    bool is_valid() const;
    
    // Size and capacity
    size_t transaction_count() const noexcept { return transactions_.size(); }
    size_t size() const;
    bool is_full() const;
    
    // Validation
    bool validate_header() const;
    bool validate_transactions() const;
    bool validate_size() const;
    
    // Utility methods
    std::string to_string() const;
    std::string to_json() const;

private:
    BlockHeader header_;
    std::vector<Transaction> transactions_;
    mutable std::optional<Hash> cached_hash_;
    
    // Helper methods
    void update_merkle_root();
    void invalidate_cache();
    bool validate_gas_limits() const;
};

// Free functions
std::ostream& operator<<(std::ostream& os, const Block& block);
bool operator==(const Block& lhs, const Block& rhs);
bool operator!=(const Block& lhs, const Block& rhs);

// Block creation helpers
Block create_genesis_block(ChainId chain_id);
Block create_block(BlockHeight height, const Hash& parent_hash, const Timestamp& timestamp);

// Block validation
bool is_valid_block(const Block& block);
bool is_valid_block_header(const BlockHeader& header);

} // namespace chainforge::core
