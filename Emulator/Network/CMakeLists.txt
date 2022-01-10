#
# Add source files required to buid Einstein.
#

target_sources ( Einstein PRIVATE
	TNetworkManager.cpp
	TNetworkManager.h
	CMakeLists.txt
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  target_sources ( Einstein PRIVATE
    TTapNetwork.cpp
    TTapNetwork.h
		TUsermodeNetwork.cpp
		TUsermodeNetwork.h
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_sources ( Einstein PRIVATE
    TUsermodeNetwork.cpp
    TUsermodeNetwork.h
  )
elseif (WIN32)
  target_sources ( Einstein PRIVATE
		TUsermodeNetwork.cpp
		TUsermodeNetwork.h
	)
endif ()
