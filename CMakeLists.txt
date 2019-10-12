cmake_minimum_required(VERSION 3.10)
project(bmp_rip C)

set(CMAKE_C_STANDARD 11)

add_executable(bmp_rip
        bmp.c syscalls.c syscalls.h bmp.h)
