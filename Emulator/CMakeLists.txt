#
# Add source files required to build Einstein.
#

include ( Emulator/Files/CMakeLists.txt )
include ( Emulator/Host/CMakeLists.txt )
include ( Emulator/JIT/CMakeLists.txt )
include ( Emulator/Log/CMakeLists.txt )
include ( Emulator/NativeCalls/CMakeLists.txt )
include ( Emulator/Network/CMakeLists.txt )
include ( Emulator/PCMCIA/CMakeLists.txt )
include ( Emulator/Platform/CMakeLists.txt )
include ( Emulator/ROM/CMakeLists.txt )
include ( Emulator/Screen/CMakeLists.txt )
include ( Emulator/Serial/CMakeLists.txt )
include ( Emulator/Sound/CMakeLists.txt )

list ( APPEND cmake_sources
	Emulator/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/TARMProcessor.cpp
	Emulator/TARMProcessor.h
	Emulator/TDMAManager.cpp
	Emulator/TDMAManager.h
	Emulator/TEmulator.cpp
	Emulator/TEmulator.h
	Emulator/TFlash.cpp
	Emulator/TFlash.h
	Emulator/THashMapCache.h
	Emulator/TInterruptManager.cpp
	Emulator/TInterruptManager.h
	Emulator/TMMU.cpp
	Emulator/TMMU.h
	Emulator/TMemory.cpp
	Emulator/TMemory.h
	Emulator/TMemoryConsts.h
	Emulator/TNativePrimitives.cpp
	Emulator/TNativePrimitives.h
)
