#
# Add source files required to build Einstein.
#

include ( K/Defines/CMakeLists.txt )
include ( K/Exceptions/CMakeLists.txt )
include ( K/Misc/CMakeLists.txt )
include ( K/Streams/CMakeLists.txt )
include ( K/Threads/CMakeLists.txt )
include ( K/Unicode/CMakeLists.txt )

list ( APPEND cmake_sources
  K/CMakeLists.txt
)
