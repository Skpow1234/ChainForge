# Metrics Module CMakeLists.txt
# Prometheus metrics collection for ChainForge

# Find required packages
find_conan_package(nlohmann_json)
# Note: prometheus-cpp temporarily disabled due to CI issues

# Create the metrics library
add_library(chainforge_metrics
    src/simple_metrics.cpp
    src/metrics_registry.cpp
    src/counter.cpp
    src/gauge.cpp
    src/histogram.cpp
    src/metrics_server.cpp
    src/chainforge_metrics.cpp
    src/logging_metrics.cpp
)

# Set target properties
target_compile_features(chainforge_metrics PUBLIC cxx_std_20)

# Include directories
target_include_directories(chainforge_metrics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link dependencies
target_link_libraries(chainforge_metrics
    PUBLIC
        nlohmann_json::nlohmann_json
        chainforge_logging
)

# Compiler-specific options
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(chainforge_metrics PRIVATE
        -Wall -Wextra -Werror
        -Wno-unused-parameter
    )
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(chainforge_metrics PRIVATE
        /W4 /WX
        /wd4100  # unreferenced formal parameter
    )
endif()

# Export targets
set_target_properties(chainforge_metrics PROPERTIES EXPORT_NAME metrics)
