#include "chainforge/crypto/hash.hpp"
#include "chainforge/storage/database.hpp"
#include "chainforge/consensus/consensus.hpp"
#include "chainforge/mempool/mempool.hpp"
#include "chainforge/rpc/rpc_server.hpp"

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

namespace {

void print_help(const char* argv0) {
    std::cout
        << "ChainForge node (skeleton)\n"
        << "Usage: " << argv0 << " [--help] [--rpc-port PORT] [--self-check]\n"
        << "\n"
        << "  --help         Show this help\n"
        << "  --rpc-port N   Start JSON-RPC on port N (default: 8545)\n"
        << "  --self-check   Verify keccak/sha vectors and exit\n";
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

    std::cout << "self-check ok\n"
              << "  keccak256(\"\")=" << got << "\n"
              << "  sha256(\"\")=" << Hash::to_hex(empty_sha.value) << "\n";
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    uint16_t rpc_port = 8545;
    bool do_self_check = false;

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

    auto db = chainforge::storage::create_database("memory");
    if (!db) {
        std::cerr << "Failed to create in-memory storage\n";
        return 1;
    }
    std::cout << "Storage: in-memory backend ready\n";

    auto mempool = chainforge::mempool::create_mempool();
    if (!mempool) {
        std::cerr << "Failed to create mempool\n";
        return 1;
    }
    std::cout << "Mempool: ready\n";

    auto consensus = chainforge::consensus::create_pow_consensus(1);
    if (!consensus) {
        std::cerr << "Failed to create PoW consensus\n";
        return 1;
    }
    std::cout << "Consensus: PoW ready (difficulty=1)\n";

    chainforge::rpc::RpcServerConfig rpc_config;
    rpc_config.host = "127.0.0.1";
    rpc_config.port = rpc_port;

    auto rpc = chainforge::rpc::create_rpc_server();
    if (!rpc) {
        std::cerr << "Failed to create RPC server\n";
        return 1;
    }

    if (!rpc->start(rpc_config)) {
        std::cerr << "Failed to start RPC on " << rpc_config.host << ":" << rpc_port << "\n";
        return 1;
    }

    std::cout << "RPC listening on http://" << rpc_config.host << ":" << rpc_port << "\n";
    std::cout << "Node idle (Ctrl+C to stop).\n";

    while (rpc->is_running()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    rpc->stop();
    return 0;
}
