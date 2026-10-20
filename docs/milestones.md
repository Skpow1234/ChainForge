# Milestones

High-level plan and **honest status vs the codebase**. Full specs for 1–60: [ROADMAP.md](./ROADMAP.md). Extended backlog (500+): [milestones-backlog.md](./milestones-backlog.md). Template: [MILESTONE_TEMPLATE.md](./MILESTONE_TEMPLATE.md).

## Status legend

| Mark | Meaning |
|------|---------|
| Done | Meets acceptance well enough to build on |
| Partial | Scaffolding or incomplete; stubs, mocks, or unfinished wiring |
| Todo | Not started in a meaningful way |

Default build now enables: `crypto`, `core`, `serialization`, `p2p`, `logging`, `metrics`, `storage`, `consensus`, `mempool`, `rpc`, `node`. Still missing: `execution`, `explorer`.

## Recent progress

| Item | Status |
|------|--------|
| Real Keccak-f[1600] / Keccak-256 (Eth padding) | Done |
| Core hashes wired through crypto | Done |
| secp256k1 curve ops via libsecp256k1 | Done |
| Address keccak derivation + EIP-55 `to_hex` | Done |
| Block/tx keccak hashing; merkle/header checks | Done |
| PoW mining uses `Block::calculate_hash`; cheats removed | Done |
| `ChainState` + RPC wired to storage/mempool (not mock maps) | Partial |
| `chainforge-node --mine-once` genesis+mine+RPC | Partial |
| Extended backlog milestones 61–1086 | Done (tracking) |

## Next up

| Priority | Milestone | Why |
|----------|-----------|-----|
| 1 | Real RocksDB backend (4) | Still memory for `"rocksdb"` |
| 2 | RLP encode/decode for txs (2/8) | `eth_sendRawTransaction` is still a hex seed path |
| 3 | ECDSA tx signature verify (3) | Signature field size-check only |
| 4 | Peer management → message protocol (18–19) | Phase 2 |
| 5 | Health + YAML config in node (9/12) | Ops readiness |

## Phase 1 — Foundation (1–15)

| ID | Title | Status | Notes |
|----|--------|--------|-------|
| 1 | Project setup & build system | Done | CMake, Conan, CI |
| 2 | Core domain models | Partial | Hashes/addresses improved; RLP still open |
| 3 | Cryptographic foundation | Partial | Keccak + secp256k1 curves; BLS/recovery open |
| 4 | Database abstraction layer | Partial | Enabled; RocksDB still memory stub |
| 5 | Basic block structure | Partial | Keccak header hash + merkle check; RLP later |
| 6 | Transaction pool (mempool) | Partial | Enabled; used by ChainState |
| 7 | Simple Proof of Work | Partial | Mine/validate via real block hash; E2E in node |
| 8 | Basic RPC server | Partial | ChainState-backed; raw tx not full RLP |
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
