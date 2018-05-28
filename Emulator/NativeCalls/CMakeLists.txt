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

if(${WIN32})
    target_include_directories(nativecalls PUBLIC
        ../Win32)
endif()

target_link_libraries(nativecalls k)
