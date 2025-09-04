# Tests configuration for ChainForge

# Find Google Test
find_conan_package(gtest)

# Enable testing
enable_testing()

# Add test executables
add_executable(core_tests
    unit/core/test_hash.cpp
    unit/core/test_address.cpp
    unit/core/test_amount.cpp
    unit/core/test_timestamp.cpp
    unit/core/test_block.cpp
    unit/core/test_transaction.cpp
)

# Link test dependencies
target_link_libraries(core_tests
    PRIVATE
        chainforge-core
        gtest::gtest
        gtest::gtest_main
)

# Add tests
add_test(NAME CoreTests COMMAND core_tests)

# Set test properties
set_tests_properties(CoreTests PROPERTIES
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

# Install test binaries
install(TARGETS core_tests
    RUNTIME DESTINATION bin/tests
)
