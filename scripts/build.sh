#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
PROJECT_ROOT=$(realpath "$SCRIPT_DIR/..")
BUILD_DIR="$PROJECT_ROOT/build"

cd "$PROJECT_ROOT" || exit

echo "Wiping and rebuilding..."

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" || exit
cmake --build "$BUILD_DIR" -- -j$(nproc 2>/dev/null || echo 16) || exit
