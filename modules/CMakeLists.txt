# Modules CMakeLists.txt
# Each module creates its own target with proper dependencies

add_subdirectory(core)
add_subdirectory(crypto)
add_subdirectory(storage)
add_subdirectory(p2p)
add_subdirectory(consensus)
add_subdirectory(execution)
add_subdirectory(mempool)
add_subdirectory(rpc)
add_subdirectory(node)
add_subdirectory(explorer)
