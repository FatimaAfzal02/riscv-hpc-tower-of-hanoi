# Tower of Hanoi  RISC-V Demonstration

Recursive vs iterative solution implementations with performance benchmarking and cross-compilation support for RISC-V architecture.

## Overview

Two algorithmic approaches to the Tower of Hanoi problem:

| Approach | Characteristics | Use Case |
|---|---|---|
| Recursive | O(n) stack space, mirrors domain decomposition | FEM solvers, AMR algorithms |
| Iterative | O(1) space, peg-rotation algorithm | Linear solvers (CG, GMRES) |

Both solutions verified correct and benchmarked on x86_64 and RISC-V under QEMU emulation.

## Requirements

### Native Build
- g++ >= 9.0
- CMake >= 3.14

### RISC-V Cross-Compile
- `riscv64-linux-gnu-g++`
- `qemu-riscv64`

Install cross-compile toolchain:
```bash
sudo apt install gcc-riscv64-linux-gnu g++-riscv64-linux-gnu qemu-user-static
```

## Build Instructions

### Option 1: Direct Compile

Native:
```bash
g++ -O2 -std=c++17 -o tower_of_hanoi tower_of_hanoi_riscv.cpp
```

RISC-V:
```bash
riscv64-linux-gnu-g++ -O2 -std=c++17 -static -march=rv64gc -mabi=lp64d \
    -o tower_of_hanoi_riscv tower_of_hanoi_riscv.cpp
```

### Option 2: CMake

Native:
```bash
mkdir build && cd build
cmake ..
make
```

RISC-V:
```bash
mkdir build-riscv && cd build-riscv
cmake .. -DCMAKE_TOOLCHAIN_FILE=../toolchain-riscv64.cmake
make
```

### Option 3: Build Script

```bash
chmod +x build_riscv.sh
./build_riscv.sh
```

Builds both binaries, verifies RISC-V ELF, and runs tests.

## Usage

```
./tower_of_hanoi [-v] [-b] [-B] [-g] [-n disks] [-h]

  -v        Verbose mode (print each move)
  -b        Benchmark up to N disks (scaling table)
  -B        Single benchmark at N disks
  -g        ASCII graphics visualization
  -n N      Number of disks (default: 4, max for -g: 6)
  -h        Help
```

### Examples

Visual demo (5 disks):
```bash
./tower_of_hanoi -g -n 5
```

Verbose output (4 disks):
```bash
./tower_of_hanoi -v -n 4
```

Scaling benchmark (up to 12 disks):
```bash
./tower_of_hanoi -b -n 12
```

Single benchmark (15 disks):
```bash
./tower_of_hanoi -B -n 15
```

QEMU RISC-V test:
```bash
qemu-riscv64 -L /usr/riscv64-linux-gnu ./tower_of_hanoi_riscv -g -n 5
```

## Performance

### Time Complexity
Both algorithms: O(2^n) moves

### Space Complexity
- Recursive: O(n) stack frames
- Iterative: O(1)

### Benchmark Results (x86_64)

```
Disks    Recursion(us)    Iteration(us)
  1             0                0
  4             1                1
  8            19               16
 12           318              261
 15          2541             2089
```

Iteration becomes faster around n=8-10 due to reduced call overhead. On RISC-V hardware, crossover point depends on branch predictor and cache configuration.

## RISC-V Architecture Notes

### Compilation Flags
- `-march=rv64gc` : Base ISA + compressed + single/double FP
- `-mabi=lp64d` : 64-bit pointers, double-precision FP registers
- `-static` : Required for QEMU user-mode execution

### Stack Depth Considerations
Recursive solution depth scales with n. Embedded RISC-V cores may have limited stack (4-16 KB). For deep recursive HPC codes, profile with `-fstack-usage`.

### Vector Extension (RVV)
Iterative solution inner loop is a candidate for RVV acceleration. Use `-march=rv64gcv` when targeting boards with Vector Extension.

## Project Structure

```
.
├── tower_of_hanoi_riscv.cpp    Main source
├── CMakeLists.txt              CMake configuration
├── toolchain-riscv64.cmake     RISC-V cross-compile toolchain
├── build_riscv.sh              Build script
└── README.md                   Documentation
```

## Testing

Run test suite:
```bash
cd build
ctest
```

AddressSanitizer build (native only):
```bash
mkdir build-asan && cd build-asan
cmake ..
make tower_of_hanoi_asan
./tower_of_hanoi_asan -b -n 10
```

## License

MIT License
