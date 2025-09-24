# ==============================
# File:     CMakeLists.txt
# Project:  Einstein
#
# Copyright 2003-2022 by Paul Guyot (pguyot@kallisys.net) and others.
# ==============================

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

project( "Einstein" VERSION "2025.9.18" )

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

# ---- Configuration options

# Default to FLTK front end. The SDL3 option is under development.
option ( EINSTEIN_FRONTEND "Einstein GUI Frontend (FLTK, SDL)" FLTK )

# ---- Configuration for all targets on all platforms

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# ---- Include some additional functionality

include( CheckSymbolExists )
include( CheckFunctionExists )
include( CheckIncludeFile )
include( FetchContent )

# ---- Configuration per platform

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )

	# TODO: enable this for release builds
	#set( CMAKE_OSX_ARCHITECTURES arm64;x86_64 )
	# TODO: set this for FLTK and NEWT64 as well
	#set( CMAKE_OSX_DEPLOYMENT_TARGET 10.13 ) # Can't go any lower.
	# TODO: Fix this for release builds
	set( CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO" )
	set( CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "" )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

	# Linux: nothing to do here

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" )

	set_property ( GLOBAL PROPERTY FIND_LIBRARY_USE_OPENBSD_VERSIONING 1 )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

	# allow to build with multithreading libraries with VisualC
	#	cmake_policy( SET CMP0091 NEW )
	#	set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )

elseif ( ANDROID )

    message( "Configuring to build Einstein for Android." )
    set ( EINSTEIN_FRONTEND SDL )

else ()

	# Warn the caller that this is not supported
	message( FATAL_ERROR "Einstein is not configured to build on this platform: " ${CMAKE_SYSTEM_NAME} "." )

endif ()

#
# ==== External Dependencies ===================================================
#
# ---- Declare dependencies on outside content
#

FetchContent_Declare (
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )

FetchContent_Declare(
  FLTK
  GIT_REPOSITORY  https://github.com/fltk/fltk
  GIT_TAG         release-1.4.4
  GIT_SHALLOW     TRUE
)

FetchContent_Declare(
  NEWT64
  GIT_REPOSITORY  https://github.com/MatthiasWM/NEWT64
  GIT_TAG         master
  GIT_SHALLOW     TRUE
)

endif ()

if ( ${EINSTEIN_FRONTEND} STREQUAL "SDL" )

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    # GIT_TAG release-3.2.x
    GIT_TAG release-3.2.16
    # GIT_TAG "main"
    EXCLUDE_FROM_ALL
)

endif ()

#
# ---- Setup Google Testing
#

# For Windows: Prevent overriding the parent project's compiler/linker settings
set( gtest_force_shared_crt ON CACHE BOOL "" FORCE )

# Check if population has already been performed
FetchContent_GetProperties(googletest)
if ( NOT googletest_POPULATED )
	# Fetch the content using previously declared details
	FetchContent_MakeAvailable(googletest)

	# Bring the populated content into the build
	#add_subdirectory( ${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} )
endif ()

enable_testing()

include( GoogleTest )

#
# ---- Newt/64 library ---------------------------------------------------------
#

if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )

message(STATUS "Downloading and configuring NEWT64 if necessary, please wait...")
FetchContent_MakeAvailable(NEWT64)
message(STATUS "Downloading and configuring NEWT64 - done.")
# Newt64_BINARY_DIR
set( NEWT64_FOUND true )
set( newt64_incl_path "${Newt64_SOURCE_DIR}/src/newt_core/incs/" )
set( newt64_lib libnewt64 )

endif ()

#
# ---- FLTK library ------------------------------------------------------------
#

if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )

message(STATUS "Downloading and configuring FLTK if necessary, please wait...")
set( FLTK_USE_SYSTEM_LIBJPEG OFF )
set( FLTK_USE_SYSTEM_ZLIB OFF )
set( FLTK_USE_SYSTEM_LIBPNG OFF )
set( FLTK_BUILD_TEST OFF )
set( FLTK_BUILD_GL OFF )
set( FLTK_BUILD_FORMS OFF )
set( FLTK_BUILD_FLTK_OPTIONS OFF )
FetchContent_MakeAvailable(FLTK)
message(STATUS "Downloading and configuring FLTK - done.")

