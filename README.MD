# ChainForge

> **Production-grade crypto/blockchain client skeleton in modern C++ (C++20/23)**

[![C++](https://img.shields.io/badge/C++-20/23-blue.svg?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg?style=for-the-badge&logo=cmake)](https://cmake.org/)
[![Conan](https://img.shields.io/badge/Conan-2.0+-orange.svg?style=for-the-badge&logo=conan)](https://conan.io/)
[![Docker](https://img.shields.io/badge/Docker-Ready-blue.svg?style=for-the-badge&logo=docker)](https://docker.com/)
[![Kubernetes](https://img.shields.io/badge/Kubernetes-Helm-blue.svg?style=for-the-badge&logo=kubernetes)](https://kubernetes.io/)
[![RocksDB](https://img.shields.io/badge/RocksDB-Storage-red.svg?style=for-the-badge&logo=rocksdb)](https://rocksdb.org/)
[![PostgreSQL](https://img.shields.io/badge/PostgreSQL-Database-blue.svg?style=for-the-badge&logo=postgresql)](https://postgresql.org/)
[![Redis](https://img.shields.io/badge/Redis-Cache-red.svg?style=for-the-badge&logo=redis)](https://redis.io/)
[![Prometheus](https://img.shields.io/badge/Prometheus-Metrics-orange.svg?style=for-the-badge&logo=prometheus)](https://prometheus.io/)
[![Grafana](https://img.shields.io/badge/Grafana-Visualization-orange.svg?style=for-the-badge&logo=grafana)](https://grafana.com/)
[![OpenSSL](https://img.shields.io/badge/OpenSSL-Crypto-green.svg?style=for-the-badge&logo=openssl)](https://openssl.org/)
[![Boost](https://img.shields.io/badge/Boost-Libraries-blue.svg?style=for-the-badge&logo=boost)](https://boost.org/)
[![Protocol Buffers](https://img.shields.io/badge/Protobuf-Serialization-blue.svg?style=for-the-badge&logo=protobuf)](https://developers.google.com/protocol-buffers)

---

## Table of Contents

- [Quick Start](#quick-start)
- [Architecture](#architecture)
- [Development](#development)
- [Docker Services](#docker-services)
- [Monitoring & Observability](#monitoring--observability)
- [Project Structure](#project-structure)
- [Documentation](#documentation)
- [Development Guidelines](#development-guidelines)
- [Security Features](#security-features)
- [Performance](#performance)
- [CI/CD Pipeline](#cicd-pipeline)
- [Roadmap](#roadmap)
- [Contributing](#contributing)

---

## Quick Start

### Prerequisites

- **Docker & Docker Compose** - Container orchestration
- **CMake 3.20+** - Build system
- **Conan 2.0+** - Dependency management
- **C++20 compiler** - GCC 11+, Clang 14+, MSVC 2019+

### Local Development

```bash
# Start all services
docker compose up -d

# Build the project
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Run specific tests
ctest -R "core_tests" --output-on-failure
```

### Docker Services

| Service | Purpose | Port | Status |
|---------|---------|------|--------|
| **PostgreSQL** | Persistent blockchain data | 5432 | Ready |
| **Redis** | Caching layer for explorer | 6379 | Ready |
| **chainforge-node** | Full node service | 8080, 8545 | Building |
| **chainforge-explorer** | Blockchain explorer API | 8081, 4000 | Building |
| **Prometheus** | Metrics collection | 9090 | Ready |
| **Grafana** | Metrics visualization | 3000 | Ready |
| **Jaeger** | Distributed tracing | 16686 | Ready |

---

## Architecture

### Core Modules

```bash
┌─────────────────────────────────────────────────────────────┐
│                    ChainForge System                       │
├─────────────────────────────────────────────────────────────┤
│  Explorer API  │  Node Service  │  Tools Service  │
├─────────────────────────────────────────────────────────────┤
│  RPC Server    │  P2P Network   │  Consensus      │
├─────────────────────────────────────────────────────────────┤
│  Execution     │  Storage       │  Core Models    │
├─────────────────────────────────────────────────────────────┤
│  Crypto        │  Mempool       │  Metrics        │
└─────────────────────────────────────────────────────────────┘
```

### Technology Stack

| Category | Technology | Purpose |
|----------|------------|---------|
| **Language** | C++20/23 | Core implementation |
| **Build** | CMake + Conan | Build system & deps |
| **Container** | Docker | Application packaging |
| **Orchestration** | Kubernetes | Production deployment |
| **Database** | RocksDB + PostgreSQL | Storage & persistence |
| **Cache** | Redis | Performance optimization |
| **Networking** | Boost.Asio + libp2p | P2P communication |
| **Security** | OpenSSL + libsecp256k1 | Cryptographic operations |
| **Monitoring** | Prometheus + Grafana | Observability |
| **Tracing** | Jaeger + OpenTelemetry | Distributed tracing |

---

## Development

### Build Commands

```bash
# Configure build
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build all targets
cmake --build build --parallel

# Run tests
cmake --build build --target test

# Install
cmake --build build --target install

# Clean build
cmake --build build --target clean
```

### Development Tools

```bash
# Code formatting
make format

# Linting
make lint

# Testing
make test

# Docker operations
make docker-up
make docker-down
make docker-logs

# Security checks
make security-check

# SBOM generation
make sbom
```

---

## Monitoring & Observability

### Metrics Endpoints

- **Prometheus**: `http://localhost:9090` - Metrics collection
- **Grafana**: `http://localhost:3000` - Dashboards & visualization
- **Jaeger**: `http://localhost:16686` - Distributed tracing
- **Node Metrics**: `http://localhost:8080/metrics` - ChainForge metrics
- **Explorer Metrics**: `http://localhost:8081/metrics` - Explorer metrics

### Key Metrics

| Metric | Description | Target |
|--------|-------------|---------|
| **Block Height** | Current blockchain height | Real-time |
| **Transaction Rate** | TPS (Transactions per second) | >10,000 TPS |
| **Active Peers** | Connected P2P nodes | >50 peers |
| **Memory Usage** | System memory consumption | <4GB |
| **CPU Usage** | System CPU utilization | <80% |
| **Network Latency** | P2P communication delay | <100ms |

---

## Project Structure

```bash
ChainForge/
├─ CMakeLists.txt          # Root CMake configuration
├─ conanfile.txt           # Conan dependencies
├─ cmake/                  # CMake modules and presets
├─ .github/workflows/      # CI/CD pipelines
├─ documentation/           # Project documentation
├─ modules/                # Core modules
│  ├─ core/               # Domain models (blocks, tx, hashing)
│  ├─ crypto/             # Cryptographic primitives
│  ├─ storage/            # Database abstraction
│  ├─ p2p/                # Peer-to-peer networking
│  ├─ consensus/          # Consensus algorithms
│  ├─ execution/          # State machine execution
│  ├─ mempool/            # Transaction pool management
│  ├─ rpc/                # RPC server implementation
│  ├─ node/                # Node composition root
│  └─ explorer/           # Blockchain explorer
├─ tools/                  # Utility tools
├─ tests/                  # Test suite
└─ deploy/                 # Deployment configurations
    ├─ docker/            # Docker configurations
    ├─ helm/              # Kubernetes Helm charts
    ├─ prometheus/        # Monitoring setup
    └─ grafana/           # Dashboard configurations
```

---

## Documentation

| Document | Description | Audience |
|----------|-------------|----------|
| [ROADMAP](./documentation/ROADMAP.md) | 60+ development milestones | Product, Engineering |
| [PROJECT_OVERVIEW](./documentation/PROJECT_OVERVIEW.md) | System architecture & vision | Architects, Stakeholders |
| [MILESTONE_TEMPLATE](./documentation/MILESTONE_TEMPLATE.md) | Milestone tracking template | Project Managers |
| [README](./documentation/README.md) | Documentation overview | All users |

---

## Development Guidelines

### C++ Standards & Practices

- **Standard**: C++20 (prefer), C++23 optional
- **Error Handling**: `std::expected<T, E>` for recoverable errors
- **Ownership**: `unique_ptr` by default, `shared_ptr` only when necessary
- **Concurrency**: Explicit thread pools, no implicit global threads
- **Security**: ASAN/UBSAN/TSAN in CI, fuzzing, SBOM generation

### Code Quality

```bash
# Static analysis
clang-tidy build/compile_commands.json

# Test coverage
gcov --coverage build/tests/

# Code metrics
cloc modules/ --include-lang=C++

# Security scan
make security-scan
```

---

## Security Features

### Security Measures

| Feature | Description | Status |
|---------|-------------|--------|
| **Cryptographic Primitives** | OpenSSL, libsecp256k1, libsodium | ✅ Implemented |
| **Memory Safety** | ASAN/UBSAN/TSAN sanitizers | 🚧 In Progress |
| **Fuzzing** | libFuzzer/AFL++ integration | 🚧 In Progress |
| **SBOM Generation** | Software Bill of Materials | ✅ Implemented |
| **Image Signing** | Cosign/Sigstore integration | 🚧 In Progress |
| **Security Audits** | Regular security assessments | 📅 Planned |

### Security Best Practices

- **Zero-trust architecture** - Verify everything
- **Defense in depth** - Multiple security layers
- **Continuous monitoring** - Real-time threat detection
- **Security documentation** - Comprehensive guidelines
- **Regular testing** - Penetration testing & audits

---

## Performance

### Performance Targets

| Metric | Target | Current | Status |
|--------|--------|---------|--------|
| **Transaction Throughput** | >10,000 TPS | 🚧 TBD | 🚧 In Progress |
| **Block Time** | <1 second | 🚧 TBD | 🚧 In Progress |
| **Network Latency** | <100ms | 🚧 TBD | 🚧 In Progress |
| **Memory Usage** | <4GB | 🚧 TBD | 🚧 In Progress |
| **Storage I/O** | >100K ops/sec | 🚧 TBD | 🚧 In Progress |

### Optimization Strategies

- **Parallel execution** - Multi-threaded processing
- **Efficient caching** - Redis + RocksDB optimization
- **Network optimization** - Connection pooling & load balancing
- **Profiling** - Continuous performance monitoring
- **Benchmarking** - Automated performance testing

---

## CI/CD Pipeline

### GitHub Actions Workflow

| Job | Purpose | Triggers |
|-----|---------|----------|
| **Build** | Multi-compiler builds | Push, PR |
| **Security** | Sanitizers & fuzzing | Push, PR |
| **Docker** | Image building & testing | Push, PR |
| **SBOM** | Software Bill of Materials | Push, PR |
| **Deploy** | Production deployment | Main branch |

### Quality Gates

- ✅ **Build Success** - All compilers pass
- ✅ **Test Coverage** - >90% coverage required
- ✅ **Static Analysis** - No critical issues
- ✅ **Security Scan** - No vulnerabilities
- ✅ **Performance** - No regressions

---

## Roadmap

### Phase 1: Foundation (Months 1-6)

- [] Project setup & build system
- [] Core domain models
- [] Cryptographic foundation
- [ ] Database abstraction layer
- [ ] Basic block structure
- [ ] Transaction pool (mempool)
- [ ] Simple Proof of Work
- [ ] Basic RPC server

### Phase 2: Networking (Months 7-12)

- [ ] Network transport layer
- [ ] Peer discovery
- [ ] Peer management
- [ ] Message protocol
- [ ] Block synchronization
- [ ] Transaction propagation

### Phase 3: Consensus & Execution (Months 13-18)

- [ ] Advanced consensus engine
- [ ] State management
- [ ] Smart contract engine
- [ ] Gas model
- [ ] Transaction execution
- [ ] Block execution

### Phase 4: Production (Months 19-24)

- [ ] Advanced RPC APIs
- [ ] GraphQL support
- [ ] Advanced indexing
- [ ] Analytics engine
- [ ] Production deployment
- [ ] Documentation & training

---

## Contributing

### Getting Started

1. **Fork the repository**
2. **Create a feature branch**
3. **Make your changes**
4. **Add tests**
5. **Update documentation**
6. **Submit a pull request**

### Development Setup

```bash
# Clone repository
git clone https://github.com/Skpow1234/ChainForge.git
cd ChainForge

# Install dependencies
make install-deps

# Build project
make build

# Run tests
make test

# Code quality checks
make lint
make format
```

## Acknowledgments

- **OpenSSL** - Cryptographic primitives
- **Boost Libraries** - Networking & utilities
- **RocksDB** - High-performance storage
- **Prometheus** - Metrics collection
- **Grafana** - Visualization platform
- **Jaeger** - Distributed tracing
