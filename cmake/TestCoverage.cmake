# Test Coverage Configuration for ChainForge
# Provides coverage reporting using gcov/lcov or llvm-cov

option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

if(ENABLE_COVERAGE)
    message(STATUS "Code coverage reporting enabled")
    
    # Check compiler support
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        # Add coverage flags
        set(COVERAGE_COMPILE_FLAGS "-g -O0 --coverage -fprofile-arcs -ftest-coverage")
        set(COVERAGE_LINK_FLAGS "--coverage")
        
        # Apply flags to all targets
        add_compile_options(${COVERAGE_COMPILE_FLAGS})
        add_link_options(${COVERAGE_LINK_FLAGS})
        
        message(STATUS "Coverage flags: ${COVERAGE_COMPILE_FLAGS}")
        
        # Find coverage tools
        find_program(LCOV_PATH lcov)
        find_program(GENHTML_PATH genhtml)
        
        if(LCOV_PATH AND GENHTML_PATH)
            message(STATUS "Found lcov: ${LCOV_PATH}")
            message(STATUS "Found genhtml: ${GENHTML_PATH}")
            
            # Add custom target for generating coverage report
            add_custom_target(coverage
                COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
                
                # Initialize coverage
                COMMAND ${LCOV_PATH} --directory . --zerocounters
                
                # Run tests
                COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
                
                # Capture coverage data
                COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
                
                # Remove system/external headers
                COMMAND ${LCOV_PATH} --remove coverage.info '/usr/*' '*/conan/*' '*/tests/*' --output-file coverage.info.cleaned
                
                # Generate HTML report
                COMMAND ${GENHTML_PATH} -o ${CMAKE_BINARY_DIR}/coverage coverage.info.cleaned
                
                # Summary
                COMMAND ${LCOV_PATH} --summary coverage.info.cleaned
                
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating code coverage report..."
            )
            
            # Add target for cleaning coverage data
            add_custom_target(coverage-clean
                COMMAND ${LCOV_PATH} --directory . --zerocounters
                COMMAND ${CMAKE_COMMAND} -E remove -f coverage.info coverage.info.cleaned
                COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/coverage
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Cleaning coverage data..."
            )
            
            message(STATUS "Coverage targets added: 'make coverage' and 'make coverage-clean'")
            message(STATUS "HTML report will be generated in: ${CMAKE_BINARY_DIR}/coverage/index.html")
        else()
            message(WARNING "lcov or genhtml not found. Coverage reporting disabled.")
            message(STATUS "Install with: sudo apt-get install lcov (Ubuntu/Debian)")
        endif()
        
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        message(WARNING "Code coverage not yet implemented for MSVC")
        message(STATUS "Consider using OpenCppCoverage: https://github.com/OpenCppCoverage/OpenCppCoverage")
        
    else()
        message(WARNING "Code coverage not supported for compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()
else()
    message(STATUS "Code coverage reporting disabled (use -DENABLE_COVERAGE=ON to enable)")
endif()

# Function to exclude files from coverage
function(exclude_from_coverage target)
    if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -fno-profile-arcs -fno-test-coverage)
    endif()
endfunction()

# Function to add coverage to specific target
function(add_coverage_to_target target)
    if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${target} PRIVATE --coverage)
    endif()
endfunction()

