#
# Add source files required to buid Einstein.
#

target_sources ( Einstein PRIVATE
	JIT.h
	TJIT.h
	TJITCache.cpp
	TJITCache.h
	TJITPage.cpp
	TJITPage.h
	TJITPerformance.cpp
	TJITPerformance.h
	CMakeLists.txt
)

add_subdirectory(Generic)