function ( build_with_fluid name dir )
	add_custom_command (
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${dir}"
		COMMAND fltk::fluid -c ${name}.fl
		DEPENDS "${CMAKE_SOURCE_DIR}/${dir}/${name}.fl"
		DEPENDS "${ARGN}"
		OUTPUT "${CMAKE_SOURCE_DIR}/${dir}/${name}.cpp"
		OUTPUT "${CMAKE_SOURCE_DIR}/${dir}/${name}.h"
	)
	set_source_files_properties (
		${CMAKE_SOURCE_DIR}/${dir}/${name}.cpp
		PROPERTIES GENERATED TRUE
	)
	set_source_files_properties (
		${CMAKE_SOURCE_DIR}/${dir}/${name}.h
		PROPERTIES GENERATED TRUE
	)
endfunction ()

endif ()

#
# ---- SDL3 library ------------------------------------------------------------
#

#if ( ${EINSTEIN_FRONTEND} STREQUAL "SDL" AND NOT ANDROID )
if ( ${EINSTEIN_FRONTEND} STREQUAL "SDL" )

message(STATUS "Downloading and configuring SDL3 if necessary, please wait...")

if ( ANDROID )
set( SDL_SHARED ON )
set( SDL_STATIC ON )
else ()
set( SDL_SHARED OFF )
set( SDL_STATIC ON )
endif ()
FetchContent_MakeAvailable(SDL3)
message(STATUS "Downloading and configuring SDL3 - done.")

endif ()

#
# ==== Configuring Einstein Build ==============================================
#
# ---- Einstein Source Files ---------------------------------------------------
#

set( cmake_sources )
set( common_sources )
set( app_sources )
set( test_sources )
set( fluid_sources )

# collect source code from the source code directory tree
include( app/CMakeLists.txt )
include( Emulator/CMakeLists.txt )
include( Monitor/CMakeLists.txt )
include( K/CMakeLists.txt )
if ( NEWT64_FOUND MATCHES true )
	include( Toolkit/CMakeLists.txt )
endif ()
include( _Tests_/CMakeLists.txt )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )
	# additional source code
	list ( APPEND app_sources
		# Apple macOS required resources
		Resources/macOS/Info.plist.in
		Resources/macOS/Einstein.icns
		Resources/macOS/Entitlements.plist
	)
endif ()

#
# ---- Common IDE setup --------------------------------------------------------
#

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
source_group( TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${cmake_sources} )
source_group( TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${common_sources} )
source_group( TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${app_sources} )
source_group( TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${test_sources} )
source_group( TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES ${fluid_sources} )
if ( WIN32 )
	source_group(TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES Resources/MSWindows/Einstein.rc.in)
endif ()

