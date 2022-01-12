#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
  Emulator/Host/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Host/THostInfo.cpp
	Emulator/Host/THostInfo.h
	Emulator/Host/UserInfoDefinitions.h
)

