# Tests configuration for ChainForge

# Find Google Test
find_conan_package(gtest)

# Enable testing
enable_testing()

# Add unit test executables
add_executable(core_tests
    unit/core/test_hash.cpp
    unit/core/test_address.cpp
    unit/core/test_amount.cpp
    unit/core/test_timestamp.cpp
    unit/core/test_block.cpp
    unit/core/test_transaction.cpp
    unit/core/test_error_handling.cpp
)

add_executable(crypto_tests
    unit/crypto/test_keypair.cpp
    unit/crypto/test_signature.cpp
    unit/crypto/test_hash.cpp
)

add_executable(logging_tests
    unit/logging/test_logger.cpp
    unit/logging/test_log_manager.cpp
)

# Add integration test executables
add_executable(integration_tests
    integration/test_blockchain_integration.cpp
    integration/test_p2p_integration.cpp
)

# Add fuzz test executables
add_executable(fuzz_tests
    fuzz/test_hash_fuzz.cpp
    fuzz/test_transaction_fuzz.cpp
)

# Add metrics test executables
add_executable(metrics_tests
    unit/metrics/test_metrics.cpp
    unit/metrics/test_metrics_registry.cpp
)

# Link test dependencies
target_link_libraries(core_tests
    PRIVATE
        chainforge-core
        gtest::gtest
        gtest::gtest_main
)

target_link_libraries(crypto_tests
    PRIVATE
        chainforge-crypto
        chainforge-core
        gtest::gtest
        gtest::gtest_main
)

target_link_libraries(logging_tests
    PRIVATE
        chainforge_logging
        gtest::gtest
        gtest::gtest_main
)

target_link_libraries(integration_tests
    PRIVATE
        chainforge-core
        chainforge-crypto
        chainforge-p2p
        gtest::gtest
        gtest::gtest_main
)

target_link_libraries(fuzz_tests
    PRIVATE
        chainforge-core
        chainforge-crypto
        gtest::gtest
        gtest::gtest_main
)

target_link_libraries(metrics_tests
    PRIVATE
        chainforge_metrics
        gtest::gtest
        gtest::gtest_main
)

# Add tests with labels
add_test(NAME CoreTests COMMAND core_tests)
add_test(NAME CryptoTests COMMAND crypto_tests)
add_test(NAME LoggingTests COMMAND logging_tests)
add_test(NAME MetricsTests COMMAND metrics_tests)
add_test(NAME IntegrationTests COMMAND integration_tests)
add_test(NAME FuzzTests COMMAND fuzz_tests)

# Set test labels
set_tests_properties(CoreTests PROPERTIES
    LABELS "unit;core"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

set_tests_properties(CryptoTests PROPERTIES
    LABELS "unit;crypto"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

set_tests_properties(LoggingTests PROPERTIES
    LABELS "unit;logging"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

set_tests_properties(MetricsTests PROPERTIES
    LABELS "unit;metrics"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

set_tests_properties(IntegrationTests PROPERTIES
    LABELS "integration;blockchain;p2p"
    TIMEOUT 600
    ENVIRONMENT "GTEST_COLOR=1"
)

set_tests_properties(FuzzTests PROPERTIES
    LABELS "fuzz;hash;transaction"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

# Install test binaries
install(TARGETS core_tests crypto_tests logging_tests metrics_tests integration_tests fuzz_tests
    RUNTIME DESTINATION bin/tests
)
