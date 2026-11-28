#
# EinsteinTests: unit tests, mainly for the JIT compiler.
#
# This file is only included if EINSTEIN_BUILD_TESTS is ON, so normal builds of
# Einstein don't download googletest or compile the tests. It is included at the
# end of the main CMakeLists.txt without depending on any UI frontend.
#

# ---- Setup Google Testing

FetchContent_Declare (
	googletest
	URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

# For Windows: the runtime is linked statically (/MT), like Einstein
set( gtest_force_shared_crt OFF CACHE BOOL "" FORCE )

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
		NS_BLOCK_ASSERTIONS=1
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		pthread
		"-framework Foundation"
		"-framework AddressBook"
		"-framework CoreFoundation"
		"-framework CoreServices"
	)

elseif ( UNIX AND NOT APPLE AND NOT ANDROID )

	add_executable ( EinsteinTests
		${common_sources}
		${test_sources}
		${cmake_sources}
	)
	target_compile_options ( EinsteinTests PUBLIC
		-Wall -Wno-multichar -Wno-misleading-indentation -Wno-unused-result
		-Wno-missing-field-initializers -Werror
	)
	if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
		target_compile_options ( EinsteinTests PUBLIC -Wno-stringop-truncation )
	endif ()
	target_link_libraries ( EinsteinTests
		${system_libs}
		pthread
		${CMAKE_DL_LIBS}
	)
	if ( HAVE_LIBFFI )
		target_include_directories ( EinsteinTests SYSTEM PUBLIC ${ffi_incl_dir} )
		target_link_libraries ( EinsteinTests ${ffi_lib} )
	endif ()

elseif ( WIN32 )

	add_executable ( EinsteinTests
		${common_sources} ${test_sources}
	)
	if ( MSVC )
		target_compile_options( EinsteinTests PUBLIC "/bigobj" )
	elseif ( MINGW )
		target_compile_options( EinsteinTests PUBLIC "-Wa,-mbig-obj" )
		target_link_options( EinsteinTests PRIVATE -static )
	endif ()
	target_compile_definitions ( EinsteinTests PRIVATE
		WIN32_LEAN_AND_MEAN=1 _CRT_SECURE_NO_WARNINGS=1
	)
	target_link_libraries ( EinsteinTests
		${system_libs}
		gdiplus
		ws2_32
	)

endif ()

# ---- Platform independent rules

target_include_directories (
	EinsteinTests BEFORE PUBLIC
	$<TARGET_PROPERTY:cDCL::K,INTERFACE_INCLUDE_DIRECTORIES>
)
target_include_directories (
	EinsteinTests PUBLIC
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}/generated
)
target_compile_definitions ( EinsteinTests PUBLIC "$<$<CONFIG:DEBUG>:_DEBUG>" USE_CMAKE EINSTEIN_HAVE_GENERATED_CONFIG=1 )

target_link_libraries ( EinsteinTests gtest_main cDCL::K )

# List the tests when ctest runs, not after linking, so the build does not
# depend on running the test program (it hangs on the macOS CI runner).
gtest_discover_tests ( EinsteinTests DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST )
