// ==============================
// Fichier:			KDebug.h
// Projet:			K
// 
// CrŽŽ le:			3/8/2003
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
// The Original Code is KDebug.h.
//
// The Initial Developer of the Original Code is Paul Guyot.
// Portions created by the Initial Developer are Copyright (C) 2003-2004
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Paul Guyot <pguyot@kallisys.net> (original author)
//
// ***** END LICENSE BLOCK *****
// ===========
// $Id: KDebug.h,v 1.12 2007/07/11 01:36:30 pguyot Exp $
// ===========

#ifndef _KDEBUG_H
#define _KDEBUG_H

// K Defines
#include <K/Defines/KDefinitions.h>

#if TARGET_OS_MACOS
	#include <K/Misc/UPStrings.h>
	#ifdef __MWERKS__
		#pragma	warn_resultnotused	off
	#endif
	#include <OSUtils.h>
	#include <stdio.h>

	#ifdef __MWERKS__
		#pragma	warn_resultnotused	on
	#endif
#endif

#if TARGET_OS_OPENSTEP
	#include <K/Misc/UPStrings.h>

	#ifdef __MWERKS__
		#pragma	warn_resultnotused	off
	#endif

#if TARGET_IOS
#import <CFNetwork/CFNetwork.h>
#else
#import <CoreServices/CoreServices.h>
#endif
#include <stdio.h>
	#include <syslog.h>
	#include <stdarg.h>

	#ifdef __MWERKS__
		#pragma	warn_resultnotused	on
	#endif
#endif

#if TARGET_OS_COMPAT_POSIX
	#include <stdio.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <signal.h>
#endif

#if TARGET_OS_WIN32
#	if _MSC_VER
#		include <crtdbg.h>
#		include <stdio.h>
#		include <string.h>
#		include <sys/types.h>
#		include <signal.h>
#	else
#		include <stdio.h>
#		include <sys/types.h>
#		include <unistd.h>
#		include <signal.h>
#	endif
#endif

// Macros pour le dŽverminage.
// Utilise certaines classes dans K Tests.

// Ce fichier doit tre inclus avec au prŽalable un certain nombre de macros dŽfinies.

// -- KDebugOn/forDebug/NDEBUG
// undef(KDebugOn) &&  TARGET_OS_NEWTON && forDebug		-> KDebugOn = 1
// undef(KDebugOn) &&  TARGET_OS_NEWTON && !forDebug	-> KDebugOn = 0
// undef(KDebugOn) && !TARGET_OS_NEWTON && NDEBUG		-> KDebugOn = 0
// undef(KDebugOn) && !TARGET_OS_NEWTON && !NDEBUG		-> KDebugOn = 1

// Trois niveaux de dŽverminage (en fait indŽpendants).
// - error -> on s'arrte lorsqu'une erreur est survenue (nŽcessite KDEBUGSTR)
// - trace -> on raconte l'entrŽe/la sortie de mŽthodes (nŽcessite KDPRINTF)
// - debug -> on raconte sa vie (nŽcessite KDPRINTF)

// Les versions par dŽfaut de ces macros sont dŽfinies plus loin et dependent de l'OS.
// Elles peuvent tre remplacŽes par d'autres mŽthodes.

// L'utilisation de chacun de ces niveaux se fait en ajoutant dans le fichier .cp:
// 
// #undef KERROR_ENABLED
// #if defined(error_NOM_DU_FICHIER) && error_NOM_DU_FICHIER
// #define KERROR_ENABLED 1
// #else
// #define KERROR_ENABLED 0
// #endif
//
// #undef KTRACE_ENABLED
// #if defined(trace_NOM_DU_FICHIER) && trace_NOM_DU_FICHIER
// #define KTRACE_ENABLED 1
// #else
// #define KTRACE_ENABLED 0
// #endif
//
// #undef KDEBUG_ENABLED
// #if defined(debug_NOM_DU_FICHIER) && debug_NOM_DU_FICHIER
// #define KDEBUG_ENABLED 1
// #else
// #define KDEBUG_ENABLED 0
// #endif
//
// En cas d'absence de ces lignes, on n'a que KERROR_ENABLED ˆ un.
#ifndef KERROR_ENABLED
	#define KERROR_ENABLED 1
