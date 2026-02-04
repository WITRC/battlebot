#!/bin/bash
set -e

cd "$(dirname "$0")/.."

# Build tests
echo "Building tests..."
rm -rf build_tests
mkdir -p build_tests
cd build_tests
cmake ../tests
make -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 2)

# Run tests
echo ""
echo "Running tests..."
./motor_tests
