# Extended Milestone Backlog (61+)

Generated from codebase review (TODOs, stubs, disabled targets, deploy gaps, DX/security).
Status summary for 1–60: [milestones.md](./milestones.md). Specs for 1–60: [ROADMAP.md](./ROADMAP.md).

| Mark | Meaning |
|------|---------|
| Todo | Not started / still open |
| Partial | Scaffolding exists |
| Done | Landed in recent work |

## Phase 5+: Improvement & productization backlog

### Milestone 61: Wire real Keccak256 (no SHA fallback) [Done]

- **Area**: Crypto
- **Why / hint**: modules/crypto/src/hash.cpp + keccak.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 62: Keccak empty-string KAT in CI [Todo]

- **Area**: Crypto
- **Why / hint**: self-check + unit tests
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 63: Replace secp256k1 XOR curve stubs [Done]

- **Area**: Crypto
- **Why / hint**: modules/crypto/src/curve.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 64: Ed25519 point add API [Todo]

- **Area**: Crypto
- **Why / hint**: libsodium limits
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 65: BLS12-381 via BLST [Todo]

- **Area**: Crypto
- **Why / hint**: unsupported today
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 66: ECDSA pubkey recovery [Todo]

- **Area**: Crypto
- **Why / hint**: signature.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 67: ECDSA recovery id (v) [Todo]

- **Area**: Crypto
- **Why / hint**: signature.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 68: BLS aggregate verify [Todo]

- **Area**: Crypto
- **Why / hint**: signature.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 69: Prefer libsecp256k1 ECDSA path [Todo]

- **Area**: Crypto
- **Why / hint**: signature.cpp OpenSSL DER
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 70: CSPRNG instead of mt19937 [Todo]

- **Area**: Crypto
- **Why / hint**: random.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 71: Constant-time secret compare [Todo]

- **Area**: Crypto
- **Why / hint**: types/random
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 72: Secure wipe private keys [Todo]

- **Area**: Crypto
- **Why / hint**: keypair.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 73: Validate secp256k1 scalars [Todo]

- **Area**: Crypto
- **Why / hint**: random.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 74: OpenSSL 3 non-deprecated APIs [Todo]

- **Area**: Crypto
- **Why / hint**: CMake warning silence
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 75: Simplify libsodium Conan includes [Todo]

- **Area**: Build
- **Why / hint**: crypto/CMakeLists.txt
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 76: Enable crypto unit tests [Todo]

- **Area**: Testing
- **Why / hint**: tests/CMakeLists.txt
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 77: Hash known-answer vectors [Todo]

- **Area**: Testing
- **Why / hint**: tests/unit/crypto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 78: Signature malleability tests [Todo]

- **Area**: Testing
- **Why / hint**: low-S
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 79: Fail Release on crypto stubs [Todo]

- **Area**: Security
- **Why / hint**: compile-time guards
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 80: Mining Keccak hash path [Todo]

- **Area**: Crypto
- **Why / hint**: keccak mining hooks
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 81: Core SHA256 via crypto [Done]

- **Area**: Core
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 82: Core Keccak via crypto [Done]

- **Area**: Core
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 83: Core RIPEMD160 via crypto [Done]

- **Area**: Core
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 84: Merkle combine via hash_pair [Done]

- **Area**: Core
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 85: EIP-55 checksum addresses [Todo]

- **Area**: Core
- **Why / hint**: address.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 86: Keccak pubkey to address [Todo]

- **Area**: Core
- **Why / hint**: address.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 87: CREATE contract address [Todo]

- **Area**: Core
- **Why / hint**: address.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 88: CREATE2 address support [Todo]

- **Area**: Core
- **Why / hint**: address.hpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 89: Fix is_contract heuristic [Todo]

- **Area**: Core
- **Why / hint**: address.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 90: Accept 0x-prefixed hex hashes [Done]

- **Area**: Core
- **Why / hint**: partial
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 91: RLP transaction hashing [Todo]

- **Area**: Core
- **Why / hint**: transaction.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 92: Transaction signature verify [Todo]

- **Area**: Core
- **Why / hint**: validate_signature
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 93: Real tx JSON data field [Todo]

- **Area**: Core
- **Why / hint**: transaction.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 94: RLP to_hex encoding [Todo]

- **Area**: Core
- **Why / hint**: transaction.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 95: EIP-1559 typed transactions [Todo]

- **Area**: Core
- **Why / hint**: transaction model
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 96: EIP-155 chain id replay protection [Todo]

- **Area**: Core
- **Why / hint**: transaction
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 97: Proper block header hash [Todo]

- **Area**: Core
- **Why / hint**: block.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 98: Merkle root correctness tests [Todo]

- **Area**: Core
- **Why / hint**: block.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 99: Parent-hash chain validation [Todo]

- **Area**: Core
- **Why / hint**: block.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 100: Amount from_hex edge cases [Todo]

- **Area**: Core
- **Why / hint**: amount.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 101: Timestamp drift validation [Todo]

- **Area**: Core
- **Why / hint**: timestamp + validator
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 102: Re-enable core type tests [Todo]

- **Area**: Testing
- **Why / hint**: tests/CMakeLists.txt
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 103: Crypto RNG for Hash::random [Done]

- **Area**: Core
- **Why / hint**: hash.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 104: Enable storage in CMake [Done]

- **Area**: Storage
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 105: Real RocksDB backend [Todo]

- **Area**: Storage
- **Why / hint**: rocksdb_database.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 106: Link RocksDB via Conan [Todo]

- **Area**: Storage
- **Why / hint**: conanfile.txt
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 107: Persistent open/close semantics [Todo]

- **Area**: Storage
- **Why / hint**: close clears memory
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 108: Snapshot iterator isolation [Todo]

- **Area**: Storage
- **Why / hint**: database.hpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 109: Column families for chain data [Todo]

- **Area**: Storage
- **Why / hint**: blocks/state/CF
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 110: Blockchain store facade [Todo]

- **Area**: Storage
- **Why / hint**: missing layer
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 111: State trie account DB [Todo]

- **Area**: Execution
- **Why / hint**: missing module
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 112: Batch crash-safety [Todo]

- **Area**: Storage
- **Why / hint**: MemoryWriteBatch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 113: Corruption repair hooks [Todo]

- **Area**: Storage
- **Why / hint**: CORRUPTION unused
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 114: Storage unit tests [Todo]

- **Area**: Testing
- **Why / hint**: tests/unit/storage
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 115: Map YAML storage config [Todo]

- **Area**: Config
- **Why / hint**: config.yaml
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 116: Honor WriteOptions.sync [Todo]

- **Area**: Storage
- **Why / hint**: memory backend
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 117: Compaction policy [Todo]

- **Area**: Storage
- **Why / hint**: compact_range no-op
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 118: Postgres indexer C++ client [Todo]

- **Area**: Explorer
- **Why / hint**: 01-init.sql
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 119: Redis cache client module [Todo]

- **Area**: Cache
- **Why / hint**: config redis
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 120: Enable consensus CMake [Done]

- **Area**: Consensus
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 121: Remove PoW demo mining cheats [Done]

- **Area**: Consensus
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 122: Real PoW validation path [Todo]

- **Area**: Consensus
- **Why / hint**: validate_proof_of_work
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 123: Async mining thread pool [Todo]

- **Area**: Consensus
- **Why / hint**: mine_block_async
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 124: Difficulty target encoding [Todo]

- **Area**: Consensus
- **Why / hint**: hash_to_difficulty
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 125: Config-driven difficulty [Todo]

- **Area**: Config
- **Why / hint**: consensus YAML
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 126: Uncle reward issuance [Todo]

- **Area**: Consensus
- **Why / hint**: config uncle_reward
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 127: PoS/BFT consensus factories [Todo]

- **Area**: Consensus
- **Why / hint**: create_consensus
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 128: Consensus unit tests [Todo]

- **Area**: Testing
- **Why / hint**: tests/unit/consensus
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 129: Canonical header serialization [Todo]

- **Area**: Consensus
- **Why / hint**: mine hash input
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 130: Hash-rate estimate realism [Todo]

- **Area**: Consensus
- **Why / hint**: estimate_mining_time
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 131: stop_mining lock redesign [Todo]

- **Area**: Consensus
- **Why / hint**: deadlock risk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 132: Coinbase block reward path [Todo]

- **Area**: Consensus
- **Why / hint**: core+consensus
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 133: Difficulty adjustment on chain [Todo]

- **Area**: Consensus
- **Why / hint**: DifficultyParams
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 134: Enable mempool CMake [Done]

- **Area**: Mempool
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 135: Avoid full priority rebuild [Todo]

- **Area**: Mempool
- **Why / hint**: mempool_impl.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 136: Deadlock-safe eviction [Todo]

- **Area**: Mempool
- **Why / hint**: nested locks
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 137: is_pool_full lock hygiene [Todo]

- **Area**: Mempool
- **Why / hint**: get_stats nesting
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 138: Account nonce gap queue [Todo]

- **Area**: Mempool
- **Why / hint**: validate_nonce
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 139: Tx dependency graph [Todo]

- **Area**: Mempool
- **Why / hint**: dependencies empty
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 140: OLDEST_FIRST selection [Todo]

- **Area**: Mempool
- **Why / hint**: selection strategy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 141: State-aware balance checks [Todo]

- **Area**: Mempool
- **Why / hint**: no account state
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 142: txpool RPC methods [Todo]

- **Area**: RPC
- **Why / hint**: config lists txpool
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 143: Mempool unit tests [Todo]

- **Area**: Testing
- **Why / hint**: tests/unit/mempool
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 144: Configurable eviction age [Todo]

- **Area**: Mempool
- **Why / hint**: hardcoded 3600s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 145: Replace-underpriced policy [Todo]

- **Area**: Mempool
- **Why / hint**: 1.1 fee bump
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 146: Persist mempool on restart [Todo]

- **Area**: Mempool
- **Why / hint**: in-memory only
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 147: Mempool metrics hooks [Todo]

- **Area**: Metrics
- **Why / hint**: pool size/fees
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 148: Enable RPC CMake [Done]

- **Area**: RPC
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 149: Replace mock blockchain state [Todo]

- **Area**: RPC
- **Why / hint**: blockchain_rpc_methods
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 150: Real eth_sendRawTransaction [Todo]

- **Area**: RPC
- **Why / hint**: returns zero hash
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 151: Wire RPC to storage/mempool [Todo]

- **Area**: RPC
- **Why / hint**: no backend DI
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 152: personal_* RPC APIs [Todo]

- **Area**: RPC
- **Why / hint**: config advertises
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 153: WebSocket JSON-RPC server [Todo]

- **Area**: RPC
- **Why / hint**: port 8546 unused
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 154: RPC JWT auth [Todo]

- **Area**: Security
- **Why / hint**: no auth
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 155: Enforce RPC rate limits [Todo]

- **Area**: RPC
- **Why / hint**: config ignored
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 156: CORS allowlist hardening [Todo]

- **Area**: Security
- **Why / hint**: cors *
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 157: Bind localhost by default [Todo]

- **Area**: Security
- **Why / hint**: config 0.0.0.0
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 158: HTTPS TLS for RPC [Todo]

- **Area**: Security
- **Why / hint**: enable_https false
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 159: Batch JSON-RPC arrays [Todo]

- **Area**: RPC
- **Why / hint**: rpc_server_impl
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 160: eth_call and estimateGas [Todo]

- **Area**: RPC
- **Why / hint**: missing methods
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 161: eth_getLogs filters [Todo]

- **Area**: RPC
- **Why / hint**: missing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 162: RPC integration tests [Todo]

- **Area**: Testing
- **Why / hint**: tests
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 163: Gate mocks behind flag [Todo]

- **Area**: RPC
- **Why / hint**: enable_mock_data
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 164: Boost.Asio HTTP server [Todo]

- **Area**: RPC
- **Why / hint**: Winsock today
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 165: Peer management module [Todo]

- **Area**: P2P
- **Why / hint**: M18
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 166: Wire message protocol [Todo]

- **Area**: P2P
- **Why / hint**: M19
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 167: Block sync protocol [Todo]

- **Area**: P2P
- **Why / hint**: M20
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 168: Tx propagation protocol [Todo]

- **Area**: P2P
- **Why / hint**: M21
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 169: TCP connect timeout honor [Todo]

- **Area**: P2P
- **Why / hint**: tcp_client.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 170: NAT traversal [Todo]

- **Area**: P2P
- **Why / hint**: M23
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 171: Peer scoring and ban [Todo]

- **Area**: P2P
- **Why / hint**: M24 + SQL schema
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 172: Protocol version negotiation [Todo]

- **Area**: P2P
- **Why / hint**: M28
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 173: Encrypted P2P sessions [Todo]

- **Area**: P2P
- **Why / hint**: M22
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 174: Connection pooling [Todo]

