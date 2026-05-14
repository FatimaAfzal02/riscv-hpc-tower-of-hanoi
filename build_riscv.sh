#!/bin/bash

set -e

TOOLCHAIN_PREFIX="riscv64-linux-gnu"
SYSROOT="/usr/riscv64-linux-gnu"
SRC="tower_of_hanoi_riscv.cpp"
BIN_NATIVE="tower_of_hanoi_native"
BIN_RISCV="tower_of_hanoi_riscv"

echo "Building Tower of Hanoi - RISC-V"
echo "=================================="

if ! command -v "${TOOLCHAIN_PREFIX}-g++" &> /dev/null; then
    echo "ERROR: RISC-V toolchain not found"
    echo "Install: sudo apt install gcc-riscv64-linux-gnu g++-riscv64-linux-gnu"
    exit 1
fi

if ! command -v qemu-riscv64 &> /dev/null; then
    echo "WARNING: qemu-riscv64 not found"
    echo "Install: sudo apt install qemu-user-static"
    QEMU_AVAILABLE=false
else
    QEMU_AVAILABLE=true
fi

echo ""
echo "Building native (x86_64)..."
g++ -O2 -std=c++17 -o "${BIN_NATIVE}" "${SRC}"
echo "  -> ${BIN_NATIVE}"

echo ""
echo "Building RISC-V (static)..."
"${TOOLCHAIN_PREFIX}-g++" -O2 -std=c++17 \
    -static -march=rv64gc -mabi=lp64d \
    -o "${BIN_RISCV}" "${SRC}"
echo "  -> ${BIN_RISCV}"

echo ""
echo "Verifying RISC-V binary..."
if file "${BIN_RISCV}" | grep -q "RISC-V"; then
    echo "  PASS: Confirmed RISC-V ELF"
    file "${BIN_RISCV}"
else
    echo "  FAIL: Binary is not RISC-V"
    exit 1
fi

echo ""
echo "Native test (n=4)..."
"./${BIN_NATIVE}" -n 4

if [ "${QEMU_AVAILABLE}" = true ]; then
    echo ""
    echo "QEMU RISC-V test (n=4)..."
    qemu-riscv64 -L "${SYSROOT}" "./${BIN_RISCV}" -n 4
fi

echo ""
echo "Native demo (n=5)..."
"./${BIN_NATIVE}" -g -n 5

if [ "${QEMU_AVAILABLE}" = true ]; then
    echo ""
    echo "QEMU RISC-V demo (n=5)..."
    qemu-riscv64 -L "${SYSROOT}" "./${BIN_RISCV}" -g -n 5
fi

if [ "${QEMU_AVAILABLE}" = true ]; then
    echo ""
    echo "=================================="
    echo "Performance Comparison (n=10)"
    echo "=================================="
    echo ""
    echo "Native x86_64:"
    "./${BIN_NATIVE}" -B -n 10
    echo ""
    echo "QEMU RISC-V:"
    qemu-riscv64 -L "${SYSROOT}" "./${BIN_RISCV}" -B -n 10
fi

echo ""
echo "=================================="
echo "Build complete"
echo "=================================="
echo ""
echo "Binaries:"
echo "  ./${BIN_NATIVE}   (x86_64)"
echo "  ./${BIN_RISCV}    (RISC-V, static)"
echo ""
echo "Commands:"
echo "  ./${BIN_NATIVE} -g -n 5"
echo "  ./${BIN_NATIVE} -b -n 12"
echo "  ./${BIN_NATIVE} -B -n 15"
echo "  ./${BIN_NATIVE} -v -n 4"
echo ""
echo "CMake build:"
echo "  mkdir build-riscv && cd build-riscv"
echo "  cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-riscv64.cmake"
echo "  make"
echo "  qemu-riscv64 -L ${SYSROOT} ./tower_of_hanoi -g -n 5"
