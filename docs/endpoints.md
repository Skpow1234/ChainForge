# Endpoints & Ports

Ports and URLs from `docker-compose.yml`, `config/config.yaml`, and the RPC module. Many app endpoints are planned or mocked until the node/explorer binaries are fully wired.

## Service ports (Docker Compose)

| Service | Port(s) | Purpose | Local URL |
|---------|---------|---------|-----------|
| PostgreSQL | 5432 | Explorer / app DB | `localhost:5432` |
| Redis | 6379 | Cache | `localhost:6379` |
| Prometheus | 9090 | Metrics scrape UI | http://localhost:9090 |
| Grafana | 3000 | Dashboards | http://localhost:3000 |
| Jaeger UI | 16686 | Trace UI | http://localhost:16686 |
| Jaeger collector | 14268, 14250, 6831/udp, 6832/udp | Trace ingest | — |
| chainforge-node | 8080 | Node metrics | http://localhost:8080/metrics |
| chainforge-node | 8545 | JSON-RPC | http://localhost:8545 |
| chainforge-node | 8546 | WebSocket RPC | `ws://localhost:8546` |
| chainforge-node | 30303 TCP/UDP | P2P | — |
| chainforge-explorer | 8081 | Explorer metrics | http://localhost:8081/metrics |
| chainforge-explorer | 4000 | Explorer HTTP API | http://localhost:4000 |
| chainforge-explorer | 4001 | Explorer WebSocket | `ws://localhost:4001` |

## Config defaults (`config/config.yaml`)

| Setting | Default |
|---------|---------|
| Chain / network ID | `1337` |
| JSON-RPC | `0.0.0.0:8545` |
| WebSocket | `0.0.0.0:8546` |
| Metrics | `0.0.0.0:8080`, path `/metrics` |
| P2P discovery | `30303` |
| Postgres | `localhost:5432` / db `chainforge` |
| Redis | `localhost:6379` |
| Jaeger traces | `http://localhost:14268/api/traces` |

RPC APIs enabled in config: `eth`, `net`, `web3`, `personal`, `txpool`.

## JSON-RPC methods (node)

HTTP JSON-RPC on port **8545**. Example:

```bash
curl -X POST http://localhost:8545 \
  -H "Content-Type: application/json" \
  --data '{"jsonrpc":"2.0","method":"eth_blockNumber","params":[],"id":1}'
```

| Method | Description |
|--------|-------------|
| `web3_clientVersion` | Client version string |
| `net_version` | Network version |
| `eth_chainId` | Chain ID |
| `eth_blockNumber` | Latest block number |
| `eth_getBlockByNumber` | Block by number |
| `eth_getBlockByHash` | Block by hash |
| `eth_getBalance` | Account balance |
| `eth_getTransactionCount` | Account nonce |
| `eth_gasPrice` | Gas price |
| `eth_sendRawTransaction` | Submit raw transaction |
| `eth_getTransactionByHash` | Transaction by hash |
| `eth_getTransactionReceipt` | Transaction receipt |

Implementation lives under `modules/rpc/` (not in the default CMake build yet). Responses may be mock/stub data while the node composition root is unfinished.

## Observability URLs

| System | URL |
|--------|-----|
| Prometheus | http://localhost:9090 |
| Grafana | http://localhost:3000 |
| Jaeger | http://localhost:16686 |
| Node metrics | http://localhost:8080/metrics |
| Explorer metrics | http://localhost:8081/metrics |

Default Grafana login (dev only): `admin` / `chainforge_grafana_pass`. See [Deployment](./deployment.md) for credentials and how to override them.
