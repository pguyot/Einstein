#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
  Emulator/NativeCalls/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/NativeCalls/TVirtualizedCalls.cpp
	Emulator/NativeCalls/TVirtualizedCalls.h
	Emulator/NativeCalls/TVirtualizedCallsPatches.cpp
	Emulator/NativeCalls/TVirtualizedCallsPatches.h
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	list (APPEND common_sources
		Emulator/NativeCalls/TObjCBridgeCalls.mm
		Emulator/NativeCalls/TObjCBridgeCalls.h
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	list (APPEND common_sources
			Emulator/NativeCalls/NativeCallsDefines.h
			Emulator/NativeCalls/TNativeCalls.cpp
			Emulator/NativeCalls/TNativeCalls.h
  )
elseif (WIN32)
	list (APPEND common_sources
		Emulator/NativeCalls/NativeCallsDefines.h
		Emulator/NativeCalls/TNativeCalls.cpp
		Emulator/NativeCalls/TNativeCalls.h
	)
endif ()
