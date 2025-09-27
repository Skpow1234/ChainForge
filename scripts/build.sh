#!/bin/bash

# ChainForge Build Script
# This script builds the project using CMake and Conan

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if required tools are installed
check_requirements() {
    print_status "Checking build requirements..."
    
    if ! command -v cmake &> /dev/null; then
        print_error "CMake is not installed. Please install CMake 3.20 or later."
        exit 1
    fi
    
    if ! command -v conan &> /dev/null; then
        print_error "Conan is not installed. Please install Conan 2.0 or later."
        exit 1
    fi
    
    if ! command -v ninja &> /dev/null; then
        print_warning "Ninja is not installed. Using Make instead."
        USE_NINJA=false
    else
        USE_NINJA=true
    fi
    
    print_success "Build requirements satisfied"
}

# Install Conan dependencies
install_dependencies() {
    print_status "Installing Conan dependencies..."
    
    # Create build directory
    mkdir -p build
    
    # Install dependencies
    conan install . --output-folder=build --build=missing
    
    print_success "Dependencies installed successfully"
}

# Configure the project
configure_project() {
    local build_type=${1:-Release}
    local build_dir=${2:-build}
    
    print_status "Configuring project (${build_type}) in ${build_dir}..."
    
    cd ${build_dir}
    
    if [ "$USE_NINJA" = true ]; then
        cmake .. \
            -DCMAKE_BUILD_TYPE=${build_type} \
            -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            -DTREAT_WARNINGS_AS_ERRORS=OFF \
            -G Ninja
    else
        cmake .. \
            -DCMAKE_BUILD_TYPE=${build_type} \
            -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            -DTREAT_WARNINGS_AS_ERRORS=OFF
    fi
    
    cd ..
    print_success "Project configured successfully"
}

# Build the project
build_project() {
    local build_dir=${1:-build}
    
    print_status "Building project in ${build_dir}..."
    
    cd ${build_dir}
    
    if [ "$USE_NINJA" = true ]; then
        ninja
    else
        make -j$(nproc)
    fi
    
    cd ..
    print_success "Project built successfully"
}

# Run tests
run_tests() {
    local build_dir=${1:-build}
    
    print_status "Running tests in ${build_dir}..."
    
    cd ${build_dir}
    ctest --output-on-failure --parallel
    cd ..
    
    print_success "Tests completed successfully"
}

# Install the project
install_project() {
    local build_dir=${1:-build}
    local install_prefix=${2:-/usr/local}
    
    print_status "Installing project to ${install_prefix}..."
    
    cd ${build_dir}
    
    if [ "$USE_NINJA" = true ]; then
        ninja install
    else
        make install
    fi
    
    cd ..
    print_success "Project installed successfully"
}

# Clean build directory
clean_build() {
    local build_dir=${1:-build}
    
    print_status "Cleaning build directory ${build_dir}..."
    rm -rf ${build_dir}
    print_success "Build directory cleaned"
}

# Main function
main() {
    local build_type="Release"
    local build_dir="build"
    local install_prefix="/usr/local"
    local run_tests_flag=false
    local install_flag=false
    local clean_flag=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --debug)
                build_type="Debug"
                shift
                ;;
            --release)
                build_type="Release"
                shift
                ;;
            --build-dir)
                build_dir="$2"
                shift 2
                ;;
            --install-prefix)
                install_prefix="$2"
                shift 2
                ;;
            --test)
                run_tests_flag=true
                shift
                ;;
            --install)
                install_flag=true
                shift
                ;;
            --clean)
                clean_flag=true
                shift
                ;;
            --help)
                echo "Usage: $0 [OPTIONS]"
                echo "Options:"
                echo "  --debug           Build in Debug mode"
                echo "  --release         Build in Release mode (default)"
                echo "  --build-dir DIR   Specify build directory (default: build)"
                echo "  --install-prefix  Specify install prefix (default: /usr/local)"
                echo "  --test            Run tests after building"
                echo "  --install         Install after building"
                echo "  --clean           Clean build directory before building"
                echo "  --help            Show this help message"
                exit 0
                ;;
            *)
                print_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    print_status "Starting ChainForge build process..."
    print_status "Build type: ${build_type}"
    print_status "Build directory: ${build_dir}"
    
    # Check requirements
    check_requirements
    
    # Clean if requested
    if [ "$clean_flag" = true ]; then
        clean_build "$build_dir"
    fi
    
    # Install dependencies
    install_dependencies
    
    # Configure project
    configure_project "$build_type" "$build_dir"
    
    # Build project
    build_project "$build_dir"
    
    # Run tests if requested
    if [ "$run_tests_flag" = true ]; then
        run_tests "$build_dir"
    fi
    
    # Install if requested
    if [ "$install_flag" = true ]; then
        install_project "$build_dir" "$install_prefix"
    fi
    
    print_success "Build process completed successfully!"
}

# Run main function with all arguments
main "$@"
