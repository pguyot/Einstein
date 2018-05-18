# Log CMakeLists

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(log STATIC ${sources})
target_include_directories(log PUBLIC
    ../
    ../../
    )

target_link_libraries(log k)