- **Area**: P2P
- **Why / hint**: M26
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 175: Message queue backpressure [Todo]

- **Area**: P2P
- **Why / hint**: M27
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 176: Bootstrap enode parsing [Todo]

- **Area**: P2P
- **Why / hint**: placeholder enode
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 177: mDNS discovery productization [Todo]

- **Area**: P2P
- **Why / hint**: peer_discovery
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 178: DNS seed reliability [Todo]

- **Area**: P2P
- **Why / hint**: discovery errors
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 179: P2P metrics monitoring [Todo]

- **Area**: P2P
- **Why / hint**: M25
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 180: Max inbound/outbound enforce [Todo]

- **Area**: P2P
- **Why / hint**: config unused
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 181: Enable P2P integration tests [Todo]

- **Area**: Testing
- **Why / hint**: tests/integration
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 182: DevP2P RLPx compatibility [Todo]

- **Area**: P2P
- **Why / hint**: custom stack
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 183: Enable logging CMake [Done]

- **Area**: Logging
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 184: Enable metrics CMake [Done]

- **Area**: Metrics
- **Why / hint**: DONE recent
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 185: Real Prometheus HTTP exposer [Todo]

- **Area**: Metrics
- **Why / hint**: metrics_server stub
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 186: Restore prometheus-cpp dep [Todo]

- **Area**: Deps
- **Why / hint**: conanfile
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 187: Health readiness endpoints [Todo]

- **Area**: Ops
- **Why / hint**: M12
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 188: K8s liveness probes [Todo]

- **Area**: Deploy
- **Why / hint**: HEALTHCHECK --help
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 189: C++ YAML config loader [Todo]

- **Area**: Config
- **Why / hint**: node wiring
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 190: Env-specific config overlays [Todo]

- **Area**: Config
- **Why / hint**: single yaml
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 191: Runtime config reload [Todo]

- **Area**: Config
- **Why / hint**: M9 feature
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 192: Build-time log version stamp [Todo]

- **Area**: Logging
- **Why / hint**: hardcoded 1.0.0
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 193: Unify chainforge naming [Todo]

- **Area**: Build
- **Why / hint**: underscore vs hyphen
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 194: OpenTelemetry Jaeger SDK [Todo]

- **Area**: Tracing
- **Why / hint**: compose only
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 195: Enable logging metrics tests [Todo]

- **Area**: Testing
- **Why / hint**: disabled
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 196: Node metrics catalog wiring [Todo]

- **Area**: Metrics
- **Why / hint**: chainforge_metrics
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 197: Prometheus alerting rules [Todo]

- **Area**: Ops
- **Why / hint**: rule_files empty
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 198: Fix postgres redis scrape [Todo]

- **Area**: Ops
- **Why / hint**: prometheus.yml
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 199: Prod profile disables debug [Todo]

- **Area**: Security
- **Why / hint**: development flags
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 200: Config schema validation [Todo]

- **Area**: Config
- **Why / hint**: no validation
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 201: Tx signature in protobuf [Todo]

- **Area**: Serialization
- **Why / hint**: serializer.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 202: Address format validation [Todo]

- **Area**: Serialization
- **Why / hint**: validator always ok
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 203: Amount range validation [Todo]

- **Area**: Serialization
- **Why / hint**: validator
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 204: Timestamp drift in validator [Todo]

- **Area**: Serialization
- **Why / hint**: unused constants
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 205: Hash length validation [Todo]

- **Area**: Serialization
- **Why / hint**: validator
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 206: RLP codec module [Todo]

- **Area**: Serialization
- **Why / hint**: needed by core
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 207: Proto domain model fidelity [Todo]

- **Area**: Serialization
- **Why / hint**: UTXO-ish vs account
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 208: Versioned schema migration [Todo]

- **Area**: Serialization
- **Why / hint**: proto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 209: Canonical encoding golden tests [Todo]

- **Area**: Testing
- **Why / hint**: serialization
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 210: Decode size DoS limits [Todo]

- **Area**: Security
- **Why / hint**: serializer
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 211: Deduplicate serialization tests [Todo]

- **Area**: Testing
- **Why / hint**: two trees
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 212: SSZ alternative codec [Todo]

- **Area**: Serialization
- **Why / hint**: future
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 213: Create modules/explorer [Todo]

- **Area**: Explorer
- **Why / hint**: missing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 214: Create modules/execution [Todo]

- **Area**: Execution
- **Why / hint**: missing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 215: Create tools package [Todo]

- **Area**: Tools
- **Why / hint**: missing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 216: Node composition root [Done]

- **Area**: Node
- **Why / hint**: DONE skeleton
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 217: Genesis config loader [Todo]

- **Area**: Node
- **Why / hint**: zero genesis
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 218: Chain sync state machine [Todo]

- **Area**: Node
- **Why / hint**: missing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 219: Explorer HTTP API [Todo]

- **Area**: Explorer
- **Why / hint**: ports unused
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 220: Explorer indexer pipeline [Todo]

- **Area**: Explorer
- **Why / hint**: postgres unused
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 221: Dockerfile real node binary [Done]

- **Area**: Deploy
- **Why / hint**: DONE chainforge-node
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 222: Explorer Dockerfile real target [Todo]

- **Area**: Deploy
- **Why / hint**: Dockerfile.explorer
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 223: CLI flags for node [Done]

- **Area**: Node
- **Why / hint**: --help/--self-check done
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 224: Graceful shutdown signals [Todo]

- **Area**: Node
- **Why / hint**: idle loop
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 225: Vertical slice E2E mine+rpc [Todo]

- **Area**: Node
- **Why / hint**: next
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 226: Default BUILD_TESTING ON [Todo]

- **Area**: Build
- **Why / hint**: CMakeLists
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 227: CI enables tests meaningfully [Todo]

- **Area**: CI
- **Why / hint**: ci.yml
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 228: Coverage ENABLE_COVERAGE flag [Todo]

- **Area**: CI
- **Why / hint**: TestCoverage.cmake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 229: Integration fuzz labels truth [Todo]

- **Area**: CI
- **Why / hint**: disabled tests
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 230: Performance tests binary [Todo]

- **Area**: CI
- **Why / hint**: missing target
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 231: Warnings-as-errors policy [Todo]

- **Area**: Build
- **Why / hint**: secp256k1 exceptions
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 232: Windows script parity [Todo]

- **Area**: DX
- **Why / hint**: scripts ps1/bat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 233: BUILD_TOOLS subdirectory [Todo]

- **Area**: Build
- **Why / hint**: commented
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 234: BUILD_EXAMPLES usage [Todo]

- **Area**: Build
- **Why / hint**: option unused
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 235: USE_SANITIZERS wiring [Todo]

- **Area**: CI
- **Why / hint**: option OFF
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 236: Package config consumers [Todo]

- **Area**: Build
- **Why / hint**: install cmake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 237: compile_commands export [Todo]

- **Area**: DX
- **Why / hint**: clangd
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 238: Deduplicate CI matrix jobs [Todo]

- **Area**: CI
- **Why / hint**: ci.yml
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 239: Document Conan profile reqs [Todo]

- **Area**: Docs
- **Why / hint**: ConanSetup.cmake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 240: Externalize Compose secrets [Todo]

- **Area**: Security
- **Why / hint**: docker-compose
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 241: Externalize config passwords [Todo]

- **Area**: Security
- **Why / hint**: config.yaml
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 242: Helm ExternalSecrets [Todo]

- **Area**: Deploy
- **Why / hint**: values passwords
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 243: Helm templates directory [Todo]

- **Area**: Deploy
- **Why / hint**: missing templates/
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 244: Real deploy CI job [Todo]

- **Area**: CI
- **Why / hint**: echo placeholder
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 245: Cosign image signing [Todo]

- **Area**: Security
- **Why / hint**: planned
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 246: Redis healthcheck AUTH [Todo]

- **Area**: Deploy
- **Why / hint**: redis-cli -a
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 247: Multi-arch container images [Todo]

- **Area**: Deploy
- **Why / hint**: docker
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 248: Writable data with readOnlyRoot [Todo]

- **Area**: Deploy
- **Why / hint**: helm conflict
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 249: NetworkPolicies [Todo]

- **Area**: Deploy
- **Why / hint**: helm
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 250: Ingress TLS secrets [Todo]

- **Area**: Deploy
- **Why / hint**: explorer values
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 251: HPA for node [Todo]

- **Area**: Deploy
- **Why / hint**: helm
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 252: Init containers migrations [Todo]

- **Area**: Deploy
- **Why / hint**: postgres
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 253: Prod compose overlay [Todo]

- **Area**: Deploy
- **Why / hint**: single compose
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 254: SBOM on node image [Todo]

- **Area**: CI
- **Why / hint**: syft path
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 255: Grafana admin password env [Todo]

- **Area**: Security
- **Why / hint**: default
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 256: Tools image usefulness [Todo]

- **Area**: Deploy
- **Why / hint**: Dockerfile.tools
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 257: Re-enable disabled core tests [Todo]

- **Area**: Testing
- **Why / hint**: CMake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 258: Enable crypto_tests [Todo]

- **Area**: Testing
- **Why / hint**: CMake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 259: Enable logging metrics tests [Todo]

- **Area**: Testing
- **Why / hint**: CMake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 260: Enable integration_tests [Todo]

- **Area**: Testing
- **Why / hint**: CMake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 261: Real libFuzzer harnesses [Todo]

- **Area**: Testing
- **Why / hint**: fuzz are gtest
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 262: Enable fuzz_tests target [Todo]

- **Area**: Testing
- **Why / hint**: CMake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 263: Consensus mempool storage RPC tests [Todo]

- **Area**: Testing
- **Why / hint**: missing suites
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 264: Property-based tx/block tests [Todo]

- **Area**: Testing
- **Why / hint**: framework demo
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 265: Coverage gate threshold [Todo]

- **Area**: CI
- **Why / hint**: no fail threshold
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 266: CI BUILD_TESTING ON [Todo]

- **Area**: CI
- **Why / hint**: explicit flag
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 267: Mocks for storage RPC [Todo]

- **Area**: Testing
- **Why / hint**: mock_objects
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 268: Deterministic test clocks [Todo]

- **Area**: Testing
- **Why / hint**: wall clock
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 269: Sanitizer-clean suite [Todo]

- **Area**: CI
- **Why / hint**: sanitizer jobs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 270: Windows CI runner [Todo]

- **Area**: CI
- **Why / hint**: ubuntu-centric
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 271: Golden vector corpus [Todo]

- **Area**: Testing
- **Why / hint**: crypto/serialization
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 272: RLP fuzz harness [Todo]

- **Area**: Testing
- **Why / hint**: future
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 273: Benchmark suite binary [Todo]

- **Area**: Perf
- **Why / hint**: performance_tests
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 274: Mempool PQ cost fix [Todo]

- **Area**: Perf
- **Why / hint**: O(n) rebuild
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 275: Parallel PoW hashing [Todo]

- **Area**: Perf
- **Why / hint**: single thread
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 276: Storage write benchmarks [Todo]

- **Area**: Perf
- **Why / hint**: rocksdb
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 277: Serialization throughput benches [Todo]

- **Area**: Perf
- **Why / hint**: microbench
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 278: P2P zero-copy buffers [Todo]

- **Area**: Perf
- **Why / hint**: messages
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 279: End-to-end latency histograms [Todo]

- **Area**: Metrics
- **Why / hint**: unused registry
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 280: Block cache from config [Todo]

- **Area**: Perf
- **Why / hint**: YAML unused
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 281: Mempool callback race hardening [Todo]

- **Area**: Perf
- **Why / hint**: unlock then cb
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 282: Hash cache stress tests [Todo]

- **Area**: Perf
- **Why / hint**: tx/block cache
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 283: Threat model document [Todo]

- **Area**: Security
- **Why / hint**: security.md
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 284: Ban stub crypto in Release [Todo]

- **Area**: Security
- **Why / hint**: compile guards
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 285: RPC exposure policy [Todo]

- **Area**: Security
- **Why / hint**: bind/cors
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 286: Disable debug endpoints prod [Todo]

- **Area**: Security
- **Why / hint**: config
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 287: Secret scanning in CI [Todo]

- **Area**: Security
- **Why / hint**: gitleaks
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 288: Input size limits RPC/P2P [Todo]

- **Area**: Security
- **Why / hint**: DoS
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 289: Eclipse resistance [Todo]

- **Area**: Security
- **Why / hint**: peer scoring
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 290: SBOM attestation [Todo]

- **Area**: Security
- **Why / hint**: ci
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 291: TLS everywhere [Todo]

- **Area**: Security
- **Why / hint**: rpc metrics
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 292: HSM key management hooks [Todo]

- **Area**: Security
- **Why / hint**: wallet
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 293: Supply-chain image attest [Todo]

