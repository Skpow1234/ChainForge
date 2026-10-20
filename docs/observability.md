# Observability

## Stack

| Component | Role | Default URL |
|-----------|------|-------------|
| Prometheus | Metrics collection | http://localhost:9090 |
| Grafana | Dashboards | http://localhost:3000 |
| Jaeger | Distributed tracing | http://localhost:16686 |
| Node metrics | App scrape target | http://localhost:8080/metrics |
| Explorer metrics | App scrape target | http://localhost:8081/metrics |

Provisioning:

- Prometheus: `deploy/prometheus/`
- Grafana datasources/dashboards: `deploy/grafana/`
- Node metrics path/interval: `config/config.yaml` → `metrics`

## Key metrics (targets)

| Metric | Description | Target |
|--------|-------------|--------|
| Block height | Current chain height | Real-time |
| Transaction rate | TPS | >10,000 TPS (goal) |
| Active peers | Connected P2P nodes | >50 peers (goal) |
| Memory usage | Process/system memory | <4GB (goal) |
| CPU usage | System CPU | <80% (goal) |
| Network latency | P2P delay | <100ms (goal) |

These targets are aspirational until the full node path is instrumented.

## Tracing

Jaeger OTLP/HTTP collector defaults to `http://localhost:14268/api/traces` (`tracing.jaeger` in config; Compose sets `JAEGER_ENDPOINT` on the node service).
