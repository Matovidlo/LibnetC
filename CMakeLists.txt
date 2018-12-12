cmake_minimum_required(VERSION 3.9)
project(LibnetC C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(LibnetC
        src/libnetc.c
        src/libnetc.h
        src/main.c)

target_link_libraries(LibnetC pthread)
