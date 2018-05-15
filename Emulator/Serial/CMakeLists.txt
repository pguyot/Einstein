# Serial CMakeLists.txt

# Only this one compiles for now ...
set(sources TVoyagerSerialPort.cp)

# TODO virtual serial port

set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(serial STATIC ${sources})

target_include_directories(serial PUBLIC
    ../
    ../../
    )

target_link_libraries(serial k)
