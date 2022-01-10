#
# Add source files required to buid Einstein.
#

target_sources ( Einstein PRIVATE
	TARMProcessor.cpp
	TARMProcessor.h
	TDMAManager.cpp
	TDMAManager.h
	TEmulator.cpp
	TEmulator.h
	TFlash.cpp
	TFlash.h
	THashMapCache.h
	TInterruptManager.cpp
	TInterruptManager.h
	TMMU.cpp
	TMMU.h
	TMemory.cpp
	TMemory.h
	TMemoryConsts.h
	TNativePrimitives.cpp
	TNativePrimitives.h
	CMakeLists.txt
)

add_subdirectory(Files)
add_subdirectory(Host)
add_subdirectory(JIT)
add_subdirectory(Log)
add_subdirectory(NativeCalls)
add_subdirectory(Network)
add_subdirectory(PCMCIA)
add_subdirectory(Platform)
add_subdirectory(ROM)
add_subdirectory(Screen)
add_subdirectory(Serial)
add_subdirectory(Sound)

