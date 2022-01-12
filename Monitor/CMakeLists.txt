#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
  Monitor/CMakeLists.txt
)

list ( APPEND common_sources
	Monitor/TSymbolList.cpp
	Monitor/TSymbolList.h
	Monitor/UDisasm.cpp
	Monitor/UDisasm.h
)

list ( APPEND app_sources
	Monitor/TFLMonitor.cpp
	Monitor/TFLMonitor.h
	Monitor/TMonitor.cpp
	Monitor/TMonitor.h
	Monitor/TMonitorCore.cpp
	Monitor/TMonitorCore.h
)
