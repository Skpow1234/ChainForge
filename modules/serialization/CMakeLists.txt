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

# Protobuf files (paths relative to this module)
set(PROTO_FILES
    proto/types.proto
    proto/transaction.proto
    proto/block.proto
)

find_conan_package(protobuf)

if(NOT TARGET protobuf::protoc)
    message(FATAL_ERROR
        "protobuf::protoc target missing. Conan protobuf must provide the codegen tool "
        "(do not rely on system protobuf-compiler).")
endif()
# Make CI logs show which protoc is used (must be Conan's, not /usr/bin/protoc)
get_target_property(_tokiwa_protoc_loc protobuf::protoc IMPORTED_LOCATION)
if(NOT _tokiwa_protoc_loc)
    get_target_property(_tokiwa_protoc_loc protobuf::protoc LOCATION)
endif()
message(STATUS "Using Conan protoc: ${_tokiwa_protoc_loc}")

# Generate .pb.cc/.pb.h with Conan's protoc only. Using system /usr/bin/protoc against
# Conan protobuf/3.21 headers produces the classic version-mismatch / MapEntryHelper errors.
set(PROTO_SRCS)
set(PROTO_HDRS)
foreach(_proto ${PROTO_FILES})
    get_filename_component(_proto_abs "${CMAKE_CURRENT_SOURCE_DIR}/${_proto}" ABSOLUTE)
    get_filename_component(_proto_rel_dir "${_proto}" DIRECTORY)
    get_filename_component(_proto_name "${_proto}" NAME_WE)

    set(_pb_cc "${CMAKE_CURRENT_BINARY_DIR}/${_proto_rel_dir}/${_proto_name}.pb.cc")
    set(_pb_h "${CMAKE_CURRENT_BINARY_DIR}/${_proto_rel_dir}/${_proto_name}.pb.h")

    add_custom_command(
        OUTPUT "${_pb_cc}" "${_pb_h}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${_proto_rel_dir}"
        COMMAND $<TARGET_FILE:protobuf::protoc>
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            -I ${CMAKE_CURRENT_SOURCE_DIR}
            ${_proto_abs}
        DEPENDS "${_proto_abs}" protobuf::protoc
        COMMENT "Generating ${_proto_rel_dir}/${_proto_name}.pb.cc with Conan protoc"
        VERBATIM
    )

    list(APPEND PROTO_SRCS "${_pb_cc}")
    list(APPEND PROTO_HDRS "${_pb_h}")
endforeach()

list(APPEND SERIALIZATION_SOURCES ${PROTO_SRCS})
list(APPEND SERIALIZATION_HEADERS ${PROTO_HDRS})

add_library(tokiwa-serialization STATIC ${SERIALIZATION_SOURCES} ${SERIALIZATION_HEADERS})

target_include_directories(tokiwa-serialization
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(tokiwa-serialization PRIVATE cxx_std_20)

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
