# CI/CD

Workflow: [`.github/workflows/ci.yml`](../.github/workflows/ci.yml)

## Design

- **Parallel matrix** — GCC-12 Debug/Release and Clang-15 Release run concurrently (`fail-fast: false`)
- **Concurrency** — one active run per ref; newer pushes cancel stale runs
- **Required gate** — job `CI Success` aggregates build, sanitizers, Docker, and SBOM for a single branch-protection check
- **Advisory lint** — clang-format dry-run does not block merges yet
- **Caches** — Conan (`~/.conan2`) keyed by OS/compiler/`conanfile.txt`

## Jobs

| Job | Purpose | Blocks merge? |
|-----|---------|---------------|
| Lint (advisory) | Format dry-run | No (`continue-on-error`) |
| Build (matrix) | Configure with Conan + Ninja, `BUILD_TESTING=ON`, `--self-check`, ctest | Yes |
| Sanitizers | ASan+UBSan Debug build + self-check | Yes |
| Docker | Compose infra (postgres/redis/…) + `Dockerfile.node` build/smoke | Yes (after build) |
| SBOM | Syft SPDX/JSON on Release artifact | Yes (after build) |
| CI Success | Aggregate gate | Yes |

## Local parity

```bash
conan install . --output-folder=build --build=missing -s build_type=Release
cmake -S . -B build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
  -DBUILD_TESTING=ON \
  -DTREAT_WARNINGS_AS_ERRORS=OFF
cmake --build build --parallel
./build/bin/chainforge-node --self-check
cd build && ctest --output-on-failure --parallel
```

## Artifacts

- `chainforge-linux-amd64` — Release `chainforge-node` (7-day retention)
- `sbom` — Syft JSON + SPDX (30-day retention)

See [deployment.md](./deployment.md) for images and Helm.
