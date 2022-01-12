#
# Add source files required to build Einstein.
#

include ( K/Exceptions/Errors/CMakeLists.txt )
include ( K/Exceptions/IO/CMakeLists.txt )

list ( APPEND cmake_sources
  K/Exceptions/CMakeLists.txt
)

list ( APPEND common_sources
	K/Exceptions/TException.cpp
	K/Exceptions/TException.h
)
