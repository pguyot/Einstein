#
# Patch FLTK 1.4.5's FL/math.h, so FLTK builds with the static C runtime
# (/MT) on Windows ARM64.
#
# FL/math.h defines `inline double rint()` for every Windows compiler except
# MinGW, a fallback for MSVC versions before Visual Studio 2013, which had no
# rint(). With /MT, libucrt.lib brings the real rint(), and on ARM64 the
# linker reports both (LNK2005). The patch keeps the fallback for old MSVC
# only. The FLTK fix is planned for 1.5; remove this script then.
#
# Runs as the PATCH_COMMAND of FetchContent_Declare(FLTK), in the FLTK source
# directory. It changes the file only once, so it can run again.
#
# Usage:
#   cmake -P PatchFLTKMath.cmake
#

set ( file "FL/math.h" )
set ( old "#  if (defined(_WIN32) || defined(CRAY)) && !defined(__MINGW32__)\n" )
set ( new "#  if (defined(_WIN32) || defined(CRAY)) && !defined(__MINGW32__) && (!defined(_MSC_VER) || _MSC_VER < 1800)\n" )

file ( READ "${file}" content )
string ( FIND "${content}" "${new}" patched )
if ( patched EQUAL -1 )
	string ( FIND "${content}" "${old}" found )
	if ( found EQUAL -1 )
		message ( FATAL_ERROR "PatchFLTKMath.cmake: the line to patch is not in ${file}." )
	endif ()
	string ( REPLACE "${old}" "${new}" content "${content}" )
	file ( WRITE "${file}" "${content}" )
	message ( STATUS "Patched ${file}: rint() fallback for old MSVC only" )
endif ()
