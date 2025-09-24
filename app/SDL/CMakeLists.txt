#
# Add source files required to build the FLTK variant of Einstein.
#

list ( APPEND cmake_sources
	app/SDL/CMakeLists.txt
)

list ( APPEND common_sources
)

list ( APPEND app_sources
	app/SDL/TSDLMain.cpp
	app/SDL/TSDLMain.h
)