- **Area**: Security
- **Why / hint**: cosign
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 294: Keep ROADMAP status honest [Todo]

- **Area**: Docs
- **Why / hint**: process
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 295: Document enabled modules [Todo]

- **Area**: Docs
- **Why / hint**: architecture
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 296: Endpoint reality refresh [Todo]

- **Area**: Docs
- **Why / hint**: endpoints.md
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 297: Compose runbook accuracy [Todo]

- **Area**: Docs
- **Why / hint**: deployment
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 298: Architecture map update [Todo]

- **Area**: Docs
- **Why / hint**: node added
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 299: Observability what works [Todo]

- **Area**: Docs
- **Why / hint**: stub metrics
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 300: Contributing BUILD_TESTING [Todo]

- **Area**: Docs
- **Why / hint**: contributing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 301: Remove documentation redirect debt [Todo]

- **Area**: Docs
- **Why / hint**: documentation/
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 302: Secret rotation guide [Todo]

- **Area**: Docs
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 303: Changelog versioning policy [Todo]

- **Area**: Docs
- **Why / hint**: releases
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 304: Node --self-check in CI [Todo]

- **Area**: CI
- **Why / hint**: keccak KAT
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 305: Address checksum tests [Todo]

- **Area**: Testing
- **Why / hint**: EIP-55
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 306: Block linking integration test [Todo]

- **Area**: Testing
- **Why / hint**: core+storage
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 307: Mine block E2E with difficulty 1 [Todo]

- **Area**: Consensus
- **Why / hint**: node
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 308: RPC eth_blockNumber from storage [Todo]

- **Area**: RPC
- **Why / hint**: not mock
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 309: Config loader for node main [Todo]

- **Area**: Node
- **Why / hint**: YAML
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 310: P2P start from node main [Todo]

- **Area**: Node
- **Why / hint**: not wired
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 311: Metrics scrape from node [Todo]

- **Area**: Node
- **Why / hint**: 8080
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 312: Health /readyz /livez [Todo]

- **Area**: Node
- **Why / hint**: M12
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 313: Structured logging in node [Todo]

- **Area**: Node
- **Why / hint**: logging module
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 314: Explorer stub binary [Todo]

- **Area**: Explorer
- **Why / hint**: compose
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 315: Index blocks to Postgres [Todo]

- **Area**: Explorer
- **Why / hint**: SQL schema
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 316: GraphQL schema draft [Todo]

- **Area**: API
- **Why / hint**: M47
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 317: WebSocket subscriptions [Todo]

- **Area**: API
- **Why / hint**: M46
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 318: Light client headers API [Todo]

- **Area**: Protocol
- **Why / hint**: M42
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 319: Snapshot export/import [Todo]

- **Area**: Sync
- **Why / hint**: M43
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 320: State pruning modes [Todo]

- **Area**: Storage
- **Why / hint**: M44
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 321: Gas metering skeleton [Todo]

- **Area**: Execution
- **Why / hint**: M34
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 322: EVM interpreter stub [Todo]

- **Area**: Execution
- **Why / hint**: M33
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 323: Fork choice rule [Todo]

- **Area**: Consensus
- **Why / hint**: M37
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 324: Finality gadget research [Todo]

- **Area**: Consensus
- **Why / hint**: M38
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 325: Validator set management [Todo]

- **Area**: Consensus
- **Why / hint**: M39
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 326: Slashing condition docs [Todo]

- **Area**: Consensus
- **Why / hint**: M40
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 327: Checkpoint sync [Todo]

- **Area**: Sync
- **Why / hint**: M41
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 328: Privacy feature spikes [Todo]

- **Area**: Research
- **Why / hint**: M50
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 329: Bridge design doc [Todo]

- **Area**: Research
- **Why / hint**: M51
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 330: L2 rollup interface sketch [Todo]

- **Area**: Research
- **Why / hint**: M52
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 331: Wallet module boundaries [Todo]

- **Area**: Wallet
- **Why / hint**: M53
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 332: On-chain governance sketch [Todo]

- **Area**: Governance
- **Why / hint**: M54
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 333: Formal verification plan [Todo]

- **Area**: Security
- **Why / hint**: M55
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 334: Perf budget dashboard [Todo]

- **Area**: Perf
- **Why / hint**: M56
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 335: Advanced monitoring alerts [Todo]

- **Area**: Ops
- **Why / hint**: M57
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 336: Disaster recovery runbook [Todo]

- **Area**: Ops
- **Why / hint**: M58
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 337: Production Helm complete [Todo]

- **Area**: Deploy
- **Why / hint**: M59
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 338: Training / tutorials [Todo]

- **Area**: Docs
- **Why / hint**: M60
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 339: Break crypto/core CMake cycle [Done]

- **Area**: Build
- **Why / hint**: DONE reorder
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 340: Public node bind opt-in [Todo]

- **Area**: Security
- **Why / hint**: explicit flag
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 341: Fuzz keccak sponge [Todo]

- **Area**: Testing
- **Why / hint**: keccak.cpp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 342: Property tests for curve ops [Todo]

- **Area**: Testing
- **Why / hint**: secp256k1
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 343: Benchmark keccak vs sha [Todo]

- **Area**: Perf
- **Why / hint**: crypto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 344: Audit OpenSSL usage [Todo]

- **Area**: Security
- **Why / hint**: deprecated APIs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 345: Move secrets to .env.example [Todo]

- **Area**: DX
- **Why / hint**: compose
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 346: Makefile or Taskfile [Todo]

- **Area**: DX
- **Why / hint**: README promises
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 347: CMakePresets document [Todo]

- **Area**: DX
- **Why / hint**: presets.json
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 348: clang-tidy CI job [Todo]

- **Area**: CI
- **Why / hint**: static analysis
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 349: clang-format config enforce [Todo]

- **Area**: DX
- **Why / hint**: format
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 350: pre-commit hooks [Todo]

- **Area**: DX
- **Why / hint**: git
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 351: Dependabot/Renovater [Todo]

- **Area**: CI
- **Why / hint**: deps
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 352: License header check [Todo]

- **Area**: Legal
- **Why / hint**: headers
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 353: SPDX in SBOM [Todo]

- **Area**: Compliance
- **Why / hint**: syft
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 354: Codeowners for modules [Todo]

- **Area**: DX
- **Why / hint**: .github
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 355: Issue templates [Todo]

- **Area**: DX
- **Why / hint**: github
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 356: PR template checklist [Todo]

- **Area**: DX
- **Why / hint**: tests docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 357: ADR process for consensus [Todo]

- **Area**: Docs
- **Why / hint**: architecture decisions
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 358: Module dependency diagram CI [Todo]

- **Area**: Docs
- **Why / hint**: graph
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 359: Public API freeze policy [Todo]

- **Area**: DX
- **Why / hint**: headers
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 360: ABI stability notes [Todo]

- **Area**: DX
- **Why / hint**: shared libs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 361: i18n for RPC errors [Todo]

- **Area**: RPC
- **Why / hint**: messages
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 362: Rate-limit metrics [Todo]

- **Area**: Metrics
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 363: Peer ban persistence [Todo]

- **Area**: P2P
- **Why / hint**: postgres
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 364: Bootstrap node list file [Todo]

- **Area**: P2P
- **Why / hint**: config
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 365: Discovery table size limits [Todo]

- **Area**: P2P
- **Why / hint**: DoS
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 366: Message size caps [Todo]

- **Area**: P2P
- **Why / hint**: frames
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 367: Compression for P2P [Todo]

- **Area**: P2P
- **Why / hint**: optional
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 368: QUIC transport spike [Todo]

- **Area**: P2P
- **Why / hint**: research
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 369: libp2p evaluation [Todo]

- **Area**: P2P
- **Why / hint**: README claim
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 370: UPnP mapping [Todo]

- **Area**: P2P
- **Why / hint**: NAT
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 371: STUN/TURN support [Todo]

- **Area**: P2P
- **Why / hint**: NAT
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 372: Peer reputation decay [Todo]

- **Area**: P2P
- **Why / hint**: scoring
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 373: Eclipse attack tests [Todo]

- **Area**: Testing
- **Why / hint**: p2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 374: Sybil resistance notes [Todo]

- **Area**: Security
- **Why / hint**: p2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 375: Chain ID in handshake [Todo]

- **Area**: P2P
- **Why / hint**: protocol
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 376: Genesis hash handshake [Todo]

- **Area**: P2P
- **Why / hint**: protocol
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 377: Fork ID (EIP-2124) style [Todo]

- **Area**: P2P
- **Why / hint**: compat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 378: Header-first sync [Todo]

- **Area**: Sync
- **Why / hint**: headers
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 379: Body download pipelining [Todo]

- **Area**: Sync
- **Why / hint**: bodies
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 380: State snap sync research [Todo]

- **Area**: Sync
- **Why / hint**: snap
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 381: Checkpoint trusted roots [Todo]

- **Area**: Sync
- **Why / hint**: light
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 382: Reorg handling depth [Todo]

- **Area**: Consensus
- **Why / hint**: forks
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 383: Uncle inclusion rules [Todo]

- **Area**: Consensus
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 384: Block gas limit adjustment [Todo]

- **Area**: Execution
- **Why / hint**: params
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 385: Base fee EIP-1559 [Todo]

- **Area**: Execution
- **Why / hint**: fees
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 386: Access lists [Todo]

- **Area**: Execution
- **Why / hint**: Berlin
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 387: Precompile set [Todo]

- **Area**: Execution
- **Why / hint**: ecrecover etc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 388: Contract create costs [Todo]

- **Area**: Execution
- **Why / hint**: gas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 389: Refund handling [Todo]

- **Area**: Execution
- **Why / hint**: gas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 390: Selfdestruct semantics [Todo]

- **Area**: Execution
- **Why / hint**: legacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 391: Transient storage [Todo]

- **Area**: Execution
- **Why / hint**: EIP-1153
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 392: EOF / EVM ObjectFormat track [Todo]

- **Area**: Execution
- **Why / hint**: research
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 393: Account abstraction spike [Todo]

- **Area**: Execution
- **Why / hint**: ERC-4337
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 394: Tracer / debug_traceTransaction [Todo]

- **Area**: RPC
- **Why / hint**: debug
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 395: admin_* RPC namespace [Todo]

- **Area**: RPC
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 396: debug_* gated endpoints [Todo]

- **Area**: RPC
- **Why / hint**: dev flag
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 397: net_peerCount [Todo]

- **Area**: RPC
- **Why / hint**: p2p stats
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 398: txpool_content [Todo]

- **Area**: RPC
- **Why / hint**: mempool
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 399: txpool_inspect [Todo]

- **Area**: RPC
- **Why / hint**: mempool
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 400: txpool_status [Todo]

- **Area**: RPC
- **Why / hint**: mempool
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 401: eth_syncing [Todo]

- **Area**: RPC
- **Why / hint**: sync state
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 402: eth_mining [Todo]

- **Area**: RPC
- **Why / hint**: consensus
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 403: eth_hashrate [Todo]

- **Area**: RPC
- **Why / hint**: mining stats
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 404: eth_getCode [Todo]

- **Area**: RPC
- **Why / hint**: execution
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 405: eth_getStorageAt [Todo]

- **Area**: RPC
- **Why / hint**: state
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 406: eth_getProof [Todo]

- **Area**: RPC
- **Why / hint**: light
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 407: eth_feeHistory [Todo]

- **Area**: RPC
- **Why / hint**: 1559
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 408: eth_maxPriorityFeePerGas [Todo]

- **Area**: RPC
- **Why / hint**: 1559
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 409: web3_sha3 [Todo]

- **Area**: RPC
- **Why / hint**: keccak
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 410: net_listening [Todo]

- **Area**: RPC
- **Why / hint**: p2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 411: Subscribe newHeads [Todo]

- **Area**: WS
- **Why / hint**: subscriptions
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 412: Subscribe logs [Todo]

- **Area**: WS
- **Why / hint**: filters
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 413: Subscribe pendingTx [Todo]

- **Area**: WS
- **Why / hint**: mempool
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 414: Filter timeout GC [Todo]

- **Area**: RPC
- **Why / hint**: filters
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 415: IPC endpoint optional [Todo]

- **Area**: RPC
- **Why / hint**: unix/named pipe
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 416: gRPC control plane [Todo]

- **Area**: API
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 417: REST explorer read API [Todo]

- **Area**: Explorer
- **Why / hint**: HTTP
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 418: Explorer search by address [Todo]

- **Area**: Explorer
- **Why / hint**: index
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 419: Explorer token transfers [Todo]

- **Area**: Explorer
- **Why / hint**: ERC20
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 420: Explorer NFT support [Todo]

- **Area**: Explorer
- **Why / hint**: ERC721
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 421: Explorer charts API [Todo]

- **Area**: Explorer
- **Why / hint**: analytics
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 422: CSV export for blocks [Todo]

