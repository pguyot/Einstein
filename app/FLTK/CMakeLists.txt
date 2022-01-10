#
# Add source files required to buid the FLTK variant of Einstein.
#

target_sources ( Einstein PRIVATE
	TFLApp.cpp
	TFLApp.h
	TFLAppUI.fl
	TFLAppUI.cpp
	TFLAppUI.h
	TFLRexImage.fl
	TFLRexImage.cpp
	TFLRexImage.h
	TFLSettingsUI.fl
	TFLSettingsUI.cpp
	TFLSettingsUI.h
	TFLSettings.cpp
	TFLSettings.h
	TFLAppWindow.cpp
	TFLAppWindow.h
	TFLTabs.cpp
	TFLTabs.h
	CMakeLists.txt
)

build_with_fluid(TFLAppUI ${CMAKE_CURRENT_LIST_DIR})
build_with_fluid(TFLRexImage ${CMAKE_CURRENT_LIST_DIR})
build_with_fluid(TFLSettingsUI ${CMAKE_CURRENT_LIST_DIR})

