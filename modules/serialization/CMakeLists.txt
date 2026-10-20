# Serialization module - Protocol buffer-based serialization
# Provides binary serialization with forward compatibility

set(SERIALIZATION_SOURCES
    src/serializer.cpp
    src/validator.cpp
)

set(SERIALIZATION_HEADERS
    include/tokiwa/serialization/serializer.hpp
    include/tokiwa/serialization/validator.hpp
    include/tokiwa/serialization/serialization.hpp
)

# Protobuf files
set(PROTO_FILES
    proto/types.proto
    proto/transaction.proto
    proto/block.proto
)

# Find protobuf
find_conan_package(protobuf)
find_package(Protobuf REQUIRED)

# Generate protobuf C++ files
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})
list(APPEND SERIALIZATION_SOURCES ${PROTO_SRCS})
list(APPEND SERIALIZATION_HEADERS ${PROTO_HDRS})

add_library(tokiwa-serialization STATIC ${SERIALIZATION_SOURCES} ${SERIALIZATION_HEADERS})

target_include_directories(tokiwa-serialization
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # For generated protobuf headers
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(tokiwa-serialization PRIVATE cxx_std_20)

# Link dependencies
find_conan_package(fmt)
find_conan_package(spdlog)
target_link_libraries(tokiwa-serialization
    PUBLIC
        protobuf::libprotobuf
        fmt::fmt
        spdlog::spdlog
    PRIVATE
        tokiwa-core
        tokiwa-crypto
)

# Install
install(TARGETS tokiwa-serialization
    EXPORT TokiwaTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ DESTINATION include
    FILES_MATCHING PATTERN "*.pb.h"
)
