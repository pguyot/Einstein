// ==============================
// Fichier:			KDefinitions.h
// Projet:			K
//
// Créé le:			13/01/2002
// Tabulation:		4 espaces
//
// ***** BEGIN LICENSE BLOCK *****
// Version: MPL 1.1
//
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
//
// The Original Code is KDefinitions.h.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2002-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========

#ifndef __KDEFINITIONS__
#define __KDEFINITIONS__

/// \file KDefinitions.h
///
/// Fichier pour des définitions pour différentes plateformes.
/// Utilisé dans différents projets (K, DCL, etc.)
///
/// Ce fichier est inclus par chaque fichier .h et comprend des macros
/// pour la platforme courante.
///
/// Il essaye de deviner la plateforme. Celle-ci peut-être définie
/// par TARGET_OS_XXX.
///
/// Ce qui est définit ici:
/// bool		(taille quelconque, sur certaines plateformes, un mot
///				de 32 bits est plus efficace qu'un octet)
/// false et true (normalement définis par le compilateur C++)
/// KUInt64		un entier non signé de 64 bits
/// KUInt32		un entier non signé de 32 bits
/// KSInt32		un entier signé de 32 bits
/// KUInt16		un entier non signé de 16 bits
/// KSInt16		un entier signé de 16 bits
/// KUInt8		un entier non signé de 8 bits
/// KSInt8		un entier signé de 8 bits
/// KUIntPtr	un entier non signé de la taille d'un pointeur.
///
/// La cible est définie par les macros TARGET_OS_XXX
///
/// Le sexes des octets:
/// TARGET_RT_LITTLE_ENDIAN		Petit indien (x86, ARM, etc.)
/// TARGET_RT_BIG_ENDIAN		Grand indien (PowerPC, ARM sur Newton, etc.)

///\def TARGET_OS_ANDROID
/// Macro qui vaut \c 1 si la cible est Google's Android, \c 0 sinon.

///\def TARGET_OS_BEOS
/// Macro qui vaut \c 1 si la cible est BeOS, \c 0 sinon.

///\def TARGET_OS_BSD
/// Macro qui vaut \c 1 si la cible est {Net,Free,Open}BSD, \c 0 sinon.

///\def TARGET_OS_LINUX
/// Macro qui vaut \c 1 si la cible est Linux, \c 0 sinon.

///\def TARGET_OS_MACOS
/// Macro qui vaut \c 1 si la cible est MacOS (non X), \c 0 sinon.

///\def TARGET_OS_OPENSTEP
/// Macro qui vaut \c 1 si la cible est MacOS X, \c 0 sinon.

///\def TARGET_OS_OPENSTEP_PPC
/// Macro qui vaut \c 1 si la cible est MacOS X ppc, \c 0 sinon.

///\def TARGET_OS_OPENSTEP_I386
/// Macro qui vaut \c 1 si la cible est MacOS X i386, \c 0 sinon.

///\def TARGET_OS_MAC
/// Macro qui vaut \c 1 si la cible est MacOS [X], \c 0 sinon.

///\def TARGET_OS_NEWTON
/// Macro qui vaut \c 1 si la cible est NewtonOS, \c 0 sinon.

///\def TARGET_OS_POSIX
/// Macro qui vaut \c 1 si la cible est POSIX, \c 0 sinon.

///\def TARGET_OS_WIN32
/// Macro qui vaut \c 1 si la cible est Win32, \c 0 sinon.

///\def TARGET_OS_CYGWIN
/// Macro qui vaut \c 1 si la cible est Cygwin/Win32, \c 0 sinon.

///\def TARGET_OS_COMPAT_POSIX
/// Macro qui vaut \c 1 si la cible est compatible POSIX, \c 0 sinon.
/// (compatible: le minimum dont on a besoin pour la DCL)
/// (POSIX: un peu plus large en fait, puisqu'on utilise les sockets BSD)

/// On a aussi KDebugOn qui détermine si on veut du déverminage.
/// Par défaut à 1 sauf si NDEBUG vaut 1.

