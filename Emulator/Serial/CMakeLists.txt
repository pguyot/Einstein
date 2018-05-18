# Serial CMakeLists.txt

# Only this one compiles for now ...
set(sources TSerialPortManager.cp)

# TODO virtual serial port

if(${UNIX})
    if (${APPLE})
        set (sources ${sources} TBasicSerialPortManager.cp)
    endif()
endif()


set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(serial STATIC ${sources})

target_include_directories(serial PUBLIC
    ../
    ../../
    ../../app/
    )

target_link_libraries(serial k)
