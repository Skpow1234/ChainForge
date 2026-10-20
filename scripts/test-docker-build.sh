#!/bin/bash

# Test Docker Build Script for Tokiwa
# This script tests the Docker build process

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

# Function to test Docker build
test_docker_build() {
    local dockerfile="$1"
    local service_name="$2"
    
    print_status "Testing Docker build for $service_name..."
    
    if docker build -f "$dockerfile" . --tag "test-$service_name"; then
        print_success "Docker build for $service_name completed successfully"
        return 0
    else
        print_error "Docker build for $service_name failed"
        return 1
    fi
}

# Function to clean up test images
cleanup_test_images() {
    print_status "Cleaning up test images..."
    docker rmi test-tokiwa-node test-tokiwa-explorer test-tokiwa-tools 2>/dev/null || true
    print_success "Cleanup completed"
}

# Main execution
main() {
    print_status "Starting Docker build tests for Tokiwa..."
    
    # Test each Dockerfile
    local build_success=true
    
    if ! test_docker_build "deploy/docker/Dockerfile.node" "tokiwa-node"; then
        build_success=false
    fi
    
    if ! test_docker_build "deploy/docker/Dockerfile.explorer" "tokiwa-explorer"; then
        build_success=false
    fi
    
    if ! test_docker_build "deploy/docker/Dockerfile.tools" "tokiwa-tools"; then
        build_success=false
    fi
    
    # Clean up test images
    cleanup_test_images
    
    # Report results
    if [ "$build_success" = true ]; then
        print_success "All Docker builds completed successfully!"
        print_status "You can now run: docker-compose up --build"
    else
        print_error "Some Docker builds failed. Check the output above for details."
        exit 1
    fi
}

# Run main function
main "$@"
