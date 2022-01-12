#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/Platform/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Platform/PlatformEvents.h
	Emulator/Platform/PlatformGestalt.h
	Emulator/Platform/TNewt.cpp
	Emulator/Platform/TNewt.h
	Emulator/Platform/TPlatformManager.cpp
	Emulator/Platform/TPlatformManager.h
)
