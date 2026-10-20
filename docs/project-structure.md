# Project structure

```
ChainForge/
├─ CMakeLists.txt          # Root CMake configuration
├─ conanfile.txt           # Conan dependencies
├─ CMakePresets.json       # CMake presets
├─ docker-compose.yml      # Local multi-service stack
├─ cmake/                  # CMake modules and Find* scripts
├─ .github/workflows/      # CI/CD pipelines
├─ docs/                   # Project documentation (you are here)
├─ config/                 # Runtime YAML config
├─ modules/                # Libraries
│  ├─ core/                # Domain models
│  ├─ crypto/              # Cryptographic primitives
│  ├─ serialization/       # Protobuf serialization
│  ├─ p2p/                 # Peer-to-peer networking
│  ├─ storage/             # DB abstraction (not in default build)
│  ├─ consensus/           # Consensus (not in default build)
│  ├─ mempool/             # Tx pool (not in default build)
│  ├─ rpc/                 # JSON-RPC (not in default build)
│  ├─ logging/             # Logging (not in default build)
│  └─ metrics/             # Metrics (not in default build)
├─ tests/                  # Unit / integration / fuzz tests
├─ scripts/                # Build/test helpers
└─ deploy/                 # Deployment assets
   ├─ docker/              # Dockerfiles
   ├─ helm/                # Kubernetes Helm charts
   ├─ prometheus/          # Scrape config
   ├─ grafana/             # Dashboards & provisioning
   └─ postgres/            # DB init scripts
```

Planned but not yet in-tree as packages: `modules/node`, `modules/explorer`, `modules/execution`, `tools/`.
