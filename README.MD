# ChainForge

C++20 blockchain client skeleton — core models, crypto, serialization, and P2P, with Docker/Helm scaffolding for a future full node and explorer.

[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg)](https://cmake.org/)
[![Conan](https://img.shields.io/badge/Conan-2.0+-orange.svg)](https://conan.io/)
[![Docker](https://img.shields.io/badge/Docker-Compose-blue.svg)](https://docker.com/)

## Quick start

**Prerequisites:** Docker Compose, CMake 3.20+, Conan 2, a C++20 compiler.

```bash
# Optional: local infra (Postgres, Redis, Prometheus, Grafana, Jaeger)
docker compose up -d postgres redis prometheus grafana jaeger

# Build
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel
```

Full build options, tests, and IDE notes: [docs/development.md](docs/development.md).

## Documentation

| Topic | Link |
|-------|------|
| Docs home | [docs/README.md](docs/README.md) |
| Architecture | [docs/architecture.md](docs/architecture.md) |
| Endpoints & ports | [docs/endpoints.md](docs/endpoints.md) |
| Deployment (Compose / Docker / Helm) | [docs/deployment.md](docs/deployment.md) |
| Milestones | [docs/milestones.md](docs/milestones.md) |
| Full roadmap | [docs/ROADMAP.md](docs/ROADMAP.md) |
| Contributing | [docs/contributing.md](docs/contributing.md) |

## Current status

Version **0.1.0**. Default build enables `core`, `crypto`, `serialization`, and `p2p`. Storage, consensus, RPC, mempool, and metrics exist as source but are not wired into the default CMake build; node/explorer apps are still to be composed. Treat credentials in Compose/config as **dev-only**.

## Acknowledgments

OpenSSL, Boost, RocksDB, Prometheus, Grafana, Jaeger, and the broader C++ / blockchain tooling ecosystem.
