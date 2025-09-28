# Core module - Domain models only
# No I/O, no crypto primitives here

set(CORE_SOURCES
    src/hash.cpp
    src/address.cpp
    src/amount.cpp
    src/timestamp.cpp
    src/transaction.cpp
    src/block.cpp
    src/error.cpp
)

set(CORE_HEADERS
    include/chainforge/core/types.hpp
    include/chainforge/core/error.hpp
    include/chainforge/core/error_utils.hpp
    include/chainforge/core/hash.hpp
    include/chainforge/core/address.hpp
    include/chainforge/core/amount.hpp
    include/chainforge/core/timestamp.hpp
    include/chainforge/core/block.hpp
    include/chainforge/core/transaction.hpp
)

add_library(chainforge-core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(chainforge-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(chainforge-core PRIVATE cxx_std_20)

# Link dependencies
find_conan_package(fmt)
find_conan_package(spdlog)
find_conan_package(nlohmann_json)
target_link_libraries(chainforge-core PUBLIC fmt::fmt spdlog::spdlog nlohmann_json::nlohmann_json
                                      PRIVATE chainforge-crypto)

# Fix nlohmann_json include directories (workaround for Conan Debug/Release mismatch)
# Try multiple approaches to find nlohmann_json headers

# First, try to find using pkg-config (common on Linux)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(NLOHMANN_JSON_PKG QUIET nlohmann_json)
    if(NLOHMANN_JSON_PKG_FOUND)
        target_include_directories(chainforge-core PRIVATE ${NLOHMANN_JSON_PKG_INCLUDE_DIRS})
        message(STATUS "Found nlohmann_json via pkg-config: ${NLOHMANN_JSON_PKG_INCLUDE_DIRS}")
    endif()
endif()

# If pkg-config didn't work, try Conan package paths
if(NOT NLOHMANN_JSON_PKG_FOUND)
    find_path(NLOHMANN_JSON_CONAN_INCLUDE_DIR nlohmann/json.hpp
        PATHS
            /root/.conan2/p/*/p/include
            $ENV{HOME}/.conan2/p/*/p/include
            $ENV{CONAN_USER_HOME}/.conan2/p/*/p/include
            # CI/GHA specific paths
            /home/runner/.conan2/p/*/p/include
            /home/runner/work/_temp/.conan2/p/*/p/include
            # Alternative CI paths
            $ENV{RUNNER_TEMP}/.conan2/p/*/p/include
            $ENV{AGENT_TEMPDIRECTORY}/.conan2/p/*/p/include
            # More CI paths
            /opt/conan/p/*/p/include
            /usr/local/conan/p/*/p/include
        NO_DEFAULT_PATH
    )

    if(NLOHMANN_JSON_CONAN_INCLUDE_DIR)
        get_filename_component(NLOHMANN_JSON_CONAN_ROOT "${NLOHMANN_JSON_CONAN_INCLUDE_DIR}" DIRECTORY)
        target_include_directories(chainforge-core PRIVATE ${NLOHMANN_JSON_CONAN_ROOT}/include)
        message(STATUS "Found nlohmann_json via Conan path: ${NLOHMANN_JSON_CONAN_ROOT}/include")
    else()
        # Try getting from target property as fallback
        if(TARGET nlohmann_json::nlohmann_json)
            get_target_property(NLOHMANN_JSON_INCLUDE_DIRS nlohmann_json::nlohmann_json INTERFACE_INCLUDE_DIRECTORIES)
            if(NLOHMANN_JSON_INCLUDE_DIRS)
                target_include_directories(chainforge-core PRIVATE ${NLOHMANN_JSON_INCLUDE_DIRS})
                message(STATUS "Found nlohmann_json via target property: ${NLOHMANN_JSON_INCLUDE_DIRS}")
            else()
                message(WARNING "nlohmann_json target found but no include directories. This may cause compilation errors.")
            endif()
        else()
            message(WARNING "nlohmann_json target not found. Compilation may fail.")
        endif()
    endif()
endif()

# Add test binary
add_executable(cppchain-core-test src/main.cpp)
target_link_libraries(cppchain-core-test PRIVATE chainforge-core)

# Add error handling examples (only if BUILD_EXAMPLES is ON)
if(BUILD_EXAMPLES)
    add_executable(error-handling-example examples/error_handling_example.cpp)
    target_link_libraries(error-handling-example PRIVATE chainforge-core)

    add_executable(simple-error-example examples/simple_error_example.cpp)
    target_link_libraries(simple-error-example PRIVATE chainforge-core)

    add_executable(error-integration-guide examples/error_integration_guide.cpp)
    target_link_libraries(error-integration-guide PRIVATE chainforge-core)
endif()

# Install
set(INSTALL_TARGETS chainforge-core cppchain-core-test)
if(BUILD_EXAMPLES)
    list(APPEND INSTALL_TARGETS error-handling-example simple-error-example error-integration-guide)
endif()

install(TARGETS ${INSTALL_TARGETS}
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
