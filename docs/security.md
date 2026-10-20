# Security

## Measures

| Feature | Description | Status |
|---------|-------------|--------|
| Cryptographic primitives | OpenSSL, libsecp256k1, libsodium | In progress (some stubs remain) |
| Memory safety | ASAN / UBSAN / TSAN in CI | In progress |
| Fuzzing | libFuzzer / AFL++ integration | Planned / partial |
| SBOM | Syft (or similar) in CI | Present in pipeline |
| Image signing | Cosign / Sigstore | Planned |
| Security audits | External assessments | Planned |

## Practices

- Prefer verified library crypto; do not ship stub curve/hash code in production builds
- Keep RPC/WebSocket bind addresses and CORS tight outside local lab
- Move Compose/Helm passwords to secrets or `.env` (never reuse repo defaults remotely)
- Defense in depth: sanitizers in CI, rate limits in config, TLS when exposing services

## Config flags to review before expose

From `config/config.yaml`:

- `rpc.host` / `websocket.host` default `0.0.0.0`
- `rpc.cors` / `websocket.cors` default `["*"]`
- `development.enable_mock_data` / `enable_debug_endpoints` default `true`
- `security.enable_https` default `false`
