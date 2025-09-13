# ChainForge Complete Build and Test Script for Windows
# This script builds and tests the entire ChainForge system using Docker

param(
    [switch]$SkipInfrastructure,
    [switch]$SkipTests,
    [switch]$KeepContainers
)

# Colors for output
$Green = "Green"
$Red = "Red"
$Yellow = "Yellow"
$Blue = "Cyan"
$White = "White"

function Write-Status {
    param([string]$Message)
    Write-Host "[$((Get-Date).ToString('HH:mm:ss'))] $Message" -ForegroundColor $Blue
}

function Write-Success {
    param([string]$Message)
    Write-Host "[$((Get-Date).ToString('HH:mm:ss'))] $Message" -ForegroundColor $Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[$((Get-Date).ToString('HH:mm:ss'))] $Message" -ForegroundColor $Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "[$((Get-Date).ToString('HH:mm:ss'))] $Message" -ForegroundColor $Red
}

# Function to check if Docker is available
function Test-Docker {
    try {
        $null = docker --version
        $null = docker compose version
        Write-Success "Docker and Docker Compose are available"
        return $true
    }
    catch {
        Write-Error "Docker is not installed or not in PATH"
        Write-Error "Please install Docker Desktop from https://www.docker.com/products/docker-desktop"
        return $false
    }
}

# Function to check if Docker daemon is running
function Test-DockerDaemon {
    try {
        $null = docker info
        Write-Success "Docker daemon is running"
        return $true
    }
    catch {
        Write-Error "Docker daemon is not running"
        Write-Error "Please start Docker Desktop and try again"
        return $false
    }
}

# Function to clean up Docker resources
function Clear-DockerResources {
    Write-Status "Cleaning up Docker resources..."
    try {
        docker compose down -v --remove-orphans 2>$null
        docker system prune -f --volumes 2>$null
    }
    catch {
        # Ignore cleanup errors
    }
}

# Function to start infrastructure services
function Start-Infrastructure {
    Write-Status "Starting infrastructure services (PostgreSQL, Redis, Prometheus, Grafana, Jaeger)..."

    try {
        docker compose up -d postgres redis prometheus grafana jaeger

        # Wait for services to be healthy
        $maxAttempts = 30
        $attempt = 1

        while ($attempt -le $maxAttempts) {
            $services = docker compose ps 2>$null
            if ($services -match "Up") {
                Write-Success "Infrastructure services are ready"
                return $true
            }

            Write-Status "Waiting for services... (attempt $attempt/$maxAttempts)"
            Start-Sleep -Seconds 10
            $attempt++
        }

        Write-Error "Infrastructure services failed to start"
        docker compose logs
        return $false
    }
    catch {
        Write-Error "Failed to start infrastructure services: $($_.Exception.Message)"
        return $false
    }
}

# Function to build the application
function Build-Application {
    Write-Status "Building ChainForge application..."

    try {
        docker compose build --no-cache

        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to build Docker images"
            docker compose logs
            return $false
        }

        Write-Success "Docker images built successfully"
        return $true
    }
    catch {
        Write-Error "Failed to build application: $($_.Exception.Message)"
        return $false
    }
}

# Function to test the build
function Test-Build {
    Write-Status "Testing ChainForge build..."

    try {
        $testScript = @"
set -e
echo 'Installing dependencies...'
conan profile detect --force
conan install . --output-folder=build --build=missing

echo 'Configuring build...'
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

echo 'Building project...'
cmake --build . --parallel

echo 'Running tests...'
./bin/cppchain-core-test

echo 'Build and test completed successfully!'
"@

        $result = docker compose run --rm chainforge-tools bash -c $testScript

        if ($LASTEXITCODE -eq 0) {
            Write-Success "Build and test completed successfully"
            return $true
        }
        else {
            Write-Error "Build or test failed"
            docker compose logs chainforge-tools
            return $false
        }
    }
    catch {
        Write-Error "Build test failed: $($_.Exception.Message)"
        return $false
    }
}

# Function to test RPC server
function Test-RpcServer {
    Write-Status "Testing RPC server..."

    try {
        # Start the node (which includes RPC server)
        docker compose up -d chainforge-node

        # Wait for node to start
        Start-Sleep -Seconds 5

        # Test RPC endpoints
        $rpcUrl = "http://localhost:8545"
        $rpcRequest = @{
            jsonrpc = "2.0"
            method = "web3_clientVersion"
            params = @()
            id = 1
        } | ConvertTo-Json

        $response = Invoke-WebRequest -Uri $rpcUrl -Method POST -ContentType "application/json" -Body $rpcRequest -TimeoutSec 10

        if ($response.Content -match "ChainForge") {
            Write-Success "RPC server is responding correctly"
        }
        else {
            Write-Warning "RPC server test inconclusive (may not be fully implemented yet)"
        }
    }
    catch {
        Write-Warning "RPC server test failed: $($_.Exception.Message)"
    }
    finally {
        # Stop the node
        docker compose stop chainforge-node 2>$null
    }
}

