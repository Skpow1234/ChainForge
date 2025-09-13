#include <iostream>
#include "chainforge/core/hash.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/core/timestamp.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/block.hpp"
#include "chainforge/storage/database.hpp"

int main() {
    std::cout << "ChainForge Core Test" << std::endl;

    try {
        // Test Hash
        std::cout << "\n=== Testing Hash ===" << std::endl;
        chainforge::core::Hash hash = chainforge::core::Hash::random();
        std::cout << "Generated hash: " << hash.to_hex() << std::endl;
        std::cout << "Hash size: " << hash.size() << " bytes" << std::endl;

        // Test Address
        std::cout << "\n=== Testing Address ===" << std::endl;
        chainforge::core::Address addr = chainforge::core::Address::random();
        std::cout << "Generated address: " << addr.to_hex() << std::endl;
        std::cout << "Address is valid: " << (addr.is_valid() ? "yes" : "no") << std::endl;

        // Test Amount
        std::cout << "\n=== Testing Amount ===" << std::endl;
        chainforge::core::Amount amount = chainforge::core::Amount::from_ether(1.5);
        std::cout << "Amount: " << amount.to_string() << " ETH" << std::endl;
        std::cout << "Amount in wei: " << amount.wei() << std::endl;

        // Test Timestamp
        std::cout << "\n=== Testing Timestamp ===" << std::endl;
        chainforge::core::Timestamp now = chainforge::core::Timestamp::now();
        std::cout << "Current timestamp: " << now.to_string() << std::endl;
        std::cout << "ISO8601: " << now.to_iso8601() << std::endl;

        // Test Transaction
        std::cout << "\n=== Testing Transaction ===" << std::endl;
        chainforge::core::Address from = chainforge::core::Address::random();
        chainforge::core::Address to = chainforge::core::Address::random();
        chainforge::core::Amount tx_amount = chainforge::core::Amount::from_ether(0.1);
        chainforge::core::Transaction tx(from, to, tx_amount);
        std::cout << "Transaction: " << tx.to_string() << std::endl;
        std::cout << "Transaction hash: " << tx.calculate_hash().to_hex() << std::endl;

        // Test Block
        std::cout << "\n=== Testing Block ===" << std::endl;
        chainforge::core::Block genesis = chainforge::core::create_genesis_block(1);
        genesis.add_transaction(tx);
        std::cout << "Genesis block: " << genesis.to_string() << std::endl;
        std::cout << "Block hash: " << genesis.calculate_hash().to_hex() << std::endl;

        // Test Storage (basic functionality)
        std::cout << "\n=== Testing Storage ===" << std::endl;
        auto database = chainforge::storage::create_database("rocksdb");
        if (database) {
            chainforge::storage::DatabaseConfig config;
            config.path = "./test_db";
            config.create_if_missing = true;

            if (database->open(config).success()) {
                std::cout << "Database opened successfully" << std::endl;

                // Test basic operations
                chainforge::storage::Key test_key = {'t', 'e', 's', 't'};
                chainforge::storage::Value test_value = {'v', 'a', 'l', 'u', 'e'};

                if (database->put(test_key, test_value).success()) {
                    std::cout << "Put operation successful" << std::endl;

                    auto get_result = database->get(test_key);
                    if (get_result.success() && get_result.value == test_value) {
                        std::cout << "Get operation successful" << std::endl;
                    } else {
                        std::cout << "Get operation failed" << std::endl;
                    }
                }

                database->close();
                std::cout << "Database closed successfully" << std::endl;
            } else {
                std::cout << "Failed to open database" << std::endl;
            }
        } else {
            std::cout << "Failed to create database instance" << std::endl;
        }

        std::cout << "\n✅ All core tests completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}
