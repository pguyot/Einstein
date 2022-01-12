#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/Log/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Log/TBufferLog.cpp
	Emulator/Log/TBufferLog.h
	Emulator/Log/TFileLog.cpp
	Emulator/Log/TFileLog.h
	Emulator/Log/TLog.cpp
	Emulator/Log/TLog.h
	Emulator/Log/TRAMLog.cpp
	Emulator/Log/TRAMLog.h
	Emulator/Log/TStdOutLog.cpp
	Emulator/Log/TStdOutLog.h
)