- **Area**: Tools
- **Why / hint**: export
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 423: Genesis generator tool [Todo]

- **Area**: Tools
- **Why / hint**: genesis.json
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 424: Keygen CLI [Todo]

- **Area**: Tools
- **Why / hint**: secp256k1
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 425: Tx signer CLI [Todo]

- **Area**: Tools
- **Why / hint**: raw tx
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 426: DB inspect CLI [Todo]

- **Area**: Tools
- **Why / hint**: storage
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 427: Peer ping CLI [Todo]

- **Area**: Tools
- **Why / hint**: p2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 428: Benchmark CLI [Todo]

- **Area**: Tools
- **Why / hint**: perf
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 429: Faucet for devnet [Todo]

- **Area**: Tools
- **Why / hint**: dev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 430: Local multi-node compose [Todo]

- **Area**: Deploy
- **Why / hint**: 3-node
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 431: Chaos kill-pod tests [Todo]

- **Area**: Deploy
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 432: Backup PVC snapshots [Todo]

- **Area**: Deploy
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 433: Restore drill runbook [Todo]

- **Area**: Ops
- **Why / hint**: DR
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 434: SLO definitions [Todo]

- **Area**: Ops
- **Why / hint**: latency/error
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 435: Error budget policy [Todo]

- **Area**: Ops
- **Why / hint**: sre
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 436: On-call alert routing [Todo]

- **Area**: Ops
- **Why / hint**: pager
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 437: Log retention policy [Todo]

- **Area**: Ops
- **Why / hint**: logging
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 438: PII redaction in logs [Todo]

- **Area**: Security
- **Why / hint**: logging
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 439: Audit log channel [Todo]

- **Area**: Security
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 440: RBAC for admin RPC [Todo]

- **Area**: Security
- **Why / hint**: auth
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 441: mTLS between node-explorer [Todo]

- **Area**: Security
- **Why / hint**: mesh
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 442: NetworkPolicy deny-all default [Todo]

- **Area**: Deploy
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 443: PodSecurity restricted [Todo]

- **Area**: Deploy
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 444: Image digest pins [Todo]

- **Area**: Deploy
- **Why / hint**: compose/helm
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 445: Base image CVE scanning [Todo]

- **Area**: CI
- **Why / hint**: trivy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 446: Dependency license check [Todo]

- **Area**: CI
- **Why / hint**: fossa/ort
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 447: Reproducible builds [Todo]

- **Area**: Build
- **Why / hint**: bit-identical
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 448: Deterministic protobuf [Todo]

- **Area**: Build
- **Why / hint**: proto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 449: Pinned Conan lockfile [Todo]

- **Area**: Build
- **Why / hint**: conan.lock
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 450: Conan center mirror policy [Todo]

- **Area**: Build
- **Why / hint**: ci
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 451: Offline build mode [Todo]

- **Area**: DX
- **Why / hint**: airgap
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 452: Devcontainer support [Todo]

- **Area**: DX
- **Why / hint**: vscode
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 453: Nix flake optional [Todo]

- **Area**: DX
- **Why / hint**: repro
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 454: Bazel evaluation [Todo]

- **Area**: Build
- **Why / hint**: research
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 455: Module interface units [Todo]

- **Area**: Build
- **Why / hint**: C++20 modules
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 456: Reduce Boost surface [Todo]

- **Area**: Build
- **Why / hint**: asio only
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 457: Header hygiene IWYU [Todo]

- **Area**: DX
- **Why / hint**: includes
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 458: Unity build option [Todo]

- **Area**: Build
- **Why / hint**: compile time
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 459: ccache in CI [Todo]

- **Area**: CI
- **Why / hint**: speed
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 460: Sccache Windows [Todo]

- **Area**: CI
- **Why / hint**: speed
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 461: Split mega CI workflow [Todo]

- **Area**: CI
- **Why / hint**: reusable
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 462: Required status checks [Todo]

- **Area**: CI
- **Why / hint**: branch protection
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 463: Merge queue [Todo]

- **Area**: DX
- **Why / hint**: github
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 464: Auto-label PRs by path [Todo]

- **Area**: DX
- **Why / hint**: actions
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 465: Size-limit bot for binaries [Todo]

- **Area**: CI
- **Why / hint**: artifacts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 466: Release please / changesets [Todo]

- **Area**: DX
- **Why / hint**: versioning
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 467: Signed commits required [Todo]

- **Area**: Security
- **Why / hint**: git
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 468: DCO or CLA [Todo]

- **Area**: Legal
- **Why / hint**: contributing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 469: Security.md disclosure [Todo]

- **Area**: Security
- **Why / hint**: policy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 470: Bug bounty readiness [Todo]

- **Area**: Security
- **Why / hint**: process
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 471: Pentest checklist [Todo]

- **Area**: Security
- **Why / hint**: external
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 472: Crypto review checklist [Todo]

- **Area**: Security
- **Why / hint**: milestones
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 473: Constant-time audit [Todo]

- **Area**: Security
- **Why / hint**: crypto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 474: Side-channel tests [Todo]

- **Area**: Testing
- **Why / hint**: timing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 475: Memory sanitizer on crypto [Todo]

- **Area**: CI
- **Why / hint**: asan
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 476: Undefined behavior sanitizer [Todo]

- **Area**: CI
- **Why / hint**: ubsan
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 477: Thread sanitizer on p2p [Todo]

- **Area**: CI
- **Why / hint**: tsan
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 478: Valgrind optional job [Todo]

- **Area**: CI
- **Why / hint**: linux
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 479: Coverage for keccak paths [Todo]

- **Area**: Testing
- **Why / hint**: branches
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 480: Mutation testing spike [Todo]

- **Area**: Testing
- **Why / hint**: research
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 481: Contract tests vs geth [Todo]

- **Area**: Testing
- **Why / hint**: compat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 482: Hive Ethereum tests subset [Todo]

- **Area**: Testing
- **Why / hint**: interop
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 483: Execution-spec tests [Todo]

- **Area**: Testing
- **Why / hint**: EEST
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 484: RLP test suite from ethereum [Todo]

- **Area**: Testing
- **Why / hint**: fixtures
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 485: Trie test suite [Todo]

- **Area**: Testing
- **Why / hint**: state
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 486: Difficulty test vectors [Todo]

- **Area**: Testing
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 487: Transaction test vectors [Todo]

- **Area**: Testing
- **Why / hint**: tx
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 488: Block test vectors [Todo]

- **Area**: Testing
- **Why / hint**: blocks
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 489: VM test vectors [Todo]

- **Area**: Testing
- **Why / hint**: execution
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 490: Blake2/sha3 distinction docs [Todo]

- **Area**: Docs
- **Why / hint**: keccak vs sha3
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 491: Endianness notes for keccak [Todo]

- **Area**: Docs
- **Why / hint**: sponge
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 492: Encoding standards ADR [Todo]

- **Area**: Docs
- **Why / hint**: RLP vs protobuf
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 493: Why protobuf on wire [Todo]

- **Area**: Docs
- **Why / hint**: design
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 494: Migration off mock RPC [Todo]

- **Area**: Docs
- **Why / hint**: guide
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 495: Local net tutorial [Todo]

- **Area**: Docs
- **Why / hint**: getting started
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 496: Mining tutorial [Todo]

- **Area**: Docs
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 497: RPC client examples [Todo]

- **Area**: Docs
- **Why / hint**: curl/web3
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 498: Grafana dashboard polish [Todo]

- **Area**: Ops
- **Why / hint**: dashboards
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 499: Jaeger sampling config [Todo]

- **Area**: Ops
- **Why / hint**: tracing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 500: Log correlation IDs [Todo]

- **Area**: Ops
- **Why / hint**: logging
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 501: Request ID middleware [Todo]

- **Area**: RPC
- **Why / hint**: http
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 502: Slow-query logging [Todo]

- **Area**: Storage
- **Why / hint**: rocksdb
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 503: Compaction stall metrics [Todo]

- **Area**: Storage
- **Why / hint**: rocksdb
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 504: Memtable size tuning guide [Todo]

- **Area**: Docs
- **Why / hint**: storage
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 505: Peer churn simulation [Todo]

- **Area**: Testing
- **Why / hint**: p2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 506: Bandwidth budget tests [Todo]

- **Area**: Perf
- **Why / hint**: p2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 507: Disk IO budget tests [Todo]

- **Area**: Perf
- **Why / hint**: storage
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 508: CPU budget for mining [Todo]

- **Area**: Perf
- **Why / hint**: consensus
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 509: Memory budget for mempool [Todo]

- **Area**: Perf
- **Why / hint**: mempool
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 510: GC-like state prune jobs [Todo]

- **Area**: Storage
- **Why / hint**: pruning
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 511: Cold/hot storage tiers [Todo]

- **Area**: Storage
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 512: Archive node mode [Todo]

- **Area**: Node
- **Why / hint**: full history
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 513: Full node mode [Todo]

- **Area**: Node
- **Why / hint**: default
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 514: Light node mode [Todo]

- **Area**: Node
- **Why / hint**: headers
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 515: Validator node mode [Todo]

- **Area**: Node
- **Why / hint**: pos future
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 516: Bootnode mode [Todo]

- **Area**: Node
- **Why / hint**: discovery only
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 517: RPC-only gateway mode [Todo]

- **Area**: Node
- **Why / hint**: no p2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 518: Read-only replica explorer [Todo]

- **Area**: Explorer
- **Why / hint**: scale
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 519: Sharded indexer [Todo]

- **Area**: Explorer
- **Why / hint**: scale
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 520: CQRS read models [Todo]

- **Area**: Explorer
- **Why / hint**: design
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 521: Event sourcing for index [Todo]

- **Area**: Explorer
- **Why / hint**: design
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 522: Outbox for explorer sync [Todo]

- **Area**: Explorer
- **Why / hint**: reliability
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 523: Exactly-once index claims [Todo]

- **Area**: Explorer
- **Why / hint**: dedupe
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 524: Reindex from genesis tool [Todo]

- **Area**: Tools
- **Why / hint**: explorer
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 525: Prune explorer tables [Todo]

- **Area**: Ops
- **Why / hint**: sql
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 526: DB migration tooling [Todo]

- **Area**: Ops
- **Why / hint**: flyway/liquibase
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 527: Schema version table [Todo]

- **Area**: Ops
- **Why / hint**: postgres
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 528: Connection pool metrics [Todo]

- **Area**: Ops
- **Why / hint**: postgres/redis
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 529: Circuit breaker to redis [Todo]

- **Area**: Explorer
- **Why / hint**: resilience
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 530: Cache stampede protection [Todo]

- **Area**: Explorer
- **Why / hint**: redis
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 531: CDN for static explorer UI [Todo]

- **Area**: Deploy
- **Why / hint**: future UI
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 532: Explorer UI skeleton [Todo]

- **Area**: Explorer
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 533: Wallet connect docs [Todo]

- **Area**: Docs
- **Why / hint**: dapp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 534: MetaMask custom network guide [Todo]

- **Area**: Docs
- **Why / hint**: chain 1337
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 535: Hardhat/Foundry local target [Todo]

- **Area**: DX
- **Why / hint**: devex
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 536: Anvil-compatible subset [Todo]

- **Area**: RPC
- **Why / hint**: compat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 537: JSON-RPC method matrix [Todo]

- **Area**: Docs
- **Why / hint**: endpoints
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 538: Error code catalog [Todo]

- **Area**: Docs
- **Why / hint**: rpc errors
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 539: Capability bits in web3_clientVersion [Todo]

- **Area**: RPC
- **Why / hint**: discovery
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 540: Node info metrics labels [Todo]

- **Area**: Metrics
- **Why / hint**: version/network
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 541: Build id / git sha stamp [Todo]

- **Area**: Build
- **Why / hint**: -DVERSION
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 542: Feature flags file [Todo]

- **Area**: Config
- **Why / hint**: runtime
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 543: Kill switch for mining [Todo]

- **Area**: Ops
- **Why / hint**: admin
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 544: Kill switch for tx accept [Todo]

- **Area**: Ops
- **Why / hint**: mempool
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 545: Maintenance mode RPC [Todo]

- **Area**: Ops
- **Why / hint**: 503
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 546: Drain peers before shutdown [Todo]

- **Area**: P2P
- **Why / hint**: graceful
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 547: Flush mempool on shutdown [Todo]

- **Area**: Mempool
- **Why / hint**: persist
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 548: Flush storage on shutdown [Todo]

- **Area**: Storage
- **Why / hint**: sync
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 549: Crash recovery tests [Todo]

- **Area**: Testing
- **Why / hint**: kill -9
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 550: Power-loss simulation [Todo]

- **Area**: Testing
- **Why / hint**: fsync
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 551: Clock skew tolerance [Todo]

