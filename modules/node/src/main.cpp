#include "chainforge/node/config.hpp"
#include "chainforge/crypto/hash.hpp"
#include "chainforge/crypto/signature.hpp"
#include "chainforge/crypto/keypair.hpp"
#include "chainforge/core/block.hpp"
#include "chainforge/core/address.hpp"
#include "chainforge/core/amount.hpp"
#include "chainforge/storage/database.hpp"
#include "chainforge/consensus/consensus.hpp"
#include "chainforge/mempool/mempool.hpp"
#include "chainforge/rpc/rpc_server.hpp"
#include "chainforge/rpc/chain_state.hpp"
#include "chainforge/p2p/peer_manager.hpp"
#include "chainforge/p2p/sync.hpp"
#include "chainforge/core/hash.hpp"
#include "chainforge/core/transaction.hpp"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace {

void print_help(const char* argv0) {
    std::cout
        << "ChainForge node\n"
        << "Usage: " << argv0 << " [options]\n"
        << "  --help              Show help\n"
        << "  --config PATH       YAML config (default: config/config.yaml)\n"
        << "  --rpc-port N        Override RPC port\n"
        << "  --self-check        Crypto/address/P2P protocol self-check and exit\n"
        << "  --mine-once         Mine one block after genesis\n";
}

int self_check() {
    using chainforge::crypto::Hash;
    const auto empty_keccak = Hash::keccak256(std::string{});
    if (!empty_keccak.success()) return 1;
    const std::string expected =
        "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470";
    if (Hash::to_hex(empty_keccak.value) != expected) {
        std::cerr << "keccak self-check failed\n";
        return 1;
    }
    std::vector<uint8_t> fake_pub(64, 0x11);
    auto addr = chainforge::core::Address::from_public_key(fake_pub);

    std::string proto_err;
    if (!chainforge::p2p::protocol_self_check(&proto_err)) {
        std::cerr << "p2p protocol self-check failed: " << proto_err << "\n";
        return 1;
    }

    // Compact ECDSA r||s (+ recoverable r||s||v) round-trip
    {
        auto kp = chainforge::crypto::KeyPair::generate_secp256k1();
        if (!kp.success()) {
            std::cerr << "ecdsa keygen self-check failed\n";
            return 1;
        }
        const std::vector<uint8_t> msg = {'c', 'f', 'g'};
        auto sig = chainforge::crypto::Signature::ecdsa_secp256k1_sign(msg, kp.value.private_key);
        if (!sig.success() || sig.value.size() != 64) {
            std::cerr << "ecdsa compact sign self-check failed\n";
            return 1;
        }
        auto ver = chainforge::crypto::Signature::ecdsa_secp256k1_verify(
            msg, sig.value, kp.value.public_key);
        if (!ver.success() || !ver.value) {
            std::cerr << "ecdsa compact verify self-check failed\n";
            return 1;
        }
        auto sig65 = chainforge::crypto::Signature::ecdsa_secp256k1_sign_recoverable(
            msg, kp.value.private_key);
        if (!sig65.success()) {
            std::cerr << "ecdsa recoverable sign self-check failed\n";
            return 1;
        }
        chainforge::crypto::Secp256k1Signature compact{};
        std::copy(sig65.value.begin(), sig65.value.begin() + 64, compact.begin());
        auto rec = chainforge::crypto::Signature::ecdsa_secp256k1_recover_public_key(
            msg, compact, sig65.value[64]);
        if (!rec.success() || rec.value != kp.value.public_key) {
            std::cerr << "ecdsa recover self-check failed\n";
            return 1;
        }

        chainforge::core::Address to("0x742d35cc6634c0532925a3b844bc454e4438f44e");
        chainforge::core::Transaction tx(
            chainforge::core::Address::zero(), to,
            chainforge::core::Amount::from_wei(1));
        tx.set_chain_id(1337);
        tx.set_nonce(0);
        std::vector<uint8_t> sk(kp.value.private_key.begin(), kp.value.private_key.end());
        if (!tx.sign(sk) || tx.signature().size() != 65) {
            std::cerr << "tx sign self-check failed\n";
            return 1;
        }
    }

    std::cout << "self-check ok address=" << addr.to_hex()
              << " p2p_protocol=ok ecdsa_compact=ok\n";
    return 0;
}

chainforge::p2p::ChainView make_chain_view(
    const std::shared_ptr<chainforge::rpc::ChainState>& state) {
    chainforge::p2p::ChainView view;
    view.chain_id = state->chain_id();
    view.agent = "chainforge-node";
    view.tip_height = [state] { return state->latest_height(); };
    view.tip_hash = [state] {
        auto tip = state->get_block_by_number(state->latest_height());
        if (!tip) return std::vector<uint8_t>(32, 0);
        return tip->calculate_hash().to_bytes();
    };
    view.header_hash_at = [state](uint64_t h) -> std::optional<std::vector<uint8_t>> {
        auto block = state->get_block_by_number(h);
        if (!block) return std::nullopt;
        return block->calculate_hash().to_bytes();
    };
    view.block_json_at = [state](uint64_t h) -> std::optional<std::string> {
        auto block = state->get_block_by_number(h);
        if (!block) return std::nullopt;
        return block->to_json();
    };
    view.block_json_by_hash = [state](const std::vector<uint8_t>& hash32)
        -> std::optional<std::string> {
        if (hash32.size() != 32) return std::nullopt;
        chainforge::core::Hash hash(hash32);
        auto block = state->get_block_by_hash(hash);
        if (!block) return std::nullopt;
        return block->to_json();
    };
    view.import_block_json = [state](const std::string& json) {
        auto block = chainforge::core::Block::from_json(json);
        if (!block) return false;
        return state->import_block(*block);
    };
    view.accept_tx_json = [state](const std::string& json) {
        auto tx = chainforge::core::Transaction::from_json(json);
        if (!tx) return false;
        state->submit_transaction(*tx);
        return true;
    };
    return view;
}

} // namespace

