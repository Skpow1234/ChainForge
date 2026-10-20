# Milestones

High-level view of the development plan. Full detail (problem, solution, acceptance, tests) is in [ROADMAP.md](./ROADMAP.md). Use [MILESTONE_TEMPLATE.md](./MILESTONE_TEMPLATE.md) when adding or tracking a milestone.

> Status marks in the roadmap are aspirational relative to the current codebase: only `core`, `crypto`, `serialization`, and `p2p` are in the default build; node/explorer/execution packages are not present yet.

## Phases

| Phase | Milestones | Theme |
|-------|------------|--------|
| 1 — Foundation | 1–15 | Build system, core models, crypto, storage, mempool, PoW, RPC, config, logging, metrics, health, errors, serialization, testing |
| 2 — Networking | 16–30 | Transport, discovery, peers, sync, propagation, NAT, scoring, topology |
| 3 — Consensus & execution | 31–45 | Consensus, state, contracts, gas, execution, finality, validators, light client, pruning |
| 4 — Production | 46–60 | Advanced RPC, GraphQL, indexing, analytics, security, K8s deploy, docs |

## Snapshot checklist

### Phase 1 — Foundation

| ID | Title |
|----|--------|
| 1 | Project setup & build system |
| 2 | Core domain models |
| 3 | Cryptographic foundation |
| 4 | Database abstraction layer |
| 5 | Basic block structure |
| 6 | Transaction pool (mempool) |
| 7 | Simple Proof of Work |
| 8 | Basic RPC server |
| 9 | Configuration management |
| 10 | Logging system |
| 11 | Metrics collection |
| 12 | Health check system |
| 13 | Basic error handling |
| 14 | Serialization framework |
| 15 | Basic testing framework |

### Phase 2 — Networking

| ID | Title |
|----|--------|
| 16 | Network transport layer |
| 17 | Peer discovery |
| 18 | Peer management |
| 19 | Message protocol |
| 20 | Block synchronization |
| 21 | Transaction propagation |
| 22 | Network security |
| 23 | NAT traversal |
| 24 | Peer scoring system |
| 25 | Network monitoring |
| 26 | Connection pooling |
| 27 | Message queuing |
| 28 | Protocol versioning |
| 29 | Network topology |
| 30 | P2P load balancing |

### Phase 3 — Consensus & execution

| ID | Title |
|----|--------|
| 31–36 | Advanced consensus, state, contracts, gas, tx/block execution |
| 37–42 | Fork choice, finality, validators, slashing, checkpoints, light client |
| 43–45 | Snapshots, pruning, execution optimization |

### Phase 4 — Advanced & production

| ID | Title |
|----|--------|
| 46–49 | Advanced RPC, GraphQL, indexing, analytics |
| 50–54 | Privacy, bridges, L2, wallet, governance |
| 55–60 | Security, performance, monitoring, DR, **production deployment**, documentation |

## Suggested near-term focus (realistic vs code)

1. Finish crypto hashing/signatures (no stubs) — Milestone 3
2. Enable storage + PoW + RPC in the build — Milestones 4, 7, 8
3. Thin `node` binary composing those modules — Milestone 59 prep
4. Peer management & message protocol on top of existing transport/discovery — Milestones 18–19

## README phase overview (compact)

Same four phases as historically listed in the root README: Foundation → Networking → Consensus & Execution → Production. Prefer this file and [ROADMAP.md](./ROADMAP.md) as the source of truth going forward.
