#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/PCMCIA/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/PCMCIA/TATACard.cpp
	Emulator/PCMCIA/TATACard.h
	Emulator/PCMCIA/TLinearCard.cpp
	Emulator/PCMCIA/TLinearCard.h
	Emulator/PCMCIA/TNE2000Card.cpp
	Emulator/PCMCIA/TNE2000Card.h
	Emulator/PCMCIA/TPCMCIACard.cpp
	Emulator/PCMCIA/TPCMCIACard.h
	Emulator/PCMCIA/TPCMCIAController.cpp
	Emulator/PCMCIA/TPCMCIAController.h
)
