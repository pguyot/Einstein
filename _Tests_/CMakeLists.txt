#
# Add source files required to build Einstein Tests.
#

list ( APPEND cmake_sources
	_Tests_/CMakeLists.txt
)

list ( APPEND test_sources
	_Tests_/EinsteinTests.cpp
	_Tests_/ExecuteInstructionTests.t
	_Tests_/UProcessorTests.cpp
	_Tests_/UProcessorTests.h
)
