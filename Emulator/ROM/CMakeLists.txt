#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
  Emulator/ROM/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/ROM/TAIFFile.cpp
	Emulator/ROM/TAIFFile.h
	Emulator/ROM/TAIFROMImageWithREXes.cpp
	Emulator/ROM/TAIFROMImageWithREXes.h
	Emulator/ROM/TFlatROMImageWithREX.cpp
	Emulator/ROM/TFlatROMImageWithREX.h
	Emulator/ROM/TROMImage.cpp
	Emulator/ROM/TROMImage.h
)
