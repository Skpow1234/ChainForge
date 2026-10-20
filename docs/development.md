# Development

## Prerequisites

- Docker & Docker Compose
- CMake 3.20+
- Conan 2.0+
- C++20 compiler (GCC 11+, Clang 14+, MSVC 2019+)

## Quick local build (recommended)

```bash
# Infra (optional while coding libraries)
docker compose up -d postgres redis prometheus grafana jaeger

# Dependencies
conan install . --output-folder=build --build=missing

# Configure & build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build . --parallel

# Tests
ctest --output-on-failure
```

Helper scripts (if present):

```bash
# Linux/macOS
./scripts/build-and-test.sh

# Windows PowerShell
.\scripts\build-and-test.ps1
```

See [BUILD_README.md](./BUILD_README.md) for Docker-based and IDE setup.

## Common CMake commands

```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
cmake --build build --target test
cmake --build build --target install
cmake --build build --target clean
```

Useful options (root `CMakeLists.txt`):

| Option | Default | Meaning |
|--------|---------|---------|
| `BUILD_TESTING` | `OFF` | Build `tests/` |
| `BUILD_TOOLS` | `OFF` | Tools targets (when present) |
| `USE_SANITIZERS` | `OFF` | ASAN/UBSAN/etc. |

## Code standards

- **Standard**: C++20 preferred; C++23 optional
- **Errors**: `std::expected<T, E>` for recoverable failures
- **Ownership**: `unique_ptr` by default; `shared_ptr` only when needed
- **Concurrency**: Explicit thread pools; avoid hidden global threads
- **Safety**: ASAN/UBSAN/TSAN in CI; fuzzing and SBOM over time

## Quality tooling

```bash
clang-tidy -p build
# Coverage (when tests built with coverage flags)
# gcov / lcov against build/tests
cloc modules/ --include-lang=C++
```

More on error handling: [ERROR_HANDLING_SUMMARY.md](./ERROR_HANDLING_SUMMARY.md).  
Testing: [TESTING_FRAMEWORK.md](./TESTING_FRAMEWORK.md).