- **Area**: Consensus
- **Why / hint**: timestamp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 552: Future block rejection [Todo]

- **Area**: Consensus
- **Why / hint**: timestamp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 553: Too-old block rejection [Todo]

- **Area**: Consensus
- **Why / hint**: timestamp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 554: Gas limit floor/ceil [Todo]

- **Area**: Consensus
- **Why / hint**: params
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 555: ExtraData field policy [Todo]

- **Area**: Core
- **Why / hint**: block header
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 556: Bloom filters for logs [Todo]

- **Area**: Execution
- **Why / hint**: receipts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 557: Receipts trie [Todo]

- **Area**: Execution
- **Why / hint**: blocks
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 558: Transactions trie [Todo]

- **Area**: Execution
- **Why / hint**: blocks
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 559: Withdrawals field prep [Todo]

- **Area**: Core
- **Why / hint**: shanghai
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 560: Blob tx type prep [Todo]

- **Area**: Core
- **Why / hint**: cancun
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 561: Beacon API client spike [Todo]

- **Area**: Consensus
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 562: Engine API spike [Todo]

- **Area**: Consensus
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 563: JWT for Engine API [Todo]

- **Area**: Security
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 564: Fee recipient config [Todo]

- **Area**: Consensus
- **Why / hint**: coinbase
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 565: MEV research notes [Todo]

- **Area**: Research
- **Why / hint**: pbs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 566: Builder API spike [Todo]

- **Area**: Research
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 567: PBS compatibility track [Todo]

- **Area**: Research
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 568: Restaking research [Todo]

- **Area**: Research
- **Why / hint**: eigen
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 569: Account abstraction roadmap [Todo]

- **Area**: Research
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 570: ZK light client research [Todo]

- **Area**: Research
- **Why / hint**: zk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 571: Validity proof verification [Todo]

- **Area**: Research
- **Why / hint**: zk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 572: Data availability sampling [Todo]

- **Area**: Research
- **Why / hint**: das
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 573: Portal network evaluation [Todo]

- **Area**: Research
- **Why / hint**: light
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 574: Stateless client research [Todo]

- **Area**: Research
- **Why / hint**: verkle
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 575: Verkle trie track [Todo]

- **Area**: Research
- **Why / hint**: verkle
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 576: SSZ for consensus types [Todo]

- **Area**: Serialization
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 577: Snappy framing [Todo]

- **Area**: P2P
- **Why / hint**: compression
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 578: Noise XX handshake [Todo]

- **Area**: P2P
- **Why / hint**: security
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 579: Certificate pinning optional [Todo]

- **Area**: Security
- **Why / hint**: tls
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 580: OCSP stapling [Todo]

- **Area**: Security
- **Why / hint**: tls
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 581: HSTS for explorer [Todo]

- **Area**: Security
- **Why / hint**: http
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 582: CSP for explorer UI [Todo]

- **Area**: Security
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 583: SRI for static assets [Todo]

- **Area**: Security
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 584: Dependency SBOM diff PRs [Todo]

- **Area**: CI
- **Why / hint**: syft
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 585: License allowlist enforce [Todo]

- **Area**: CI
- **Why / hint**: policy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 586: Secretless CI OIDC [Todo]

- **Area**: CI
- **Why / hint**: cloud
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 587: Ephemeral runners [Todo]

- **Area**: CI
- **Why / hint**: security
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 588: Hermetic test containers [Todo]

- **Area**: Testing
- **Why / hint**: testcontainers
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 589: Contract fuzz with Echidna [Todo]

- **Area**: Testing
- **Why / hint**: optional
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 590: Differential fuzz vs ref [Todo]

- **Area**: Testing
- **Why / hint**: keccak
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 591: Wycheproof vectors [Todo]

- **Area**: Testing
- **Why / hint**: ecdsa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 592: RFC 6979 deterministic ECDSA [Todo]

- **Area**: Crypto
- **Why / hint**: signatures
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 593: Low-S normalization [Todo]

- **Area**: Crypto
- **Why / hint**: ecdsa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 594: Public key validation on import [Todo]

- **Area**: Crypto
- **Why / hint**: keypair
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 595: Mnemonic / BIP39 optional [Todo]

- **Area**: Wallet
- **Why / hint**: keys
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 596: HD wallet BIP32 path [Todo]

- **Area**: Wallet
- **Why / hint**: keys
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 597: Hardware wallet APDU spike [Todo]

- **Area**: Wallet
- **Why / hint**: ledger
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 598: Multisig account abstraction [Todo]

- **Area**: Wallet
- **Why / hint**: m-of-n
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 599: Social recovery research [Todo]

- **Area**: Wallet
- **Why / hint**: recovery
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 600: Threshold signatures research [Todo]

- **Area**: Crypto
- **Why / hint**: tss
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 601: MPC signing research [Todo]

- **Area**: Crypto
- **Why / hint**: mpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 602: Post-quantum migration notes [Todo]

- **Area**: Research
- **Why / hint**: pq
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 603: Hybrid classical/PQ signatures [Todo]

- **Area**: Research
- **Why / hint**: pq
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 604: Formal grammar for enode URLs [Todo]

- **Area**: P2P
- **Why / hint**: parsing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 605: IPv6 first-class support [Todo]

- **Area**: P2P
- **Why / hint**: addresses
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 606: Dual-stack bind [Todo]

- **Area**: P2P
- **Why / hint**: sockets
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 607: Happy Eyeballs connect [Todo]

- **Area**: P2P
- **Why / hint**: tcp
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 608: SO_REUSEPORT for RPC [Todo]

- **Area**: RPC
- **Why / hint**: linux
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 609: Accept filter / firewall hooks [Todo]

- **Area**: Security
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 610: IP allowlist for admin [Todo]

- **Area**: Security
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 611: GeoIP optional metrics [Todo]

- **Area**: Ops
- **Why / hint**: privacy caution
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 612: Tor hidden service guide [Todo]

- **Area**: Docs
- **Why / hint**: privacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 613: Censorship resistance notes [Todo]

- **Area**: Docs
- **Why / hint**: p2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 614: Localization of docs [Todo]

- **Area**: Docs
- **Why / hint**: i18n
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 615: Diagram generation from code [Todo]

- **Area**: Docs
- **Why / hint**: doxygen/graphviz
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 616: Public roadmap board [Todo]

- **Area**: Docs
- **Why / hint**: github projects
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 617: Quarterly milestone review [Todo]

- **Area**: Process
- **Why / hint**: status sync
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 618: Definition of Done template [Todo]

- **Area**: Process
- **Why / hint**: milestones
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 619: Acceptance test per milestone [Todo]

- **Area**: Process
- **Why / hint**: QA
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 620: Risk register update [Todo]

- **Area**: Process
- **Why / hint**: risks
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 621: Tech debt scoreboard [Todo]

- **Area**: Process
- **Why / hint**: todos count
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 622: TODO budget CI fail [Todo]

- **Area**: CI
- **Why / hint**: max TODOs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 623: Stub symbol linker errors Release [Todo]

- **Area**: Build
- **Why / hint**: no XOR stubs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 624: Feature macro CHAINFORGE_HAS_BLS [Todo]

- **Area**: Build
- **Why / hint**: optional
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 625: Feature macro HAS_ROCKSDB [Todo]

- **Area**: Build
- **Why / hint**: optional
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 626: Feature macro HAS_PROMETHEUS [Todo]

- **Area**: Build
- **Why / hint**: optional
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 627: Optional execution module flag [Todo]

- **Area**: Build
- **Why / hint**: cmake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 628: Optional explorer module flag [Todo]

- **Area**: Build
- **Why / hint**: cmake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 629: Sanitizers preset [Todo]

- **Area**: DX
- **Why / hint**: CMakePresets
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 630: Coverage preset [Todo]

- **Area**: DX
- **Why / hint**: CMakePresets
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 631: RelWithDebInfo default CI [Todo]

- **Area**: CI
- **Why / hint**: symbols
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 632: Split debug symbols package [Todo]

- **Area**: Deploy
- **Why / hint**: dbg
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 633: Crash pad / minidump [Todo]

- **Area**: Ops
- **Why / hint**: diagnostics
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 634: Core dump policy [Todo]

- **Area**: Ops
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 635: Watchdog restart policy [Todo]

- **Area**: Deploy
- **Why / hint**: compose
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 636: PID 1 signal forwarding [Todo]

- **Area**: Deploy
- **Why / hint**: docker
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 637: tini/dumb-init in images [Todo]

- **Area**: Deploy
- **Why / hint**: docker
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 638: Distroless runtime eval [Todo]

- **Area**: Deploy
- **Why / hint**: images
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 639: Chainguard/base rebuild [Todo]

- **Area**: Deploy
- **Why / hint**: supply chain
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 640: SBOM attest in registry [Todo]

- **Area**: Deploy
- **Why / hint**: cosign
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 641: Provenance SLSA level [Todo]

- **Area**: CI
- **Why / hint**: supply chain
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 642: Rekor transparency log [Todo]

- **Area**: Security
- **Why / hint**: sigstore
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 643: Policy controller admit images [Todo]

- **Area**: Deploy
- **Why / hint**: kyverno
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 644: Falco runtime rules [Todo]

- **Area**: Security
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 645: Network egress lockdown [Todo]

- **Area**: Deploy
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 646: Service mesh eval [Todo]

- **Area**: Deploy
- **Why / hint**: istio/linkerd
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 647: Multi-cluster federation [Todo]

- **Area**: Deploy
- **Why / hint**: research
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 648: Edge RPC CDN pattern [Todo]

- **Area**: Deploy
- **Why / hint**: architecture
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 649: Read replica RPC routing [Todo]

- **Area**: Deploy
- **Why / hint**: lb
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 650: Sticky sessions for WS [Todo]

- **Area**: Deploy
- **Why / hint**: lb
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 651: gRPC-JSON transcoder [Todo]

- **Area**: API
- **Why / hint**: gateway
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 652: OpenAPI for explorer [Todo]

- **Area**: API
- **Why / hint**: docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 653: Protobuf buf lint [Todo]

- **Area**: CI
- **Why / hint**: proto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 654: Breaking change detect proto [Todo]

- **Area**: CI
- **Why / hint**: buf breaking
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 655: Semantic versioning modules [Todo]

- **Area**: DX
- **Why / hint**: versions
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 656: Conformance suite nightly [Todo]

- **Area**: CI
- **Why / hint**: schedule
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 657: Canary deploy job [Todo]

- **Area**: CI
- **Why / hint**: deploy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 658: Blue/green node rollout [Todo]

- **Area**: Deploy
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 659: DB migration gate in deploy [Todo]

- **Area**: CI
- **Why / hint**: explorer
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 660: Smoke test post-deploy [Todo]

- **Area**: CI
- **Why / hint**: rpc ping
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 661: Rollback runbook [Todo]

- **Area**: Ops
- **Why / hint**: deploy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 662: Feature flag kill switches [Todo]

- **Area**: Ops
- **Why / hint**: config
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 663: Cost report for cloud [Todo]

- **Area**: Ops
- **Why / hint**: finops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 664: Right-size helm resources [Todo]

- **Area**: Deploy
- **Why / hint**: values
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 665: Vertical pod autoscaler [Todo]

- **Area**: Deploy
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 666: Cluster autoscaler notes [Todo]

- **Area**: Deploy
- **Why / hint**: cloud
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 667: Spot/preemptible node pools [Todo]

- **Area**: Deploy
- **Why / hint**: cost
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 668: Warm pool for miners [Todo]

- **Area**: Deploy
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 669: GPU mining research [Todo]

- **Area**: Research
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 670: ASIC resistance notes [Todo]

- **Area**: Research
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 671: RandomX evaluation [Todo]

- **Area**: Research
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 672: Ethash/Etchash eval [Todo]

- **Area**: Research
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 673: ProgPoW research [Todo]

- **Area**: Research
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 674: Memory-hard PoW option [Todo]

- **Area**: Consensus
- **Why / hint**: config
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 675: Difficulty bomb (ice age) [Todo]

- **Area**: Consensus
- **Why / hint**: optional
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 676: Merge to PoS checklist [Todo]

- **Area**: Consensus
- **Why / hint**: roadmap
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 677: Deposit contract design [Todo]

- **Area**: Consensus
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 678: Withdrawal queue [Todo]

- **Area**: Consensus
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 679: Attestation aggregation [Todo]

- **Area**: Consensus
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 680: Committee assignments [Todo]

- **Area**: Consensus
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 681: RANDAO mixing [Todo]

- **Area**: Consensus
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 682: Slashing detection service [Todo]

- **Area**: Ops
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 683: Doppelganger protection [Todo]

- **Area**: Ops
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 684: Validator client separation [Todo]

- **Area**: Architecture
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 685: Remote signer integration [Todo]

- **Area**: Security
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 686: Web3signer compatibility [Todo]

