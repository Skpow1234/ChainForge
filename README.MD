# ChainForge

Production-grade crypto/blockchain client skeleton in modern C++ (C++20/23).

## Architecture

- **Modules**: core, crypto, storage, p2p, consensus, execution, mempool, rpc, node, explorer, tools, tests
- **Binaries**: cppchain-node, cppchain-explorer
- **Build**: CMake + Conan v2
- **Packaging**: Docker (distroless)
- **Deploy**: Helm on K8s

## Quick Start

### Prerequisites

- Docker & Docker Compose
- CMake 3.20+
- Conan 2.0+
- C++20 compiler (GCC 11+, Clang 14+, MSVC 2019+)

### Local Development

```bash
# Start all services
docker-compose up -d

# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Docker Services

- **PostgreSQL**: Persistent blockchain data
- **Redis**: Caching layer for explorer
- **chainforge-node**: Full node service
- **chainforge-explorer**: Blockchain explorer API
- **Prometheus**: Metrics collection
- **Grafana**: Metrics visualization
- **Jaeger**: Distributed tracing

## Project Structure

```bash
ChainForge/
├─ CMakeLists.txt          # Root CMake configuration
├─ conanfile.txt           # Conan dependencies
├─ cmake/                  # CMake modules and presets
├─ .github/workflows/      # CI/CD pipelines
├─ modules/                # Core modules
│  ├─ core/               # Domain models (blocks, tx, hashing)
│  ├─ crypto/             # Cryptographic primitives
│  ├─ storage/            # Database abstraction
│  ├─ p2p/                # Peer-to-peer networking
│  ├─ consensus/          # Consensus algorithms
│  ├─ execution/          # State machine execution
│  ├─ mempool/            # Transaction pool management
│  ├─ rpc/                # RPC server implementation
│  ├─ node/               # Node composition root
│  └─ explorer/           # Blockchain explorer
├─ tools/                  # Utility tools
├─ tests/                  # Test suite
└─ deploy/helm/            # Kubernetes deployment
```

## Development Guidelines

- **C++ Standard**: C++20 (prefer), C++23 optional
- **Error Handling**: std::expected<T, E> for recoverable errors
- **Ownership**: unique_ptr by default, shared_ptr only when necessary
- **Concurrency**: Explicit thread pools, no implicit global threads
- **Security**: ASAN/UBSAN/TSAN in CI, fuzzing, SBOM generation

## Roadmap

1. **PoC**: Storage + hash-linked blocks + simple PoW + basic RPC
2. **P2P**: Gossip + mempool + deterministic validation
3. **State**: Verifiable state (Merkle/Trie) + snapshots/pruning
4. **Consensus**: PoS/BFT with finality + slashing
5. **Production**: Observability, hardening, CI/CD, Helm packaging
6. **Advanced**: Light client + DNS seeds + snapshot sync