#endif
#ifndef KTRACE_ENABLED
	#define KTRACE_ENABLED 0
#endif
#ifndef KDEBUG_ENABLED
	#define KDEBUG_ENABLED 0
#endif

// On peut aussi dŽfinir directement ces macros.
// Ensuite, il suffit d'appler les macros KASSERT, KERROR, KERRORC,
// KTRACE, KTRACE1, KTRACE2, KTRACE3, KTRACE4,
// KDEBUG, KDEBUG1, KDEBUG2, KDEBUG3, KDEBUG4
// Remarque: celles-ci ne sont dŽfinies par quelque chose que si (a) elles ne
// sont pas prŽcŽdemment dŽfinies et (b) KDebugOn vaut 1.

// ---- dŽtermine si on veut le dŽverminage ----

#ifndef KDebugOn
	// Sur Newton: forDebug
	#if TARGET_OS_NEWTON
		#if forDebug
			#define KDebugOn 1
		#else
			#define KDebugOn 0
		#endif
	#else
		// On regarde NDEBUG
		#if NDEBUG
			#define KDebugOn 0
		#else
			#define KDebugOn 1
		#endif
	#endif
#endif

// ---- snprintf n'est pas forcŽment disponible. ----
#ifndef KSNPRINTF
	#if _MSC_VER
		#define KSNPRINTF( output, size, format )						\
			(void) ::_snprintf( (output), (size), (format) )
		#define KSNPRINTF1( output, size, format, arg1 )				\
			(void) ::_snprintf( (output), (size), (format), (arg1) )
		#define KSNPRINTF2( output, size, format, arg1, arg2 )			\
			(void) ::_snprintf( (output), (size), (format), (arg1), (arg2) )
		#define KSNPRINTF3( output, size, format, arg1, arg2, arg3 )	\
			(void) ::_snprintf( (output), (size), (format), (arg1), (arg2), (arg3) )
		#define KSNPRINTF4( output, size, format, arg1, arg2, arg3, arg4 )	\
			(void) ::_snprintf( (output), (size), (format), (arg1), (arg2), (arg3), (arg4) )
		#define KSNPRINTF5( output, size, format, arg1, arg2, arg3, arg4, arg5 )	\
			(void) ::_snprintf( (output), (size), (format), (arg1), (arg2), (arg3), (arg4), (arg5) )
	#elif HAS_C99_SNPRINTF
		#define KSNPRINTF( output, size, format )						\
			(void) ::snprintf( (output), (size), (format) )
		#define KSNPRINTF1( output, size, format, arg1 )				\
			(void) ::snprintf( (output), (size), (format), (arg1) )
		#define KSNPRINTF2( output, size, format, arg1, arg2 )			\
			(void) ::snprintf( (output), (size), (format), (arg1), (arg2) )
		#define KSNPRINTF3( output, size, format, arg1, arg2, arg3 )	\
			(void) ::snprintf( (output), (size), (format), (arg1), (arg2), (arg3) )
		#define KSNPRINTF4( output, size, format, arg1, arg2, arg3, arg4 )	\
			(void) ::snprintf( (output), (size), (format), (arg1), (arg2), (arg3), (arg4) )
		#define KSNPRINTF5( output, size, format, arg1, arg2, arg3, arg4, arg5 )	\
			(void) ::snprintf( (output), (size), (format), (arg1), (arg2), (arg3), (arg4), (arg5) )
	#else
		#define KSNPRINTF( output, size, format )						\
			(void) ::sprintf( (output), (format) )
		#define KSNPRINTF1( output, size, format, arg1 )				\
			(void) ::sprintf( (output), (format), (arg1) )
		#define KSNPRINTF2( output, size, format, arg1, arg2 )			\
			(void) ::sprintf( (output), (format), (arg1), (arg2) )
		#define KSNPRINTF3( output, size, format, arg1, arg2, arg3 )	\
			(void) ::sprintf( (output), (format), (arg1), (arg2), (arg3) )
		#define KSNPRINTF4( output, size, format, arg1, arg2, arg3, arg4 )	\
			(void) ::sprintf( (output), (format), (arg1), (arg2), (arg3), (arg4) )
		#define KSNPRINTF5( output, size, format, arg1, arg2, arg3, arg4, arg5 )	\
			(void) ::sprintf( (output), (format), (arg1), (arg2), (arg3), (arg4), (arg5) )
	#endif
