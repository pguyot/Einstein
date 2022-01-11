#
# Add source files required to build Einstein.
#

target_sources ( Einstein PRIVATE
	CMakeLists.txt
)

add_subdirectory(Defines)
add_subdirectory(Exceptions)
add_subdirectory(Misc)
add_subdirectory(Streams)
add_subdirectory(Threads)
add_subdirectory(Unicode)

