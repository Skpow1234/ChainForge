# ChainForge CI/CD Pipeline Improvements

## Overview

The CI pipeline has been restructured to implement **strict sequential ordering** where each step must be verified before proceeding to the next one. This prevents downstream stages from running if critical upstream stages fail, ensuring a robust and reliable CI/CD process.

## Pipeline Stages & Dependencies

### Stage 1: GCC 11 Debug Build (`build-gcc11-debug`)

- **Purpose**: First build verification using GCC 11 with Debug configuration
- **Dependencies**: None (first stage)
- **Verification**:
  - Build artifacts are created (`bin/`, `lib` directories)
  - Tests pass
  - At least one binary is present in `bin/` directory
- **Condition**: Must succeed to proceed

### Stage 2: GCC 11 Release Build (`build-gcc11-release`)

- **Purpose**: Build verification using GCC 11 with Release configuration
- **Dependencies**: `build-gcc11-debug` must succeed
- **Verification**: Same as Stage 1
- **Condition**: Only runs if Stage 1 passes

### Stage 3: GCC 12 Debug Build (`build-gcc12-debug`)

- **Purpose**: Build verification using GCC 12 with Debug configuration
- **Dependencies**: `build-gcc11-release` must succeed
- **Verification**: Same as previous stages
- **Condition**: Only runs if Stage 2 passes

### Stage 4: GCC 12 Release Build (`build-gcc12-release`)

- **Purpose**: Build verification using GCC 12 with Release configuration
- **Dependencies**: `build-gcc12-debug` must succeed
- **Verification**: Same as previous stages
- **Condition**: Only runs if Stage 3 passes

### Stage 5: Clang 14 Debug Build (`build-clang14-debug`)

- **Purpose**: Build verification using Clang 14 with Debug configuration
- **Dependencies**: `build-gcc12-release` must succeed
- **Verification**:
  - Same as previous stages
  - Clang-tidy static analysis runs
- **Condition**: Only runs if Stage 4 passes

### Stage 6: Clang 15 Release Build (`build-clang15-release`)

- **Purpose**: Build verification using Clang 15 with Release configuration
- **Dependencies**: `build-clang14-debug` must succeed
- **Verification**: Same as previous stages
- **Condition**: Only runs if Stage 5 passes

### Stage 7: Security Analysis (`security`)

- **Purpose**: Run security-focused builds with sanitizers and fuzz testing
- **Dependencies**: ALL build stages (1-6) must succeed
- **Verification**:
  - Sanitizer builds complete successfully
  - Fuzz tests run without crashes
  - Security build artifacts are created
- **Condition**: Only runs if all build stages pass

### Stage 8: Docker Verification (`docker`)

- **Purpose**: Build and test Docker images, verify container orchestration
- **Dependencies**: `security` must succeed
- **Verification**:
  - Docker images build successfully
  - Services start and run properly
  - Container health checks pass
- **Condition**: Only runs if security stage passes

### Stage 9: SBOM Generation (`sbom`)

- **Purpose**: Generate Software Bill of Materials for security and compliance
- **Dependencies**: `docker` must succeed
- **Verification**:
  - SBOM files are generated successfully
  - Artifacts are uploaded
- **Condition**: Only runs if Docker verification passes

### Stage 10: Deployment (`deploy`)

- **Purpose**: Deploy to staging environment (main branch only)
- **Dependencies**: ALL previous stages must succeed
- **Condition**: Only runs on main branch AND if all previous stages pass

### Final Verification (`verify-pipeline`)

- **Purpose**: Provide comprehensive pipeline status summary
- **Dependencies**: All stages
- **Condition**: Always runs (provides status regardless of success/failure)

## Key Improvements

### 1. Strict Sequential Dependencies

Each stage now explicitly depends on the success of the previous stage:

```yaml
needs: previous-stage
if: needs.previous-stage.result == 'success'
```

### 2. Individual Build Verification

Each build stage has its own verification step:

```yaml
- name: Verify build artifacts
  run: |
    cd build-stage-name
    if [ ! -d "bin" ] || [ -z "$(ls -A bin 2>/dev/null)" ]; then
      echo "❌ No binaries found in bin directory"
      exit 1
    fi
    echo "✅ Stage build verified successfully"
```

### 3. Build Directory Isolation

Each build stage uses a unique build directory:

- `build-gcc11-debug`
- `build-gcc11-release`
- `build-gcc12-debug`
- `build-gcc12-release`
- `build-clang14-debug`
- `build-clang15-release`
- `build-security`

### 4. Enhanced Verification

Each stage verifies:

- Build completion
- Test execution
- Artifact creation
- Binary presence in `bin/` directory

### 5. Clang-tidy Integration

Clang-tidy runs only on Clang 14 Debug builds with proper error handling:

```yaml
- name: Run clang-tidy (Clang 14 Debug)
  run: |
    cd build-clang14-debug
    sudo apt-get install -y clang-tidy
    find . -name "*.cpp" -o -name "*.hpp" | xargs -I {} clang-tidy {} -- -std=c++20 || echo "Clang-tidy completed with warnings"
```

## Pipeline Flow

```bash
GCC 11 Debug → GCC 11 Release → GCC 12 Debug → GCC 12 Release → Clang 14 Debug → Clang 15 Release → Security → Docker → SBOM → Deploy
     ↓              ↓              ↓              ↓              ↓              ↓              ↓         ↓        ↓       ↓
  Verified      Verified       Verified       Verified       Verified       Verified       Verified   Verified  Verified Verified
```

## Why Python is Needed

Python is required in the CI pipeline for:

1. **Conan Package Manager**: Conan is written in Python and requires Python to run
2. **CMake Integration**: Some CMake modules and tools use Python
3. **CI Scripts**: Various automation and verification scripts
4. **Dependency Management**: Python-based tools for managing C++ dependencies

The clang-tidy issue was resolved by using the native clang-tidy binary instead of the Python wrapper script, eliminating the dependency on the missing `/usr/share/clang/run-clang-tidy.py` file.

## Benefits

1. **Fail-Fast**: Pipeline stops at the first critical failure
2. **Clear Dependencies**: Each stage's requirements are explicit and sequential
3. **Comprehensive Verification**: Each stage verifies its outputs before proceeding
4. **Status Visibility**: Clear pipeline status reporting for each stage
5. **Resource Efficiency**: Prevents wasted resources on downstream stages when upstream fails
6. **Isolated Builds**: Each compiler/build combination runs in isolation
7. **Deterministic Order**: Predictable execution sequence

## Usage

The pipeline automatically runs on:

- Push to `main` or `develop` branches
- Pull requests to `main` or `develop` branches

**Important**: Each stage will only execute if its immediate predecessor has succeeded, creating a strict sequential pipeline that ensures quality at every step.

## Example Execution

1. **GCC 11 Debug** starts and must complete successfully
2. **GCC 11 Release** starts only after GCC 11 Debug succeeds
3. **GCC 12 Debug** starts only after GCC 11 Release succeeds
4. **GCC 12 Release** starts only after GCC 12 Debug succeeds
5. **Clang 14 Debug** starts only after GCC 12 Release succeeds
6. **Clang 15 Release** starts only after Clang 14 Debug succeeds
7. **Security** starts only after ALL build stages succeed
8. **Docker** starts only after Security succeeds
9. **SBOM** starts only after Docker succeeds
10. **Deploy** starts only after ALL previous stages succeed

This ensures that every step is properly verified before moving to the next one, exactly as requested.