///\def HAS_C99_SNPRINTF
///	Macro qui vaut \c 1 si on a l'extension C99 snprintf, \c 0 sinon.

#define TARGET_OS_UNDEFINED 1

#define HAS_EXCEPTION_HANDLING 1

#if TARGET_OS_OPENSTEP_PPC
	#define	TARGET_OS_OPENSTEP 1
#endif

#if TARGET_OS_OPENSTEP_I386
        #define	TARGET_OS_OPENSTEP 1
#endif

#if TARGET_OS_OPENSTEP_ARM
        #define	TARGET_OS_OPENSTEP 1
#endif

#if DARWIN
    #define TARGET_OS_OPENSTEP 1
#endif

#if TARGET_OS_MACOS
	#define TARGET_OS_MAC 1
	#define TARGET_API_MAC_OS8 1
#endif

#if TARGET_OS_OPENSTEP
	#define TARGET_OS_MAC 1
	#define TARGET_API_MAC_CARBON 1
	#define TARGET_API_MAC_OSX 1
    #define TARGET_OS_COMPAT_POSIX 1
#endif

#ifdef __MRC__
	// Compilateur MrC[pp] d'Apple
	#define TARGET_OS_MAC 1
	// Parfois stupide
	#define MPW_COMPILERS_WORKAROUND 1
#endif

#if defined(__SC__) && (defined(MPW_CPLUS) || defined(MPW_C))
	// Compilateur SC[pp] d'Apple
	#define TARGET_OS_MAC 1
	// Parfois stupide
	#define MPW_COMPILERS_WORKAROUND 1
#endif

#ifdef __MWERKS__
	// Les compilateurs Metrowerks ont cette fonction plutôt sympa.
	#pragma	warn_resultnotused	on
	
	// On active l'optimisation du résultat.
	// Cf: <hinnant-3969E3.10055518092003@syrcnyrdrs-03-ge0.nyroc.rr.com>
	#pragma opt_classresults on
	#if macintosh
		// A priori, la cible est MacOS [X]
		#ifndef TARGET_OS_MAC
			#define TARGET_OS_MAC 1
		#endif
	#endif
#endif

#if TARGET_OS_NEWTON
	#define TARGET_OS_BEOS 0
	#define TARGET_OS_MACOS 0
	#define TARGET_OS_OPENSTEP 0
	#define TARGET_OS_POSIX 0
	#define TARGET_OS_WIN32	0
	#define TARGET_OS_CYGWIN 0
	#ifdef TARGET_OS_MAC
		#undef TARGET_OS_MAC
	#endif
	#define TARGET_OS_MAC 0
	#define TARGET_OS_COMPAT_POSIX 0
	#undef	TARGET_OS_UNDEFINED
		
	// Macro pour le compilateur ARM du Newton qui est particulièrement
	// stupide.
	#define ARMCPP 1

	#include <Newton.h>

	#define TARGET_RT_LITTLE_ENDIAN 0
	#define TARGET_RT_BIG_ENDIAN 1

	// La bibliothèque C n'a pas snprintf
	#ifndef HAS_C99_SNPRINTF
		#define HAS_C99_SNPRINTF 0
	#endif

	// Gestion des portées.
	#ifndef HAS_NAMESPACES
		#define HAS_NAMESPACES 0
	#endif
	
#endif

