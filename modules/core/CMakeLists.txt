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

# Add test binary
add_executable(cppchain-core-test src/main.cpp)
target_link_libraries(cppchain-core-test PRIVATE chainforge-core)

# Add error handling examples
add_executable(error-handling-example examples/error_handling_example.cpp)
target_link_libraries(error-handling-example PRIVATE chainforge-core)

add_executable(simple-error-example examples/simple_error_example.cpp)
target_link_libraries(simple-error-example PRIVATE chainforge-core)

add_executable(error-integration-guide examples/error_integration_guide.cpp)
target_link_libraries(error-integration-guide PRIVATE chainforge-core)

# Install
install(TARGETS chainforge-core cppchain-core-test error-handling-example simple-error-example error-integration-guide
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
