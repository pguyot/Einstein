#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/Network/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Network/TNetworkManager.cpp
	Emulator/Network/TNetworkManager.h
	Emulator/Network/TUsermodeNetwork.cpp
	Emulator/Network/TUsermodeNetwork.h
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	list (APPEND app_sources
		Emulator/Network/TTapNetwork.cpp
		Emulator/Network/TTapNetwork.h
	)
endif ()
