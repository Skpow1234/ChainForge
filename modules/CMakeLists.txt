# Modules CMakeLists.txt
# Crypto first (no core dependency); core then depends on crypto.

add_subdirectory(crypto)
add_subdirectory(core)
add_subdirectory(serialization)
add_subdirectory(p2p)
add_subdirectory(logging)
add_subdirectory(metrics)
add_subdirectory(storage)
add_subdirectory(consensus)
add_subdirectory(mempool)
add_subdirectory(rpc)
add_subdirectory(node)
# add_subdirectory(execution)   # Not present yet
# add_subdirectory(explorer)    # Not present yet
