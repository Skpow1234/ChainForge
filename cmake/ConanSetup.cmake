# Conan dependency management setup for Tokiwa
# Handles Conan v2 integration and dependency resolution

# Find Conan
find_package(PkgConfig REQUIRED)

# Include Conan toolchain if available
if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    include("${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
    message(STATUS "Using Conan toolchain from ${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
else()
    message(WARNING "Conan toolchain not found. Please run 'conan install' first.")
endif()

# Set Conan-specific variables
set(CONAN_CMAKE_TARGETS_FILE "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
set(CONAN_CMAKE_FIND_ROOT_PATH "${CMAKE_BINARY_DIR}")

# Function to find Conan packages
# Some Conan recipes use a different cmake_file_name than the package name
# (e.g. boost -> Boost). Map those here so callers can use either form.
function(find_conan_package PACKAGE_NAME)
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
        message(FATAL_ERROR "Conan toolchain not available. Please run 'conan install' first.")
    endif()

    set(_pkg "${PACKAGE_NAME}")
    if(_pkg STREQUAL "boost")
        set(_pkg "Boost")
    elseif(_pkg STREQUAL "gtest" OR _pkg STREQUAL "GTest")
        set(_pkg "GTest")
    endif()

    find_package(${_pkg} REQUIRED)
endfunction()

# Special handling for OpenSSL Conan package
function(find_openssl_conan)
    if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
        # Try different OpenSSL target names that Conan might provide
        find_package(OpenSSL QUIET)
        if(NOT OpenSSL_FOUND)
            find_package(openssl REQUIRED)
        endif()
    else()
        message(FATAL_ERROR "Conan toolchain not available. Please run 'conan install' first.")
    endif()
endfunction()

# Function to link Conan targets
function(link_conan_targets TARGET_NAME)
    if(EXISTS "${CMAKE_BINARY_DIR}/conan_toolchain.cmake")
        target_link_libraries(${TARGET_NAME} ${ARGN})
    else()
        message(FATAL_ERROR "Conan toolchain not available. Please run 'conan install' first.")
    endif()
endfunction()
