#
# Add source files required to buid Einstein.
#

target_sources ( Einstein PRIVATE
	TBufferedSoundManager.cpp
	TBufferedSoundManager.h
	TNullSoundManager.cpp
	TNullSoundManager.h
	TSoundManager.cpp
	TSoundManager.h
	CMakeLists.txt
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
  target_sources ( Einstein PRIVATE
    TCoreAudioSoundManager.cpp
		TCoreAudioSoundManager.h
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_sources ( Einstein PRIVATE
    TPulseAudioSoundManager.cpp
    TPulseAudioSoundManager.h
  )
elseif (WIN32)
  target_sources ( Einstein PRIVATE
		TWaveSoundManager.cpp
		TWaveSoundManager.h
	)
endif ()
