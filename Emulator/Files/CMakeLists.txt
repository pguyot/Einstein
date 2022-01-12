#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
  Emulator/Files/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Files/TFileManager.cpp
	Emulator/Files/TFileManager.h
)