#if TARGET_OS_MAC
	#if TARGET_OS_MACOS
		#include <ConditionalMacros.h>
	#endif
	#if TARGET_OS_OPENSTEP
		#if __MWERKS__
			#pragma warn_implicitconv off
		#endif

                #if TARGET_IOS
                        #import <CFNetwork/CFNetwork.h>
                #else
                        #import <CoreServices/CoreServices.h>
                #endif

		#if __MWERKS__
			#pragma warn_implicitconv reset
		#endif
	#endif
	
	#if TARGET_API_MAC_OSX
		#define TARGET_OS_ANDROID 0
		#define TARGET_OS_BEOS 0
		#define TARGET_OS_BSD 0
		#define TARGET_OS_LINUX 0
		#define TARGET_OS_MACOS 0
		#define TARGET_OS_NEWTON 0
		#define TARGET_OS_OPENSTEP 1
		#define TARGET_OS_POSIX 0
		#define TARGET_OS_WIN32	0
		#define TARGET_OS_CYGWIN 0
		#define TARGET_OS_COMPAT_POSIX 1
		#undef	TARGET_OS_UNDEFINED
		
	#else
		#define TARGET_OS_ANDROID 0
		#define TARGET_OS_BEOS 0
		#define TARGET_OS_BSD 0
		#define TARGET_OS_LINUX 0
		#define TARGET_OS_MACOS 1
		#define TARGET_OS_NEWTON 0
		#define TARGET_OS_OPENSTEP 0
		#define TARGET_OS_POSIX 0
		#define TARGET_OS_WIN32	0
		#define TARGET_OS_CYGWIN 0
		#define TARGET_OS_COMPAT_POSIX 0
		#undef	TARGET_OS_UNDEFINED
		
	#endif

	#if TARGET_OS_MACOS
//		#include <MacTypes.h>
	#endif
	
	#if TARGET_OS_OPENSTEP
		// pthread_cond_timedwait_relative_np is available on MacOS X.
		#define HAS_COND_TIMEDWAIT_RELATIVE_NP 0
	#endif
#endif

#ifdef _WIN32
	#ifndef TARGET_OS_WIN32
		#define TARGET_OS_WIN32	1
	#endif
#endif

#if TARGET_OS_CYGWIN
	#ifndef TARGET_OS_WIN32
		#define TARGET_OS_WIN32 1
	#endif
#endif

#if TARGET_OS_WIN32
	#define TARGET_OS_ANDROID 0
	#define TARGET_OS_BEOS 0
	#define TARGET_OS_BSD 0
	#define TARGET_OS_LINUX 0
	#define TARGET_OS_MACOS 0
	#define TARGET_OS_NEWTON 0
	#define TARGET_OS_OPENSTEP 0
	#define TARGET_OS_MAC 0
	#define TARGET_OS_POSIX 0
	// Ça marche peut-être sous windoze. (?)
	#ifdef TARGET_OS_CYGWIN
		#if TARGET_OS_CYGWIN
			#define TARGET_OS_COMPAT_POSIX 1
		#else
			#define TARGET_OS_COMPAT_POSIX 0
		#endif
	#else
		#define TARGET_OS_CYGWIN 0
		#define TARGET_OS_COMPAT_POSIX 0
	#endif	
	#undef	TARGET_OS_UNDEFINED

	// On suppose que c'est Win32 sur x86
	#ifndef TARGET_RT_LITTLE_ENDIAN
		#define TARGET_RT_LITTLE_ENDIAN 1
	#endif
	#ifndef TARGET_RT_BIG_ENDIAN
		#define TARGET_RT_BIG_ENDIAN 0
	#endif

	#define HAS_COND_TIMEDWAIT_RELATIVE_NP 1
#endif

#if TARGET_OS_ANDROID
	#define TARGET_OS_BEOS 0
	#define TARGET_OS_BSD 0
	#define TARGET_OS_LINUX 0
	#define TARGET_OS_MAC 0
	#define TARGET_OS_MACOS 0
	#define TARGET_OS_NEWTON 0
	#define TARGET_OS_OPENSTEP 0
	#define TARGET_OS_WIN32	0
	#define TARGET_OS_CYGWIN 0
	#define TARGET_OS_COMPAT_POSIX 1
	#undef	TARGET_OS_UNDEFINED

	#undef HAS_EXCEPTION_HANDLING
	#define HAS_EXCEPTION_HANDLING 0

	#include <endian.h>
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#ifndef TARGET_RT_LITTLE_ENDIAN
			#define TARGET_RT_LITTLE_ENDIAN 1
		#endif
		#ifndef TARGET_RT_BIG_ENDIAN
			#define TARGET_RT_BIG_ENDIAN 0
		#endif
	#elif __BYTE_ORDER == __BIG_ENDIAN
		#ifndef TARGET_RT_LITTLE_ENDIAN
			#define TARGET_RT_LITTLE_ENDIAN 0
		#endif
		#ifndef TARGET_RT_BIG_ENDIAN
			#define TARGET_RT_BIG_ENDIAN 1
		#endif
	#else
		#if (!defined(TARGET_RT_LITTLE_ENDIAN) || !defined(TARGET_RT_BIG_ENDIAN))
			#error "Could not guess endianness on Android platform with <endian.h>"
		#endif
	#endif
