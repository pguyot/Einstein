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

