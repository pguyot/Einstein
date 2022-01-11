#
# At this time we only support CMake builds for the FLTK variant of Einstein.
#

add_subdirectory( FLTK )

target_sources ( Einstein PRIVATE
    Version.h
	Version_CMake.h.in
	TPathHelper.cpp
	TPathHelper.h
	CMakeLists.txt
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	target_sources ( Einstein PRIVATE
		TCocoaPathHelper.mm
		TCocoaPathHelper.h
	)
endif()
