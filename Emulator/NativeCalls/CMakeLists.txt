# NativeCalls CMakeLists

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(nativecalls STATIC ${sources})
target_include_directories(nativecalls PUBLIC
    ../
    ../JIT/
    ../JIT/Generic/
    ../../
    )

target_link_libraries(nativecalls k)
