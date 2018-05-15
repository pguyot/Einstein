# Screen CMakeLists.txt

file(GLOB headers *.h)

set(sources TScreenManager.cp TNullScreenManager.cp)

if(EINSTEIN_SCREEN_MANAGER STREQUAL "X11")
    find_package(X11)

    if (X11_FOUND)
        list(APPEND LINKLIBS ${X11_LIBRARIES})
    else()
        message(FATAL_ERROR "Missing library: X11")
    endif()
elseif(EINSTEIN_SCREEN_MANAGER STREQUAL "FL")
    find_package(FLTK)

    if (FLTK_FOUND)
        list(APPEND LINKLIBS FLTK_LIBRARIES)
    else()
        message(FATAL_ERROR "Missing library: fltk")
    endif()
endif()

set(sources ${sources} "T${EINSTEIN_SCREEN_MANAGER}ScreenManager.cp")

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(screen STATIC ${sources})

target_include_directories(screen PUBLIC
    ../
    ../JIT/
    ../JIT/Generic/
    ../../
)

target_link_libraries(screen ${LINKLIBS} k)
