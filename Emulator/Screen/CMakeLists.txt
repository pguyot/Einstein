#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/Screen/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Screen/TNullScreenManager.cpp
	Emulator/Screen/TNullScreenManager.h
	Emulator/Screen/TScreenManager.cpp
	Emulator/Screen/TScreenManager.h
)

if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )

list ( APPEND app_sources
	Emulator/Screen/TFLScreenManager.cpp
	Emulator/Screen/TFLScreenManager.h
)

endif ()

if ( ${EINSTEIN_FRONTEND} STREQUAL "SDL" )

list ( APPEND app_sources
	Emulator/Screen/TSDLScreenManager.cpp
	Emulator/Screen/TSDLScreenManager.h
)

endif ()