#
# ---- Platform specific rules ----------------------------------------------------------
#

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )

	# take care of the app icon
	set( MACOSX_BUNDLE_ICON_FILE "Einstein.icns" )
    set( MACOSX_BUNDLE_GUI_IDENTIFIER "org.messagepad.einstein" )
	set_source_files_properties (
		Resources/macOS/${MACOSX_BUNDLE_ICON_FILE}
		PROPERTIES MACOSX_PACKAGE_LOCATION "Resources"
	)

	# create the applications
	add_executable ( Einstein MACOSX_BUNDLE
		${common_sources}
		${app_sources}
		${cmake_sources}
	)
	add_executable ( EinsteinTests
		${common_sources}
		${test_sources}
		${cmake_sources}
	)
	if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
		add_library ( EinsteinFLGUI STATIC
			${fluid_sources}
		)
	endif()

	# how to compile and link
	target_compile_options( Einstein PUBLIC
		-Wall -Wextra -Wpedantic -Wno-missing-field-initializers -Werror
	)
	target_compile_options( EinsteinTests PUBLIC
		-Wall -Wextra -Wpedantic -Wno-missing-field-initializers -Werror
	)
	if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
		target_compile_options( EinsteinFLGUI PUBLIC
			-Wall -Wextra -Wpedantic -Wno-missing-field-initializers -Werror
		)
	endif ()
	if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
		target_compile_definitions ( Einstein PRIVATE
			TARGET_UI_FLTK=1 NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1
			TARGET_OS_MAC=1 NS_BLOCK_ASSERTIONS=1
		)
	endif ()
	if ( ${EINSTEIN_FRONTEND} STREQUAL "SDL" )
		target_compile_definitions ( Einstein PRIVATE
			TARGET_UI_SDL=1 NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1
			TARGET_OS_MAC=1 NS_BLOCK_ASSERTIONS=1
		)
	endif ()
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1
		TARGET_OS_MAC=1 NS_BLOCK_ASSERTIONS=1
	)
	if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
		target_compile_definitions ( EinsteinFLGUI PRIVATE
			TARGET_UI_FLTK=1 NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1
			TARGET_OS_MAC=1 NS_BLOCK_ASSERTIONS=1
		)
	endif ()
	target_link_libraries ( Einstein
		${system_libs}
		pthread
		"-framework AddressBook"
		"-framework AudioUnit"
		"-framework AppKit"
		"-framework CoreAudio"
		"-framework Cocoa"
	)
	if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
		target_link_libraries ( Einstein
			fltk::fltk fltk::images fltk::png fltk::z
			EinsteinFLGUI
		)
	endif ()
	if ( ${EINSTEIN_FRONTEND} STREQUAL "SDL" )
		target_link_libraries ( Einstein
			SDL3::SDL3-static
		)
	endif ()
	if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
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
	if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
		target_link_libraries ( EinsteinFLGUI
			fltk::fltk fltk::images fltk::png fltk::z
		)
  	endif ()

	set_target_properties ( Einstein PROPERTIES
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Resources/macOS/Info.plist.in"
	)
	set_target_properties ( Einstein PROPERTIES
		CPACK_BUNDLE_APPLE_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/Resources/macOS/Entitlements.plist"
	)
	if ( XCODE )
		# Fix indentation settings that are cleared by CMake at every generator call.
		add_custom_target ( FixTabSettings
			COMMAND ${CMAKE_HOME_DIRECTORY}/cmake/xcodeFixIndentation
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		)
	endif ()

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" OR ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" )

	# create the application
	add_executable ( Einstein
		${common_sources}
		${app_sources}
		${cmake_sources}
	)
	add_executable ( EinsteinTests
		${common_sources}
		${test_sources}
		${cmake_sources}
	)
	add_library ( EinsteinFLGUI STATIC
		${fluid_sources}
	)

	# how to compile and link
	target_compile_options ( Einstein PUBLIC
		-Wall -Wno-multichar -Wno-misleading-indentation -Wno-unused-result
		-Wno-missing-field-initializers -Wno-stringop-truncation # -Werror
		# Werror is disabled for testing purposes. Must reenable as soon as all Linux warnings are fixed.
	)
	target_compile_options ( EinsteinTests PUBLIC
		-Wall -Wno-multichar -Wno-misleading-indentation -Wno-unused-result
		-Wno-missing-field-initializers -Wno-stringop-truncation -Werror
	)
	target_compile_options ( EinsteinFLGUI PUBLIC
		-Wall -Wno-multichar -Wno-misleading-indentation -Wno-unused-result
		-Wno-missing-field-initializers -Wno-stringop-truncation -Werror
	)
	target_compile_definitions ( Einstein PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_LINUX=1
	)
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_LINUX=1
	)
	target_compile_definitions ( EinsteinFLGUI PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_LINUX=1
	)
	target_link_libraries ( Einstein
		${system_libs}
		fltk::fltk fltk::images fltk::png fltk::z
		EinsteinFLGUI
		pthread
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		fltk::fltk
		EinsteinFLGUI
		pthread
	)
	target_link_libraries ( EinsteinFLGUI
		fltk::fltk fltk::images fltk::png fltk::z
	)

	if ( ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" )
		# Under OpenBSD, libffi is in ports (i.e. /usr/local) not base (i.e. /usr)
		find_library ( ffi_lib NAMES ffi )
		find_file ( ffi_incl NAMES ffi.h )
		if ( ffi_lib MATCHES ".*NOTFOUND" OR ffi_incl MATCHES ".*NOTFOUND" )
			message ( FATAL_ERROR "libffi not found!" )
		else ()
			get_filename_component ( ffi_lib_path ${ffi_lib} DIRECTORY )
			get_filename_component ( ffi_incl_path ${ffi_incl} DIRECTORY )
			message ( STATUS "libffi found in " ${ffi_lib_path} )
			target_include_directories ( Einstein SYSTEM PUBLIC ${ffi_incl_path} )
			target_link_libraries ( Einstein ${ffi_lib} )
		endif ()

		# Under OpenBSD, pulseaudio is in ports (i.e. /usr/local) not base (i.e. /usr)
		find_library ( pulse_lib NAMES pulse )
		if ( pulse_lib MATCHES ".*NOTFOUND" )
			message ( FATAL_ERROR "libpulse not found! " )
		else ()
			get_filename_component ( pulse_lib_path ${pulse_lib} DIRECTORY )
			message ( STATUS "libpulse found in " ${pulse_lib_path} )
			target_link_libraries ( Einstein ${pulse_lib} )
		endif ()

		# Under OpenBSD, X11 is in /usr/X11R6
		include ( FindX11 )
		if ( X11_FOUND )
			target_include_directories ( Einstein SYSTEM PUBLIC ${X11_INCLUDE_DIR} )
			target_link_libraries ( Einstein ${X11_LIBRARIES} )
		endif ()
	else ()
		# Under Linux, link pulseaudio normally
		target_link_libraries ( Einstein pulse )
	endif ()

elseif ( WIN32 )

	# Create a resources file for Windows
	configure_file (
		Resources/MSWindows/Einstein.rc.in
		Einstein.rc
	)

	# create the binaries
	add_executable ( Einstein WIN32
		${common_sources} ${app_sources} ${cmake_sources} ${data}
		${CMAKE_CURRENT_BINARY_DIR}/Einstein.rc
	)
	add_executable ( EinsteinTests
		${common_sources} ${test_sources}
	)
	add_library ( EinsteinFLGUI STATIC
		${fluid_sources}
	)

	# how to compile and link
	target_compile_options( Einstein PUBLIC "/bigobj" )
	target_compile_options( EinsteinTests PUBLIC "/bigobj" )
	target_compile_options( EinsteinFLGUI PUBLIC "/bigobj" )
	if ( $<CONFIG:Debug> )
		target_compile_options( Einstein PUBLIC "/ZI" )
		target_compile_options( EinsteinTests PUBLIC "/ZI" )
		target_compile_options( EinsteinFLGUI PUBLIC "/ZI" )
	endif ()
	target_compile_definitions ( Einstein PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_WIN32=1
		WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1
	)
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_WIN32=1
		WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1
	)
	target_compile_definitions ( EinsteinFLGUI PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_WIN32=1
		WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1
	)
	target_link_libraries ( Einstein
		${system_libs}
		fltk::fltk fltk::images fltk::png fltk::z
		EinsteinFLGUI
		winmm
		gdiplus
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		fltk::fltk
		EinsteinFLGUI
		gdiplus
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		fltk::fltk
		gdiplus
	)
	target_link_libraries ( EinsteinFLGUI
		fltk::fltk fltk::images fltk::png fltk::z
	)

