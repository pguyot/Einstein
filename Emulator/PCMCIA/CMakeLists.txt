#
# Add source files required to build Einstein.
#

target_sources ( Einstein PRIVATE
	TATACard.cpp
	TATACard.h
	TLinearCard.cpp
	TLinearCard.h
	TNE2000Card.cpp
	TNE2000Card.h
	TPCMCIACard.cpp
	TPCMCIACard.h
	TPCMCIAController.cpp
	TPCMCIAController.h
	CMakeLists.txt
)

