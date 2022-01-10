#
# Add source files required to buid Einstein.
#

target_sources ( Einstein PRIVATE
	TVirtualizedCalls.cpp
	TVirtualizedCalls.h
	TVirtualizedCallsPatches.cpp
	TVirtualizedCallsPatches.h
	CMakeLists.txt
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_sources ( Einstein PRIVATE
		TObjCBridgeCalls.mm
		TObjCBridgeCalls.h
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	target_sources ( Einstein PRIVATE
		NativeCallsDefines.h
		TNativeCalls.cpp
		TNativeCalls.h
	)
elseif (WIN32)
	target_sources ( Einstein PRIVATE
		NativeCallsDefines.h
		TNativeCalls.cpp
		TNativeCalls.h
	)
endif ()
