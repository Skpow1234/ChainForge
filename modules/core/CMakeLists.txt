# Core module - Domain models only
# No I/O, no crypto primitives here

set(CORE_SOURCES
    src/block.cpp
    src/transaction.cpp
    src/hash.cpp
    src/address.cpp
    src/amount.cpp
    src/timestamp.cpp
)

set(CORE_HEADERS
    include/chainforge/core/block.hpp
    include/chainforge/core/transaction.hpp
    include/chainforge/core/hash.hpp
    include/chainforge/core/address.hpp
    include/chainforge/core/amount.hpp
    include/chainforge/core/timestamp.hpp
    include/chainforge/core/types.hpp
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
find_package(fmt REQUIRED)
target_link_libraries(chainforge-core PUBLIC fmt::fmt)

# Install
install(TARGETS chainforge-core
    EXPORT ChainForgeTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/ DESTINATION include)