#endif

// ---- Valeurs par dŽfaut de KDEBUGSTR et KDPRINTF ----
#ifndef KDPRINTF
	#if TARGET_OS_NEWTON
		// KDPRINTF va sur stdout
		#define KDPRINTF( str )								\
						(void) ::printf( (str) )
		#define KDPRINTF1( str, arg1 )						\
						(void) ::printf( (str), (arg1) )
		#define KDPRINTF2( str, arg1, arg2 )				\
						(void) ::printf( (str), (arg1), (arg2) )
		#define KDPRINTF3( str, arg1, arg2, arg3 )			\
						(void) ::printf( (str), (arg1), (arg2), (arg3) )
		#define KDPRINTF4( str, arg1, arg2, arg3, arg4 )	\
						(void) ::printf( (str), (arg1), (arg2), (arg3), (arg4) )
		#define KDPRINTF5( str, arg1, arg2, arg3, arg4, arg5 )	\
						(void) ::printf( (str), (arg1), (arg2), (arg3), (arg4), (arg5) )
		#define KDPRINTF_SETUP
	#elif TARGET_OS_MACOS
		// KDPRINTF va sur stdout (pas top)
		#define KDPRINTF( str )								\
						(void) ::printf( (str) )
		#define KDPRINTF1( str, arg1 )						\
						(void) ::printf( (str), (arg1) )
		#define KDPRINTF2( str, arg1, arg2 )				\
						(void) ::printf( (str), (arg1), (arg2) )
		#define KDPRINTF3( str, arg1, arg2, arg3 )			\
						(void) ::printf( (str), (arg1), (arg2), (arg3) )
		#define KDPRINTF4( str, arg1, arg2, arg3, arg4 )	\
						(void) ::printf( (str), (arg1), (arg2), (arg3), (arg4) )
		#define KDPRINTF5( str, arg1, arg2, arg3, arg4, arg5 )	\
						(void) ::printf( (str), (arg1), (arg2), (arg3), (arg4), (arg5) )
		#define KDPRINTF_SETUP
	#elif TARGET_OS_COMPAT_POSIX
		#ifdef KD_CONSOLE_LOGLEVEL
			// KDPRINTF va sur la console.
			#define KDPRINTF( str )								\
							::syslog( KD_CONSOLE_LOGLEVEL, (str) )
			#define KDPRINTF1( str, arg1 )						\
							::syslog( KD_CONSOLE_LOGLEVEL, (str), (arg1) )
			#define KDPRINTF2( str, arg1, arg2 )				\
							::syslog( KD_CONSOLE_LOGLEVEL, (str), (arg1), (arg2) )
			#define KDPRINTF3( str, arg1, arg2, arg3 )			\
							::syslog( KD_CONSOLE_LOGLEVEL, (str), (arg1), (arg2), (arg3) )
			#define KDPRINTF4( str, arg1, arg2, arg3, arg4 )	\
							::syslog( KD_CONSOLE_LOGLEVEL, (str), (arg1), (arg2), (arg3), (arg4) )
			#define KDPRINTF5( str, arg1, arg2, arg3, arg4, arg5 )	\
							::syslog( KD_CONSOLE_LOGLEVEL, (str), (arg1), (arg2), (arg3), (arg4), (arg5) )
			#define KDPRINTF_SETUP
		#else
			// KDPRINTF va sur stderr
			#define KDPRINTF( str )								\
							(void) ::fprintf( stderr, (str) )
			#define KDPRINTF1( str, arg1 )						\
							(void) ::fprintf( stderr, (str), (arg1) )
			#define KDPRINTF2( str, arg1, arg2 )				\
							(void) ::fprintf( stderr, (str), (arg1), (arg2) )
			#define KDPRINTF3( str, arg1, arg2, arg3 )			\
							(void) ::fprintf( stderr, (str), (arg1), (arg2), (arg3) )
			#define KDPRINTF4( str, arg1, arg2, arg3, arg4 )	\
							(void) ::fprintf( stderr, (str), (arg1), (arg2), (arg3), (arg4) )
			#define KDPRINTF5( str, arg1, arg2, arg3, arg4, arg5 )	\
							(void) ::fprintf( stderr, (str), (arg1), (arg2), (arg3), (arg4), (arg5) )
			#define KDPRINTF_SETUP
		#endif
	#elif TARGET_OS_WIN32 && _MSC_VER
		// KDPRINTF to the VisualC debugging console
		#define KDPRINTF( str )								\
						_RPT0(_CRT_WARN, (str) )
		#define KDPRINTF1( str, arg1 )						\
						(void) ::printf( (str), (arg1) )
		#define KDPRINTF2( str, arg1, arg2 )				\
						(void) ::printf( (str), (arg1), (arg2) )
		#define KDPRINTF3( str, arg1, arg2, arg3 )			\
						(void) ::printf( (str), (arg1), (arg2), (arg3) )
		#define KDPRINTF4( str, arg1, arg2, arg3, arg4 )	\
						(void) ::printf( (str), (arg1), (arg2), (arg3), (arg4) )
		#define KDPRINTF5( str, arg1, arg2, arg3, arg4, arg5 )	\
						(void) ::printf( (str), (arg1), (arg2), (arg3), (arg4), (arg5) )
		#define KDPRINTF_SETUP
	#else
		#if KDebugOn
			#warning KDPRINTF n«est pas dŽfini
		#endif
	#endif
