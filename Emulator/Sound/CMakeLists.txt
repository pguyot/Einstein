#
# Add source files required to build Einstein.
#

list ( APPEND cmake_sources
	Emulator/Sound/CMakeLists.txt
)

list ( APPEND common_sources
	Emulator/Sound/TBufferedSoundManager.cpp
	Emulator/Sound/TBufferedSoundManager.h
	Emulator/Sound/TNullSoundManager.cpp
	Emulator/Sound/TNullSoundManager.h
	Emulator/Sound/TSoundManager.cpp
	Emulator/Sound/TSoundManager.h
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	list (APPEND app_sources
		Emulator/Sound/TCoreAudioSoundManager.cpp
		Emulator/Sound/TCoreAudioSoundManager.h
	)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD")
	list (APPEND app_sources
		Emulator/Sound/TPulseAudioSoundManager.cpp
		Emulator/Sound/TPulseAudioSoundManager.h
	)
elseif (WIN32)
	list (APPEND app_sources
		Emulator/Sound/TWaveSoundManager.cpp
		Emulator/Sound/TWaveSoundManager.h
	)
endif ()
