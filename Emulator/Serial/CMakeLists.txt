#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/Serial/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Serial/TSerialPortDriverTcpClient.cpp
	Emulator/Serial/TSerialPortDriverTcpClient.h
	Emulator/Serial/TBasicSerialPortManager.cpp
	Emulator/Serial/TBasicSerialPortManager.h
	Emulator/Serial/TSerialPortManager.cpp
	Emulator/Serial/TSerialPortManager.h
	Emulator/Serial/TSerialPorts.cpp
	Emulator/Serial/TSerialPorts.h
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	list (APPEND common_sources
		Emulator/Serial/TSerialHostPort.h
		Emulator/Serial/TSerialHostPortDirect.h
		Emulator/Serial/TSerialHostPortDirect.cpp
		Emulator/Serial/TSerialHostPortPTY.h
		Emulator/Serial/TSerialHostPortPTY.cpp
		Emulator/Serial/TBasiliskIISerialPortManager.cpp
		Emulator/Serial/TBasiliskIISerialPortManager.h
		Emulator/Serial/TPipesSerialPortManager.cpp
		Emulator/Serial/TPipesSerialPortManager.h
		Emulator/Serial/TPtySerialPortManager.cpp
		Emulator/Serial/TPtySerialPortManager.h
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
	list (APPEND common_sources
		Emulator/Serial/TSerialHostPort.h
		Emulator/Serial/TSerialHostPortDirect.h
		Emulator/Serial/TSerialHostPortDirect.cpp
		Emulator/Serial/TSerialHostPortPTY.h
		Emulator/Serial/TSerialHostPortPTY.cpp
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