elseif ( ANDROID )

	# create the applications
	add_library ( sdl-helloworld SHARED
		${common_sources}
		${app_sources}
		${cmake_sources}
	)
	target_compile_definitions ( sdl-helloworld PRIVATE
		TARGET_UI_SDL=1 TARGET_OS_ANDROID=1
	)
	target_link_libraries ( sdl-helloworld PRIVATE
		SDL3::SDL3
	)

endif ()

# ---- Late NEWT64 Processing --------------------------------------------------

if ( NEWT64_FOUND MATCHES true )
	if ( WIN32 )
		target_include_directories ( Einstein SYSTEM PUBLIC
			${newt64_incl_path}
			${newt64_incl_path}/win
		)
		target_link_libraries ( Einstein shlwapi )
	else ()
		# On Linux/macOS, use darwin includes
		target_include_directories ( Einstein SYSTEM PUBLIC
			${newt64_incl_path}
			${newt64_incl_path}/darwin
		)
	endif ()
	target_link_libraries ( Einstein ${newt64_lib} )
	message ( STATUS "NEWT64 library at " ${newt64_lib} )
	target_compile_definitions ( Einstein PRIVATE USE_TOOLKIT=1 )
    add_dependencies( Einstein ${newt64_lib} )
endif ()

#
# ---- Einstein Platform Independent Build Instructions ------------------------
#

if ( ANDROID )

target_include_directories (
	sdl-helloworld PUBLIC
	${CMAKE_SOURCE_DIR}
)

else ()

