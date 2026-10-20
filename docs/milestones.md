# Milestones

High-level plan and **honest status vs the codebase**. Full specs for 1–60: [ROADMAP.md](./ROADMAP.md). Extended backlog (500+): [milestones-backlog.md](./milestones-backlog.md). Template: [MILESTONE_TEMPLATE.md](./MILESTONE_TEMPLATE.md).

## Status legend

| Mark | Meaning |
|------|---------|
| Done | Meets acceptance well enough to build on |
| Partial | Scaffolding or incomplete; stubs, mocks, or unfinished wiring |
| Todo | Not started in a meaningful way |

Default build now enables: `crypto`, `core`, `serialization`, `p2p`, `logging`, `metrics`, `storage`, `consensus`, `mempool`, `rpc`, `node`. Still missing: `execution`, `explorer`.

## Recent progress (this pass)

| Item | Status |
|------|--------|
| Real Keccak-f[1600] / Keccak-256 (Eth padding) | Done |
| Core hashes wired through crypto (SHA/Keccak/RIPEMD/Merkle) | Done |
| secp256k1 curve ops via libsecp256k1 (XOR stubs removed) | Done |
| Ed25519 base/scalar mult via libsodium; BLS explicitly unsupported | Partial |
| PoW demo mining cheats removed | Done |
| Modules enabled in CMake + `chainforge-node` binary | Partial (RPC still mock-backed) |
| Docker node image builds `chainforge-node` | Done |
| Extended backlog milestones 61–1086 | Done (tracking) |

## Next up

| Priority | Milestone | Why |
|----------|-----------|-----|
| 1 | Block/tx hashing + address derivation (2/5) | Still TODOs in `block.cpp` / `transaction.cpp` / `address.cpp` |
| 2 | Real RocksDB + storage tests (4) | Still in-memory for `"rocksdb"` |
| 3 | PoW validation + mine E2E (7) | `validate_proof_of_work` still weak |
| 4 | Replace mock RPC with storage/mempool (8) | Backend DI missing |
| 5 | Peer management → message protocol (18–19) | Natural Phase 2 follow-on |
| 6 | Health endpoints + YAML config in node (9/12) | Ops readiness |

## Phase 1 — Foundation (1–15)

| ID | Title | Status | Notes |
|----|--------|--------|-------|
| 1 | Project setup & build system | Done | CMake, Conan, CI |
| 2 | Core domain models | Partial | Hashes wired; block/tx/address still incomplete |
| 3 | Cryptographic foundation | Partial | Keccak + secp256k1 curves improved; BLS/recovery open |
| 4 | Database abstraction layer | Partial | Enabled; RocksDB still memory stub |
| 5 | Basic block structure | Partial | Structure yes; header hashing incomplete |
| 6 | Transaction pool (mempool) | Partial | Enabled in build |
| 7 | Simple Proof of Work | Partial | Enabled; cheats removed; validation incomplete |
| 8 | Basic RPC server | Partial | Enabled; still mock state |
| 9 | Configuration management | Partial | YAML exists; node does not load it yet |
| 10 | Logging system | Partial | Enabled in build |
| 11 | Metrics collection | Partial | Enabled; HTTP exposer still stubby |
| 12 | Health check system | Todo | |
| 13 | Basic error handling | Partial | |
| 14 | Serialization framework | Partial | |
| 15 | Basic testing framework | Partial | Many suites still disabled |

## Phase 2 — Networking (16–30)

| ID | Title | Status |
|----|--------|--------|
| 16 | Network transport layer | Done |
| 17 | Peer discovery | Done |
| 18–30 | Peer management through load balancing | Todo |

## Phases 3–4 (31–60)

All **Todo** aside from deploy/docs scaffolding. See [ROADMAP.md](./ROADMAP.md).

## Extended backlog

See [milestones-backlog.md](./milestones-backlog.md) for **1000+** concrete follow-ups (IDs 61+) from code review: crypto hardening, RPC methods, P2P protocol, explorer, execution, CI/deploy, compliance, research tracks, etc.
