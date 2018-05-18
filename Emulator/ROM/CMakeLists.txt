# ROM CMakeLists

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(rom STATIC ${sources})
target_include_directories(rom PUBLIC
    ../
    ../JIT/
    ../JIT/Generic/
    ../Screen/
    ../../
    )

target_link_libraries(rom k)
