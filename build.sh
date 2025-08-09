#!/usr/bin/env bash

# Universal build script
# Usage: ./build.sh [clean] [debug|release]
#
# - Scans current directory recursively for source files.
# - Compiles C/C++ via gcc/g++.
# - Assembles ASM via nasm + ld (x86_64 ELF).
# - Compiles Java via javac.
# - Copies PHP/HTML and other static web assets unchanged.
# - Places all outputs under build/<mode>/.
#
# Requirements: gcc, g++, nasm, ld, javac must be in PATH.

set -euo pipefail

MODE="release"
ACTION="build"

# Parse CLI arguments
for arg in "$@"; do
  case "${arg,,}" in
    clean)
      ACTION="clean"
      ;;
    debug)
      MODE="debug"
      ;;
    release)
      MODE="release"
      ;;
    *)
      echo "[WARN] Ignoring unknown argument: $arg" >&2
      ;;
  esac
done

BUILD_DIR="build/${MODE}"

if [[ "$ACTION" == "clean" ]]; then
  echo "[INFO] Cleaning build artifacts…"
  rm -rf build
  exit 0
fi

mkdir -p "$BUILD_DIR"

CFLAGS="-Wall"
CXXFLAGS="-Wall"
ASMFLAGS="-f elf64"

if [[ "$MODE" == "debug" ]]; then
  CFLAGS+=" -g -O0"
  CXXFLAGS+=" -g -O0"
else
  CFLAGS+=" -O2"
  CXXFLAGS+=" -O2"
fi

# Recursively find files; process each by extension
while IFS= read -r -d '' file; do
  ext="${file##*.}"
  base="$(basename "$file")"
  name="${base%.*}"

  case "$ext" in
    c)
      echo "[C]     $file -> $BUILD_DIR/$name"
      gcc $CFLAGS "$file" -o "$BUILD_DIR/$name"
      ;;
    cpp|cc|cxx|CPP|C)
      echo "[C++]   $file -> $BUILD_DIR/$name"
      g++ $CXXFLAGS "$file" -o "$BUILD_DIR/$name"
      ;;
    asm|s)
      echo "[ASM]   $file -> $BUILD_DIR/$name"
      nasm $ASMFLAGS "$file" -o "$BUILD_DIR/$name.o"
      ld "$BUILD_DIR/$name.o" -o "$BUILD_DIR/$name"
      ;;
    java)
      echo "[Java]  $file -> $BUILD_DIR/"
      # Preserve package structure inside build directory
      javac -d "$BUILD_DIR" "$file"
      ;;
    php|html|htm|css|js)
      dest_dir="$BUILD_DIR/static/$(dirname "${file#./}")"
      mkdir -p "$dest_dir"
      echo "[Static] $file -> $dest_dir/"
      cp "$file" "$dest_dir/"
      ;;
    *)
      # Ignore unhandled extensions
      ;;
  esac

done < <(find . -type f -print0)

echo "[DONE] Build artifacts are located in $BUILD_DIR"