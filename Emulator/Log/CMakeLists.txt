#
# Add source files required to buid Einstein.
#

target_sources ( Einstein PRIVATE
	TBufferLog.cpp
	TBufferLog.h
	TFileLog.cpp
	TFileLog.h
	TLog.cpp
	TLog.h
	TRAMLog.cpp
	TRAMLog.h
	TStdOutLog.cpp
	TStdOutLog.h
	CMakeLists.txt
)