#endif

#if TARGET_OS_BEOS
	#define TARGET_OS_MACOS 0
	#define TARGET_OS_NEWTON 0
	#define TARGET_OS_OPENSTEP 0
	#define TARGET_OS_MAC 0
	#define TARGET_OS_POSIX 0
	#define TARGET_OS_WIN32	0
	#define TARGET_OS_CYGWIN 0
	#define TARGET_OS_COMPAT_POSIX 1
	#undef	TARGET_OS_UNDEFINED

	#include <ByteOrder.h>
	#ifndef TARGET_RT_LITTLE_ENDIAN
		#define TARGET_RT_LITTLE_ENDIAN B_HOST_IS_LENDIAN
	#endif
	#ifndef TARGET_RT_BIG_ENDIAN
		#define TARGET_RT_BIG_ENDIAN B_HOST_IS_BENDIAN
	#endif
	
	#include <SupportDefs.h>
#endif

#if TARGET_OS_BSD
	#define TARGET_OS_ANDROID 0
	#define TARGET_OS_BEOS 0
	#define TARGET_OS_LINUX 0
	#define TARGET_OS_MAC 0
	#define TARGET_OS_MACOS 0
	#define TARGET_OS_NEWTON 0
	#define TARGET_OS_OPENSTEP 0
	#define TARGET_OS_WIN32	0
	#define TARGET_OS_CYGWIN 0
	#define TARGET_OS_COMPAT_POSIX 1
	#undef	TARGET_OS_UNDEFINED

	// Détermination du sexe via endian.h
	#include <sys/endian.h>
	#if BYTE_ORDER == LITTLE_ENDIAN
		#ifndef TARGET_RT_LITTLE_ENDIAN
			#define TARGET_RT_LITTLE_ENDIAN 1
		#endif
		#ifndef TARGET_RT_BIG_ENDIAN
			#define TARGET_RT_BIG_ENDIAN 0
		#endif
	#elif BYTE_ORDER == BIG_ENDIAN
		#ifndef TARGET_RT_LITTLE_ENDIAN
			#define TARGET_RT_LITTLE_ENDIAN 0
		#endif
		#ifndef TARGET_RT_BIG_ENDIAN
			#define TARGET_RT_BIG_ENDIAN 1
		#endif
	#else
		#if (!defined(TARGET_RT_LITTLE_ENDIAN) || !defined(TARGET_RT_BIG_ENDIAN))
			#error "Could not guess endianness on BSD platform with <sys/endian.h>"
		#endif
	#endif
#endif

#if TARGET_OS_LINUX
#include <cstdint>
	#define TARGET_OS_ANDROID 0
	#define TARGET_OS_BEOS 0
	#define TARGET_OS_BSD 0
	#define TARGET_OS_MAC 0
	#define TARGET_OS_MACOS 0
	#define TARGET_OS_NEWTON 0
	#define TARGET_OS_OPENSTEP 0
	#define TARGET_OS_WIN32	0
	#define TARGET_OS_CYGWIN 0
	#define TARGET_OS_COMPAT_POSIX 1
	#undef	TARGET_OS_UNDEFINED

	// Autoconf...
