cmake_minimum_required(VERSION 3.1)
project(LibnetC)

set(CMAKE_CXX_STANDARD 11)
enable_testing()
#find_package(GTest REQUIRED)
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# This is used for compilation of libnetc without testing.
include_directories(src log.c/src)

add_library(log
	log.c/src/log.c
        log.c/src/log.h)

add_library(libnet
        src/libnetc.c
        src/libnetc.h)
target_link_libraries(libnet log)
#set_target_properties(log libnet PROPERTIES LINKER_LANGUAGE CXX)

add_executable(test_connection 
        tests/libnet/connection.cpp)

target_link_libraries(test_connection libnet ${GTEST_LIBRARIES} gtest_main pthread)

add_test(NAME test_address_info COMMAND test_connection)

add_executable(LibnetC
        src/libnetc.c
        src/libnetc.h
        src/main.c)

target_link_libraries(LibnetC log pthread)

