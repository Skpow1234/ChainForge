# Compiler flags configuration for ChainForge
# Provides consistent flags across different compilers and build types

include(CheckCXXCompilerFlag)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Base compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

# Platform-specific flags
if(MSVC)
    # MSVC flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
    
    # MSVC-specific warnings
    add_compile_options(/W4 /wd4251 /wd4275)
else()
    # GCC/Clang flags
    add_compile_options(-Wall -Wextra -Wpedantic)
    
    # Check for specific flags
    check_cxx_compiler_flag("-Wshadow" COMPILER_HAS_WSHADOW)
    if(COMPILER_HAS_WSHADOW)
        add_compile_options(-Wshadow)
    endif()
    
    check_cxx_compiler_flag("-Wconversion" COMPILER_HAS_WCONVERSION)
    if(COMPILER_HAS_WCONVERSION)
        add_compile_options(-Wconversion)
    endif()
    
    check_cxx_compiler_flag("-Wsign-conversion" COMPILER_HAS_WSIGN_CONVERSION)
    if(COMPILER_HAS_WSIGN_CONVERSION)
        add_compile_options(-Wsign-conversion)
    endif()
endif()

# Sanitizer support
if(USE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Use compatible sanitizer combinations
    # Address + Undefined Behavior are compatible
    set(SANITIZER_FLAGS_ADDRESS_UB "-fsanitize=address,undefined")
    # Thread sanitizer must be used separately
    set(SANITIZER_FLAGS_THREAD "-fsanitize=thread")
    
    # Default to address + undefined behavior for debug builds
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZER_FLAGS_ADDRESS_UB}")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS_ADDRESS_UB}")
    
    # Note: Thread sanitizer builds should be configured separately
    # to avoid conflicts with address sanitizer
endif()

# Treat warnings as errors in CI
if(DEFINED ENV{CI} OR CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/WX)
    else()
        add_compile_options(-Werror)
    endif()
endif()

# Link time optimization for release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    check_cxx_compiler_flag("-flto" COMPILER_HAS_LTO)
    if(COMPILER_HAS_LTO)
        add_compile_options(-flto)
        add_link_options(-flto)
    endif()
endif()
