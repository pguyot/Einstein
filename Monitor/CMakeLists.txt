#
# Add source files required to build Einstein.
#

target_sources ( Einstein PRIVATE
	TFLMonitor.cpp
	TFLMonitor.h
	TMonitor.cpp
	TMonitor.h
	TMonitorCore.cpp
	TMonitorCore.h
	TSymbolList.cpp
	TSymbolList.h
	UDisasm.cpp
	UDisasm.h
	CMakeLists.txt
)

