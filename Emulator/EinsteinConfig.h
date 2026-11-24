#ifndef _EINSTEINCONFIG_H
#define _EINSTEINCONFIG_H

#ifdef EINSTEIN_HAVE_GENERATED_CONFIG
#include "EinsteinGeneratedConfig.h"
#else
// Fallback defaults for non-CMake builds (e.g., _Build_/Xcode/Einstein.xcodeproj)
#ifdef __APPLE__
#include <TargetConditionals.h>
#define HAVE_PTY (!TARGET_OS_IPHONE)
#define HAVE_SOCKETS 1
#define HAVE_COREAUDIO 1
#define HAVE_PULSEAUDIO 0
#define HAVE_WAVEOUT 0
#define HAVE_LIBFFI 0
#else
#error "Non-CMake builds must define capability macros or EINSTEIN_HAVE_GENERATED_CONFIG"
#endif
#endif

#endif
