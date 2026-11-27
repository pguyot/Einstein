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

if ( EINSTEIN_FLTK_FRONTEND OR EINSTEIN_HEADLESS_FRONTEND )
	if ( HAVE_COREAUDIO )
		list (APPEND app_sources
			Emulator/Sound/TCoreAudioSoundManager.cpp
			Emulator/Sound/TCoreAudioSoundManager.h
		)
	elseif ( HAVE_PULSEAUDIO )
		list (APPEND app_sources
			Emulator/Sound/TPulseAudioSoundManager.cpp
			Emulator/Sound/TPulseAudioSoundManager.h
		)
	elseif ( HAVE_WAVEOUT )
		list (APPEND app_sources
			Emulator/Sound/TWaveSoundManager.cpp
			Emulator/Sound/TWaveSoundManager.h
		)
	endif ()
endif ()
