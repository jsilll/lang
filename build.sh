#!/bin/bash

set -e

LLVM_DIR="/usr/lib/llvm-14"
BUILD_DIR="build"
BUILD_TYPE="Debug"
C_COMPILER="clang"
CXX_COMPILER="clang++"
CCACHE="ccache"
GENERATOR="Ninja"

usage() {
    echo "Usage: $0 <command> [options]"
    echo "Commands:"
    echo "  configure  - Configure the project using CMake"
    echo "  clean      - Clean the build directory"
    echo "  build      - Build the project"
    echo "  all        - Configure, clean, and build the project"
    echo "Options:"
    echo "  --llvm-dir <path>    - Set LLVM_DIR (default: $LLVM_DIR)"
    echo "  --build-dir <path>   - Set BUILD_DIR (default: $BUILD_DIR)"
    echo "  --build-type <type>  - Set CMAKE_BUILD_TYPE (default: $BUILD_TYPE)"
    echo "  --c-compiler <path>  - Set CMAKE_C_COMPILER (default: $C_COMPILER)"
    echo "  --cxx-compiler <path>- Set CMAKE_CXX_COMPILER (default: $CXX_COMPILER)"
    echo "  --ccache <path>      - Set CMAKE_CXX_COMPILER_LAUNCHER (default: $CCACHE)"
    echo "  --generator <path>   - Set CMAKE_GENERATOR (default: $GENERATOR)"
    exit 1
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --llvm-dir)
            LLVM_DIR="$2"
            shift 2
            ;;
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --c-compiler)
            C_COMPILER="$2"
            shift 2
            ;;
        --cxx-compiler)
            CXX_COMPILER="$2"
            shift 2
            ;;
        --ccache)
            CCACHE="$2"
            shift 2
            ;;
        --generator)
            GENERATOR="$2"
            shift 2
            ;;
        *)
            break
            ;;
    esac
done

if [ $# -eq 0 ]; then
    usage
fi

case "$1" in
    configure)
        mkdir -p "$BUILD_DIR"
        cmake -S . -B "$BUILD_DIR" \
            -DLLVM_DIR:PATH="$LLVM_DIR" \
            -DCMAKE_BUILD_TYPE:STRING="$BUILD_TYPE" \
            -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
            -DCMAKE_C_COMPILER:FILEPATH="$C_COMPILER" \
            -DCMAKE_CXX_COMPILER:FILEPATH="$CXX_COMPILER" \
            -DCMAKE_CXX_COMPILER_LAUNCHER:FILEPATH="$CCACHE" \
            -G "$GENERATOR"
        ;;
    clean)
        rm -rf "$BUILD_DIR"
        ;;
    build)
        cmake --build "$BUILD_DIR"
        ;;
    all)
        "$0" clean
        "$0" configure
        "$0" build
        ;;
    *)
        echo "Error: Unknown command '$1'" >&2
        usage
        ;;
esac
