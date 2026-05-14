cmake_minimum_required(VERSION 3.14)
project(TowerOfHanoiRISCV VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CROSSCOMPILING AND CMAKE_SYSTEM_PROCESSOR MATCHES "riscv")
    add_compile_options(-march=rv64gc -mabi=lp64d -O2 -ftree-vectorize)
    add_compile_definitions(RISCV_TARGET)
    message(STATUS "RISC-V optimizations enabled")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpmachine
        OUTPUT_VARIABLE COMPILER_TARGET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(COMPILER_TARGET MATCHES "riscv")
        add_compile_options(-march=rv64gc -mabi=lp64d -O2 -ftree-vectorize)
        add_compile_definitions(RISCV_TARGET)
        message(STATUS "RISC-V compiler detected")
    endif()
endif()

add_executable(tower_of_hanoi tower_of_hanoi_riscv.cpp)

if(NOT CMAKE_CROSSCOMPILING)
    add_executable(tower_of_hanoi_asan tower_of_hanoi_riscv.cpp)
    target_compile_options(tower_of_hanoi_asan PRIVATE -fsanitize=address -g)
    target_link_options(tower_of_hanoi_asan PRIVATE -fsanitize=address)
endif()

enable_testing()
add_test(NAME hanoi_correctness_4  COMMAND tower_of_hanoi -n 4)
add_test(NAME hanoi_correctness_6  COMMAND tower_of_hanoi -n 6)
add_test(NAME hanoi_verbose        COMMAND tower_of_hanoi -v -n 3)
add_test(NAME hanoi_visual         COMMAND tower_of_hanoi -g -n 3)
add_test(NAME hanoi_benchmark_upto COMMAND tower_of_hanoi -b -n 8)
add_test(NAME hanoi_benchmark_at   COMMAND tower_of_hanoi -B -n 10)

install(TARGETS tower_of_hanoi DESTINATION bin)
