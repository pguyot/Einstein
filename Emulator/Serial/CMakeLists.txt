#
# Add source files required to buid Einstein.
#

target_sources ( Einstein PRIVATE
	TTcpClientSerialPortManager.cpp
	TTcpClientSerialPortManager.h
	TBasicSerialPortManager.cpp
	TBasicSerialPortManager.h
	TSerialPortManager.cpp
	TSerialPortManager.h
	TSerialPorts.cpp
	TSerialPorts.h
	CMakeLists.txt
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  target_sources ( Einstein PRIVATE
		TBasiliskIISerialPortManager.cpp
		TBasiliskIISerialPortManager.h
		TPipesSerialPortManager.cpp
		TPipesSerialPortManager.h
		TPtySerialPortManager.cpp
		TPtySerialPortManager.h
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_sources ( Einstein PRIVATE
		Emulator/Network/TUsermodeNetwork.cpp
		Emulator/Network/TUsermodeNetwork.h
    TBasiliskIISerialPortManager.cpp
    TBasiliskIISerialPortManager.h
    TPipesSerialPortManager.cpp
    TPipesSerialPortManager.h
    TPtySerialPortManager.cpp
    TPtySerialPortManager.h
  )
elseif (WIN32)
  # No additional sources
endif ()
