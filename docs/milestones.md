# Milestones

High-level plan and **honest status vs the codebase**. Full specs (problem, solution, acceptance, tests) live in [ROADMAP.md](./ROADMAP.md). Use [MILESTONE_TEMPLATE.md](./MILESTONE_TEMPLATE.md) when adding work.

## Status legend

| Mark | Meaning |
|------|---------|
| Done | Meets acceptance well enough to build on |
| Partial | Scaffolding or incomplete; stubs, mocks, or not in default CMake build |
| Todo | Not started in a meaningful way |

Default build today: `core`, `crypto`, `serialization`, `p2p` only. No `modules/node`, `explorer`, or `execution` packages yet.

## Where we are

Roughly **mid Phase 1** (foundation incomplete) plus **early Phase 2** (transport + discovery). Roadmap checkmarks previously overstated completion; they are aligned with this file.

## Next up (recommended order)

| Priority | Milestone | Why |
|----------|-----------|-----|
| 1 | **3** Cryptographic foundation | Finish real SHA/Keccak/secp256k1; remove XOR/curve stubs |
| 2 | **2 / 5** Core models + blocks | Wire hashing through crypto; real block/tx hash & linking |
| 3 | **4** Storage | Enable in CMake; working KV (in-memory OK first, then RocksDB) |
| 4 | **7** Simple PoW | Enable consensus; mine/validate against real hashes |
| 5 | **6** Mempool | Enable; validate & queue txs for block building |
| 6 | **8** RPC | Enable; replace mocks with storage-backed reads where possible |
| 7 | **Node binary** (prep **59**) | Thin `modules/node` composing the above for Compose/Helm |
| 8 | **18 → 19** Peer management → message protocol | Natural Phase 2 follow-on after discovery |

Then: sync/propagation (**20–21**), harden **9–12** / **15** as the node becomes runnable.

## Phase 1 — Foundation (1–15)

| ID | Title | Status | Notes |
|----|--------|--------|-------|
| 1 | Project setup & build system | Done | CMake, Conan, CI |
| 2 | Core domain models | Partial | Types exist; hash/address/tx TODOs |
| 3 | Cryptographic foundation | Partial | In build; Keccak/curve stubs remain |
| 4 | Database abstraction layer | Partial | Source present; **not in default build** |
| 5 | Basic block structure | Partial | Structure yes; header hashing incomplete |
| 6 | Transaction pool (mempool) | Partial | Source only; disabled in CMake |
| 7 | Simple Proof of Work | Partial | Source only; disabled |
| 8 | Basic RPC server | Partial | Mock RPC; disabled |
| 9 | Configuration management | Partial | `config/config.yaml` exists; C++ load/compose incomplete |
| 10 | Logging system | Partial | Module present; not in default build |
| 11 | Metrics collection | Partial | Stub metrics server; not in default build |
| 12 | Health check system | Todo | No real health/readiness surface |
| 13 | Basic error handling | Partial | `std::expected` in core; not system-wide |
| 14 | Serialization framework | Partial | Protobuf in build; validators incomplete |
| 15 | Basic testing framework | Partial | GTest tree exists; many targets off / `BUILD_TESTING` default OFF |

## Phase 2 — Networking (16–30)

| ID | Title | Status | Notes |
|----|--------|--------|-------|
| 16 | Network transport layer | Done | TCP/UDP in default build |
| 17 | Peer discovery | Done | In default build |
| 18 | Peer management | Todo | **Next after foundation vertical slice** |
| 19 | Message protocol | Todo | |
| 20 | Block synchronization | Todo | |
| 21 | Transaction propagation | Todo | |
| 22 | Network security | Todo | |
| 23 | NAT traversal | Todo | |
| 24 | Peer scoring system | Todo | |
| 25 | Network monitoring | Todo | |
| 26 | Connection pooling | Todo | |
| 27 | Message queuing | Todo | |
| 28 | Protocol versioning | Todo | |
| 29 | Network topology | Todo | |
| 30 | P2P load balancing | Todo | |

## Phase 3 — Consensus & execution (31–45)

All **Todo**. Requires storage, PoW/mempool, and a node process first. Includes advanced consensus, state, contracts, gas, execution, finality, validators, light client, snapshots, pruning.

## Phase 4 — Advanced & production (46–60)

All **Todo** (deploy scaffolding exists early). Includes advanced RPC, GraphQL, indexing, analytics, privacy/bridges/L2, wallet, governance, hardening, and:

- **59** Production deployment — Compose/Helm/CI exist; deploy job is a placeholder; no production-ready node image yet
- **60** Documentation & training — `docs/` reorganized; keep status honest as code moves

## Related docs

- [ROADMAP.md](./ROADMAP.md) — full milestone write-ups
- [deployment.md](./deployment.md) — how to run what exists today
- [architecture.md](./architecture.md) — module map
