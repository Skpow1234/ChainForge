# Crypto module - Cryptographic primitives
# Wraps vetted libraries; constant-time where required

set(CRYPTO_SOURCES
    src/random.cpp
    src/hash.cpp
    src/signature.cpp
    src/keypair.cpp
    src/curve.cpp
    src/keccak.cpp
)

set(CRYPTO_HEADERS
    include/chainforge/crypto/random.hpp
    include/chainforge/crypto/hash.hpp
    include/chainforge/crypto/signature.hpp
    include/chainforge/crypto/keypair.hpp
    include/chainforge/crypto/curve.hpp
    include/chainforge/crypto/keccak.hpp
    include/chainforge/crypto/types.hpp
)

add_library(chainforge-crypto STATIC ${CRYPTO_SOURCES} ${CRYPTO_HEADERS})

target_include_directories(chainforge-crypto
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(chainforge-crypto PRIVATE cxx_std_20)

# Temporarily disable deprecated warnings for OpenSSL 3.0 migration
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(chainforge-crypto PRIVATE -Wno-deprecated-declarations)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(chainforge-crypto PRIVATE /wd4996)
endif()

# Link dependencies
find_conan_package(fmt)
find_conan_package(spdlog)
find_conan_package(nlohmann_json)
find_openssl_conan()
find_conan_package(libsodium)

# Find secp256k1 using our custom module
find_package(Secp256k1 REQUIRED)

# Determine OpenSSL targets based on what was found
# Conan's OpenSSL package typically provides these targets
if(TARGET openssl::ssl AND TARGET openssl::crypto)
    set(OPENSSL_TARGETS openssl::ssl openssl::crypto)
elseif(TARGET OpenSSL::SSL AND TARGET OpenSSL::Crypto)
    set(OPENSSL_TARGETS OpenSSL::SSL OpenSSL::Crypto)
elseif(TARGET openssl::openssl)
    set(OPENSSL_TARGETS openssl::openssl)
else()
    message(FATAL_ERROR "OpenSSL targets not found. Available targets: ${CMAKE_FIND_PACKAGE_NAME}. Check Conan configuration.")
endif()

# secp256k1 include directories are handled by the imported target

target_link_libraries(chainforge-crypto
    PUBLIC
        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        ${OPENSSL_TARGETS}
        libsodium::libsodium
        secp256k1::secp256k1
    PRIVATE
        chainforge-core
)

# Fix libsodium include directories (workaround for Conan Debug/Release mismatch)
# Try multiple approaches to find libsodium headers

# First, try to find using pkg-config (common on Linux)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(LIBSODIUM_PKG QUIET libsodium)
    if(LIBSODIUM_PKG_FOUND)
        target_include_directories(chainforge-crypto PRIVATE ${LIBSODIUM_PKG_INCLUDE_DIRS})
        message(STATUS "Found libsodium via pkg-config: ${LIBSODIUM_PKG_INCLUDE_DIRS}")
    endif()
endif()

# If pkg-config didn't work, try Conan package paths
if(NOT LIBSODIUM_PKG_FOUND)
    find_path(LIBSODIUM_CONAN_INCLUDE_DIR sodium.h
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

    if(LIBSODIUM_CONAN_INCLUDE_DIR)
        get_filename_component(LIBSODIUM_CONAN_ROOT "${LIBSODIUM_CONAN_INCLUDE_DIR}" DIRECTORY)
        target_include_directories(chainforge-crypto PRIVATE ${LIBSODIUM_CONAN_ROOT}/include)
        message(STATUS "Found libsodium via Conan path: ${LIBSODIUM_CONAN_ROOT}/include")
    else()
        # Try getting from target property as fallback
        if(TARGET libsodium::libsodium)
            get_target_property(LIBSODIUM_INCLUDE_DIRS libsodium::libsodium INTERFACE_INCLUDE_DIRECTORIES)
            if(LIBSODIUM_INCLUDE_DIRS)
                target_include_directories(chainforge-crypto PRIVATE ${LIBSODIUM_INCLUDE_DIRS})
                message(STATUS "Found libsodium via target property: ${LIBSODIUM_INCLUDE_DIRS}")
            else()
                message(WARNING "libsodium target found but no include directories. This may cause compilation errors.")
            endif()
        else()
            message(WARNING "libsodium target not found. Compilation may fail.")
        endif()
    endif()
endif()

# Install
install(TARGETS chainforge-crypto
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
