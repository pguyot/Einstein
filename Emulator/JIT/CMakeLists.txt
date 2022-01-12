#
# Add source files required to build Einstein.
#

include ( Emulator/JIT/Generic/CMakeLists.txt )

list ( APPEND cmake_sources
  Emulator/JIT/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/JIT/JIT.h
	Emulator/JIT/TJIT.h
	Emulator/JIT/TJITCache.cpp
	Emulator/JIT/TJITCache.h
	Emulator/JIT/TJITPage.cpp
	Emulator/JIT/TJITPage.h
	Emulator/JIT/TJITPerformance.cpp
	Emulator/JIT/TJITPerformance.h
)
