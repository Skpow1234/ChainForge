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

# Link dependencies
find_package(OpenSSL REQUIRED)
find_package(libsecp256k1 REQUIRED)
find_package(libsodium REQUIRED)
find_package(blst REQUIRED)

target_link_libraries(chainforge-crypto
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        libsecp256k1::libsecp256k1
        libsodium::libsodium
        blst::blst
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