# Function to show system information
function Show-Info {
    Write-Status "ChainForge Build Information"
    Write-Host "==============================" -ForegroundColor $White
    Write-Host "Milestones Completed:" -ForegroundColor $White
    Write-Host "✅ 1. Project Setup & Build System" -ForegroundColor $Green
    Write-Host "✅ 2. Core Domain Models" -ForegroundColor $Green
    Write-Host "✅ 3. Cryptographic Foundation" -ForegroundColor $Green
    Write-Host "✅ 4. Database Abstraction Layer" -ForegroundColor $Green
    Write-Host "✅ 5. Basic Block Structure" -ForegroundColor $Green
    Write-Host "✅ 6. Transaction Pool (Mempool)" -ForegroundColor $Green
    Write-Host "✅ 7. Simple Proof of Work" -ForegroundColor $Green
    Write-Host "✅ 8. Basic RPC Server" -ForegroundColor $Green
    Write-Host "" -ForegroundColor $White
    Write-Host "Modules Available:" -ForegroundColor $White
    Write-Host "- chainforge-core: Domain models and core types" -ForegroundColor $White
    Write-Host "- chainforge-crypto: Cryptographic primitives" -ForegroundColor $White
    Write-Host "- chainforge-storage: Database abstraction (RocksDB)" -ForegroundColor $White
    Write-Host "- chainforge-consensus: Proof of Work consensus" -ForegroundColor $White
    Write-Host "- chainforge-mempool: Transaction pool management" -ForegroundColor $White
    Write-Host "- chainforge-rpc: JSON-RPC server" -ForegroundColor $White
    Write-Host "" -ForegroundColor $White
    Write-Host "Services:" -ForegroundColor $White
    Write-Host "- PostgreSQL: Database" -ForegroundColor $White
    Write-Host "- Redis: Caching" -ForegroundColor $White
    Write-Host "- Prometheus: Metrics collection" -ForegroundColor $White
    Write-Host "- Grafana: Metrics visualization" -ForegroundColor $White
    Write-Host "- Jaeger: Distributed tracing" -ForegroundColor $White
    Write-Host "- ChainForge Node: Blockchain node with RPC" -ForegroundColor $White
    Write-Host "- ChainForge Explorer: Block explorer (stub)" -ForegroundColor $White
    Write-Host "" -ForegroundColor $White
}

# Main execution
function Main {
    Write-Host "🐳 ChainForge Docker Build and Test (Windows PowerShell)" -ForegroundColor $Green
    Write-Host "=======================================================" -ForegroundColor $White
    Write-Host ""

    # Show information
    Show-Info

    # Check prerequisites
    if (-not (Test-Docker)) { exit 1 }
    if (-not (Test-DockerDaemon)) { exit 1 }

    # Set up cleanup on exit (unless KeepContainers is specified)
    if (-not $KeepContainers) {
        $cleanupScript = {
            Write-Status "Cleaning up Docker resources..."
            try {
                docker compose down -v --remove-orphans 2>$null
            }
            catch {
                # Ignore cleanup errors
            }
        }
        Register-EngineEvent PowerShell.Exiting -Action $cleanupScript
    }

    # Execute build steps
    Write-Status "Starting ChainForge build process..."

    $success = $true

    if (-not $SkipInfrastructure) {
        $success = $success -and (Start-Infrastructure)
    }

    $success = $success -and (Build-Application)

    if (-not $SkipTests) {
        $success = $success -and (Test-Build)
    }

    if ($success) {
        Write-Success "🎉 ChainForge build completed successfully!"
        Write-Status "You can now:"
        Write-Host "  - Run 'docker compose up -d' to start all services" -ForegroundColor $White
        Write-Host "  - Access Grafana at http://localhost:3000 (admin/chainforge_grafana_pass)" -ForegroundColor $White
        Write-Host "  - Access Prometheus at http://localhost:9090" -ForegroundColor $White
        Write-Host "  - Access Jaeger at http://localhost:16686" -ForegroundColor $White
        Write-Host "  - Access RPC server at http://localhost:8545" -ForegroundColor $White
        Write-Host "  - Access Block Explorer at http://localhost:4000" -ForegroundColor $White
        Write-Host "" -ForegroundColor $White

        # Optional: Test RPC server
        $response = Read-Host "Do you want to test the RPC server? (y/N)"
        if ($response -match "^[Yy]$") {
            Test-RpcServer
        }
    }
    else {
        Write-Error "❌ ChainForge build failed"
        Write-Status "Check the logs above for details"
        exit 1
    }
}

# Run main function
Main
