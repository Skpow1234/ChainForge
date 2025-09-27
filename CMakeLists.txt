cmake_minimum_required(VERSION 3.20)
project(ChainForge VERSION 0.1.0 LANGUAGES CXX)

# Set project properties
set(ChainForge_VERSION_MAJOR 0)
set(ChainForge_VERSION_MINOR 1)
set(ChainForge_VERSION_PATCH 0)

# Add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include cmake modules
include(cmake/CompilerFlags.cmake)
include(cmake/ConanSetup.cmake)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Build options - temporarily disable for basic build
option(BUILD_TESTING "Build tests" OFF)
option(BUILD_TOOLS "Build tools" OFF)
option(BUILD_EXAMPLES "Build examples" OFF)
option(USE_SANITIZERS "Enable sanitizers" OFF)

# Add subdirectories
add_subdirectory(modules)

# Temporarily disabled for basic build
# if(BUILD_TOOLS)
#     add_subdirectory(tools)
# endif()

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install configuration
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Write package config files
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ChainForgeConfigVersion.cmake"
    VERSION ${ChainForge_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ChainForgeConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ChainForgeConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ChainForge
)

# Install package config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ChainForgeConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ChainForgeConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ChainForge
)

# Install export targets
install(EXPORT ChainForgeTargets
    FILE ChainForgeTargets.cmake
    NAMESPACE ChainForge::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ChainForge
)

# Print configuration summary
message(STATUS "ChainForge ${ChainForge_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build testing: ${BUILD_TESTING}")
message(STATUS "Build tools: ${BUILD_TOOLS}")
message(STATUS "Use sanitizers: ${USE_SANITIZERS}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
