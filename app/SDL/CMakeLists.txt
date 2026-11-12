#
# Add source files required to build the FLTK variant of Einstein.
#

list ( APPEND cmake_sources
	app/SDL/CMakeLists.txt
)

list ( APPEND common_sources
)

list ( APPEND app_sources
	app/SDL/TSDLApp.cpp
	app/SDL/TSDLApp.h
	app/SDL/TSDLSoundManager.cpp
	app/SDL/TSDLSoundManager.h
	app/SDL/TSDLRexImage.h
	app/SDL/TSDLNE2KImage.h
)

if ( ANDROID )
list ( APPEND app_sources
	app/SDL/TSDLAndroid.cpp
	app/SDL/TSDLAndroid.h
)
endif ()

embed_resource ( _Data_/Einstein.rex app/SDL/TSDLRexImage.h gEinsteinRexImage )
# NE2K.pkg, the driver of the network card, installed from the menu.
embed_resource ( _Data_/NE2K.pkg app/SDL/TSDLNE2KImage.h gNE2KPackage )
