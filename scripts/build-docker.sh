#!/bin/bash

# ChainForge Docker Build Script
set -e

echo "🐳 Building ChainForge with Docker..."

# Function to clean up
cleanup() {
    echo "🧹 Cleaning up..."
    docker compose down -v 2>/dev/null || true
}

# Set up cleanup on script exit
trap cleanup EXIT

# Build the Docker images
echo "🏗️  Building Docker images..."
docker compose build --no-cache

# Start infrastructure services
echo "🚀 Starting infrastructure services..."
docker compose up -d postgres redis prometheus grafana jaeger

# Wait for services to be healthy
echo "⏳ Waiting for services to be ready..."
sleep 30

# Test the build
echo "🧪 Testing build..."
if docker compose run --rm chainforge-tools /bin/bash -c "
    echo 'Testing Conan install...' &&
    conan profile detect --force &&
    conan install . --output-folder=build --build=missing &&
    echo 'Testing CMake configure...' &&
    cd build &&
    cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release &&
    echo 'Testing CMake build...' &&
    cmake --build . --parallel &&
    echo 'Testing binary execution...' &&
    ./bin/cppchain-core-test
"; then
    echo "✅ Build and test completed successfully!"
else
    echo "❌ Build or test failed!"
    exit 1
fi

echo "🎉 ChainForge build verification complete!"
