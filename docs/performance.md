# Performance

## Targets

| Metric | Target | Current |
|--------|--------|---------|
| Transaction throughput | >10,000 TPS | TBD |
| Block time | <1 second | TBD (config default `block_time: 15`) |
| Network latency | <100ms | TBD |
| Memory usage | <4GB | TBD |
| Storage I/O | >100K ops/sec | TBD |

## Strategies (planned / in design)

- Parallel execution and explicit thread pools
- Redis + RocksDB caching/storage tuning
- Connection pooling and load balancing for P2P
- Continuous profiling and automated benchmarks in CI over time

Track related work under later milestones in [ROADMAP.md](./ROADMAP.md) (e.g. 45, 56).
