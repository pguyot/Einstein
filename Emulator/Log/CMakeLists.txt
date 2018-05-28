# Log CMakeLists

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(log STATIC ${sources})
target_include_directories(log PUBLIC
    ../
    ../../
    )

if(${WIN32})
    target_include_directories(log PUBLIC
        ../Win32)
endif()

target_link_libraries(log k)
