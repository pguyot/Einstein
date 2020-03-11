# NativeCalls CMakeLists

file(GLOB headers *.h)
file(GLOB sources *.cp)

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

pkg_check_modules(FFI libffi)

if (!FFI_FOUND)
    message(FATAL_ERROR "Missing library: libffi")
endif()

add_library(nativecalls STATIC ${sources})

target_include_directories(nativecalls PUBLIC
    ${FFI_STATIC_INCLUDE_DIRS}
    ../
    ../JIT/
    ../JIT/Generic/
    ../../
    )

target_link_libraries(nativecalls k ${FFI_STATIC_LIBRARIES})
