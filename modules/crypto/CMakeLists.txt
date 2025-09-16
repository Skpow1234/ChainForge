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

# Find system libsecp256k1
find_package(PkgConfig REQUIRED)
pkg_check_modules(SECP256K1 REQUIRED libsecp256k1)

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

# Add secp256k1 include directories
target_include_directories(chainforge-crypto PRIVATE ${SECP256K1_INCLUDE_DIRS})

target_link_libraries(chainforge-crypto
    PUBLIC
        fmt::fmt
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        ${OPENSSL_TARGETS}
        libsodium::libsodium
        ${SECP256K1_LIBRARIES}
    PRIVATE
        chainforge-core
)

# Install
install(TARGETS chainforge-crypto
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
