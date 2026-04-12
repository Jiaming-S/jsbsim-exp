#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
PROJECT_ROOT=$(realpath "$SCRIPT_DIR/..")
BUILD_DIR="$PROJECT_ROOT/build"
BINARY_NAME="jsbsimexp"

if [ -d "$BUILD_DIR" ]; then
  echo "Rebuilding..."
  cmake --build "$BUILD_DIR" -- -j$(nproc 2>/dev/null || echo 16)

  "$BUILD_DIR/bin/$BINARY_NAME"
else
  echo "Run build.sh first"
  exit 1
fi
