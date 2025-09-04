#!/bin/bash

# Test Package Availability Script
# This script tests which packages are available in Ubuntu 22.04

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

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

# Function to test package availability
test_package() {
    local package="$1"
    local description="$2"
    
    print_status "Testing package: $package ($description)"
    
    if apt-cache show "$package" >/dev/null 2>&1; then
        print_success "✓ Package $package is available"
        return 0
    else
        print_error "✗ Package $package is NOT available"
        return 1
    fi
}

# Function to find alternative package
find_alternative() {
    local package="$1"
    local search_term="$2"
    
    print_status "Searching for alternatives to $package..."
    
    local alternatives=$(apt-cache search "$search_term" 2>/dev/null | head -5)
    if [ -n "$alternatives" ]; then
        print_warning "Alternative packages found:"
        echo "$alternatives" | sed 's/^/  /'
    else
        print_error "No alternatives found for $package"
    fi
}

# Main execution
main() {
    print_status "Testing package availability in Ubuntu 22.04..."
    
    # Update package cache
    sudo apt-get update >/dev/null 2>&1
    
    local all_packages_available=true
    
    # Test essential packages
    declare -A packages=(
        ["libstdc++6"]="C++ Standard Library Runtime"
        ["libgcc-s1"]="GCC Support Library"
        ["libssl3"]="OpenSSL 3.x Runtime"
        ["ca-certificates"]="SSL Certificate Authorities"
        ["libboost-system1.74.0"]="Boost System Library"
        ["libboost-filesystem1.74.0"]="Boost Filesystem Library"
        ["libboost-thread1.74.0"]="Boost Thread Library"
        ["libboost-chrono1.74.0"]="Boost Chrono Library"
        ["libboost-atomic1.74.0"]="Boost Atomic Library"
        ["libprotobuf23"]="Protocol Buffers Runtime"
        ["libc6"]="GNU C Library"
    )
    
    # Test each package
    for package in "${!packages[@]}"; do
        if ! test_package "$package" "${packages[$package]}"; then
            all_packages_available=false
            find_alternative "$package" "$package"
        fi
    done
    
    # Test RocksDB specifically
    print_status "Testing RocksDB packages..."
    local rocksdb_found=false
    
    for version in "8.9" "8.8" "8.7" "8.6" "8.5"; do
        if apt-cache show "librocksdb$version" >/dev/null 2>&1; then
            print_success "✓ RocksDB package found: librocksdb$version"
            rocksdb_found=true
            break
        fi
    done
    
    if [ "$rocksdb_found" = false ]; then
        print_warning "No RocksDB runtime package found, will need to copy from build stage"
        find_alternative "rocksdb" "rocksdb"
    fi
    
    # Report results
    if [ "$all_packages_available" = true ] && [ "$rocksdb_found" = true ]; then
        print_success "All packages are available! Docker build should work."
    else
        print_warning "Some packages are missing. Docker build may need adjustments."
    fi
    
    print_status "Package availability test completed."
}

# Run main function
main "$@"
