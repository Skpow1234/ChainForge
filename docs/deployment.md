# Deployment

Tokiwa can be run locally with Docker Compose, packaged via multi-stage Dockerfiles, or deployed to Kubernetes with Helm. Application images currently build the core demo binary as a stand-in until a full `node` / `explorer` target exists.

## 1. Docker Compose (local / lab)

From the repo root:

```bash
# Infrastructure + apps
docker compose up -d

# Infra only
docker compose up -d postgres redis prometheus grafana jaeger

# Build images
docker compose build
docker compose build tokiwa-node

# Optional tools profile
docker compose --profile tools up -d tokiwa-tools
```

| Compose service | Dockerfile | Notes |
|-----------------|------------|-------|
| `postgres` | official image | Init SQL under `deploy/postgres/init/` |
| `redis` | official image | Password required in URL |
| `prometheus` / `grafana` / `jaeger` | official images | Config under `deploy/` |
| `tokiwa-node` | `deploy/docker/Dockerfile.node` | Ports 8080, 8545, 8546, 30303 |
| `tokiwa-explorer` | `deploy/docker/Dockerfile.explorer` | Ports 8081, 4000, 4001 |
| `tokiwa-tools` | `deploy/docker/Dockerfile.tools` | Profile `tools` |

Config is mounted from `./config` into the containers. Port map: [endpoints.md](./endpoints.md).

### Dev credentials (change before any shared deploy)

| System | User / secret |
|--------|----------------|
| Grafana | `admin` / `tokiwa_grafana_pass` |
| PostgreSQL | `tokiwa` / `tokiwa_pass` |
| Redis | password `tokiwa_redis_pass` |

Prefer env files or secret managers; do not reuse these defaults outside localhost.

### Node binary

```bash
# After Conan + CMake build:
./build/bin/tokiwa-node --config config/config.yaml --mine-once
# Health (same port as RPC):
curl http://127.0.0.1:8545/health
curl http://127.0.0.1:8545/ready
```

Storage backends: `memory`, `file` (length-prefixed KV file), `rocksdb` (real RocksDB when `librocksdb` is linked at build time; falls back to file-KV otherwise).

## 2. Docker images

Multi-stage builds under `deploy/docker/`:

- **Dockerfile.node** — Conan + CMake build, runtime image for the node service
- **Dockerfile.explorer** — Explorer service image
- **Dockerfile.tools** — Dev/build tooling container

Typical flow:

```bash
docker build -f deploy/docker/Dockerfile.node --target runtime -t tokiwa/tokiwa-node:0.1.0 .
docker build -f deploy/docker/Dockerfile.explorer --target runtime -t tokiwa/tokiwa-explorer:0.1.0 .
```

## 3. Kubernetes (Helm)

Charts live under `deploy/helm/`:

| Chart | Path | Purpose |
|-------|------|---------|
| Node | `deploy/helm/node/` | Tokiwa node Deployment/Service |
| Explorer | `deploy/helm/explorer/` | Explorer Deployment/Service |

Example (after images are available to the cluster):

```bash
helm upgrade --install tokiwa-node ./deploy/helm/node \
  --namespace tokiwa --create-namespace

helm upgrade --install tokiwa-explorer ./deploy/helm/explorer \
  --namespace tokiwa
```

Node chart defaults (`deploy/helm/node/values.yaml`) include:

- Image `tokiwa/tokiwa-node:0.1.0`
- Service ports: RPC `8545`, metrics `8080`, P2P `30303`
- Persistence `100Gi` (when enabled)
- Resource requests/limits (e.g. 500m–2000m CPU, 1–4Gi memory)
- Non-root security context

Override image tags, ingress, and secrets via a custom `values.yaml` or `--set`.

## 4. Observability stack

- Prometheus scrape config: `deploy/prometheus/`
- Grafana provisioning & dashboards: `deploy/grafana/`

Details: [observability.md](./observability.md).

## 5. CI deploy job

`.github/workflows/ci.yml` includes build, sanitizers, Docker, and SBOM jobs. The **Deploy** step on `main` is currently a placeholder and does not push to a production cluster automatically.

## Related

- [BUILD_README.md](./BUILD_README.md) — end-to-end build/run
- [DOCKER-FIXES.md](./DOCKER-FIXES.md) — container troubleshooting
- Milestone 59 in [ROADMAP.md](./ROADMAP.md) — production deployment goals
