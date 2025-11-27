# Tests configuration for ChainForge

# Find Google Test
find_conan_package(gtest)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

# Enable testing
enable_testing()

# Add unit test executables
add_executable(core_tests
    unit/core/test_hash.cpp
    # unit/core/test_address.cpp      # Disabled - missing functions
    # unit/core/test_amount.cpp       # Disabled - missing functions
    # unit/core/test_timestamp.cpp    # Disabled - missing functions
    # unit/core/test_block.cpp        # Disabled - missing functions
    # unit/core/test_transaction.cpp  # Disabled - missing functions
    unit/core/test_error_handling.cpp
)

# Add framework demo tests
add_executable(framework_tests
    unit/test_framework_demo.cpp
)

# Add network transport tests
add_executable(network_tests
    unit/test_network_transport.cpp
)

# Add main function for tests
target_sources(core_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
)

# Disabled until crypto module is fully implemented
# add_executable(crypto_tests
#     unit/crypto/test_keypair.cpp
#     unit/crypto/test_signature.cpp
#     unit/crypto/test_hash.cpp
# )

# Add serialization test executables
add_executable(serialization_tests
    unit/test_serialization.cpp
)

# Disabled until logging module is implemented
# add_executable(logging_tests
#     unit/logging/test_logger.cpp
#     unit/logging/test_log_manager.cpp
# )

# Add integration test executables
# Disabled until modules are fully implemented
# add_executable(integration_tests
#     integration/test_blockchain_integration.cpp
#     integration/test_p2p_integration.cpp
# )

# Add fuzz test executables
# Disabled until modules are fully implemented
# add_executable(fuzz_tests
#     fuzz/test_hash_fuzz.cpp
#     fuzz/test_transaction_fuzz.cpp
# )

# Add metrics test executables
# Disabled until metrics module is implemented
# add_executable(metrics_tests
#     unit/metrics/test_metrics.cpp
#     unit/metrics/test_metrics_registry.cpp
# )

# Link test dependencies
target_link_libraries(core_tests
    PRIVATE
        chainforge-core
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

# Link framework test dependencies
target_link_libraries(framework_tests
    PRIVATE
        chainforge-core
        chainforge-serialization
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

# Include test helpers directory
target_include_directories(framework_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link network test dependencies
target_link_libraries(network_tests
    PRIVATE
        chainforge-core
        chainforge-p2p
        GTest::gtest
        GTest::gtest_main
        Threads::Threads
)

# Disabled until crypto module is fully implemented
# target_link_libraries(crypto_tests
#     PRIVATE
#         chainforge-crypto
#         chainforge-core
#         GTest::gtest
#         GTest::gtest_main
# )

# Link serialization test dependencies
target_link_libraries(serialization_tests
    PRIVATE
        chainforge-serialization
        chainforge-core
        GTest::gtest
        GTest::gtest_main
)

# Disabled until logging module is implemented
# target_link_libraries(logging_tests
#     PRIVATE
#         chainforge_logging
#         GTest::gtest
#         GTest::gtest_main
# )

# Disabled until modules are fully implemented
# target_link_libraries(integration_tests
#     PRIVATE
#         chainforge-core
#         chainforge-crypto
#         chainforge-p2p
#         GTest::gtest
#         GTest::gtest_main
# )

# target_link_libraries(fuzz_tests
#     PRIVATE
#         chainforge-core
#         chainforge-crypto
#         GTest::gtest
#         GTest::gtest_main
# )

# Disabled until metrics module is implemented
# target_link_libraries(metrics_tests
#     PRIVATE
#         chainforge_metrics
#         GTest::gtest
#         GTest::gtest_main
# )

# Add tests with labels
add_test(NAME CoreTests COMMAND core_tests)
add_test(NAME SerializationTests COMMAND serialization_tests)
add_test(NAME FrameworkTests COMMAND framework_tests)
add_test(NAME NetworkTests COMMAND network_tests)
# Disabled until modules are fully implemented
# add_test(NAME CryptoTests COMMAND crypto_tests)
# add_test(NAME LoggingTests COMMAND logging_tests)
# add_test(NAME MetricsTests COMMAND metrics_tests)
# add_test(NAME IntegrationTests COMMAND integration_tests)
# add_test(NAME FuzzTests COMMAND fuzz_tests)

# Set test labels
set_tests_properties(CoreTests PROPERTIES
    LABELS "unit;core"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

set_tests_properties(SerializationTests PROPERTIES
    LABELS "unit;serialization"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

set_tests_properties(FrameworkTests PROPERTIES
    LABELS "unit;framework;demo"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

set_tests_properties(NetworkTests PROPERTIES
    LABELS "unit;network;p2p"
    TIMEOUT 300
    ENVIRONMENT "GTEST_COLOR=1"
)

# Disabled until modules are fully implemented
# set_tests_properties(CryptoTests PROPERTIES
#     LABELS "unit;crypto"
#     TIMEOUT 300
#     ENVIRONMENT "GTEST_COLOR=1"
# )

# Disabled until modules are implemented
# set_tests_properties(LoggingTests PROPERTIES
#     LABELS "unit;logging"
#     TIMEOUT 300
#     ENVIRONMENT "GTEST_COLOR=1"
# )

# set_tests_properties(MetricsTests PROPERTIES
#     LABELS "unit;metrics"
#     TIMEOUT 300
#     ENVIRONMENT "GTEST_COLOR=1"
# )

# Disabled until modules are fully implemented
# set_tests_properties(IntegrationTests PROPERTIES
#     LABELS "integration;blockchain;p2p"
#     TIMEOUT 600
#     ENVIRONMENT "GTEST_COLOR=1"
# )

# set_tests_properties(FuzzTests PROPERTIES
#     LABELS "fuzz;hash;transaction"
#     TIMEOUT 300
#     ENVIRONMENT "GTEST_COLOR=1"
# )

# Install test binaries
install(TARGETS core_tests serialization_tests framework_tests network_tests
    RUNTIME DESTINATION bin/tests
)
