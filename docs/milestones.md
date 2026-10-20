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
| RLP codec + tx hash/sign/from_rlp | Done |
| ECDSA recover (secp256k1 recovery module) | Partial |
| File-backed KV storage (`file`/`rocksdb` alias) | Partial |
| PeerManager scaffolding (M18) | Partial |
| Node YAML config subset + GET `/health` `/ready` | Done |
| Extended backlog milestones 61–1086 | Done (tracking) |

## Next up

| Priority | Milestone | Why |
|----------|-----------|-----|
| 1 | Real RocksDB (not file-KV stand-in) (4) | `rocksdb` backend uses file-backed KV today |
| 2 | ChainState reload from durable DB | Indexes are memory-only across restarts |
| 3 | Message protocol + sync (19–21) | PeerManager exists; no wire codec yet |
| 4 | ECDSA sign path as compact r\|\|s (not DER) | Sign still OpenSSL DER-shaped |
| 5 | Full YAML library / schema validation (9) | Best-effort line parser only |

## Phase 1 — Foundation (1–15)

| ID | Title | Status | Notes |
|----|--------|--------|-------|
| 1 | Project setup & build system | Done | CMake, Conan, CI |
| 2 | Core domain models | Partial | Hashes/addresses improved; RLP still open |
| 3 | Cryptographic foundation | Partial | Keccak + secp256k1 curves; BLS/recovery open |
| 4 | Database abstraction layer | Partial | File-KV durable; real RocksDB still open |
| 5 | Basic block structure | Partial | Keccak header hash + merkle check |
| 6 | Transaction pool (mempool) | Partial | Enabled; used by ChainState |
| 7 | Simple Proof of Work | Partial | Mine/validate via real block hash; E2E in node |
| 8 | Basic RPC server | Partial | ChainState + RLP raw tx path |
| 9 | Configuration management | Partial | Node loads YAML subset |
| 10 | Logging system | Partial | Enabled in build |
| 11 | Metrics collection | Partial | Enabled; HTTP exposer still stubby |
| 12 | Health check system | Partial | GET `/health` `/ready` on RPC port |
| 13 | Basic error handling | Partial | |
| 14 | Serialization framework | Partial | |
| 15 | Basic testing framework | Partial | Many suites still disabled |

## Phase 2 — Networking (16–30)

| ID | Title | Status |
|----|--------|--------|
| 16 | Network transport layer | Done |
| 17 | Peer discovery | Done |
| 18 | Peer management | Partial | `PeerManager` scaffolding |
| 19–30 | Message protocol through load balancing | Todo | |

## Phases 3–4 (31–60)

All **Todo** aside from deploy/docs scaffolding. See [ROADMAP.md](./ROADMAP.md).

## Extended backlog

See [milestones-backlog.md](./milestones-backlog.md) for **1000+** concrete follow-ups (IDs 61+) from code review: crypto hardening, RPC methods, P2P protocol, explorer, execution, CI/deploy, compliance, research tracks, etc.