#endif

#ifndef KDEBUGSTR
	#if TARGET_OS_NEWTON
		// DebugStr existe et prend une CStr.
		#define KDEBUGSTR( cstr )	DebugStr( cstr )
	#elif TARGET_OS_COMPAT_POSIX
		// DebugStr n'existe pas. On utilise signal pour envoyer un SIGINT
		#define KDEBUGSTR( cstr )							\
			{												\
				(void) ::fprintf( stderr, 					\
						"DebugStr - %s (%s:%i)",			\
						cstr, __FILE__, __LINE__ );			\
				(void) ::kill( ::getpid(), SIGINT );		\
			}
	#elif TARGET_OS_MAC
		// DebugStr existe mais prend une PStr.
		#define KDEBUGSTR( cstr ) 							\
			{												\
				Str255 theDebugStr;							\
				UPStrings::C2PStrCopy( theDebugStr, cstr );	\
				::DebugStr( theDebugStr );					\
			}
	#elif TARGET_OS_WIN32 && _MSC_VER
		// KDPRINTF to the VisualC debugging console
		#define KDEBUGSTR( cstr )							\
				_RPT3(_CRT_ERROR, 							\
						"DebugStr - %s (%s:%i)",			\
						cstr, __FILE__, __LINE__ );			
	#else
		#if KDebugOn
			#warning KDEBUGSTR n«est pas dŽfini
		#endif
	#endif
#endif

// Magie avec la pile.
#ifndef KPRINT_STACK
	#define KPRINT_STACK
#endif
#ifndef KMONITOR_FIELD
	#define KMONITOR_FIELD
#endif
#ifndef KMONITOR_CONSTRUCTOR
	#define KMONITOR_CONSTRUCTOR
#endif
#ifndef KMONITOR_METHOD
	#define KMONITOR_METHOD
#endif
#ifndef KMONITOR_FUNCTION
	#define KMONITOR_FUNCTION
#endif

