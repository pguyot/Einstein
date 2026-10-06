#
# EinsteinTests: unit tests, mainly for the JIT compiler.
#
# This file is only included if EINSTEIN_BUILD_TESTS is ON, so normal builds of
# Einstein don't download googletest or compile the tests. It is included at the
# end of the main CMakeLists.txt, after the Einstein and EinsteinFLGUI targets
# exist.
#

# ---- Setup Google Testing

FetchContent_Declare (
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set( gtest_force_shared_crt ON CACHE BOOL "" FORCE )

# Check if population has already been performed
FetchContent_GetProperties(googletest)
if ( NOT googletest_POPULATED )
	# Fetch the content using previously declared details
	FetchContent_MakeAvailable(googletest)
endif ()

enable_testing()

include( GoogleTest )

# ---- Platform specific rules

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )

	add_executable ( EinsteinTests
		${common_sources}
		${test_sources}
		${cmake_sources}
	)
	target_compile_options( EinsteinTests PUBLIC
		-Wall -Wextra -Wpedantic -Wno-missing-field-initializers -Werror
	)
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1
		TARGET_OS_MAC=1 NS_BLOCK_ASSERTIONS=1
	)
	if ( EINSTEIN_FLTK_FRONTEND )
		target_link_libraries ( EinsteinTests
			${system_libs}
			fltk::fltk
			EinsteinFLGUI
			pthread
			"-framework AddressBook"
			"-framework AudioUnit"
			"-framework AppKit"
			"-framework CoreAudio"
			"-framework Cocoa"
		)
	endif ()

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" )

	add_executable ( EinsteinTests
		${common_sources}
		${test_sources}
		${cmake_sources}
	)
	target_compile_options ( EinsteinTests PUBLIC
		-Wall -Wno-multichar -Wno-misleading-indentation -Wno-unused-result
		-Wno-missing-field-initializers -Wno-stringop-truncation -Werror
	)
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_LINUX=1
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		fltk::fltk
		EinsteinFLGUI
		pthread
	)

elseif ( WIN32 )

	add_executable ( EinsteinTests
		${common_sources} ${test_sources}
	)
	target_compile_options( EinsteinTests PUBLIC "/bigobj" )
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_WIN32=1
		WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		fltk::fltk
		EinsteinFLGUI
		gdiplus
	)

endif ()

# ---- Platform independent rules

target_include_directories (
	EinsteinTests PUBLIC
	${CMAKE_SOURCE_DIR}
	${FLTK_INCLUDE_DIRS}
)
target_compile_definitions ( EinsteinTests PUBLIC "$<$<CONFIG:DEBUG>:_DEBUG>" USE_CMAKE )

target_link_libraries ( EinsteinTests gtest_main )

gtest_discover_tests ( EinsteinTests )