- **Area**: Security
- **Why / hint**: keys
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 687: Dirk/Vouch evaluation [Todo]

- **Area**: Security
- **Why / hint**: keys
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 688: Keystore v3/v4 support [Todo]

- **Area**: Wallet
- **Why / hint**: keys
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 689: Scrypt/PBKDF2 keystore [Todo]

- **Area**: Wallet
- **Why / hint**: keys
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 690: USB HID signer [Todo]

- **Area**: Wallet
- **Why / hint**: hw
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 691: QR offline signing [Todo]

- **Area**: Wallet
- **Why / hint**: airgap
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 692: PSBT-like workflow [Todo]

- **Area**: Wallet
- **Why / hint**: batch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 693: Batch tx simulation [Todo]

- **Area**: RPC
- **Why / hint**: eth_call many
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 694: State override in eth_call [Todo]

- **Area**: RPC
- **Why / hint**: debug
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 695: Block override in estimate [Todo]

- **Area**: RPC
- **Why / hint**: debug
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 696: Tracer configs [Todo]

- **Area**: RPC
- **Why / hint**: callTracer
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 697: Opcode step debugger [Todo]

- **Area**: Tools
- **Why / hint**: vm
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 698: Gas profiler UI [Todo]

- **Area**: Explorer
- **Why / hint**: dev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 699: Source verify sourcify [Todo]

- **Area**: Explorer
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 700: ABI decode in explorer [Todo]

- **Area**: Explorer
- **Why / hint**: logs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 701: Event topic index [Todo]

- **Area**: Explorer
- **Why / hint**: sql
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 702: Full-text search blocks [Todo]

- **Area**: Explorer
- **Why / hint**: sql
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 703: Address labels curated [Todo]

- **Area**: Explorer
- **Why / hint**: ux
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 704: ENS-like name service spike [Todo]

- **Area**: Research
- **Why / hint**: names
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 705: Token list integration [Todo]

- **Area**: Explorer
- **Why / hint**: tokens
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 706: Price oracle optional [Todo]

- **Area**: Explorer
- **Why / hint**: fiat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 707: Fiat display toggle [Todo]

- **Area**: Explorer
- **Why / hint**: ui
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 708: Dark/light explorer theme [Todo]

- **Area**: Explorer
- **Why / hint**: ui
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 709: Mobile-responsive explorer [Todo]

- **Area**: Explorer
- **Why / hint**: ui
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 710: A11y audit explorer [Todo]

- **Area**: Explorer
- **Why / hint**: ui
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 711: i18n explorer UI [Todo]

- **Area**: Explorer
- **Why / hint**: ui
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 712: PWA offline cache [Todo]

- **Area**: Explorer
- **Why / hint**: ui
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 713: SSR for SEO [Todo]

- **Area**: Explorer
- **Why / hint**: ui
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 714: Rate-limit explorer API [Todo]

- **Area**: Security
- **Why / hint**: api
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 715: API keys for explorer [Todo]

- **Area**: Security
- **Why / hint**: api
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 716: GraphQL depth limits [Todo]

- **Area**: Security
- **Why / hint**: graphql
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 717: Persisted GraphQL queries [Todo]

- **Area**: Perf
- **Why / hint**: graphql
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 718: DataLoader batching [Todo]

- **Area**: Perf
- **Why / hint**: graphql
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 719: CDN cache headers [Todo]

- **Area**: Perf
- **Why / hint**: http
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 720: ETag support [Todo]

- **Area**: Perf
- **Why / hint**: http
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 721: Compression middleware [Todo]

- **Area**: Perf
- **Why / hint**: http
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 722: HTTP/2 for RPC [Todo]

- **Area**: Perf
- **Why / hint**: server
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 723: HTTP/3 experiment [Todo]

- **Area**: Perf
- **Why / hint**: quic
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 724: Connection coalescing [Todo]

- **Area**: Perf
- **Why / hint**: clients
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 725: Client SDK TypeScript [Todo]

- **Area**: DX
- **Why / hint**: sdk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 726: Client SDK Python [Todo]

- **Area**: DX
- **Why / hint**: sdk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 727: Client SDK Go [Todo]

- **Area**: DX
- **Why / hint**: sdk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 728: Client SDK Rust [Todo]

- **Area**: DX
- **Why / hint**: sdk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 729: Generated OpenAPI clients [Todo]

- **Area**: DX
- **Why / hint**: sdk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 730: Postman collection [Todo]

- **Area**: DX
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 731: Insomnia collection [Todo]

- **Area**: DX
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 732: Load test k6 scripts [Todo]

- **Area**: Perf
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 733: Soak test 24h [Todo]

- **Area**: Perf
- **Why / hint**: stability
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 734: Chaos mesh experiments [Todo]

- **Area**: Testing
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 735: Fault injection P2P [Todo]

- **Area**: Testing
- **Why / hint**: net
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 736: Packet loss simulation [Todo]

- **Area**: Testing
- **Why / hint**: net
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 737: Latency injection [Todo]

- **Area**: Testing
- **Why / hint**: net
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 738: Byzantine peer simulator [Todo]

- **Area**: Testing
- **Why / hint**: consensus
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 739: Long-range attack notes [Todo]

- **Area**: Security
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 740: Nothing-at-stake notes [Todo]

- **Area**: Security
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 741: Grindability analysis [Todo]

- **Area**: Security
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 742: Selfish mining analysis [Todo]

- **Area**: Security
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 743: Fee sniping mitigation [Todo]

- **Area**: Mempool
- **Why / hint**: policy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 744: Blob fee market prep [Todo]

- **Area**: Execution
- **Why / hint**: 4844
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 745: PeerDAS research [Todo]

- **Area**: Research
- **Why / hint**: das
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 746: EOF validation [Todo]

- **Area**: Execution
- **Why / hint**: eof
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 747: Verkle proof verify [Todo]

- **Area**: Execution
- **Why / hint**: verkle
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 748: Stateless witness format [Todo]

- **Area**: Execution
- **Why / hint**: witness
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 749: Engine API version matrix [Todo]

- **Area**: Consensus
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 750: CL/EL handshake tests [Todo]

- **Area**: Testing
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 751: Interoperability with geth [Todo]

- **Area**: Testing
- **Why / hint**: devp2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 752: Interoperability with nethermind [Todo]

- **Area**: Testing
- **Why / hint**: devp2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 753: Interoperability with erigon [Todo]

- **Area**: Testing
- **Why / hint**: devp2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 754: Interoperability with reth [Todo]

- **Area**: Testing
- **Why / hint**: devp2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 755: Hive simulators subset [Todo]

- **Area**: Testing
- **Why / hint**: hive
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 756: Kurtosis localnet [Todo]

- **Area**: DX
- **Why / hint**: kurtosis
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 757: Assertoor test plans [Todo]

- **Area**: Testing
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 758: Shadow fork playbook [Todo]

- **Area**: Ops
- **Why / hint**: mainnet prep
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 759: Testnet faucet ops [Todo]

- **Area**: Ops
- **Why / hint**: devnet
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 760: Public testnet launch [Todo]

- **Area**: Ops
- **Why / hint**: community
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 761: Bug bounty launch [Todo]

- **Area**: Security
- **Why / hint**: community
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 762: Ambassador docs program [Todo]

- **Area**: Docs
- **Why / hint**: community
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 763: Hackathon starter kit [Todo]

- **Area**: DX
- **Why / hint**: community
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 764: Grant proposal template [Todo]

- **Area**: Process
- **Why / hint**: funding
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 765: Trademark / brand guide [Todo]

- **Area**: Legal
- **Why / hint**: brand
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 766: Code of conduct enforce [Todo]

- **Area**: Community
- **Why / hint**: moderation
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 767: Security working group [Todo]

- **Area**: Process
- **Why / hint**: governance
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 768: Protocol working group [Todo]

- **Area**: Process
- **Why / hint**: governance
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 769: Release captain rotation [Todo]

- **Area**: Process
- **Why / hint**: releases
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 770: LTS support policy [Todo]

- **Area**: Process
- **Why / hint**: versions
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 771: Deprecation policy [Todo]

- **Area**: Process
- **Why / hint**: apis
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 772: Semver for RPC methods [Todo]

- **Area**: Process
- **Why / hint**: compat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 773: Capability negotiation doc [Todo]

- **Area**: Protocol
- **Why / hint**: handshake
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 774: Wire protocol versioning [Todo]

- **Area**: Protocol
- **Why / hint**: M28 detail
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 775: Message ID registry [Todo]

- **Area**: Protocol
- **Why / hint**: codec
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 776: Snappy vs zstd choice ADR [Todo]

- **Area**: Protocol
- **Why / hint**: compression
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 777: Checksum framing [Todo]

- **Area**: Protocol
- **Why / hint**: integrity
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 778: Replay protection window [Todo]

- **Area**: Protocol
- **Why / hint**: messages
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 779: Peer score decay curves [Todo]

- **Area**: P2P
- **Why / hint**: tuning
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 780: Gossipsub evaluation [Todo]

- **Area**: P2P
- **Why / hint**: libp2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 781: Topic meshes for txs/blocks [Todo]

- **Area**: P2P
- **Why / hint**: gossip
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 782: IHAVE/IWANT control msgs [Todo]

- **Area**: P2P
- **Why / hint**: gossip
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 783: Opportunistic grafting [Todo]

- **Area**: P2P
- **Why / hint**: gossip
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 784: PX peer exchange limits [Todo]

- **Area**: P2P
- **Why / hint**: discovery
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 785: ENR support [Todo]

- **Area**: P2P
- **Why / hint**: discv5
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 786: discv5 implementation [Todo]

- **Area**: P2P
- **Why / hint**: discovery
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 787: discv4 legacy bridge [Todo]

- **Area**: P2P
- **Why / hint**: compat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 788: Node record signatures [Todo]

- **Area**: P2P
- **Why / hint**: ENR
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 789: Multiaddr support [Todo]

- **Area**: P2P
- **Why / hint**: libp2p
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 790: Circuit relay v2 eval [Todo]

- **Area**: P2P
- **Why / hint**: NAT
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 791: AutoNAT service [Todo]

- **Area**: P2P
- **Why / hint**: NAT
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 792: DCUtR hole punching [Todo]

- **Area**: P2P
- **Why / hint**: NAT
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 793: Bandwidth manager [Todo]

- **Area**: P2P
- **Why / hint**: quotas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 794: ConnMgr waterline [Todo]

- **Area**: P2P
- **Why / hint**: limits
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 795: Resource manager [Todo]

- **Area**: P2P
- **Why / hint**: DoS
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 796: Bloom filter for seen txs [Todo]

- **Area**: Mempool
- **Why / hint**: dedupe
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 797: Cuckoo filter for blocks [Todo]

- **Area**: Sync
- **Why / hint**: dedupe
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 798: LRU header cache [Todo]

- **Area**: Sync
- **Why / hint**: memory
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 799: Prefetch bodies [Todo]

- **Area**: Sync
- **Why / hint**: pipelining
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 800: Parallel state download [Todo]

- **Area**: Sync
- **Why / hint**: snap
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 801: Heal phase for snap sync [Todo]

- **Area**: Sync
- **Why / hint**: snap
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 802: Pivot block selection [Todo]

- **Area**: Sync
- **Why / hint**: fast
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 803: Ancient store freezer [Todo]

- **Area**: Storage
- **Why / hint**: geth-like
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 804: Freezer migration tool [Todo]

- **Area**: Tools
- **Why / hint**: storage
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 805: Era1 file format support [Todo]

- **Area**: Sync
- **Why / hint**: history
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 806: History expiry EIP [Todo]

- **Area**: Protocol
- **Why / hint**: history
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 807: Portal history network [Todo]

- **Area**: Research
- **Why / hint**: portal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 808: Portal state network [Todo]

- **Area**: Research
- **Why / hint**: portal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 809: Portal beacon network [Todo]

- **Area**: Research
- **Why / hint**: portal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 810: Light client sync committee [Todo]

- **Area**: Consensus
- **Why / hint**: altair
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 811: Light client optimistic updates [Todo]

- **Area**: Consensus
- **Why / hint**: capella
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 812: Light client finality updates [Todo]

- **Area**: Consensus
- **Why / hint**: finality
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 813: Checkpointz compatibility [Todo]

- **Area**: Ops
- **Why / hint**: light
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 814: Weak subjectivity checkpoints [Todo]

- **Area**: Security
- **Why / hint**: pos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 815: WS checkpoint distribution [Todo]

- **Area**: Ops
- **Why / hint**: cdn
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 816: Trusted setup ceremony notes [Todo]

- **Area**: Research
- **Why / hint**: zk
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 817: KZG verification [Todo]

- **Area**: Crypto
- **Why / hint**: 4844
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 818: Blob sidecar handling [Todo]

