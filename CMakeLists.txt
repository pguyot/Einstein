cmake_minimum_required(VERSION 3.11)

project(einstein)

set(INCLUDEPKGS TestBigEndian FindPkgConfig CheckLibraryExists)
foreach(pname IN LISTS INCLUDEPKGS)
    include(${pname})
endforeach()

option(portaudio "PortAudio output support" OFF)
option(nativeportaudio "PortAudio (native library) support" OFF)
option(pulseaudio "PulseAudio output support" ON)
option(armlejit "Use ARMLE JIT" OFF)
option(appX11 "X11+CLI application" ON)
option(appFLTK "FLTK application" OFF)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

# Release / Debug compile flags.
set(_FLAGS_DEBUG "-g -O0 -DDEBUG -DDEBUG_SOUND -DPA_ENABLE_DEBUG_OUTPUT -DHAS_C99_LONGLONG")
set(_FLAGS_RELEASE "-O3 -DRELEASE -DHAS_C99_LONGLONG")

set(CMAKE_CXX_FLAGS_RELEASE "${_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELEASE "${_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_DEBUG "${_FLAGS_DEBUG}")

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wwrite-strings -Wno-multichar -Wfloat-equal -Wshadow -Wpointer-arith -Wconversion")
endif()

set(LINKLIBS)

find_library(M_LIB m)
list(APPEND LINKLIBS ${M_LIB})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

    set(CMAKE_THREAD_PREFER_PTHREAD)
    find_package(Threads)
    if (Threads_FOUND)
        list(APPEND LINKLIBS pthread)
    else()
        message(FATAL_ERROR "Missing library: pthread")
    endif()


    find_library(FFI_LIB ffi)
    find_library(DL_LIB dl)
    list(APPEND LINKLIBS ${DL_LIB} ${PTHREAD_LIB} ${FFI_LIB})
endif()

TEST_BIG_ENDIAN(_IS_BIG_ENDIAN)
add_definitions(-DTARGET_RT_BIG_ENDIAN=$<BOOL:${_IS_BIG_ENDIAN}> -DTARGET_RT_LITTLE_ENDIAN=$<NOT:${_IS_BIG_ENDIAN}>)

# K Libs
add_subdirectory(K)

###########################
# Einstein library code

## 1) Gather all common sources

file(GLOB elib_common_sources "Emulator/*.cp")
# file(GLOB elib_Monitor_sources "Monitor/*.cp")

set_source_files_properties(${elib_common_sources} PROPERTIES LANGUAGE CXX)
# set_source_files_properties(${elib_Monitor_sources} PROPERTIES LANGUAGE CXX)

# Add "einsteinlib" as a library target
add_library(einsteinlib STATIC ${elib_common_sources})

# add include directories ... do I need this?
target_include_directories(einsteinlib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:einsteinlib>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/Emulator
    $<INSTALL_INTERFACE:einsteinlib>/Emulator
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/Emulator/JIT/Generic
    $<INSTALL_INTERFACE:einsteinlib>/Emulator/JIT/Generic
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/Monitor/
    $<INSTALL_INTERFACE:einsteinlib>/Monitor/
)

# Platform-specific stuff
if(${WIN32})
    target_sources(einsteinlib PUBLIC "Emulator/Win32/CompatibilityWin32.cp")
    set(EINSTEIN_TARGET "WIN32")
    set(EINSTEIN_SCREEN_MANAGER "FL")

    target_include_directories(einsteinlib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/Emulator/Win32
        $<INSTALL_INTERFACE:einsteinlib>/Emulator/Win32
    )
    set_source_files_properties("Emulator/Win32/CompatibilityWin32.cp" PROPERTIES LANGUAGE CXX)
elseif(${UNIX})

    if (${APPLE})
        # FIXME OS X is called OPENSTEP ...
        set(EINSTEIN_TARGET "OPENSTEP")
        # FIXME add OS X sources here.
        set(EINSTEIN_SCREEN_MANAGER "Cocoa")
    else()
        if (appX11)
            set(EINSTEIN_SCREEN_MANAGER "X11")
        endif()

        if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "BSD")
            set(EINSTEIN_TARGET "BSD")

        else()
            set(EINSTEIN_TARGET "LINUX")
        endif()
    endif()
endif()

add_definitions(-DTARGET_OS_${EINSTEIN_TARGET})

if (portaudio OR nativeportaudio)
    add_definitions("-DAUDIO_PORTAUDIO=1")
endif()

if (pulseaudio)
    add_definitions("-DAUDIO_PULSEAUDIO=1")
endif()

# Add the Emulator/ sub-dir, which has CMakeLists all the way down
add_subdirectory(Emulator)
add_subdirectory(Monitor)

install(TARGETS einsteinlib DESTINATION lib/${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})

target_link_libraries(einsteinlib monitor files host jit log nativecalls network pcmcia platform rom screen serial sound ${LINKLIBS} k)


############################
# EINSTEIN EMULATOR
add_executable(einstein app/einstein.cp app/TCLIApp.cp)

install(TARGETS einstein
DESTINATION bin/${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})

set_source_files_properties(app/einstein.cp app/TCLIApp.cp PROPERTIES LANGUAGE CXX)
target_link_libraries(einstein k einsteinlib)
