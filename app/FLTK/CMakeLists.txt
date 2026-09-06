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
	${CMAKE_BINARY_DIR}/app/FLTK/TFLRexImage.cpp
	${CMAKE_BINARY_DIR}/app/FLTK/TFLRexImage.h
	${CMAKE_BINARY_DIR}/app/FLTK/TFLAppUI.cpp
	${CMAKE_BINARY_DIR}/app/FLTK/TFLAppUI.h
	${CMAKE_BINARY_DIR}/app/FLTK/TFLSettingsUI.cpp
	${CMAKE_BINARY_DIR}/app/FLTK/TFLSettingsUI.h
)

build_with_fluid( TFLAppUI      app/FLTK "app/Version_CMake.h" )
build_with_fluid( TFLRexImage   app/FLTK "_Data_/Einstein.rex" )
build_with_fluid( TFLSettingsUI app/FLTK )

