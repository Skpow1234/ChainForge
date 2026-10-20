# ChainForge Build System

This document describes how to build ChainForge using the CMake + Conan v2 build system.

## Prerequisites

### Required Tools

- **CMake**: Version 3.20 or later
- **Conan**: Version 2.0 or later
- **Compiler**: GCC 11+, Clang 14+, or MSVC 2019+
- **Build System**: Ninja (recommended) or Make

### Installing Prerequisites

#### Ubuntu/Debian

```bash
# Install CMake
sudo apt update
sudo apt install cmake ninja-build

# Install Conan
pip install conan

# Install build tools
sudo apt install build-essential pkg-config
```

#### macOS

```bash
# Install CMake and Ninja
brew install cmake ninja

# Install Conan
pip install conan

# Install Xcode Command Line Tools
xcode-select --install
```

#### Windows

```bash
# Install CMake
# Download from https://cmake.org/download/

# Install Conan
pip install conan

# Install Visual Studio Build Tools or Visual Studio Community
# Download from https://visualstudio.microsoft.com/downloads/

# Install Ninja (optional)
# Download from https://github.com/ninja-build/ninja/releases
```

## Quick Start

### 1. Clone the Repository

```bash
git clone <repository-url>
cd ChainForge
```

### 2. Build the Project

#### Using the Build Script (Recommended)

```bash
# Build in Release mode
./build.sh --release

# Build in Debug mode with tests
./build.sh --debug --test

# Build and install
./build.sh --release --install

# Clean build and rebuild
./build.sh --clean --release --test
```

#### Using CMake Directly

```bash
# Create build directory
mkdir build && cd build

# Install dependencies
conan install .. --output-folder=. --build=missing

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake

# Build
cmake --build . --parallel

# Run tests
ctest --output-on-failure --parallel
```

### 3. Build Options

The build system supports several configuration options:

- **BUILD_TESTING**: Enable/disable tests (default: ON)
- **BUILD_TOOLS**: Enable/disable tools (default: ON)
- **BUILD_EXAMPLES**: Enable/disable examples (default: OFF)
- **USE_SANITIZERS**: Enable sanitizers for Debug builds (default: OFF)

```bash
# Configure with custom options
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
    -DBUILD_TESTING=ON \
    -DBUILD_TOOLS=ON \
    -DUSE_SANITIZERS=ON
```

## Build Configurations

### Release Build

```bash
./build.sh --release
```

- Optimized for performance
- No debug symbols
- No sanitizers

### Debug Build

```bash
./build.sh --debug
```

- Includes debug symbols
- Optional sanitizers
- Slower execution

### Debug with Sanitizers

```bash
./build.sh --debug
# Then configure with sanitizers
cmake .. -DUSE_SANITIZERS=ON
```

- Address Sanitizer (ASAN)
- Undefined Behavior Sanitizer (UBSAN)
- Thread Sanitizer (TSAN)

## Project Structure

```bash
ChainForge/
├── CMakeLists.txt              # Main CMake configuration
├── CMakePresets.json           # CMake presets for different configurations
├── conanfile.txt               # Conan dependencies
├── cmake/                      # CMake modules
│   ├── CompilerFlags.cmake     # Compiler flags configuration
│   ├── ConanSetup.cmake        # Conan integration
│   └── ChainForgeConfig.cmake.in # Package configuration template
├── modules/                    # Source modules
│   ├── core/                   # Core domain models
│   ├── crypto/                 # Cryptographic primitives
│   ├── storage/                # Storage abstraction
│   └── ...                     # Other modules
├── tests/                      # Test suite
├── tools/                      # Build tools
└── build/                      # Build output directory
```

## Dependencies

ChainForge uses Conan v2 for dependency management. Key dependencies include:

- **Core Libraries**: OpenSSL, Boost, Protobuf, FlatBuffers
- **Logging**: spdlog, fmt
- **Database**: RocksDB
- **Networking**: Asio, libp2p
- **Crypto**: libsecp256k1, libsodium, BLST
- **Testing**: Google Test, Catch2
- **Observability**: Prometheus, OpenTelemetry

### Adding New Dependencies

1. Add the dependency to `conanfile.txt`:

```txt
[requires]
new_library/1.0.0

[generators]
CMakeDeps
CMakeToolchain
```

2.Update the module's `CMakeLists.txt`:

```cmake
find_conan_package(new_library)
target_link_libraries(module_name PUBLIC new_library::new_library)
```

## Cross-Platform Building

### Linux

```bash
# GCC
export CC=gcc-11
export CXX=g++-11
./build.sh --release

# Clang
export CC=clang-14
export CXX=clang++-14
./build.sh --release
```

### macOS cross

```bash
# Using Homebrew Clang
export CC=clang
export CXX=clang++
./build.sh --release
```

### Windows cross

```bash
# Using Visual Studio
build.bat --release

# Using MinGW
export CC=gcc
export CXX=g++
./build.sh --release
```

## CI/CD Integration

The project includes GitHub Actions workflows for:

- **Build Matrix**: Multiple compilers and configurations
- **Security**: Sanitizer builds and fuzzing
- **Docker**: Container image building and testing
- **SBOM**: Software Bill of Materials generation
- **Deployment**: Automated deployment to staging

### Local CI Testing

```bash
# Test the full CI pipeline locally
./build.sh --clean --release --test

# Run with sanitizers
./build.sh --clean --debug --test
cmake .. -DUSE_SANITIZERS=ON
```

## Troubleshooting

### Common Issues

#### Conan Dependencies Not Found

```bash
# Clean and reinstall dependencies
rm -rf build
conan install . --output-folder=build --build=missing
```

#### Compiler Not Found

```bash
# Check available compilers
conan profile list
conan profile show

# Create new profile
conan profile detect --force
```

#### Build Failures

```bash
# Check build logs
tail -f build/CMakeFiles/CMakeOutput.log

# Verify dependencies
conan list
conan inspect <package>
```

#### Test Failures

```bash
# Run tests with verbose output
cd build
ctest --output-on-failure --verbose

# Run specific test
./bin/core_tests --gtest_filter=HashTest.*
```

### Getting Help

1. Check the build logs in the `build/` directory
2. Verify all prerequisites are installed
3. Check the [ChainForge documentation](README.md)
4. Open an issue on GitHub with build logs

## Performance Optimization

### Build Performance

```bash
# Use Ninja for faster builds
./build.sh --release

# Parallel builds
cmake --build . --parallel $(nproc)

# Use ccache for incremental builds
export CCACHE_DIR=/path/to/ccache
cmake .. -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
```

### Runtime Performance

```bash
# Release build with optimizations
./build.sh --release

# Enable LTO (Link Time Optimization)
cmake .. -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON

# Profile-guided optimization
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fprofile-generate"
# Run tests, then rebuild with:
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-fprofile-use"
```

## Next Steps

After successfully building ChainForge:

1. **Run Tests**: Verify everything works with `./build.sh --test`
2. **Explore Modules**: Check out the different modules in `modules/`
3. **Run Examples**: Build and run example applications
4. **Contribute**: Check the [Contributing Guide](CONTRIBUTING.md)

For more information, see the main [README.md](README.md) and [ROADMAP.md](documentation/ROADMAP.md).