//	#include <linux/autoconf.h>

	// Definitions specifiques a Linux.
	#ifdef _XOPEN_SOURCE
		#undef _XOPEN_SOURCE
	#endif
	#define _XOPEN_SOURCE 500
	#ifdef _XOPEN_SOURCE_EXTENDED
		#undef _XOPEN_SOURCE_EXTENDED
	#endif
	#define _XOPEN_SOURCE_EXTENDED 1

	// Détermination du sexe via endian.h
	#include <endian.h>
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#ifndef TARGET_RT_LITTLE_ENDIAN
			#define TARGET_RT_LITTLE_ENDIAN 1
		#endif
		#ifndef TARGET_RT_BIG_ENDIAN
			#define TARGET_RT_BIG_ENDIAN 0
		#endif
	#elif __BYTE_ORDER == __BIG_ENDIAN
		#ifndef TARGET_RT_LITTLE_ENDIAN
			#define TARGET_RT_LITTLE_ENDIAN 0
		#endif
		#ifndef TARGET_RT_BIG_ENDIAN
			#define TARGET_RT_BIG_ENDIAN 1
		#endif
	#else
		#if (!defined(TARGET_RT_LITTLE_ENDIAN) || !defined(TARGET_RT_BIG_ENDIAN))
			#error "Could not guess endianness on Linux platform with <endian.h>"
		#endif
	#endif
#endif

#if TARGET_OS_POSIX
	#define TARGET_OS_ANDROID 0
	#define TARGET_OS_BEOS 0
	#define TARGET_OS_BSD 0
	#define TARGET_OS_LINUX 0
	#define TARGET_OS_MAC 0
	#define TARGET_OS_MACOS 0
	#define TARGET_OS_NEWTON 0
	#define TARGET_OS_OPENSTEP 0
	#define TARGET_OS_WIN32	0
	#define TARGET_OS_CYGWIN 0
	#define TARGET_OS_COMPAT_POSIX 1
	#undef	TARGET_OS_UNDEFINED
#endif

// snprintf
#ifndef HAS_C99_SNPRINTF
	#define HAS_C99_SNPRINTF 1
#endif

#ifdef TARGET_OS_UNDEFINED
	#error "No valid target was specified!"
#endif

#ifndef TARGET_RT_LITTLE_ENDIAN
	#if __BIG_ENDIAN__
		#define TARGET_RT_LITTLE_ENDIAN 0
	#elif __LITTLE_ENDIAN__
		#define TARGET_RT_LITTLE_ENDIAN 1
	#else
		#error "Endianness isn't (completely) defined!"
	#endif
#endif
#ifndef TARGET_RT_BIG_ENDIAN
	#if __BIG_ENDIAN__
		#define TARGET_RT_BIG_ENDIAN 1
	#elif __LITTLE_ENDIAN__
		#define TARGET_RT_BIG_ENDIAN 0
	#else
		#error "Endianness isn't (completely) defined!"
	#endif
#endif

#ifndef NULL
	#define NULL		nullptr
#endif
#ifndef nil
	#define nil			nullptr
#endif

#ifndef HAS_COND_TIMEDWAIT_RELATIVE_NP
	#define HAS_COND_TIMEDWAIT_RELATIVE_NP 0
#endif

#include <cstdint>

typedef	uint64_t	KUInt64;
typedef	int64_t		KSInt64;
typedef	uint32_t	KUInt32;
typedef	int32_t		KSInt32;
typedef	uint16_t	KUInt16;
typedef	int16_t		KSInt16;
typedef uint8_t     KUInt8;
typedef	int8_t		KSInt8;
typedef uintptr_t   KUIntPtr;

#if TARGET_OS_WIN32
typedef signed long ssize_t;
#endif

static_assert(sizeof(KUInt8)==1, "Size of KUInt8 must be 1 byte");
static_assert(sizeof(KUInt16)==2, "Size of KUInt16 must be 2 bytes");
static_assert(sizeof(KUInt32)==4, "Size of KUInt32 must be 4 bytes");
static_assert(sizeof(KUInt64)==8, "Size of KUInt64 must be 8 bytes");
static_assert(sizeof(KUIntPtr)==sizeof(void*), "Size of KUIntPtr must the same as the size of a pointer");

#endif
		// __KDEFINITIONS__

// ============================= //
// Pause for storage relocation. //
// ============================= //
