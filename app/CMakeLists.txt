#
# At this time we only support CMake builds for the FLTK variant of Einstein.
#

if ( EINSTEIN_FLTK_FRONTEND )
	include ( app/FLTK/CMakeLists.txt )
endif ()

if ( EINSTEIN_SDL3_FRONTEND )
	include ( app/SDL/CMakeLists.txt )
endif ()

list ( APPEND cmake_sources
	app/CMakeLists.txt
)

list ( APPEND common_sources
	app/TPathHelper.cpp
	app/TPathHelper.h
)

list ( APPEND common_sources
	app/Version.h
	app/Version_CMake.h.in
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	list (APPEND common_sources
		app/TCocoaPathHelper.mm
		app/TCocoaPathHelper.h
	)
endif()
