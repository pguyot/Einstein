#
# Add source files required to build the FLTK variant of Einstein.
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

# Fluid custom builds must be defined in main file before CMake 3.18 because
# we need to set source file properties which are local to this CMake file

