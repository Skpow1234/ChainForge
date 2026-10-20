# CI/CD

Workflow: `.github/workflows/ci.yml`

## Jobs

| Job | Purpose | Triggers |
|-----|---------|----------|
| Build | Multi-compiler builds (GCC/Clang Debug/Release) | Push, PR |
| Security | Sanitizers (ASAN/UBSAN, TSAN) | Push, PR |
| Docker | Image build / Compose checks | Push, PR |
| SBOM | Software bill of materials | Push, PR |
| Deploy | Placeholder production deploy | Main (stub) |

## Quality gates (stated goals)

- Build success across supported compilers
- Test coverage target (>90% stated; enable `BUILD_TESTING` and grow suite)
- Static analysis without critical issues
- Security scan / sanitizer cleanliness
- No unexplained performance regressions (when benchmarks exist)

See also [deployment.md](./deployment.md) for how images and Helm relate to CI.
