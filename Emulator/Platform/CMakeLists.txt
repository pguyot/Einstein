# Platform CMakeLists

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(platform STATIC ${sources})
target_include_directories(platform PUBLIC
    ../
    ../JIT/
    ../JIT/Generic
    ../../
    )

target_link_libraries(platform host pcmcia k)
