#
# Add source files required to build Einstein.
#

target_sources ( Einstein PRIVATE
	TAIFFile.cpp
	TAIFFile.h
	TAIFROMImageWithREXes.cpp
	TAIFROMImageWithREXes.h
	TFlatROMImageWithREX.cpp
	TFlatROMImageWithREX.h
	TROMImage.cpp
	TROMImage.h
	CMakeLists.txt
)

