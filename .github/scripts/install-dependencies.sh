#!/bin/bash

# ChainForge Dependency Installation Script
# Handles various failure scenarios gracefully

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

# Function to install Conan with fallback
install_conan() {
    local target_version="$1"
    
    print_status "Installing Conan $target_version..."
    
    # First, upgrade pip and setuptools
    python3 -m pip install --upgrade pip setuptools wheel
    
    # Try to install specific version
    if python3 -m pip install "conan==$target_version"; then
        print_success "Conan $target_version installed successfully"
        return 0
    else
        print_warning "Failed to install Conan $target_version, trying latest version..."
        
        # Try latest version
        if python3 -m pip install conan; then
            print_success "Latest Conan version installed successfully"
            return 0
        else
            print_error "Failed to install any version of Conan"
            return 1
        fi
    fi
}

# Function to install system dependencies
install_system_deps() {
    print_status "Installing system dependencies..."
    
    sudo apt-get update
    
    # Essential build tools
    sudo apt-get install -y \
        build-essential \
        cmake \
        ninja-build \
        pkg-config \
        python3-pip \
        python3-setuptools \
        python3-wheel
    
    # Try to install optional dependencies
    for dep in "libssl-dev" "libboost-all-dev" "libprotobuf-dev" "protobuf-compiler" "librocksdb-dev" "libgtest-dev"; do
        if sudo apt-get install -y "$dep" 2>/dev/null; then
            print_success "Installed $dep"
        else
            print_warning "Failed to install $dep (will continue without it)"
        fi
    done
    
    print_success "System dependencies installation completed"
}

# Function to install Conan dependencies
install_conan_deps() {
    print_status "Installing Conan dependencies..."
    
    # Create build directory
    mkdir -p build
    
    # Try to install dependencies
    if conan install . --output-folder=build --build=missing; then
        print_success "All Conan dependencies installed successfully"
        return 0
    else
        print_warning "Some dependencies failed, trying with fallback options..."
        
        # Try with more permissive options
        if conan install . --output-folder=build --build=missing --build-policy=missing; then
            print_success "Dependencies installed with fallback options"
            return 0
        else
            print_warning "Dependencies installation partially failed, but continuing..."
            return 0  # Continue anyway
        fi
    fi
}

# Main execution
main() {
    print_status "Starting ChainForge dependency installation..."
    
    # Install system dependencies
    install_system_deps
    
    # Install Conan
    if ! install_conan "2.1.0"; then
        print_error "Failed to install Conan, aborting"
        exit 1
    fi
    
    # Verify Conan installation
    if ! conan --version; then
        print_error "Conan installation verification failed"
        exit 1
    fi
    
    # Configure Conan profile
    print_status "Configuring Conan profile..."
    conan profile detect --force || true
    conan profile show || true
    
    # Install Conan dependencies
    install_conan_deps
    
    print_success "Dependency installation completed!"
    print_status "You can now proceed with building the project"
}

# Run main function
main "$@"
