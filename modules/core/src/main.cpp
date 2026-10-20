#include <iostream>
#include "tokiwa/core/hash.hpp"
#include "tokiwa/core/address.hpp"
#include "tokiwa/core/amount.hpp"
#include "tokiwa/core/timestamp.hpp"
#include "tokiwa/core/transaction.hpp"
#include "tokiwa/core/block.hpp"

int main() {
    std::cout << "Tokiwa Core Test" << std::endl;

    try {
        // Test Hash
        std::cout << "\n=== Testing Hash ===" << std::endl;
        tokiwa::core::Hash hash = tokiwa::core::Hash::random();
        std::cout << "Generated hash: " << hash.to_hex() << std::endl;
        std::cout << "Hash size: " << hash.size() << " bytes" << std::endl;

        // Test Address
        std::cout << "\n=== Testing Address ===" << std::endl;
        tokiwa::core::Address addr = tokiwa::core::Address::random();
        std::cout << "Generated address: " << addr.to_hex() << std::endl;
        std::cout << "Address is valid: " << (addr.is_valid() ? "yes" : "no") << std::endl;

        // Test Amount
        std::cout << "\n=== Testing Amount ===" << std::endl;
        tokiwa::core::Amount amount = tokiwa::core::Amount::from_ether(1.5);
        std::cout << "Amount: " << amount.to_string() << " ETH" << std::endl;
        std::cout << "Amount in wei: " << amount.wei() << std::endl;

        // Test Timestamp
        std::cout << "\n=== Testing Timestamp ===" << std::endl;
        tokiwa::core::Timestamp now = tokiwa::core::Timestamp::now();
        std::cout << "Current timestamp: " << now.to_string() << std::endl;
        std::cout << "ISO8601: " << now.to_iso8601() << std::endl;

        // Test Transaction
        std::cout << "\n=== Testing Transaction ===" << std::endl;
        tokiwa::core::Address from = tokiwa::core::Address::random();
        tokiwa::core::Address to = tokiwa::core::Address::random();
        tokiwa::core::Amount tx_amount = tokiwa::core::Amount::from_ether(0.1);
        tokiwa::core::Transaction tx(from, to, tx_amount);
        std::cout << "Transaction: " << tx.to_string() << std::endl;
        std::cout << "Transaction hash: " << tx.calculate_hash().to_hex() << std::endl;

        // Test Block
        std::cout << "\n=== Testing Block ===" << std::endl;
        tokiwa::core::Block genesis = tokiwa::core::create_genesis_block(1);
        genesis.add_transaction(tx);
        std::cout << "Genesis block: " << genesis.to_string() << std::endl;
        std::cout << "Block hash: " << genesis.calculate_hash().to_hex() << std::endl;

        // Test JSON serialization
        std::cout << "\n=== Testing JSON Serialization ===" << std::endl;
        std::cout << "Transaction JSON: " << tx.to_json() << std::endl;
        std::cout << "Block JSON: " << genesis.to_json() << std::endl;

        std::cout << "\n✅ All core tests completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}
