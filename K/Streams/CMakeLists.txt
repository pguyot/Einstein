#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	K/Streams/CMakeLists.txt
)

list ( APPEND common_sources
	K/Streams/TFileStream.cpp
	K/Streams/TFileStream.h
	K/Streams/TRandomAccessStream.h
	K/Streams/TStream.cpp
	K/Streams/TStream.h
)
