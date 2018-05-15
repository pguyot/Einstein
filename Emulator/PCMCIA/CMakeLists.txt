# CMakeLists.txt

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(pcmcia STATIC ${sources})
target_include_directories(pcmcia PUBLIC
    ../
    ../JIT/
    ../JIT/Generic
    ../../
    )

target_link_libraries(pcmcia k)
