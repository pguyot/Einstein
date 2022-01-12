#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	K/Exceptions/Errors/CMakeLists.txt
)

list ( APPEND common_sources
	K/Exceptions/Errors/TError.cpp
	K/Exceptions/Errors/TError.h
	K/Exceptions/Errors/TMemError.cpp
	K/Exceptions/Errors/TMemError.h
)
