# Logging Module CMakeLists.txt
# Structured logging with spdlog for ChainForge

# Find required packages
find_conan_package(spdlog)
find_conan_package(nlohmann_json)
find_conan_package(fmt)

# Create the logging library
add_library(chainforge_logging
    src/logger.cpp
    src/json_formatter.cpp
    src/performance_logger.cpp
    src/log_manager.cpp
    src/config_loader.cpp
)

# Set target properties
target_compile_features(chainforge_logging PUBLIC cxx_std_20)

# Include directories
target_include_directories(chainforge_logging
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link dependencies
target_link_libraries(chainforge_logging
    PUBLIC
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        fmt::fmt
)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(chainforge_logging PRIVATE
        -Wall -Wextra -Werror
        -Wno-unused-parameter
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(chainforge_logging PRIVATE
        /W4 /WX
        /wd4100  # unreferenced formal parameter
    )
endif()

# Export targets
set_target_properties(chainforge_logging PROPERTIES EXPORT_NAME logging)
