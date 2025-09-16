# FindSecp256k1.cmake - Find or build secp256k1 library
# This module provides secp256k1 as an imported target for ChainForge
# Follows production-grade practices for blockchain projects

include(ExternalProject)
include(FetchContent)

# Try to find secp256k1 via pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(PC_SECP256K1 QUIET libsecp256k1)
endif()

# Look for system installation
find_path(SECP256K1_INCLUDE_DIR
    NAMES secp256k1.h
    PATHS ${PC_SECP256K1_INCLUDE_DIRS}
    PATH_SUFFIXES include
)

find_library(SECP256K1_LIBRARY
    NAMES secp256k1
    PATHS ${PC_SECP256K1_LIBRARY_DIRS}
    PATH_SUFFIXES lib
)

# If not found, build from source
if(NOT SECP256K1_INCLUDE_DIR OR NOT SECP256K1_LIBRARY)
    message(STATUS "secp256k1 not found system-wide, building from source...")
    
    # Use FetchContent to download and build secp256k1
    FetchContent_Declare(
        secp256k1_external
        GIT_REPOSITORY https://github.com/bitcoin-core/secp256k1.git
        GIT_TAG v0.4.1  # Use stable release
        GIT_SHALLOW TRUE
    )
    
    # Configure secp256k1 build options
    set(SECP256K1_BUILD_BENCHMARK OFF CACHE BOOL "Build secp256k1 benchmarks" FORCE)
    set(SECP256K1_BUILD_TESTS OFF CACHE BOOL "Build secp256k1 tests" FORCE)
    set(SECP256K1_BUILD_EXHAUSTIVE_TESTS OFF CACHE BOOL "Build secp256k1 exhaustive tests" FORCE)
    set(SECP256K1_BUILD_CTIME_TESTS OFF CACHE BOOL "Build secp256k1 constant time tests" FORCE)
    set(SECP256K1_BUILD_EXAMPLES OFF CACHE BOOL "Build secp256k1 examples" FORCE)
    
    # Enable required modules for blockchain use
    set(SECP256K1_ENABLE_MODULE_RECOVERY ON CACHE BOOL "Enable secp256k1 recovery module" FORCE)
    set(SECP256K1_ENABLE_MODULE_EXTRAKEYS ON CACHE BOOL "Enable secp256k1 extrakeys module" FORCE)
    set(SECP256K1_ENABLE_MODULE_SCHNORRSIG ON CACHE BOOL "Enable secp256k1 schnorrsig module" FORCE)
    
    FetchContent_MakeAvailable(secp256k1_external)
    
    # Set variables for the built library
    set(SECP256K1_INCLUDE_DIR ${secp256k1_external_SOURCE_DIR}/include)
    set(SECP256K1_LIBRARY secp256k1)
    set(SECP256K1_BUILT_FROM_SOURCE TRUE)
else()
    message(STATUS "Found secp256k1: ${SECP256K1_LIBRARY}")
    set(SECP256K1_BUILT_FROM_SOURCE FALSE)
endif()

# Create imported target
if(NOT TARGET secp256k1::secp256k1)
    if(SECP256K1_BUILT_FROM_SOURCE)
        # For built-from-source, use the CMake target directly
        add_library(secp256k1::secp256k1 ALIAS secp256k1)
    else()
        # For system library, create imported target
        add_library(secp256k1::secp256k1 UNKNOWN IMPORTED)
        set_target_properties(secp256k1::secp256k1 PROPERTIES
            IMPORTED_LOCATION "${SECP256K1_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SECP256K1_INCLUDE_DIR}"
        )
    endif()
endif()

# Set standard variables
set(SECP256K1_FOUND TRUE)
set(SECP256K1_INCLUDE_DIRS ${SECP256K1_INCLUDE_DIR})
set(SECP256K1_LIBRARIES ${SECP256K1_LIBRARY})

# Provide compatibility variables for pkg-config style
set(SECP256K1_CFLAGS_OTHER "")
set(SECP256K1_LIBRARY_DIRS "")

mark_as_advanced(SECP256K1_INCLUDE_DIR SECP256K1_LIBRARY)

# Print status
if(SECP256K1_FOUND)
    message(STATUS "secp256k1 configuration:")
    message(STATUS "  Include dir: ${SECP256K1_INCLUDE_DIR}")
    message(STATUS "  Library: ${SECP256K1_LIBRARY}")
    message(STATUS "  Built from source: ${SECP256K1_BUILT_FROM_SOURCE}")
endif()
