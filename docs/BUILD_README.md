# ChainForge Build Guide

This guide explains how to build and run the ChainForge blockchain client.

## 🎯 Quick Start

### Prerequisites

- **Docker Desktop** (recommended)
- **Git**
- **At least 8GB RAM** for Docker containers

### One-Command Build

```bash
# Linux/macOS
./scripts/build-and-test.sh

# Windows PowerShell
.\scripts\build-and-test.ps1
```

## 🏗️ Manual Build Steps

### 1. Clone and Setup

```bash
git clone <repository-url>
cd ChainForge
```

### 2. Start Infrastructure

```bash
# Start all services
docker compose up -d

# Or start individual services
docker compose up -d postgres redis prometheus grafana jaeger
```

### 3. Build Application

```bash
# Build all Docker images
docker compose build

# Or build specific service
docker compose build chainforge-node
```

### 4. Run Tests

```bash
# Run the build and test suite
docker compose run --rm chainforge-tools

# Inside the container:
conan profile detect --force
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
./bin/cppchain-core-test
```

## 🐳 Services Overview

| Service | Port | Description | Access |
|---------|------|-------------|--------|
| **PostgreSQL** | 5432 | Database | Internal |
| **Redis** | 6379 | Cache | Internal |
| **Prometheus** | 9090 | Metrics | <http://localhost:9090> |
| **Grafana** | 3000 | Dashboards | <http://localhost:3000> |
| **Jaeger** | 16686 | Tracing | <http://localhost:16686> |
| **ChainForge Node** | 8545 (RPC), 30303 (P2P) | Blockchain node | <http://localhost:8545> |
| **ChainForge Explorer** | 4000 | Block explorer | <http://localhost:4000> |

### Default Credentials

- **Grafana**: admin / chainforge_grafana_pass
- **PostgreSQL**: chainforge / chainforge_pass
- **Redis**: password: chainforge_redis_pass

## 🔧 Development Setup

### Local Development (without Docker)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake ninja-build pkg-config \
                       libssl-dev libboost-all-dev librocksdb-dev \
                       libgtest-dev git python3 python3-pip

# Install Conan
pip3 install conan
conan profile detect --force

# Install dependencies
conan install . --output-folder=build --build=missing

# Build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel

# Run tests
ctest --output-on-failure
```

### IDE Setup

#### VS Code

1. Install C++ extension
2. Configure CMake: `Ctrl+Shift+P` → "CMake: Configure"
3. Build: `Ctrl+Shift+P` → "CMake: Build"

#### CLion

1. Import project as CMake project
2. Configure toolchain with Conan
3. Build and run tests

## 📊 Monitoring and Observability

### Grafana Dashboards

1. Open <http://localhost:3000>
2. Login with admin/chainforge_grafana_pass
3. Import dashboard from `deploy/grafana/dashboards/`

### Prometheus Metrics

- View raw metrics: <http://localhost:9090>
- Query blockchain metrics with PromQL

### Distributed Tracing

- View traces: <http://localhost:16686>
- Trace RPC calls, database operations, and consensus

## 🔌 API Usage

### JSON-RPC API

The node exposes an Ethereum-compatible JSON-RPC API:

```bash
# Get client version
curl -X POST -H "Content-Type: application/json" \
  --data '{"jsonrpc":"2.0","method":"web3_clientVersion","params":[],"id":1}' \
  http://localhost:8545

# Get current block number
curl -X POST -H "Content-Type: application/json" \
  --data '{"jsonrpc":"2.0","method":"eth_blockNumber","params":[],"id":1}' \
  http://localhost:8545

# Get account balance
curl -X POST -H "Content-Type: application/json" \
  --data '{"jsonrpc":"2.0","method":"eth_getBalance","params":["0x742d35cc6634c0532925a3b844bc454e4438f44e","latest"],"id":1}' \
  http://localhost:8545
```

### Available RPC Methods

- `web3_clientVersion` - Get client version
- `net_version` - Get network version
- `eth_chainId` - Get chain ID
- `eth_blockNumber` - Get latest block number
- `eth_getBlockByNumber` - Get block by number
- `eth_getBlockByHash` - Get block by hash
- `eth_getBalance` - Get account balance
- `eth_getTransactionCount` - Get account nonce
- `eth_gasPrice` - Get gas price
- `eth_sendRawTransaction` - Submit transaction
- `eth_getTransactionByHash` - Get transaction details
- `eth_getTransactionReceipt` - Get transaction receipt

## 🧪 Testing

### Unit Tests

```bash
# Run all tests
docker compose run --rm chainforge-tools ctest --output-on-failure

# Run specific test
docker compose run --rm chainforge-tools ctest -R TestName
```

### Integration Tests

```bash
# Test full system
docker compose up -d
docker compose run --rm chainforge-tools ./scripts/test-integration.sh
```

### Performance Benchmarks

```bash
# Run benchmarks
docker compose run --rm chainforge-tools ./bin/cppchain-benchmark
```

## 🚀 Deployment

### Production Deployment

```bash
# Build production images
docker compose -f docker-compose.prod.yml build

# Deploy to Kubernetes
kubectl apply -f deploy/helm/

# Or use Docker Swarm
docker stack deploy -c docker-compose.swarm.yml chainforge
```

### Configuration

Environment variables for customization:

```bash
# Node configuration
NODE_ENV=production
LOG_LEVEL=info
RPC_PORT=8545
P2P_PORT=30303

# Database
POSTGRES_HOST=postgres
POSTGRES_DB=chainforge
POSTGRES_USER=chainforge

# Consensus
DIFFICULTY=1000
BLOCK_TIME_TARGET=15
```

## 🐛 Troubleshooting

### Common Issues

**Build fails with Conan errors:**

```bash
# Clear Conan cache
conan remove "*" -f
conan install . --output-folder=build --build=missing
```

**Docker containers won't start:**

```bash
# Check Docker resources
docker system df

# Clean up and restart
docker compose down -v
docker compose up -d
```

**RPC server not responding:**

```bash
# Check node logs
docker compose logs chainforge-node

# Test connectivity
curl -v http://localhost:8545
```

**Database connection issues:**

```bash
# Check PostgreSQL logs
docker compose logs postgres

# Reset database
docker compose down -v
docker compose up -d postgres
```

### Debug Mode

```bash
# Run with debug logging
docker compose run --rm -e LOG_LEVEL=debug chainforge-node

# Attach debugger
docker compose run --rm --cap-add=SYS_PTRACE chainforge-tools gdb ./bin/cppchain-node
```

## 📚 Architecture Overview

ChainForge is built with a modular architecture:

- **Core**: Domain models (Block, Transaction, Address, etc.)
- **Crypto**: Cryptographic primitives (secp256k1, Ed25519, BLS)
- **Storage**: Database abstraction (RocksDB backend)
- **Consensus**: Proof of Work mining and validation
- **Mempool**: Transaction pool management
- **RPC**: JSON-RPC server for external communication

Each module is independently testable and can be replaced or extended.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes with tests
4. Run the full test suite
5. Submit a pull request

### Development Workflow

```bash
# Create feature branch
git checkout -b feature/new-feature

# Make changes
# ... edit code ...

# Test changes
docker compose run --rm chainforge-tools ctest

# Build and verify
./scripts/build-and-test.sh

# Submit PR
git push origin feature/new-feature
```

## 📄 License

ChainForge is licensed under the MIT License. See LICENSE file for details.

## 🆘 Support

- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Documentation**: See `documentation/` directory
- **Wiki**: Project Wiki

---
