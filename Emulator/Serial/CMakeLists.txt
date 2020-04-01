# Serial CMakeLists.txt

set(sources 
	TSerialPorts.cp 
	TSerialPortManager.cp
        TBasicSerialPortManager.cp 
  	TPipesSerialPortManager.cp
  	TTcpClientSerialPortManager.cp
	TBasiliskIISerialPortManager.cp  
	TPtySerialPortManager.cp
	)


set_source_files_properties(${sources} PROPERTIES LANGUAGE CXX)

add_library(serial STATIC ${sources})

target_include_directories(serial PUBLIC
    ../
    ../../
    ../../app/
    )

target_link_libraries(serial k)
