# Network CMakeLists.txt

# Only this one compiles for now ...
set(sources TNetworkManager.cp TUsermodeNetwork.cp)

# TODO virtual serial port

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(network STATIC ${sources})

target_include_directories(network PUBLIC
    ../
    ../JIT/
    ../JIT/Generic
    ../../
    )

target_link_libraries(network k)
