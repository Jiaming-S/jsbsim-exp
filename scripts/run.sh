#!/bin/bash

SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
PROJECT_ROOT=$(realpath "$SCRIPT_DIR/..")
BUILD_DIR="$PROJECT_ROOT/build"
BINARY_NAME="jsbsimexp"

if [ -d "$BUILD_DIR" ]; then
  echo "Rebuilding..."
  cmake --build "$BUILD_DIR"

  "$BUILD_DIR/Debug/bin/$BINARY_NAME"
else
  echo "Run build.sh first"
  exit 1
fi
