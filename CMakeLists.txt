# ==============================
# File:     CMakeLists.txt
# Project:  Einstein
#
# Copyright 2003-2022 by Paul Guyot (pguyot@kallisys.net) and others.
# ==============================

cmake_minimum_required(VERSION 3.15)

project( "Einstein" VERSION "2022.4.20" )

# ---- Configuration for all targets on all platforms

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS OFF )

# ---- Include some additional functionality

include( CheckSymbolExists )
include( CheckFunctionExists )
include( CheckIncludeFile )
include( FetchContent )

# ---- Setup Google Testing

FetchContent_Declare (
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set( gtest_force_shared_crt ON CACHE BOOL "" FORCE )

# Check if population has already been performed
FetchContent_GetProperties(googletest)
if ( NOT googletest_POPULATED )
	# Fetch the content using previously declared details
	FetchContent_Populate(googletest)

	# Bring the populated content into the build
	add_subdirectory( ${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} )
endif ()

enable_testing()

include( GoogleTest )

# ---- Configuration per platform

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )

	# TODO: enable this for release builds
	#set( CMAKE_OSX_ARCHITECTURES arm64;x86_64 )
	# TODO: set this for FLTK and NEWT64 as well
	#set( CMAKE_OSX_DEPLOYMENT_TARGET 10.9 ) # Can't go any lower.
	# TODO: Fix this for release builds
	set( CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO" )
	set( CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "" )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

	# Linux: nothing to do here

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD" )

	set_property ( GLOBAL PROPERTY FIND_LIBRARY_USE_OPENBSD_VERSIONING 1 )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

	# allow to build with multithreading libraries with VisualC
	cmake_policy( SET CMP0091 NEW )
	set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )

else ()

	# Warn the caller that this is not supported
	message( FATAL_ERROR "Einstein is not configured to build on this platform: " ${CMAKE_SYSTEM_NAME} "." )

endif ()

#
# ==== External Dependencies ===================================================
#
# ---- Newt/64 library ---------------------------------------------------------
#

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
	# Generates Newt64_FOUND, Newt64_DIR
	find_package( Newt64 CONFIG HINTS ${CMAKE_CURRENT_SOURCE_DIR}/newt64/build )
endif()

find_library ( newt64_lib NAMES newt64
	HINTS
		${CMAKE_CURRENT_SOURCE_DIR}/newt64/build/Release
		${CMAKE_CURRENT_SOURCE_DIR}/newt64/build/Debug
		${CMAKE_CURRENT_SOURCE_DIR}/newt64/build
)
find_file( newt64_incl NAMES NewtCore.h PATH_SUFFIXES newt64 HINTS ${CMAKE_CURRENT_SOURCE_DIR}/newt64/src/newt_core/incs )

if ( newt64_lib MATCHES ".*NOTFOUND" OR newt64_incl MATCHES ".*NOTFOUND" )
	message( WARNING "Newt64 not found!" )
	set( NEWT64_FOUND false )
else ()
	set( NEWT64_FOUND true )
	get_filename_component( newt64_lib_path ${newt64_lib} DIRECTORY )
	get_filename_component( newt64_incl_path ${newt64_incl} DIRECTORY )
	message( STATUS "Newt64 found in " ${newt64_lib_path} )
endif ()

#
# ---- FLTK library ------------------------------------------------------------
#

# TODO: update this to automatically download and build in .../Einstein/fltk
set( FLTK_SKIP_OPENGL true )
find_program( LOCAL_FLTK_FLUID_EXECUTABLE fluid
	HINTS
		${CMAKE_SOURCE_DIR}/fltk/build/bin/Release
		${CMAKE_SOURCE_DIR}/fltk/build/bin/Debug
		${CMAKE_SOURCE_DIR}/fltk/build/bin
)
find_package( FLTK REQUIRED NO_MODULE HINTS ${CMAKE_SOURCE_DIR}/fltk/build )

if ( ${LOCAL_FLTK_FLUID_EXECUTABLE} MATCHES ".*NOTFOUND" )
	message( WARNING "Fluid (FLTK User Interface Designer) not found!" )
else ()
	message( STATUS "Fluid (FLTK User Interface Designer) found at " ${LOCAL_FLTK_FLUID_EXECUTABLE})
endif ()