#if KDebugOn
	#ifndef KTRACE
		#define KTRACE( str )												\
			if (KTRACE_ENABLED)												\
			{																\
				KDPRINTF3( "T]%s:%i> %s\n", __FILE__, __LINE__, (str) );	\
			}
	#endif
	#ifndef KTRACE1
		#define KTRACE1( str, arg1 )										\
			if (KTRACE_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF1( theMsg, 512, (str), (arg1) );					\
				KDPRINTF3( "T]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif
	#ifndef KTRACE2
		#define KTRACE2( str, arg1, arg2 )									\
			if (KTRACE_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF2( theMsg, 512, (str), (arg1), (arg2) );			\
				KDPRINTF3( "T]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif
	#ifndef KTRACE3
		#define KTRACE3( str, arg1, arg2, arg3 )							\
			if (KTRACE_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF3( theMsg, 512, (str), (arg1), (arg2), (arg3) );	\
				KDPRINTF3( "T]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif
	#ifndef KTRACE4
		#define KTRACE4( str, arg1, arg2, arg3, arg4 )						\
			if (KTRACE_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF4( theMsg, 512, (str), (arg1), (arg2), (arg3), (arg4) );	\
				KDPRINTF3( "T]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif
	#ifndef KTRACE5
		#define KTRACE5( str, arg1, arg2, arg3, arg4, arg5 )				\
			if (KTRACE_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF5( theMsg, 512, (str), (arg1), (arg2), (arg3), (arg4), (arg5) );	\
				KDPRINTF3( "T]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif

	#ifndef KDEBUG
		#define KDEBUG( str )												\
			if (KDEBUG_ENABLED)												\
			{																\
				KDPRINTF3( "D]%s:%i> %s\n", __FILE__, __LINE__, (str) );	\
			}
	#endif
	#ifndef KDEBUG1
		#define KDEBUG1( str, arg1 )										\
			if (KDEBUG_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF1( theMsg, 512, (str), (arg1) );					\
				KDPRINTF3( "D]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif
	#ifndef KDEBUG2
		#define KDEBUG2( str, arg1, arg2 )									\
			if (KDEBUG_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF2( theMsg, 512, (str), (arg1), (arg2) );			\
				KDPRINTF3( "D]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif
	#ifndef KDEBUG3
		#define KDEBUG3( str, arg1, arg2, arg3 )							\
			if (KDEBUG_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF3( theMsg, 512, (str), (arg1), (arg2), (arg3) );	\
				KDPRINTF3( "D]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif
	#ifndef KDEBUG4
		#define KDEBUG4( str, arg1, arg2, arg3, arg4 )						\
			if (KDEBUG_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF4( theMsg, 512, (str), (arg1), (arg2), (arg3), (arg4) );	\
				KDPRINTF3( "D]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif
	#ifndef KDEBUG5
		#define KDEBUG5( str, arg1, arg2, arg3, arg4, arg5 )				\
			if (KDEBUG_ENABLED)												\
			{																\
				char theMsg[512];											\
				KSNPRINTF5( theMsg, 512, (str), (arg1), (arg2), (arg3), (arg4), (arg5) );	\
				KDPRINTF3( "D]%s:%i> %s\n", __FILE__, __LINE__, theMsg );	\
			}
	#endif

	#ifndef KERROR
		#define KERROR( func, err )										\
			if (KERROR_ENABLED && (err)) {								\
				KDEBUGSTR( "Erreur!" );									\
				KDPRINTF_SETUP;											\
				KDPRINTF4( "E]%s:%i> %s a retournŽ une erreur: %i\n",	\
						__FILE__, __LINE__, func, err );				\
				KPRINT_STACK;											\
			}
	#endif
	#ifndef KERRORC
		#define KERRORC( func, err, cond )								\
			if (KERROR_ENABLED && (err) && (cond)) {					\
				KDEBUGSTR( "Erreur!" );									\
				KDPRINTF4( "E]%s:%i> %s a retournŽ une erreur: %i\n",	\
					__FILE__, __LINE__, func, err );					\
				KPRINT_STACK;											\
			}
	#endif
	#ifndef KASSERT
		#define KASSERT( cond )											\
			if (KERROR_ENABLED && !(cond)) {							\
				KDEBUGSTR( "Assertion!" );								\
				KDPRINTF2( "A]%s:%i> l'assertion ("#cond") a ŽchouŽ\n",	\
					__FILE__, __LINE__ );								\
				KPRINT_STACK;											\
			}
	#endif
	#ifndef KFAIL
		#define KFAIL( str )											\
			if (KERROR_ENABLED) {										\
				KDEBUGSTR( "Echec!" );									\
				KDPRINTF3( "A]%s:%i> Žchec inconditionnel (%s)\n",		\
					__FILE__, __LINE__, (str) );						\
				KPRINT_STACK;											\
			}
	#endif
	// Source: http://www.jaggersoft.com/pubs/CVu11_5.html
	#ifndef KCOMPILE_TIME_ASSERT
		#define KCOMPILE_TIME_ASSERT( cond )							\
			switch (0) {												\
				case 0:													\
				case (cond):											\
				;														\
			}
	#endif
	#ifndef KCOMPILE_TIME_ASSERT_EQUAL
		#define KCOMPILE_TIME_ASSERT_EQUAL( one, two )					\
			KCOMPILE_TIME_ASSERT( one == two )
	#endif
	#ifndef KCOMPILE_TIME_ASSERT_SIZE_EQUAL
		#define KCOMPILE_TIME_ASSERT_SIZE_EQUAL( one, two )				\
			KCOMPILE_TIME_ASSERT( sizeof( one ) == sizeof( two ) )
	#endif
	#ifndef KCOMPILE_TIME_ASSERT_SIZE
		#define KCOMPILE_TIME_ASSERT_SIZE( type, size )					\
			KCOMPILE_TIME_ASSERT( sizeof( type ) == size )
	#endif
#else
	#ifndef KTRACE
		#define KTRACE( str )
	#endif
	#ifndef KTRACE1
		#define KTRACE1( str, arg1 )
	#endif
	#ifndef KTRACE2
		#define KTRACE2( str, arg1, arg2 )
	#endif
	#ifndef KTRACE3
		#define KTRACE3( str, arg1, arg2, arg3 )
	#endif
	#ifndef KTRACE4
		#define KTRACE4( str, arg1, arg2, arg3, arg4 )
	#endif
	#ifndef KTRACE5
		#define KTRACE5( str, arg1, arg2, arg3, arg4, arg5 )
	#endif

	#ifndef KDEBUG
		#define KDEBUG( str )
	#endif
	#ifndef KDEBUG1
		#define KDEBUG1( str, arg1 )
	#endif
	#ifndef KDEBUG2
		#define KDEBUG2( str, arg1, arg2 )
	#endif
	#ifndef KDEBUG3
		#define KDEBUG3( str, arg1, arg2, arg3 )
	#endif
	#ifndef KDEBUG4
		#define KDEBUG4( str, arg1, arg2, arg3, arg4 )
	#endif
	#ifndef KDEBUG5
		#define KDEBUG5( str, arg1, arg2, arg3, arg4, arg5 )
	#endif

	#ifndef KERROR
		#define KERROR( func, err )
	#endif
	#ifndef KERRORC
		#define KERRORC( func, err, cond )
	#endif
	#ifndef KASSERT
		#define KASSERT( cond )
	#endif
	#ifndef KFAIL
		#define KFAIL( str )
	#endif
	#ifndef KCOMPILE_TIME_ASSERT
		#define KCOMPILE_TIME_ASSERT( cond )
	#endif
	#ifndef KCOMPILE_TIME_ASSERT_SIZE_EQUAL
		#define KCOMPILE_TIME_ASSERT_SIZE_EQUAL( one, two )
	#endif
	#ifndef KCOMPILE_TIME_ASSERT_SIZE
		#define KCOMPILE_TIME_ASSERT_SIZE( type, size )
	#endif
#endif

#endif
		// _KDEBUG_H

// =========================================================================== //
// The fountain code has been tightened slightly so you can no longer dip      //
// objects into a fountain or drink from one while you are floating in mid-air //
// due to levitation.                                                          //
//         Teleporting to hell via a teleportation trap will no longer occur   //
// if the character does not have fire resistance.                             //
//                 -- README file from the NetHack game                        //
// =========================================================================== //