- **Area**: Protocol
- **Why / hint**: 4844
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 819: Blob mempool [Todo]

- **Area**: Mempool
- **Why / hint**: 4844
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 820: Blob gossip topic [Todo]

- **Area**: P2P
- **Why / hint**: 4844
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 821: Blob fee market metrics [Todo]

- **Area**: Metrics
- **Why / hint**: 4844
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 822: Peer scoring for blob useful [Todo]

- **Area**: P2P
- **Why / hint**: 4844
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 823: DAS sampling client [Todo]

- **Area**: Research
- **Why / hint**: peerdas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 824: Custody columns [Todo]

- **Area**: Research
- **Why / hint**: peerdas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 825: Reconstruction from columns [Todo]

- **Area**: Research
- **Why / hint**: peerdas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 826: Builder API auth [Todo]

- **Area**: Security
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 827: Relay circuit breakers [Todo]

- **Area**: Ops
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 828: Fallback to local block [Todo]

- **Area**: Consensus
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 829: MEV-Boost compatibility [Todo]

- **Area**: Consensus
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 830: Commit-boost evaluation [Todo]

- **Area**: Consensus
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 831: Inclusion lists research [Todo]

- **Area**: Research
- **Why / hint**: censorship
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 832: FOCIL research [Todo]

- **Area**: Research
- **Why / hint**: censorship
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 833: ePBS research [Todo]

- **Area**: Research
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 834: Execution tickets research [Todo]

- **Area**: Research
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 835: APS research [Todo]

- **Area**: Research
- **Why / hint**: mev
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 836: Based rollups notes [Todo]

- **Area**: Research
- **Why / hint**: l2
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 837: Native AA notes [Todo]

- **Area**: Research
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 838: EOF + AA interactions [Todo]

- **Area**: Research
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 839: RIP precompiles track [Todo]

- **Area**: Research
- **Why / hint**: l2
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 840: Precompile gas repricing [Todo]

- **Area**: Execution
- **Why / hint**: osaka
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 841: ModExp repricing [Todo]

- **Area**: Execution
- **Why / hint**: eip
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 842: Point evaluation precompile [Todo]

- **Area**: Execution
- **Why / hint**: kzg
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 843: BLS precompiles [Todo]

- **Area**: Execution
- **Why / hint**: eip-2537
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 844: secp256r1 precompile [Todo]

- **Area**: Execution
- **Why / hint**: rip7212
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 845: Verkle gas schedule [Todo]

- **Area**: Execution
- **Why / hint**: verkle
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 846: State conversion tool [Todo]

- **Area**: Tools
- **Why / hint**: verkle
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 847: Overlay tree during transition [Todo]

- **Area**: Execution
- **Why / hint**: verkle
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 848: Witness gas costs [Todo]

- **Area**: Execution
- **Why / hint**: stateless
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 849: Block-level access lists [Todo]

- **Area**: Execution
- **Why / hint**: bal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 850: Parallel tx execution [Todo]

- **Area**: Perf
- **Why / hint**: execution
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 851: Speculative execution [Todo]

- **Area**: Perf
- **Why / hint**: execution
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 852: State prefetch threads [Todo]

- **Area**: Perf
- **Why / hint**: execution
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 853: Lock-free account cache [Todo]

- **Area**: Perf
- **Why / hint**: state
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 854: Shard state by address [Todo]

- **Area**: Perf
- **Why / hint**: state
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 855: IO uring storage reads [Todo]

- **Area**: Perf
- **Why / hint**: linux
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 856: Memory-mapped frozen files [Todo]

- **Area**: Perf
- **Why / hint**: freezer
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 857: Huge pages optional [Todo]

- **Area**: Perf
- **Why / hint**: os
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 858: NUMA-aware mining [Todo]

- **Area**: Perf
- **Why / hint**: pow
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 859: CPU affinity controls [Todo]

- **Area**: Ops
- **Why / hint**: tuning
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 860: cgroup awareness [Todo]

- **Area**: Ops
- **Why / hint**: k8s
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 861: Graceful degradation under load [Todo]

- **Area**: Ops
- **Why / hint**: backpressure
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 862: Load-shed RPC methods [Todo]

- **Area**: RPC
- **Why / hint**: priority
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 863: Fair queuing RPC [Todo]

- **Area**: RPC
- **Why / hint**: qos
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 864: Per-IP token buckets [Todo]

- **Area**: Security
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 865: Per-method cost weights [Todo]

- **Area**: Security
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 866: Adaptive rate limits [Todo]

- **Area**: Security
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 867: Banlist share across nodes [Todo]

- **Area**: Ops
- **Why / hint**: redis
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 868: Global mempool of network [Todo]

- **Area**: Research
- **Why / hint**: shared
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 869: Private tx relay [Todo]

- **Area**: Research
- **Why / hint**: privacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 870: Encrypted mempool research [Todo]

- **Area**: Research
- **Why / hint**: privacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 871: Threshold decryption research [Todo]

- **Area**: Research
- **Why / hint**: privacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 872: UserOp mempool [Todo]

- **Area**: Mempool
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 873: Bundler compatibility [Todo]

- **Area**: Execution
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 874: Paymaster simulation [Todo]

- **Area**: Execution
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 875: EntryPoint tracking [Todo]

- **Area**: Explorer
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 876: Session keys [Todo]

- **Area**: Wallet
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 877: Passkey / WebAuthn [Todo]

- **Area**: Wallet
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 878: Social login research [Todo]

- **Area**: Wallet
- **Why / hint**: ux
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 879: Account recovery flows [Todo]

- **Area**: Wallet
- **Why / hint**: ux
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 880: Transaction simulation UX [Todo]

- **Area**: Wallet
- **Why / hint**: eth_call
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 881: Gas sponsorship UX [Todo]

- **Area**: Wallet
- **Why / hint**: paymaster
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 882: Batch user ops UI [Todo]

- **Area**: Wallet
- **Why / hint**: aa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 883: Hardware policy engine [Todo]

- **Area**: Wallet
- **Why / hint**: rules
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 884: Allowlist dapps [Todo]

- **Area**: Wallet
- **Why / hint**: security
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 885: Phishing URL blocklist [Todo]

- **Area**: Wallet
- **Why / hint**: security
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 886: Clear signing standards [Todo]

- **Area**: Wallet
- **Why / hint**: hw
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 887: EIP-712 typed data [Todo]

- **Area**: Wallet
- **Why / hint**: signing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 888: EIP-4361 SIWE [Todo]

- **Area**: Wallet
- **Why / hint**: auth
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 889: WalletConnect v2 bridge [Todo]

- **Area**: Wallet
- **Why / hint**: wc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 890: Deep link mobile wallet [Todo]

- **Area**: Wallet
- **Why / hint**: mobile
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 891: Push notification infra [Todo]

- **Area**: Ops
- **Why / hint**: mobile
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 892: Mobile light client [Todo]

- **Area**: Client
- **Why / hint**: mobile
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 893: Browser extension spike [Todo]

- **Area**: Client
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 894: WASM node experiment [Todo]

- **Area**: Research
- **Why / hint**: wasm
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 895: WASI storage bindings [Todo]

- **Area**: Research
- **Why / hint**: wasm
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 896: Embedded ARM build [Todo]

- **Area**: Build
- **Why / hint**: iot
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 897: Raspberry Pi image [Todo]

- **Area**: Deploy
- **Why / hint**: edge
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 898: Cross-compile CI [Todo]

- **Area**: CI
- **Why / hint**: arm64
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 899: Android NDK experiment [Todo]

- **Area**: Research
- **Why / hint**: mobile
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 900: iOS constraints notes [Todo]

- **Area**: Research
- **Why / hint**: mobile
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 901: Game engine SDK notes [Todo]

- **Area**: DX
- **Why / hint**: unity/unreal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 902: Unity RPC plugin [Todo]

- **Area**: DX
- **Why / hint**: gaming
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 903: Unreal RPC plugin [Todo]

- **Area**: DX
- **Why / hint**: gaming
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 904: Godot addon [Todo]

- **Area**: DX
- **Why / hint**: gaming
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 905: Education sandbox chain [Todo]

- **Area**: Docs
- **Why / hint**: learn
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 906: Interactive tutorials [Todo]

- **Area**: Docs
- **Why / hint**: learn
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 907: Workshop slides [Todo]

- **Area**: Docs
- **Why / hint**: train
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 908: Certification quiz [Todo]

- **Area**: Docs
- **Why / hint**: train
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 909: University course pack [Todo]

- **Area**: Docs
- **Why / hint**: edu
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 910: Capture-the-flag challenges [Todo]

- **Area**: Security
- **Why / hint**: edu
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 911: Bug garden intentional vulns [Todo]

- **Area**: Security
- **Why / hint**: edu
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 912: Static analysis custom checks [Todo]

- **Area**: CI
- **Why / hint**: clang-tidy modules
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 913: Semgrep ruleset [Todo]

- **Area**: CI
- **Why / hint**: sast
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 914: CodeQL queries [Todo]

- **Area**: CI
- **Why / hint**: sast
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 915: Coverity optional [Todo]

- **Area**: CI
- **Why / hint**: sast
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 916: Infer optional [Todo]

- **Area**: CI
- **Why / hint**: sast
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 917: Include-what-you-use CI [Todo]

- **Area**: CI
- **Why / hint**: hygiene
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 918: Cyclomatic complexity gate [Todo]

- **Area**: CI
- **Why / hint**: quality
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 919: Duplication detection [Todo]

- **Area**: CI
- **Why / hint**: jscpd
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 920: Comment coverage optional [Todo]

- **Area**: CI
- **Why / hint**: docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 921: Doxygen publish pages [Todo]

- **Area**: Docs
- **Why / hint**: api
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 922: Godbolt links for hot paths [Todo]

- **Area**: Docs
- **Why / hint**: perf
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 923: Flamegraph CI artifact [Todo]

- **Area**: CI
- **Why / hint**: perf
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 924: Perf record scripts [Todo]

- **Area**: Perf
- **Why / hint**: linux
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 925: VTune notes [Todo]

- **Area**: Perf
- **Why / hint**: intel
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 926: Heaptrack notes [Todo]

- **Area**: Perf
- **Why / hint**: memory
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 927: Massif notes [Todo]

- **Area**: Perf
- **Why / hint**: memory
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 928: Cachegrind notes [Todo]

- **Area**: Perf
- **Why / hint**: cpu
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 929: Branch mispredict hunt [Todo]

- **Area**: Perf
- **Why / hint**: crypto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 930: SIMD keccak experiment [Todo]

- **Area**: Perf
- **Why / hint**: keccak
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 931: AES-NI unrelated audit [Todo]

- **Area**: Security
- **Why / hint**: ensure unused
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 932: Constant-time verify path [Todo]

- **Area**: Crypto
- **Why / hint**: ecdsa
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 933: Blinding for ECDSA [Todo]

- **Area**: Crypto
- **Why / hint**: sidechannel
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 934: RNG health tests [Todo]

- **Area**: Crypto
- **Why / hint**: random
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 935: Entropy starvation handling [Todo]

- **Area**: Crypto
- **Why / hint**: random
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 936: Fork safety for RNG [Todo]

- **Area**: Crypto
- **Why / hint**: pid
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 937: VM clone RNG reseed [Todo]

- **Area**: Crypto
- **Why / hint**: cloud
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 938: Container entropy notes [Todo]

- **Area**: Docs
- **Why / hint**: random
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 939: FIPS mode evaluation [Todo]

- **Area**: Compliance
- **Why / hint**: fips
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 940: Common Criteria notes [Todo]

- **Area**: Compliance
- **Why / hint**: cc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 941: SOC2 evidence pack [Todo]

- **Area**: Compliance
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 942: ISO27001 mapping [Todo]

- **Area**: Compliance
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 943: GDPR data map for explorer [Todo]

- **Area**: Compliance
- **Why / hint**: privacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 944: Right-to-erasure limits [Todo]

- **Area**: Compliance
- **Why / hint**: chain immutability
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 945: Data retention for logs [Todo]

- **Area**: Compliance
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 946: PII classification [Todo]

- **Area**: Compliance
- **Why / hint**: explorer
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 947: Cookie policy if UI [Todo]

- **Area**: Legal
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 948: Terms for public RPC [Todo]

- **Area**: Legal
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 949: Acceptable use policy [Todo]

- **Area**: Legal
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 950: DMCA process for UI [Todo]

- **Area**: Legal
- **Why / hint**: content
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 951: Export control notes [Todo]

- **Area**: Legal
- **Why / hint**: crypto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 952: Sanction screening optional [Todo]

- **Area**: Compliance
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 953: Travel rule research [Todo]

- **Area**: Compliance
- **Why / hint**: cex
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 954: OFAC list handling [Todo]

- **Area**: Compliance
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 955: Geo-fencing optional [Todo]

