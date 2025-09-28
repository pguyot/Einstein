#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/Printer/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Printer/TPrinterManager.cpp
	Emulator/Printer/TPrinterManager.h
)

if ( EINSTEIN_FLTK_FRONTEND )

list ( APPEND app_sources
	Emulator/Printer/TFLPrinterManager.cpp
	Emulator/Printer/TFLPrinterManager.h
)

endif ()
