cmake_minimum_required(VERSION 3.20)
project(ChainForge VERSION 0.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")

# Treat warnings as errors in CI
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR DEFINED ENV{CI})
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror -Wall -Wextra -Wpedantic)
    endif()
endif()

# Find Conan
find_package(PkgConfig REQUIRED)
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake OPTIONAL)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add subdirectories
add_subdirectory(modules)
add_subdirectory(tools)
add_subdirectory(tests)

# Enable testing
enable_testing()
add_subdirectory(tests)
