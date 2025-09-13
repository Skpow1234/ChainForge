# Modules CMakeLists.txt
# Each module creates its own target with proper dependencies

add_subdirectory(core)
add_subdirectory(crypto)
add_subdirectory(storage)
# add_subdirectory(p2p)         # Requires networking libraries
add_subdirectory(consensus)
# add_subdirectory(execution)   # Requires storage
add_subdirectory(mempool)
add_subdirectory(rpc)
# add_subdirectory(node)         # Requires all modules
# add_subdirectory(explorer)    # Requires all modules
