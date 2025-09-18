# ChainForge CI/CD

This directory contains the CI/CD configuration and scripts for ChainForge.

## CI Pipeline Issues & Fixes

### Problem: Conan Installation Failure

The original CI pipeline was failing with:

```bash
error: subprocess-exited-with-error
× Getting requirements to build wheel did not run successfully
│ exit code: 1
```

**Root Cause**: PyYAML 6.0 build incompatibility with Python 3.12 in Ubuntu 22.04+ runners.

### Solution: Robust Dependency Installation

1. **Updated Conan Version**: From 2.0.0 to 2.1.0 (better Python 3.12 compatibility)
2. **Fallback Mechanism**: If specific version fails, try latest version
3. **Graceful Degradation**: Continue build even if some optional dependencies fail
4. **Dependency Script**: Centralized installation logic in `.github/scripts/install-dependencies.sh`

## Files

- **`.github/workflows/ci.yml`**: Main CI pipeline configuration
- **`.github/scripts/install-dependencies.sh`**: Robust dependency installation script
- **`conanfile.txt`**: Minimal dependency list (only essential, well-supported packages)

## How It Works

### 1. Dependency Installation Script

The script handles:

- System package installation with fallbacks
- Conan installation with version fallback
- Graceful handling of failed dependencies
- Clear status reporting

### 2. CI Workflow

- Uses the dependency script for all jobs
- Continues build even if some dependencies fail
- Provides clear error reporting
- Supports multiple compiler configurations

### 3. Minimal Dependencies

Current `conanfile.txt` only includes:

- **Essential**: OpenSSL, Boost, Protobuf, fmt
- **Database**: RocksDB
- **Testing**: Google Test
- **Build**: CMake

## Usage

### Local Development

```bash
# Install dependencies locally
chmod +x .github/scripts/install-dependencies.sh
.github/scripts/install-dependencies.sh

# Build project
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### CI/CD

The pipeline automatically:

1. Runs dependency installation script
2. Sets up compiler environment
3. Builds project with CMake
4. Runs tests
5. Generates SBOM
6. Builds Docker images

## Troubleshooting

### Common Issues

1. **Conan Installation Fails**
   - Script automatically falls back to latest version
   - Check Python version compatibility

2. **Dependencies Missing**
   - Script reports which packages failed
   - Build continues with available dependencies

3. **Build Failures**
   - Check compiler compatibility
   - Verify CMake configuration

### Debug Mode

```bash
# Run with verbose output
.github/scripts/install-dependencies.sh 2>&1 | tee install.log

# Check specific dependency
conan list
conan inspect <package>
```

## Future Improvements

1. **Add More Dependencies**: Gradually add back removed packages as they become compatible
2. **Multi-Platform**: Extend to macOS and Windows runners
3. **Caching**: Implement Conan and build cache for faster CI
4. **Matrix Testing**: Add more compiler and configuration combinations

## Contributing

When adding new dependencies:

1. Test compatibility with Python 3.12
2. Verify availability in Conan Center
3. Add fallback handling in the script
4. Update this documentation