target_include_directories (
	Einstein PUBLIC
	${CMAKE_SOURCE_DIR}
	${FLTK_INCLUDE_DIRS}
)
target_include_directories (
	EinsteinTests PUBLIC
	${CMAKE_SOURCE_DIR}
	${FLTK_INCLUDE_DIRS}
)
if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
	target_include_directories (
		EinsteinFLGUI PUBLIC
		${CMAKE_SOURCE_DIR}
		${FLTK_INCLUDE_DIRS}
	)
endif ()

target_compile_definitions ( Einstein PUBLIC "$<$<CONFIG:DEBUG>:_DEBUG>" USE_CMAKE )
target_compile_definitions ( EinsteinTests PUBLIC "$<$<CONFIG:DEBUG>:_DEBUG>" USE_CMAKE )
if ( ${EINSTEIN_FRONTEND} STREQUAL "FLTK" )
	target_compile_definitions ( EinsteinFLGUI PUBLIC "$<$<CONFIG:DEBUG>:_DEBUG>" USE_CMAKE )
endif ()

target_link_libraries ( EinsteinTests gtest_main )

gtest_discover_tests ( EinsteinTests )

endif ()


# ---- Generated Files ---------------------------------------------------------

set_source_files_properties (
	/app/Version_CMake.h
	PROPERTIES GENERATED TRUE
)

string ( TIMESTAMP COMPILE_TIME_YYYY "%Y" )
string ( TIMESTAMP COMPILE_TIME_MM "%m" )
string ( TIMESTAMP COMPILE_TIME_DD "%d" )

configure_file (
	${CMAKE_CURRENT_SOURCE_DIR}/app/Version_CMake.h.in
	${CMAKE_CURRENT_SOURCE_DIR}/app/Version_CMake.h
)


#
# ---- clang-format ------------------------
#

find_program(CLANG_FORMAT_EXECUTABLE
    NAMES clang-format-14 clang-format-mp-14 clang-format
    HINTS /usr/local/opt/clang-format@14/bin/ /usr/lib/llvm-14/bin/
    DOC "clang-format executable")
if(CLANG_FORMAT_EXECUTABLE)
  execute_process(COMMAND ${CLANG_FORMAT_EXECUTABLE} -version
                  OUTPUT_VARIABLE clang_format_version
                  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  # Ubuntu clang-format version 13.0.1-++20220112083154+b9a243d1cac2-1~exp1~20220112203239.53
  # clang-format version 13.0.0
  # Note, Matt, Aug 2025: we are at V14 or even V17 by now.

  if(clang_format_version MATCHES ".*clang-format version ([0-9]+).*")
    string(REGEX
           REPLACE ".*clang-format version ([0-9]+).*"
                   "\\1"
                   CLANG_FORMAT_VERSION
                   "${clang_format_version}")
    list(GET clang_format_version 0 CLANG_FORMAT_VERSION_MAJOR)
  endif()
endif()

if(CLANG_FORMAT_EXECUTABLE)
  if (CLANG_FORMAT_VERSION GREATER 12)
    set(CLANG_FORMAT_FOUND TRUE)
    message(STATUS "Found clang-format at ${CLANG_FORMAT_EXECUTABLE}")
  else()
    message(WARNING "clang-format not usable, as version of ${CLANG_FORMAT_EXECUTABLE} doesn't match : ${CLANG_FORMAT_VERSION_MAJOR} < 13")
    set(CLANG_FORMAT_FOUND FALSE)
  endif()
else()
  set(CLANG_FORMAT_FOUND FALSE)
    message(WARNING "clang-format not found")
endif()

if(CLANG_FORMAT_FOUND)
    set( format_sources )
    list( APPEND format_sources ${common_sources} )
    list( APPEND format_sources ${app_sources} )
    list( APPEND format_sources ${tests_sources} )
    list( FILTER format_sources INCLUDE REGEX "\\.(h|cpp|mm|t|m)$" )

    foreach (SOURCE ${format_sources})
        get_property (SOURCE_IS_GENERATED
                      SOURCE "${SOURCE}"
                      PROPERTY GENERATED)
        if (NOT SOURCE_IS_GENERATED)
            list (APPEND format_sources_not_generated "${SOURCE}")
        endif ()
    endforeach ()

    add_custom_target(
            clang-format-check
            COMMAND ${CLANG_FORMAT_EXECUTABLE}
            -n
            ${format_sources_not_generated}
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    add_custom_target(
            clang-format
            COMMAND ${CLANG_FORMAT_EXECUTABLE}
            -i
            ${format_sources_not_generated}
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()
