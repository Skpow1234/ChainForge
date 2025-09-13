# ChainForge Docker Build Script for Windows
Write-Host "🐳 Building ChainForge with Docker..." -ForegroundColor Green

# Function to clean up
function Cleanup {
    Write-Host "🧹 Cleaning up..." -ForegroundColor Yellow
    docker compose down -v 2>$null
}

# Set up cleanup on script exit
$ErrorActionPreference = "Stop"
trap { Cleanup }

try {
    # Build the Docker images
    Write-Host "🏗️  Building Docker images..." -ForegroundColor Blue
    docker compose build --no-cache

    # Start infrastructure services
    Write-Host "🚀 Starting infrastructure services..." -ForegroundColor Blue
    docker compose up -d postgres redis prometheus grafana jaeger

    # Wait for services to be healthy
    Write-Host "⏳ Waiting for services to be ready..." -ForegroundColor Yellow
    Start-Sleep -Seconds 30

    # Test the build
    Write-Host "🧪 Testing build..." -ForegroundColor Blue
    docker compose run --rm chainforge-tools powershell.exe -Command "
        Write-Host 'Testing Conan install...' -ForegroundColor Blue
        conan profile detect --force
        conan install . --output-folder=build --build=missing

        Write-Host 'Testing CMake configure...' -ForegroundColor Blue
        Set-Location build
        cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

        Write-Host 'Testing CMake build...' -ForegroundColor Blue
        cmake --build . --parallel

        Write-Host 'Testing binary execution...' -ForegroundColor Blue
        .\bin\cppchain-core-test.exe
    "

    Write-Host "✅ Build and test completed successfully!" -ForegroundColor Green
}
catch {
    Write-Host "❌ Build or test failed: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

Write-Host "🎉 ChainForge build verification complete!" -ForegroundColor Green
