# Files CMakeLists.txt

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(files STATIC ${sources})
target_include_directories(files PUBLIC
    ../
    ../JIT/
    ../JIT/Generic
    ../../
    )

target_link_libraries(files k)
