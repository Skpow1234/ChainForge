# Project structure

```
Tokiwa/
├─ CMakeLists.txt          # Root CMake configuration
├─ conanfile.txt           # Conan dependencies
├─ CMakePresets.json       # CMake presets
├─ docker-compose.yml      # Local multi-service stack
├─ cmake/                  # CMake modules and Find* scripts
├─ .github/workflows/      # CI/CD pipelines
├─ docs/                   # Project documentation (you are here)
├─ config/                 # Runtime YAML config
├─ modules/                # Libraries (headers under include/tokiwa/…)
│  ├─ core/                # Domain models
│  ├─ crypto/              # Cryptographic primitives
│  ├─ serialization/       # Protobuf / RLP helpers
│  ├─ p2p/                 # Peer-to-peer networking
│  ├─ storage/             # DB abstraction (memory / file / RocksDB)
│  ├─ consensus/           # Consensus
│  ├─ mempool/             # Tx pool
│  ├─ rpc/                 # JSON-RPC + ChainState
│  ├─ logging/             # Logging
│  ├─ metrics/             # Metrics
│  └─ node/                # tokiwa-node binary
├─ tests/                  # Unit / integration / fuzz tests
├─ scripts/                # Build/test helpers
└─ deploy/                 # Deployment assets
   ├─ docker/              # Dockerfiles
   ├─ helm/                # Kubernetes Helm charts
   ├─ prometheus/          # Scrape config
   ├─ grafana/             # Dashboards & provisioning
   └─ postgres/            # DB init scripts
```

Planned but not yet in-tree as packages: `modules/explorer`, `modules/execution`, `tools/`.