function ( build_with_fluid name dir )
	add_custom_command (
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/${dir}"
		COMMAND ${LOCAL_FLTK_FLUID_EXECUTABLE} -c ${name}.fl
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

#
# ==== Configuring Einstein Build ==============================================
#
# ---- Einstein Source Files ---------------------------------------------------
#

set( cmake_sources )
set( common_sources )
set( app_sources )
set( test_sources )

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
if ( WIN32 )
	source_group(TREE "${CMAKE_SOURCE_DIR}" PREFIX "Sources" FILES Resources/MSWindows/Einstein.rc.in)
endif ()

#
# ---- Platform specific rules ----------------------------------------------------------
#

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin" )

	# take care of the app icon
	set( MACOSX_BUNDLE_ICON_FILE Einstein.icns )
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

	# how to compile and link
	target_compile_options( Einstein PUBLIC
		-Wall -Wextra -Wpedantic -Wno-missing-field-initializers -Werror
	)
	target_compile_options( EinsteinTests PUBLIC
		-Wall -Wextra -Wpedantic -Wno-missing-field-initializers -Werror
	)
	target_compile_definitions ( Einstein PRIVATE
		TARGET_UI_FLTK=1 NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1
		TARGET_OS_MAC=1 NS_BLOCK_ASSERTIONS=1
	)
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 NO_PORT_AUDIO NO_X11 TARGET_OS_OPENSTEP=1
		TARGET_OS_MAC=1 NS_BLOCK_ASSERTIONS=1
	)
	target_link_libraries ( Einstein
		${system_libs}
		PRIVATE fltk::fltk fltk::images fltk::png fltk::z
		pthread
		"-framework AddressBook"
		"-framework AudioUnit"
		"-framework AppKit"
		"-framework CoreAudio"
		"-framework Cocoa"
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		PRIVATE fltk::fltk
		pthread
		"-framework AddressBook"
		"-framework AudioUnit"
		"-framework AppKit"
		"-framework CoreAudio"
		"-framework Cocoa"
	)
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
		# Xcode can't use the same file in multiple target if there is no dependency.
		add_dependencies( Einstein EinsteinTests )
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

	# how to compile and link
	target_compile_options ( Einstein PUBLIC
		-Wall -Wno-multichar -Wno-misleading-indentation
		-Wno-missing-field-initializers # -Werror
		# Werror is disabled for testing purposes. Must reenable as soon as all Linux warnings are fixed.
	)
	target_compile_options ( EinsteinTests PUBLIC
		-Wall -Wno-multichar -Wno-misleading-indentation
		-Wno-missing-field-initializers -Werror
	)
	target_compile_definitions ( Einstein PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_LINUX=1
	)
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_LINUX=1
	)
	target_link_libraries ( Einstein
		${system_libs}
		PRIVATE fltk::fltk fltk::images fltk::png fltk::z
		pthread
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		PRIVATE fltk::fltk
		pthread
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

	# how to compile and link
	target_compile_options( Einstein PUBLIC "/bigobj" )
	target_compile_options( EinsteinTests PUBLIC "/bigobj" )
	if ( $<CONFIG:Debug> )
		target_compile_options( Einstein PUBLIC "/ZI" )
		target_compile_options( EinsteinTests PUBLIC "/ZI" )
	endif ()
	target_compile_definitions ( Einstein PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_WIN32=1
		WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1
	)
	target_compile_definitions ( EinsteinTests PRIVATE
		TARGET_UI_FLTK=1 TARGET_OS_WIN32=1
		WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1
	)
	target_link_libraries ( Einstein
		${system_libs}
		PRIVATE fltk::fltk fltk::images fltk::png fltk::z
		winmm
		gdiplus
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		PRIVATE fltk::fltk
		gdiplus
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
endif ()

#
# ---- Einstein Platform Independent Build Instructions ------------------------
#

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

target_compile_definitions ( Einstein PUBLIC "$<$<CONFIG:DEBUG>:_DEBUG>" USE_CMAKE )
target_compile_definitions ( EinsteinTests PUBLIC "$<$<CONFIG:DEBUG>:_DEBUG>" USE_CMAKE )

target_link_libraries ( EinsteinTests gtest_main )

gtest_discover_tests ( EinsteinTests )

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
    NAMES clang-format-13 clang-format-mp-13 clang-format
    HINTS /usr/local/opt/clang-format@13/bin/ /usr/lib/llvm-13/bin/
    DOC "clang-format executable")
if(CLANG_FORMAT_EXECUTABLE)
  execute_process(COMMAND ${CLANG_FORMAT_EXECUTABLE} -version
                  OUTPUT_VARIABLE clang_format_version
                  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  # Ubuntu clang-format version 13.0.1-++20220112083154+b9a243d1cac2-1~exp1~20220112203239.53
  # clang-format version 13.0.0

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
    message(STATUS "clang-format not usable, as version of ${CLANG_FORMAT_EXECUTABLE} doesn't match : ${CLANG_FORMAT_VERSION_MAJOR} < 13")
    set(CLANG_FORMAT_FOUND FALSE)
  endif()
else()
  set(CLANG_FORMAT_FOUND FALSE)
    message(STATUS "clang-format not found")
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
