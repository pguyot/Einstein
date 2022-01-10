#
# Add source files required to buid Einstein.
#

target_sources ( Einstein PRIVATE
	Errors/TError.cpp
	Errors/TError.h
	Errors/TMemError.cpp
	Errors/TMemError.h
	IO/TEOFException.cpp
	IO/TEOFException.h
	IO/TIOException.cpp
	IO/TIOException.h
	TException.cpp
	TException.h
	CMakeLists.txt
)


