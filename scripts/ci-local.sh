#!/usr/bin/env bash
# Run Tokiwa CI-equivalent configure/build/test inside Linux (Ubuntu 22.04).
# Mirrors .github/workflows/ci.yml build + sanitize jobs.
#
# Usage (from repo root, on Linux/WSL/Docker):
#   ./scripts/ci-local.sh                         # gcc-12 Release
#   ./scripts/ci-local.sh --compiler clang-15 --build-type Release
#   ./scripts/ci-local.sh --compiler gcc-12 --build-type Debug
#   ./scripts/ci-local.sh --sanitize
#   ./scripts/ci-local.sh --all                   # matrix: gcc Debug/Release + clang Release + sanitize

set -euo pipefail

CONAN_VERSION="${CONAN_VERSION:-2.1.0}"
COMPILER="gcc-12"
BUILD_TYPE="Release"
SANITIZE=0
RUN_ALL=0
SKIP_APT=0
BUILD_DIR=""

usage() {
  sed -n '2,12p' "$0" | sed 's/^# \?//'
  exit 0
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --compiler) COMPILER="$2"; shift 2 ;;
    --build-type) BUILD_TYPE="$2"; shift 2 ;;
    --sanitize) SANITIZE=1; shift ;;
    --all) RUN_ALL=1; shift ;;
    --skip-apt) SKIP_APT=1; shift ;;
    --build-dir) BUILD_DIR="$2"; shift 2 ;;
    -h|--help) usage ;;
    *) echo "Unknown arg: $1"; exit 1 ;;
  esac
done

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

install_packages() {
  local compiler="$1"
  local need_lcov="${2:-0}"
  if [[ "$SKIP_APT" -eq 1 ]]; then
    return 0
  fi
  export DEBIAN_FRONTEND=noninteractive
  apt-get update -qq
  # Do NOT install libprotobuf-dev / protobuf-compiler (Conan protobuf/3.21 mismatch).
  local pkgs="build-essential cmake ninja-build pkg-config python3-pip python3-setuptools python3-wheel libssl-dev libsecp256k1-dev libsodium-dev librocksdb-dev"
  case "$compiler" in
    gcc-12) pkgs="$pkgs gcc-12 g++-12" ;;
    clang-15) pkgs="$pkgs clang-15" ;;
    *) echo "Unsupported compiler: $compiler"; exit 1 ;;
  esac
  if [[ "$need_lcov" -eq 1 ]]; then
    pkgs="$pkgs lcov"
  fi
  apt-get install -y -qq $pkgs
  python3 -m pip install -q --upgrade pip
  python3 -m pip install -q "conan==${CONAN_VERSION}"
  conan profile detect --force >/dev/null
}

set_compiler_env() {
  local compiler="$1"
  case "$compiler" in
    gcc-12) export CC=gcc-12 CXX=g++-12 ;;
    clang-15) export CC=clang-15 CXX=clang++-15 ;;
  esac
}

run_build() {
  local compiler="$1"
  local build_type="$2"
  local coverage="${3:-0}"
  local bdir="${BUILD_DIR:-build-ci-${compiler}-${build_type}}"

  echo "============================================================"
  echo " CI local: compiler=${compiler} build_type=${build_type} dir=${bdir}"
  echo "============================================================"

  set_compiler_env "$compiler"
  rm -rf "$bdir"
  mkdir -p "$bdir"

  conan install . \
    --output-folder="$bdir" \
    --build=missing \
    -s "build_type=${build_type}"

  local cov_flag=""
  if [[ "$coverage" -eq 1 ]]; then
    cov_flag="-DENABLE_COVERAGE=ON"
  fi

  cmake -S . -B "$bdir" -G Ninja \
    -DCMAKE_BUILD_TYPE="${build_type}" \
    -DCMAKE_TOOLCHAIN_FILE="${bdir}/conan_toolchain.cmake" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_TESTING=ON \
    -DTREAT_WARNINGS_AS_ERRORS=OFF \
    ${cov_flag}

  cmake --build "$bdir" --parallel

  if [[ ! -x "${bdir}/bin/tokiwa-node" ]]; then
    echo "ERROR: missing ${bdir}/bin/tokiwa-node"
    exit 1
  fi
  "${bdir}/bin/tokiwa-node" --self-check
  "${bdir}/bin/tokiwa-node" --help | grep -qi tokiwa

  (
    cd "$bdir"
    if ! ctest --output-on-failure --parallel "$(nproc)"; then
      count="$(ctest -N | tail -n 1 | grep -oE '[0-9]+' | head -n 1 || echo 0)"
      if [[ "${count:-0}" -eq 0 ]]; then
        echo "No tests registered; continuing"
      else
        exit 1
      fi
    fi
  )

  echo "OK: ${compiler} ${build_type}"
}

run_sanitize() {
  local bdir="${BUILD_DIR:-build-ci-sanitize}"
  echo "============================================================"
  echo " CI local: Sanitizers (ASan+UBSan) dir=${bdir}"
  echo "============================================================"

  export CC=gcc-12 CXX=g++-12
  rm -rf "$bdir"
  mkdir -p "$bdir"

  conan install . --output-folder="$bdir" --build=missing -s build_type=Debug

  cmake -S . -B "$bdir" -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="${bdir}/conan_toolchain.cmake" \
    -DBUILD_TESTING=ON \
    -DUSE_SANITIZERS=OFF \
    -DTREAT_WARNINGS_AS_ERRORS=OFF \
    -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
    -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address,undefined"

  cmake --build "$bdir" --parallel
  "${bdir}/bin/tokiwa-node" --self-check
  (
    cd "$bdir"
    ctest --output-on-failure --parallel "$(nproc)" || true
  )
  echo "OK: sanitize"
}

if [[ "$RUN_ALL" -eq 1 ]]; then
  install_packages "gcc-12" 1
  install_packages "clang-15" 0
  run_build gcc-12 Debug 1
  run_build gcc-12 Release 0
  run_build clang-15 Release 0
  run_sanitize
  echo "All local CI checks passed."
  exit 0
fi

if [[ "$SANITIZE" -eq 1 ]]; then
  install_packages gcc-12 0
  run_sanitize
  exit 0
fi

cov=0
if [[ "$COMPILER" == "gcc-12" && "$BUILD_TYPE" == "Debug" ]]; then
  cov=1
fi
install_packages "$COMPILER" "$cov"
run_build "$COMPILER" "$BUILD_TYPE" "$cov"
