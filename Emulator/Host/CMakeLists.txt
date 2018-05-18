# Host CMakeLists

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(host STATIC ${sources})
target_include_directories(host PUBLIC
    ../
    ../../
    )

target_link_libraries(host k)
