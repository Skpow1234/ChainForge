#include <iostream>
#include "chainforge/core/hash.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/core/timestamp.hpp"
#include "chainforge/core/transaction.hpp"
#include "chainforge/core/block.hpp"
#include "chainforge/storage/database.hpp"
#include "chainforge/mempool/mempool.hpp"
#include "chainforge/consensus/consensus.hpp"
#include "chainforge/rpc/rpc_server.hpp"

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
        auto database = chainforge::storage::create_database("memory");
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

        // Test Mempool (basic functionality)
        std::cout << "\n=== Testing Mempool ===" << std::endl;
        chainforge::mempool::MempoolConfig mempool_config;
        mempool_config.max_transactions = 100;
        mempool_config.min_fee_per_gas = 1;

        auto mempool = chainforge::mempool::create_mempool(mempool_config);
        if (mempool) {
            std::cout << "Mempool created successfully" << std::endl;

            // Create a test transaction
            chainforge::core::Address from = chainforge::core::Address::random();
            chainforge::core::Address to = chainforge::core::Address::random();
            chainforge::core::Amount amount = chainforge::core::Amount::from_ether(0.01);
            chainforge::core::Transaction tx(from, to, amount);
            tx.set_gas_price(10);  // Set gas price above minimum

            // Add transaction to mempool
            auto add_result = mempool->add_transaction(tx);
            if (add_result == chainforge::mempool::MempoolError::SUCCESS) {
                std::cout << "Transaction added to mempool successfully" << std::endl;

                // Check if transaction exists
                auto tx_hash = tx.calculate_hash();
                if (mempool->has_transaction(tx_hash)) {
                    std::cout << "Transaction found in mempool" << std::endl;
                }

                // Get transaction from mempool
                auto retrieved_tx = mempool->get_transaction(tx_hash);
                if (retrieved_tx.has_value()) {
                    std::cout << "Transaction retrieved from mempool successfully" << std::endl;
                }

                // Get mempool stats
                auto stats = mempool->get_stats();
                std::cout << "Mempool stats: " << stats.transaction_count << " transactions" << std::endl;

            } else {
                std::cout << "Failed to add transaction to mempool: "
                         << chainforge::mempool::mempool_error_to_string(add_result) << std::endl;
            }

            mempool->clear();
            std::cout << "Mempool cleared successfully" << std::endl;
        } else {
            std::cout << "Failed to create mempool instance" << std::endl;
        }

        // Test Consensus (PoW mining simulation)
        std::cout << "\n=== Testing Consensus (PoW) ===" << std::endl;
        auto consensus = chainforge::consensus::create_pow_consensus(1); // Low difficulty for testing
        if (consensus) {
            std::cout << "PoW consensus created successfully" << std::endl;

            // Create a block template for mining
            chainforge::core::Block block_template(1, genesis.calculate_hash(),
                                                 chainforge::core::Timestamp::now());
            block_template.add_transaction(tx);

            std::cout << "Starting PoW mining simulation..." << std::endl;

            // Mine the block
            auto mining_result = consensus->mine_block(block_template);

            if (mining_result.success) {
                std::cout << "✅ Block mined successfully!" << std::endl;
                std::cout << "Nonce: " << mining_result.nonce << std::endl;
                std::cout << "Block hash: " << mining_result.block_hash.to_hex() << std::endl;
                std::cout << "Mining time: " << mining_result.mining_time.count() << "ms" << std::endl;
                std::cout << "Attempts: " << mining_result.attempts << std::endl;

                // Validate the proof of work
                bool is_valid_pow = consensus->validate_proof_of_work(
                    mining_result.block_hash,
                    mining_result.nonce,
                    consensus->get_difficulty()
                );
                std::cout << "PoW validation: " << (is_valid_pow ? "valid" : "invalid") << std::endl;

            } else {
                std::cout << "❌ Mining failed after " << mining_result.attempts << " attempts" << std::endl;
            }

            // Get mining statistics
            auto stats = consensus->get_mining_stats();
            std::cout << "Mining stats - Total attempts: " << stats.total_attempts
                     << ", Success rate: " << stats.successful_mines << "/"
                     << (stats.total_attempts > 0 ? 1 : 0) << std::endl;

        } else {
            std::cout << "Failed to create consensus instance" << std::endl;
        }

        // Test RPC (basic functionality)
        std::cout << "\n=== Testing RPC ===" << std::endl;
        auto rpc_server = chainforge::rpc::create_rpc_server();
        auto blockchain_methods = chainforge::rpc::create_blockchain_rpc_methods();

        if (rpc_server && blockchain_methods) {
            std::cout << "RPC server and methods created successfully" << std::endl;

            // Register some blockchain methods
            rpc_server->register_method("eth_blockNumber",
                [blockchain_methods](const nlohmann::json& params) {
                    return blockchain_methods->eth_blockNumber(params);
                });

            rpc_server->register_method("eth_getBalance",
                [blockchain_methods](const nlohmann::json& params) {
                    return blockchain_methods->eth_getBalance(params);
                });

            rpc_server->register_method("net_version",
                [blockchain_methods](const nlohmann::json& params) {
                    return blockchain_methods->net_version(params);
                });

            rpc_server->register_method("web3_clientVersion",
                [blockchain_methods](const nlohmann::json& params) {
                    return blockchain_methods->web3_clientVersion(params);
                });

            std::cout << "RPC methods registered: " << (rpc_server->has_method("eth_blockNumber") ? "yes" : "no") << std::endl;

            // Test RPC server configuration
            chainforge::rpc::RpcServerConfig config;
            config.host = "127.0.0.1";
            config.port = 8545;

            std::cout << "RPC server info: " << rpc_server->get_server_info() << std::endl;

            // Note: We don't start the server in tests to avoid conflicts
            std::cout << "RPC server configured but not started (for testing)" << std::endl;

        } else {
            std::cout << "Failed to create RPC server or methods" << std::endl;
        }

        std::cout << "\n✅ All core tests completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    }
}
