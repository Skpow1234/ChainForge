#Requires -Version 5.1
<#
.SYNOPSIS
  Run Tokiwa CI checks locally via Docker (Ubuntu 22.04), matching GitHub Actions.

.EXAMPLE
  .\scripts\ci-local.ps1
  .\scripts\ci-local.ps1 -Compiler clang-15 -BuildType Release
  .\scripts\ci-local.ps1 -Sanitize
  .\scripts\ci-local.ps1 -All
#>
param(
    [ValidateSet('gcc-12', 'clang-15')]
    [string]$Compiler = 'gcc-12',

    [ValidateSet('Debug', 'Release')]
    [string]$BuildType = 'Release',

    [switch]$Sanitize,
    [switch]$All
)

$ErrorActionPreference = 'Stop'
$Root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path

if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
    Write-Error 'Docker is required. Install Docker Desktop and ensure it is running.'
}

$ConanCache = Join-Path $env:USERPROFILE '.conan2-tokiwa-ci'
New-Item -ItemType Directory -Force -Path $ConanCache | Out-Null

$scriptArgs = @()
if ($All) {
    $scriptArgs += '--all'
} elseif ($Sanitize) {
    $scriptArgs += '--sanitize'
} else {
    $scriptArgs += @('--compiler', $Compiler, '--build-type', $BuildType)
}

# Ensure LF endings on the shell script (Windows checkouts are often CRLF).
$shPath = Join-Path $Root 'scripts\ci-local.sh'
$shText = [IO.File]::ReadAllText($shPath) -replace "`r`n", "`n" -replace "`r", "`n"
$utf8 = New-Object System.Text.UTF8Encoding $false
[IO.File]::WriteAllText($shPath, $shText, $utf8)

$entrypoint = 'bash /src/scripts/ci-local.sh ' + ($scriptArgs -join ' ')

Write-Host "Repo: $Root"
Write-Host "Conan cache: $ConanCache"
Write-Host "Args: $($scriptArgs -join ' ')"
Write-Host "Starting ubuntu:22.04 (first run installs apt + Conan; can take a while)..."

docker run --rm `
    -v "${Root}:/src" `
    -v "${ConanCache}:/root/.conan2" `
    -w /src `
    -e CONAN_VERSION=2.1.0 `
    ubuntu:22.04 `
    bash -lc $entrypoint

if ($LASTEXITCODE -ne 0) {
    Write-Error "Local CI failed with exit code $LASTEXITCODE"
}
Write-Host "Local CI finished successfully."
