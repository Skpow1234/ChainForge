# ChainForge CI Pipeline Fixes

## Issue Identified

The CI pipeline was failing with the error:

```bash
CMake Error: Could not find toolchain file: conan_toolchain.cmake
```

## Root Cause

The problem was in the **order of operations**:

1. **Before**: The pipeline was trying to run `cmake` immediately after creating build directories
2. **Problem**: `conan_toolchain.cmake` hadn't been generated yet because `conan install` wasn't run first
3. **Result**: CMake couldn't find the required toolchain file

## Fixes Applied

### 1. Proper Conan Setup Order

**Before (incorrect)**:

```yaml
- name: Build project
  run: |
    mkdir -p build-gcc11-debug
    cd build-gcc11-debug
    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake  # ❌ File doesn't exist yet
```

**After (correct)**:

```yaml
- name: Generate Conan toolchain
  run: |
    conan install . --output-folder=build-gcc11-debug --build=missing  # ✅ Generate first

- name: Build project
  run: |
    cd build-gcc11-debug
    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake  # ✅ File exists now
```

### 2. Dependency Installation Script Update

**Modified**: `.github/scripts/install-dependencies.sh`

**Before**: Script automatically ran `conan install` which could interfere with CI pipeline
**After**: Script only installs system dependencies and Conan, but doesn't run `conan install`

**Reason**: Each CI stage needs to run `conan install` in its own build directory with specific compiler settings

### 3. Build Directory Isolation

Each build stage now:

- Creates its own build directory (e.g., `build-gcc11-debug`)
- Runs `conan install` in that directory
- Generates the toolchain file locally
- Runs CMake from within that directory

### 4. Enhanced Debugging

Added comprehensive debugging to each Conan step:

```yaml
- name: Generate Conan toolchain
  run: |
    echo "Current directory: $(pwd)"
    echo "Listing current directory:"
    ls -la
    echo "Running conan install..."
    conan install . --output-folder=build-gcc11-debug --build=missing
    echo "Conan install completed. Listing build directory:"
    ls -la build-gcc11-debug/
    echo "Checking for conan_toolchain.cmake:"
    ls -la build-gcc11-debug/conan_toolchain.cmake || echo "conan_toolchain.cmake not found"
```

## Pipeline Flow (Fixed)

```bash
1. Install Dependencies (system + Conan)
2. Setup Compiler (GCC 11, GCC 12, Clang 14, Clang 15)
3. Generate Conan Toolchain (creates conan_toolchain.cmake)
4. Run CMake (finds conan_toolchain.cmake)
5. Build Project
6. Run Tests
7. Verify Artifacts
```

## Why This Fix Works

1. **Proper Order**: Conan generates the toolchain file before CMake tries to use it
2. **Isolated Builds**: Each compiler/build combination has its own Conan setup
3. **Compiler-Specific**: Each stage sets up the correct compiler before running Conan
4. **Debug Visibility**: Clear logging shows exactly what's happening at each step

## Testing the Fix

The pipeline should now:

1. ✅ Successfully generate `conan_toolchain.cmake` in each build directory
2. ✅ Allow CMake to find and use the toolchain file
3. ✅ Build successfully with each compiler configuration
4. ✅ Maintain the strict sequential ordering as requested

## Next Steps

1. **Monitor CI**: Watch the pipeline execution to ensure all stages complete successfully
2. **Verify Builds**: Check that binaries are generated in each `bin/` directory
3. **Test Sequential Flow**: Confirm that each stage waits for the previous one to succeed
4. **Optimize**: Once working, consider removing debug output for cleaner logs

This fix ensures that the CI pipeline follows the exact sequential order you requested while properly handling the Conan dependency management.
