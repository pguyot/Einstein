#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
  Emulator/Serial/CMakeLists.txt
)

list ( APPEND common_sources
  Emulator/Serial/TTcpClientSerialPortManager.cpp
	Emulator/Serial/TTcpClientSerialPortManager.h
  Emulator/Serial/TBasicSerialPortManager.cpp
	Emulator/Serial/TBasicSerialPortManager.h
	Emulator/Serial/TSerialPortManager.cpp
	Emulator/Serial/TSerialPortManager.h
	Emulator/Serial/TSerialPorts.cpp
	Emulator/Serial/TSerialPorts.h
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	list (APPEND common_sources
		Emulator/Serial/TBasiliskIISerialPortManager.cpp
		Emulator/Serial/TBasiliskIISerialPortManager.h
		Emulator/Serial/TPipesSerialPortManager.cpp
		Emulator/Serial/TPipesSerialPortManager.h
		Emulator/Serial/TPtySerialPortManager.cpp
		Emulator/Serial/TPtySerialPortManager.h
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	list (APPEND common_sources
    Emulator/Serial/TBasiliskIISerialPortManager.cpp
    Emulator/Serial/TBasiliskIISerialPortManager.h
    Emulator/Serial/TPipesSerialPortManager.cpp
    Emulator/Serial/TPipesSerialPortManager.h
    Emulator/Serial/TPtySerialPortManager.cpp
    Emulator/Serial/TPtySerialPortManager.h
  )
elseif (WIN32)
  # no additional drivers
endif ()
