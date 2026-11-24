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
	Emulator/Serial/TSerialPortDriver.cpp
	Emulator/Serial/TSerialPortDriver.h
	Emulator/Serial/TSerialPorts.cpp
	Emulator/Serial/TSerialPorts.h
	Emulator/Serial/TSerialHostPort.h
	Emulator/Serial/TSerialHostPortTCPServer.h
	Emulator/Serial/TSerialHostPortTCPServer.cpp
)

if ( HAVE_PTY )
	list (APPEND common_sources
		Emulator/Serial/TSerialHostPortDirect.h
		Emulator/Serial/TSerialHostPortDirect.cpp
		Emulator/Serial/TSerialHostPortPTY.h
		Emulator/Serial/TSerialHostPortPTY.cpp
		Emulator/Serial/TSerialHostPortPTYBasilisk.h
		Emulator/Serial/TSerialHostPortPTYBasilisk.cpp
		Emulator/Serial/TBasiliskIISerialPortManager.cpp
		Emulator/Serial/TBasiliskIISerialPortManager.h
		Emulator/Serial/TPipesSerialPortManager.cpp
		Emulator/Serial/TPipesSerialPortManager.h
		Emulator/Serial/TPtySerialPortManager.cpp
		Emulator/Serial/TPtySerialPortManager.h
	)
endif ()
