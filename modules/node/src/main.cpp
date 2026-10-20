#include "chainforge/crypto/hash.hpp"
#include "chainforge/core/block.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/storage/database.hpp"
#include "chainforge/consensus/consensus.hpp"
#include "chainforge/mempool/mempool.hpp"
#include "chainforge/rpc/rpc_server.hpp"
#include "chainforge/rpc/chain_state.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace {

void print_help(const char* argv0) {
    std::cout
        << "ChainForge node (skeleton)\n"
        << "Usage: " << argv0 << " [--help] [--rpc-port PORT] [--self-check] [--mine-once]\n"
        << "\n"
        << "  --help         Show this help\n"
        << "  --rpc-port N   Start JSON-RPC on port N (default: 8545)\n"
        << "  --self-check   Verify keccak/sha vectors and exit\n"
        << "  --mine-once    Mine one block on top of genesis then serve RPC\n";
}

int self_check() {
    using chainforge::crypto::Hash;
    const auto empty_keccak = Hash::keccak256(std::string{});
    const auto empty_sha = Hash::sha256(std::string{});
    if (!empty_keccak.success() || !empty_sha.success()) {
        std::cerr << "self-check: hash failed\n";
        return 1;
    }

    const std::string expected_keccak =
        "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470";
    const auto got = Hash::to_hex(empty_keccak.value);
    if (got != expected_keccak) {
        std::cerr << "self-check: keccak256(\"\") mismatch\n  got: " << got
                  << "\n  exp: " << expected_keccak << "\n";
        return 1;
    }

    // Address derivation smoke: keccak(pubkey)[12:]
    std::vector<uint8_t> fake_pub(64, 0x11);
    auto addr = chainforge::core::Address::from_public_key(fake_pub);
    std::cout << "self-check ok\n"
              << "  keccak256(\"\")=" << got << "\n"
              << "  sha256(\"\")=" << Hash::to_hex(empty_sha.value) << "\n"
              << "  sample_address=" << addr.to_hex() << "\n";
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    uint16_t rpc_port = 8545;
    bool do_self_check = false;
    bool mine_once = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            print_help(argv[0]);
            return 0;
        }
        if (arg == "--self-check") {
            do_self_check = true;
            continue;
        }
        if (arg == "--mine-once") {
            mine_once = true;
            continue;
        }
        if (arg == "--rpc-port" && i + 1 < argc) {
            rpc_port = static_cast<uint16_t>(std::stoi(argv[++i]));
            continue;
        }
        std::cerr << "Unknown argument: " << arg << "\n";
        print_help(argv[0]);
        return 2;
    }

    if (do_self_check) {
        return self_check();
    }

    std::cout << "ChainForge node starting...\n";

    auto db = std::shared_ptr<chainforge::storage::Database>(
        chainforge::storage::create_database("memory"));
    if (!db) {
        std::cerr << "Failed to create in-memory storage\n";
        return 1;
    }

    auto mempool = std::shared_ptr<chainforge::mempool::Mempool>(
        chainforge::mempool::create_mempool());
    if (!mempool) {
        std::cerr << "Failed to create mempool\n";
        return 1;
    }

    constexpr uint64_t kChainId = 1337;
    auto state = std::make_shared<chainforge::rpc::ChainState>(db, mempool, kChainId);

    auto genesis = chainforge::core::create_genesis_block(kChainId);
    if (!state->initialize_with_genesis(genesis)) {
        std::cerr << "Failed to initialize genesis\n";
        return 1;
    }
    std::cout << "Genesis: 0x" << genesis.calculate_hash().to_hex() << "\n";

    // Fund a demo account
    chainforge::core::Address demo("0x742d35cc6634c0532925a3b844bc454e4438f44e");
    state->credit(demo, 1000000000000000000ULL); // 1e18

    auto consensus = chainforge::consensus::create_pow_consensus(1);
    if (!consensus) {
        std::cerr << "Failed to create PoW consensus\n";
        return 1;
    }

    if (mine_once) {
        auto parent = state->get_block_by_number(state->latest_height());
        if (!parent) {
            std::cerr << "No parent block for mining\n";
            return 1;
        }
        auto tmpl = chainforge::core::create_block(
            parent->height() + 1,
            parent->calculate_hash(),
            chainforge::core::Timestamp::now());
        tmpl.set_chain_id(kChainId);
        tmpl.set_gas_limit(8'000'000);

        auto result = consensus->mine_block(tmpl);
        if (!result.success) {
            std::cerr << "Mining failed after " << result.attempts << " attempts\n";
            return 1;
        }
        tmpl.set_nonce(result.nonce);
        if (!consensus->validate_block(tmpl) || !state->import_block(tmpl)) {
            std::cerr << "Mined block failed validation/import\n";
            return 1;
        }
        std::cout << "Mined block #" << tmpl.height()
                  << " hash=0x" << tmpl.calculate_hash().to_hex()
                  << " nonce=" << result.nonce
                  << " attempts=" << result.attempts << "\n";
    }

    chainforge::rpc::RpcServerConfig rpc_config;
    rpc_config.host = "127.0.0.1";
    rpc_config.port = rpc_port;

    auto rpc = chainforge::rpc::create_rpc_server();
    auto methods = chainforge::rpc::create_blockchain_rpc_methods(state);
    chainforge::rpc::register_blockchain_rpc_methods(*rpc, *methods);

    if (!rpc->start(rpc_config)) {
        std::cerr << "Failed to start RPC on " << rpc_config.host << ":" << rpc_port << "\n";
        return 1;
    }

    std::cout << "RPC listening on http://" << rpc_config.host << ":" << rpc_port << "\n";
    std::cout << "latest block=" << state->latest_height() << "\n";
    std::cout << "Node idle (Ctrl+C to stop).\n";

    while (rpc->is_running()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    rpc->stop();
    return 0;
}
