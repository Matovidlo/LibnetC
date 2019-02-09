cmake_minimum_required(VERSION 3.9)
project(LibnetC C)

set(CMAKE_C_STANDARD 11)

include_directories(src log.c/src)

add_library(log.o
	log.c/src/log.c
	log.c/src/log.h)

add_executable(LibnetC
        src/libnetc.c
        src/libnetc.h
        src/main.c)

target_link_libraries(LibnetC log.o pthread)
