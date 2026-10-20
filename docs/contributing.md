# Contributing

## Workflow

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests where appropriate
5. Update docs under `docs/`
6. Open a pull request

## Local setup

```bash
git clone https://github.com/Skpow1234/ChainForge.git
cd ChainForge

conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build . --parallel
ctest --output-on-failure
```

Details: [development.md](./development.md), [BUILD_README.md](./BUILD_README.md).

## Guidelines

- Match existing C++ style and module boundaries
- Prefer small, reviewable PRs aligned with [milestones.md](./milestones.md)
- Do not commit secrets; use env overrides for passwords
- Keep the root README short; put detail in `docs/`
