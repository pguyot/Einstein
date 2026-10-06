#
# Add source files required to build the FLTK variant of Einstein.
#

list ( APPEND cmake_sources
	app/FLTK/CMakeLists.txt
)

list ( APPEND common_sources
)

list ( APPEND app_sources
	app/FLTK/TFLApp.cpp
	app/FLTK/TFLApp.h
	app/FLTK/TFLSettings.cpp
	app/FLTK/TFLSettings.h
	app/FLTK/TFLAppWindow.cpp
	app/FLTK/TFLAppWindow.h
	app/FLTK/TFLTabs.cpp
	app/FLTK/TFLTabs.h
)

list ( APPEND fluid_sources
	app/FLTK/TFLRexImage.cpp
	app/FLTK/TFLRexImage.h
	app/FLTK/TFLAppUI.fl
	app/FLTK/TFLAppUI.cpp
	app/FLTK/TFLAppUI.h
	app/FLTK/TFLSettingsUI.fl
	app/FLTK/TFLSettingsUI.cpp
	app/FLTK/TFLSettingsUI.h
)

check_fluid_sources( TFLAppUI      app/FLTK )
check_fluid_sources( TFLSettingsUI app/FLTK )

# Embed Einstein.rex into the executable as a byte array
add_custom_command (
	OUTPUT "${CMAKE_SOURCE_DIR}/app/FLTK/TFLRexImage.cpp"
	       "${CMAKE_SOURCE_DIR}/app/FLTK/TFLRexImage.h"
	COMMAND ${CMAKE_COMMAND}
		-D INPUT=${CMAKE_SOURCE_DIR}/_Data_/Einstein.rex
		-D OUTPUT_CPP=${CMAKE_SOURCE_DIR}/app/FLTK/TFLRexImage.cpp
		-D OUTPUT_H=${CMAKE_SOURCE_DIR}/app/FLTK/TFLRexImage.h
		-D NAME=gEinsteinRexImage
		-P ${CMAKE_SOURCE_DIR}/cmake/EmbedResource.cmake
	DEPENDS "${CMAKE_SOURCE_DIR}/_Data_/Einstein.rex"
	        "${CMAKE_SOURCE_DIR}/cmake/EmbedResource.cmake"
	COMMENT "Embedding _Data_/Einstein.rex"
	VERBATIM
)

