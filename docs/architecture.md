# Architecture

## System layout

```
┌─────────────────────────────────────────────────────────────┐
│                    Tokiwa System                        │
├─────────────────────────────────────────────────────────────┤
│  Explorer API  │  Node Service  │  Tools Service            │
├─────────────────────────────────────────────────────────────┤
│  RPC Server    │  P2P Network   │  Consensus                │
├─────────────────────────────────────────────────────────────┤
│  Execution     │  Storage       │  Core Models              │
├─────────────────────────────────────────────────────────────┤
│  Crypto        │  Mempool       │  Metrics                  │
└─────────────────────────────────────────────────────────────┘
```

## Module status

| Module | Role | Default build |
|--------|------|---------------|
| `core` | Blocks, tx, address, amount, hash, errors | Enabled |
| `crypto` | Keys, signatures, hashing | Enabled |
| `serialization` | Protobuf serializers / validators | Enabled |
| `p2p` | TCP/UDP transport, peer discovery | Enabled |
| `logging`, `metrics`, `storage`, `consensus`, `mempool`, `rpc` | Supporting layers | Source present, commented out in `modules/CMakeLists.txt` |
| `execution`, `node`, `explorer` | Composition / apps | Not present yet |

## Technology stack

| Category | Technology | Purpose |
|----------|------------|---------|
| Language | C++20/23 | Core implementation |
| Build | CMake + Conan | Build system & deps |
| Container | Docker | Packaging |
| Orchestration | Kubernetes + Helm | Cluster deploy |
| Database | RocksDB + PostgreSQL | Chain storage & explorer DB |
| Cache | Redis | Explorer / cache |
| Networking | Boost.Asio (+ planned libp2p) | P2P |
| Crypto | OpenSSL, libsecp256k1, libsodium | Cryptography |
| Monitoring | Prometheus + Grafana | Metrics |
| Tracing | Jaeger | Distributed tracing |
| Serialization | Protocol Buffers, nlohmann_json | Wire / RPC formats |

For vision and longer-form design, see [PROJECT_OVERVIEW.md](./PROJECT_OVERVIEW.md).
