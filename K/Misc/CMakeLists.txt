#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	K/Misc/CMakeLists.txt
)

list ( APPEND common_sources
	K/Misc/CRC32.h
	K/Misc/CRC32.cpp
	K/Misc/RelocHack.h
	K/Misc/TCircleBuffer.cpp
	K/Misc/TCircleBuffer.h
	K/Misc/TDoubleLinkedList.h
	K/Misc/TMappedFile.cpp
	K/Misc/TMappedFile.h
)
