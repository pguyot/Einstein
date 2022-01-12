#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	K/Threads/CMakeLists.txt
)

list ( APPEND common_sources
	K/Threads/TCondVar.cpp
	K/Threads/TCondVar.h
	K/Threads/TMutex.cpp
	K/Threads/TMutex.h
	K/Threads/TThread.cpp
	K/Threads/TThread.h
)
