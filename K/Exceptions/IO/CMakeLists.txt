#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	K/Exceptions/IO/CMakeLists.txt
)

list ( APPEND common_sources
	K/Exceptions/IO/TEOFException.cpp
	K/Exceptions/IO/TEOFException.h
	K/Exceptions/IO/TIOException.cpp
	K/Exceptions/IO/TIOException.h
)

