#!/bin/bash

# Simple Docker Build Test for ChainForge
# This script tests if the basic Docker build works

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
    
    # Build with no cache to ensure clean build
    if docker build --no-cache -f "$dockerfile" . --tag "test-$service_name"; then
        print_success "✓ Docker build for $service_name completed successfully"
        return 0
    else
        print_error "✗ Docker build for $service_name failed"
        return 1
    fi
}

# Function to clean up test images
cleanup_test_images() {
    print_status "Cleaning up test images..."
    docker rmi test-chainforge-node test-chainforge-explorer 2>/dev/null || true
    print_success "Cleanup completed"
}

# Main execution
main() {
    print_status "Starting simple Docker build tests for ChainForge..."
    
    # Check if Docker is running
    if ! docker info >/dev/null 2>&1; then
        print_error "Docker is not running. Please start Docker and try again."
        exit 1
    fi
    
    local build_success=true
    
    # Test node build first (most important)
    print_status "Testing node build (this may take a while)..."
    if ! test_docker_build "deploy/docker/Dockerfile.node" "chainforge-node"; then
        build_success=false
        print_error "Node build failed. This is critical."
    fi
    
    # Test explorer build
    print_status "Testing explorer build..."
    if ! test_docker_build "deploy/docker/Dockerfile.explorer" "chainforge-explorer"; then
        build_success=false
        print_warning "Explorer build failed, but this is less critical."
    fi
    
    # Clean up test images
    cleanup_test_images
    
    # Report results
    if [ "$build_success" = true ]; then
        print_success "All critical Docker builds completed successfully!"
        print_status "You can now run: docker-compose up --build"
        print_status "Or test individual services:"
        print_status "  docker-compose up --build postgres redis prometheus grafana"
        print_status "  docker-compose up --build chainforge-node"
    else
        print_error "Some Docker builds failed. Check the output above for details."
        print_status "Common issues:"
        print_status "  1. Missing packages in Ubuntu repositories"
        print_status "  2. Conan dependency issues"
        print_status "  3. Build toolchain problems"
        exit 1
    fi
}

# Run main function
main "$@"