int main(int argc, char** argv) {
    std::string config_path = "config/config.yaml";
    bool do_self_check = false;
    bool mine_once = false;
    std::optional<uint16_t> rpc_port_override;

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
        if (arg == "--config" && i + 1 < argc) {
            config_path = argv[++i];
            continue;
        }
        if (arg == "--rpc-port" && i + 1 < argc) {
            rpc_port_override = static_cast<uint16_t>(std::stoi(argv[++i]));
            continue;
        }
        std::cerr << "Unknown argument: " << arg << "\n";
        print_help(argv[0]);
        return 2;
    }

    if (do_self_check) {
        return self_check();
    }

    auto cfg = chainforge::node::load_node_config(config_path);
    if (rpc_port_override) {
        cfg.rpc_port = *rpc_port_override;
    }
    // Prefer loopback unless explicitly overridden to something else in config for local safety
    if (cfg.rpc_host == "0.0.0.0") {
        cfg.rpc_host = "127.0.0.1";
    }

    std::cout << "ChainForge node starting (config=" << config_path << ")\n";
    std::cout << "  chain_id=" << cfg.chain_id
              << " storage=" << cfg.storage_backend
              << " rpc=" << cfg.rpc_host << ":" << cfg.rpc_port << "\n";

    auto db = std::shared_ptr<chainforge::storage::Database>(
        chainforge::storage::create_database(cfg.storage_backend));
    if (!db) {
        std::cerr << "Failed to create storage backend: " << cfg.storage_backend << "\n";
        return 1;
    }
    {
        chainforge::storage::DatabaseConfig dbcfg;
        if (cfg.storage_backend == "rocksdb") {
            dbcfg.path = cfg.data_dir + "/chaindb";
        } else if (cfg.storage_backend == "file") {
            dbcfg.path = cfg.data_dir + "/chain.kv";
        } else {
            dbcfg.path = cfg.data_dir + "/memory";
        }
        dbcfg.create_if_missing = true;
        auto opened = db->open(dbcfg);
        if (!opened.success() && !db->is_open()) {
            std::cerr << "Failed to open storage path=" << dbcfg.path << "\n";
            return 1;
        }
        std::cout << "  storage path=" << dbcfg.path << "\n";
    }

    auto mempool = std::shared_ptr<chainforge::mempool::Mempool>(
        chainforge::mempool::create_mempool());
    auto state = std::make_shared<chainforge::rpc::ChainState>(db, mempool, cfg.chain_id);

    if (state->load_from_storage()) {
        std::cout << "Reloaded chain from storage tip=#" << state->latest_height() << "\n";
        if (auto tip = state->get_block_by_number(state->latest_height())) {
            std::cout << "  tip hash: 0x" << tip->calculate_hash().to_hex() << "\n";
        }
    } else {
        auto genesis = chainforge::core::create_genesis_block(
            static_cast<chainforge::core::ChainId>(cfg.chain_id));
        if (!state->initialize_with_genesis(genesis)) {
            std::cerr << "Failed to initialize genesis\n";
            return 1;
        }
        std::cout << "Genesis: 0x" << genesis.calculate_hash().to_hex() << "\n";
        chainforge::core::Address demo("0x742d35cc6634c0532925a3b844bc454e4438f44e");
        state->credit(demo, 1000000000000000000ULL);
    }

    chainforge::p2p::PeerManager peers(cfg.max_peers);
    peers.add_peer(chainforge::p2p::PeerAddress("127.0.0.1", 30303));
    auto sync = std::make_shared<chainforge::p2p::SyncSession>(make_chain_view(state));
    auto hello = sync->make_hello();
    auto hello_wire = chainforge::p2p::encode_message(hello);
    std::cout << "P2P peer manager: " << peers.size() << " peer(s)"
              << " protocol=v" << int(chainforge::p2p::PROTOCOL_VERSION)
              << " hello_frame=" << hello_wire.size() << "B\n";

    auto consensus = chainforge::consensus::create_pow_consensus(cfg.difficulty);
    if (mine_once && consensus) {
        auto parent = state->get_block_by_number(state->latest_height());
        if (parent) {
            auto tmpl = chainforge::core::create_block(
                parent->height() + 1,
                parent->calculate_hash(),
                chainforge::core::Timestamp::now());
            tmpl.set_chain_id(static_cast<chainforge::core::ChainId>(cfg.chain_id));
            tmpl.set_gas_limit(8'000'000);
            auto result = consensus->mine_block(tmpl);
            if (result.success) {
                tmpl.set_nonce(result.nonce);
                if (consensus->validate_block(tmpl)) {
                    state->import_block(tmpl);
                    std::cout << "Mined block #" << tmpl.height()
                              << " 0x" << tmpl.calculate_hash().to_hex() << "\n";
                }
            }
        }
    }

    chainforge::rpc::RpcServerConfig rpc_config;
    rpc_config.host = cfg.rpc_host;
    rpc_config.port = cfg.rpc_port;

    auto rpc = chainforge::rpc::create_rpc_server();
    auto methods = chainforge::rpc::create_blockchain_rpc_methods(state);
    chainforge::rpc::register_blockchain_rpc_methods(*rpc, *methods);

    if (!rpc->start(rpc_config)) {
        std::cerr << "Failed to start RPC\n";
        return 1;
    }

    std::cout << "RPC http://" << rpc_config.host << ":" << rpc_config.port
              << "  health=GET /health  ready=GET /ready\n";

    while (rpc->is_running()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    db->flush({});
    rpc->stop();
    return 0;
}