- **Area**: Compliance
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 956: KYC gateway pattern [Todo]

- **Area**: Compliance
- **Why / hint**: apps
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 957: Regulated RPC product mode [Todo]

- **Area**: Product
- **Why / hint**: enterprise
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 958: SLA templates [Todo]

- **Area**: Product
- **Why / hint**: enterprise
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 959: Support tier docs [Todo]

- **Area**: Product
- **Why / hint**: enterprise
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 960: Enterprise SSO for explorer [Todo]

- **Area**: Product
- **Why / hint**: saml
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 961: Audit log export SIEM [Todo]

- **Area**: Product
- **Why / hint**: enterprise
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 962: Multi-tenant explorer [Todo]

- **Area**: Product
- **Why / hint**: saas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 963: Per-tenant rate limits [Todo]

- **Area**: Product
- **Why / hint**: saas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 964: Billing meters [Todo]

- **Area**: Product
- **Why / hint**: saas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 965: Quota enforcement [Todo]

- **Area**: Product
- **Why / hint**: saas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 966: Customer success runbooks [Todo]

- **Area**: Product
- **Why / hint**: saas
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 967: Status page integration [Todo]

- **Area**: Ops
- **Why / hint**: statuspage
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 968: Public incident process [Todo]

- **Area**: Ops
- **Why / hint**: comms
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 969: Postmortem template [Todo]

- **Area**: Ops
- **Why / hint**: learning
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 970: Blameless culture notes [Todo]

- **Area**: Process
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 971: Game day calendar [Todo]

- **Area**: Ops
- **Why / hint**: dr
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 972: Tabletop exercises [Todo]

- **Area**: Ops
- **Why / hint**: security
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 973: Red team engagement [Todo]

- **Area**: Security
- **Why / hint**: external
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 974: Purple team crypto review [Todo]

- **Area**: Security
- **Why / hint**: crypto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 975: Bugcrowd/HackerOne setup [Todo]

- **Area**: Security
- **Why / hint**: bounty
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 976: Safe harbor policy [Todo]

- **Area**: Legal
- **Why / hint**: research
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 977: Vulnerability embargo process [Todo]

- **Area**: Security
- **Why / hint**: process
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 978: CVE assignment process [Todo]

- **Area**: Security
- **Why / hint**: process
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 979: Patch release SLA [Todo]

- **Area**: Process
- **Why / hint**: security
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 980: Hotfix pipeline [Todo]

- **Area**: CI
- **Why / hint**: security
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 981: Canary CVE response [Todo]

- **Area**: Ops
- **Why / hint**: security
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 982: Dependency emergency bump [Todo]

- **Area**: CI
- **Why / hint**: deps
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 983: OpenSSL incident playbook [Todo]

- **Area**: Ops
- **Why / hint**: crypto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 984: secp256k1 incident playbook [Todo]

- **Area**: Ops
- **Why / hint**: crypto
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 985: Compromise rotation playbook [Todo]

- **Area**: Ops
- **Why / hint**: keys
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 986: Key ceremony for genesis [Todo]

- **Area**: Security
- **Why / hint**: launch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 987: Genesis multisig [Todo]

- **Area**: Security
- **Why / hint**: launch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 988: Time-locked admin keys [Todo]

- **Area**: Security
- **Why / hint**: governance
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 989: On-chain pause switch [Todo]

- **Area**: Security
- **Why / hint**: governance
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 990: Upgrade proxy pattern eval [Todo]

- **Area**: Execution
- **Why / hint**: upgrades
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 991: Immutable bytecode policy [Todo]

- **Area**: Security
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 992: Compiler version pin policy [Todo]

- **Area**: Security
- **Why / hint**: solidity
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 993: Bytecode verify CI [Todo]

- **Area**: CI
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 994: Slither in CI [Todo]

- **Area**: CI
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 995: Mythril optional [Todo]

- **Area**: CI
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 996: Foundry fuzz in CI [Todo]

- **Area**: CI
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 997: Invariant tests [Todo]

- **Area**: Testing
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 998: Differential vs solc [Todo]

- **Area**: Testing
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 999: Gas snapshot CI [Todo]

- **Area**: CI
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1000: Storage layout checks [Todo]

- **Area**: CI
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1001: Upgrade safety checks [Todo]

- **Area**: CI
- **Why / hint**: contracts
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1002: Formal specs in Lean/Coq [Todo]

- **Area**: Research
- **Why / hint**: formal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1003: TLA+ for consensus [Todo]

- **Area**: Research
- **Why / hint**: formal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1004: Alloy models for sync [Todo]

- **Area**: Research
- **Why / hint**: formal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1005: Executable semantics [Todo]

- **Area**: Research
- **Why / hint**: formal
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1006: Conformance vs yellow paper [Todo]

- **Area**: Testing
- **Why / hint**: spec
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1007: EIP tracking board [Todo]

- **Area**: Process
- **Why / hint**: standards
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1008: Standards liaison [Todo]

- **Area**: Process
- **Why / hint**: ethereum magicians
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1009: Interop calls attendance [Todo]

- **Area**: Process
- **Why / hint**: community
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1010: Devconnect presence [Todo]

- **Area**: Community
- **Why / hint**: events
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1011: Regional meetup kits [Todo]

- **Area**: Community
- **Why / hint**: events
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1012: Ambassador swag guidelines [Todo]

- **Area**: Community
- **Why / hint**: brand
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1013: Translation contributors [Todo]

- **Area**: Community
- **Why / hint**: i18n
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1014: Video walkthrough series [Todo]

- **Area**: Docs
- **Why / hint**: media
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1015: Podcast FAQ [Todo]

- **Area**: Docs
- **Why / hint**: media
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1016: Architecture livestream [Todo]

- **Area**: Docs
- **Why / hint**: media
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1017: Office hours cadence [Todo]

- **Area**: Community
- **Why / hint**: support
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1018: Discord/Telegram moderation [Todo]

- **Area**: Community
- **Why / hint**: chat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1019: Forum category taxonomy [Todo]

- **Area**: Community
- **Why / hint**: discourse
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1020: StackOverflow tag [Todo]

- **Area**: Community
- **Why / hint**: support
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1021: Good first issues pack [Todo]

- **Area**: DX
- **Why / hint**: onboarding
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1022: Mentorship program [Todo]

- **Area**: Community
- **Why / hint**: contrib
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1023: Internship projects list [Todo]

- **Area**: Community
- **Why / hint**: edu
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1024: Research fellowship track [Todo]

- **Area**: Research
- **Why / hint**: grants
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1025: Academic citation style [Todo]

- **Area**: Docs
- **Why / hint**: papers
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1026: Benchmark paper draft [Todo]

- **Area**: Research
- **Why / hint**: perf
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1027: Security whitepaper [Todo]

- **Area**: Security
- **Why / hint**: docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1028: Yellow-paper-like spec [Todo]

- **Area**: Docs
- **Why / hint**: protocol
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1029: Protocol specification v0.1 [Todo]

- **Area**: Docs
- **Why / hint**: normative
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1030: Test vectors package publish [Todo]

- **Area**: Testing
- **Why / hint**: npm/crate
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1031: Reference vectors license [Todo]

- **Area**: Legal
- **Why / hint**: CC0
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1032: Interop fest planning [Todo]

- **Area**: Community
- **Why / hint**: events
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1033: Mainnet launch checklist [Todo]

- **Area**: Ops
- **Why / hint**: M59 detail
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1034: Genesis freeze ceremony [Todo]

- **Area**: Ops
- **Why / hint**: launch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1035: Public RPC endpoints launch [Todo]

- **Area**: Ops
- **Why / hint**: launch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1036: Block explorer launch [Todo]

- **Area**: Ops
- **Why / hint**: launch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1037: Bridge partners checklist [Todo]

- **Area**: Ops
- **Why / hint**: launch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1038: Oracle partners checklist [Todo]

- **Area**: Ops
- **Why / hint**: launch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1039: Wallet listing checklist [Todo]

- **Area**: Ops
- **Why / hint**: launch
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1040: Exchange listing tech pack [Todo]

- **Area**: Ops
- **Why / hint**: listing
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1041: Market maker readiness [Todo]

- **Area**: Ops
- **Why / hint**: liquidity
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1042: Foundation legal entity [Todo]

- **Area**: Legal
- **Why / hint**: governance
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1043: Tokenomics if any [Todo]

- **Area**: Product
- **Why / hint**: optional
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1044: No-token pure L1 option [Todo]

- **Area**: Product
- **Why / hint**: positioning
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1045: Public goods funding [Todo]

- **Area**: Process
- **Why / hint**: retro
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1046: Quadratic funding experiment [Todo]

- **Area**: Community
- **Why / hint**: funding
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1047: RetroPGF application [Todo]

- **Area**: Community
- **Why / hint**: funding
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1048: Open source license audit [Todo]

- **Area**: Legal
- **Why / hint**: SPDX
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1049: Contributor license clarity [Todo]

- **Area**: Legal
- **Why / hint**: DCO
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1050: Third-party notice file [Todo]

- **Area**: Legal
- **Why / hint**: NOTICE
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1051: Patent non-assert eval [Todo]

- **Area**: Legal
- **Why / hint**: patents
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1052: Trademark registration [Todo]

- **Area**: Legal
- **Why / hint**: brand
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1053: Domain / DNSSEC [Todo]

- **Area**: Ops
- **Why / hint**: infra
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1054: Email security DMARC [Todo]

- **Area**: Ops
- **Why / hint**: infra
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1055: Status subdomain [Todo]

- **Area**: Ops
- **Why / hint**: status
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1056: Docs subdomain [Todo]

- **Area**: Docs
- **Why / hint**: hosting
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1057: Specs subdomain [Todo]

- **Area**: Docs
- **Why / hint**: hosting
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1058: CDN for docs [Todo]

- **Area**: Docs
- **Why / hint**: hosting
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1059: Search for docs [Todo]

- **Area**: Docs
- **Why / hint**: algolia
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1060: Versioned docs switcher [Todo]

- **Area**: Docs
- **Why / hint**: versions
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1061: Archived docs policy [Todo]

- **Area**: Docs
- **Why / hint**: versions
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1062: Link checker CI [Todo]

- **Area**: CI
- **Why / hint**: docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1063: Spellcheck CI [Todo]

- **Area**: CI
- **Why / hint**: docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1064: Markdown lint CI [Todo]

- **Area**: CI
- **Why / hint**: docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1065: Mermaid diagram lint [Todo]

- **Area**: CI
- **Why / hint**: docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1066: Screenshot refresh bot [Todo]

- **Area**: CI
- **Why / hint**: docs
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1067: API docs from comments [Todo]

- **Area**: Docs
- **Why / hint**: doxygen
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1068: Changelog automation [Todo]

- **Area**: DX
- **Why / hint**: git-cliff
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1069: Release notes template [Todo]

- **Area**: DX
- **Why / hint**: releases
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1070: Upgrade guide generator [Todo]

- **Area**: Docs
- **Why / hint**: migrations
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1071: Breaking change announcer [Todo]

- **Area**: Process
- **Why / hint**: comms
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1072: Deprecation warnings in RPC [Todo]

- **Area**: RPC
- **Why / hint**: compat
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1073: Client version telemetry opt-in [Todo]

- **Area**: Metrics
- **Why / hint**: privacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1074: Anonymous usage metrics design [Todo]

- **Area**: Metrics
- **Why / hint**: privacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1075: Do-not-track for explorer [Todo]

- **Area**: Privacy
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1076: Local-only analytics [Todo]

- **Area**: Privacy
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1077: Zero third-party trackers [Todo]

- **Area**: Privacy
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1078: Privacy policy page [Todo]

- **Area**: Legal
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1079: Cookie-less explorer [Todo]

- **Area**: Privacy
- **Why / hint**: web
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1080: Tor-friendly RPC mode [Todo]

- **Area**: Privacy
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1081: No-logs RPC mode [Todo]

- **Area**: Privacy
- **Why / hint**: rpc
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1082: Proof of non-logging [Todo]

- **Area**: Privacy
- **Why / hint**: ops
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1083: Transparent stats only [Todo]

- **Area**: Privacy
- **Why / hint**: metrics
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1084: Differential privacy research [Todo]

- **Area**: Research
- **Why / hint**: metrics
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1085: MPC for private analytics [Todo]

- **Area**: Research
- **Why / hint**: privacy
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

### Milestone 1086: End of backlog seed set [Todo]

- **Area**: Meta
- **Why / hint**: generator marker
- **Acceptance**: Measurable improvement landed with tests or docs as appropriate

## Counts

- Seeded concrete milestones: 1026 (IDs 61..1086)
- Expanded operational backlog through milestone **560**
- Total milestones in this file: **500** (plus original 1–60 in ROADMAP)
- Grand total tracked IDs: **560**
