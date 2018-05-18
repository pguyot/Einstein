# Sound CMakeLists.txt

# SOUND STUFF

set(Sound_sources_basic
"TSoundManager.cp"
"TBufferedSoundManager.cp"
"TNullSoundManager.cp"
)
set_source_files_properties(${Sound_sources_basic} PROPERTIES LANGUAGE CXX)

if (pulseaudio)
    find_library(PULSEAUDIO_LIB pulse)
    list(APPEND LINKLIBS ${PULSEAUDIO_LIB})

    set(Sound_sources_addl_cpp "TPulseAudioSoundManager.cp")
    add_definitions("-DAUDIO_PULSEAUDIO=1")

endif()

if (portaudio OR nativeportaudio)

    set(Sound_sources_addl_cpp "TPortAudioSoundManager.cp")

    add_definitions("-DAUDIO_PORTAUDIO=1")

    if(nativeportaudio)
        # find the native portaudio library
        find_library(PORTAUDIO_LIB portaudio)
        list(APPEND LINKLIBS ${PORTAUDIO_LIB})
    elseif(portaudio)

        # include portaudio from the tree.
        set(Sound_PA_include "${CMAKE_SOURCE_DIR}/portaudio/pa_common/" "${CMAKE_SOURCE_DIR}/portaudio/pa_unix/")
        file(GLOB Sound_PA "${CMAKE_SOURCE_DIR}/portaudio/pa_common/*.c")
        set(Sound_sources_addl_c ${Sound_PA}
            "${CMAKE_SOURCE_DIR}/portaudio/pa_linux_alsa/pa_linux_alsa.c"
            "${CMAKE_SOURCE_DIR}/portaudio/pa_unix/pa_unix_util.c"
            "${CMAKE_SOURCE_DIR}/portaudio/pa_unix/pa_unix_hostapis.c")

    endif()
endif()

set_source_files_properties(${Sound_sources_addl_cpp} PROPERTIES LANGUAGE CXX)

add_library(sound STATIC ${Sound_sources_basic} ${Sound_sources_addl_cpp} ${Sound_sources_addl_c})

target_include_directories(sound PUBLIC
    ${Sound_PA_include}
    ../
    ../JIT/
    ../JIT/Generic/
    ../../
)

target_link_libraries(sound ${LINKLIBS} k)
