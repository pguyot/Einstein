// ==============================
// Fichier:			UByteSex.h
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
// The Original Code is UByteSex.h.
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

#ifndef _UBYTESEX_H
#define _UBYTESEX_H

#include <K/Defines/KDefinitions.h>
#if TARGET_OS_COMPAT_POSIX
#include <arpa/inet.h>
#endif
#if TARGET_OS_OPENSTEP
#include <libkern/OSByteOrder.h>
#endif

///
/// Classe pour gérer le sexe des octets.
///
/// Convertit des mots de grand indien vers petit indien et inversement.
/// Fournit des macros pour faire la conversion selon la plateforme.
///
/// \author	Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 1.4 $
///
class UByteSex
{
public:
	///
	/// Echange les deux octets d'un mot de 16 bits.
	/// XXYY -> YYXX
	///
	/// \param inWord	mot de 16 bits en entrée.
	/// \return le mot de 16 bits inversé.
	///
	static inline KUInt16	Swap( const KUInt16 inWord )
		{
#if TARGET_OS_OPENSTEP
			return OSSwapInt16((UInt16) inWord);
#elif TARGET_RT_LITTLE_ENDIAN && TARGET_OS_COMPAT_POSIX
			return (KUInt16) htons((uint16_t) inWord);
#else
			return (KUInt16) ((inWord << 8) | (inWord >> 8));
#endif
		}

	///
	/// Echange les octets d'un mot de 32 bits.
	/// XXYYZZTT -> TTZZYYXX
	///
	/// \param inWord	mot de 32 bits en entrée.
	/// \return le mot de 32 bits inversé.
	///
	static inline KUInt32	Swap( const KUInt32 inWord )
		{
#if TARGET_OS_OPENSTEP
			return OSSwapInt32((UInt32) inWord);
#elif TARGET_RT_LITTLE_ENDIAN && TARGET_OS_COMPAT_POSIX
			return (KUInt32) htonl((uint32_t) inWord);
#else
			return (
					((inWord >> 24)	& 0x000000FF) | 
					((inWord >> 8)	& 0x0000FF00) | 
					((inWord << 8)	& 0x00FF0000) | 
					((inWord << 24)	& 0xFF000000));
#endif
		}

#if TARGET_RT_LITTLE_ENDIAN
	// Macros pour une plateforme en petit indien
	#define	UByteSex_FromBigEndian( inWord )	( UByteSex::Swap( inWord ) )
	#define	UByteSex_FromLittleEndian( inWord )	( inWord )
	#define	UByteSex_ToBigEndian( inWord )		( UByteSex::Swap( inWord ) )
	#define	UByteSex_ToLittleEndian( inWord )	( inWord )
#else
	// Macros pour une plateforme en grand indien
	#define	UByteSex_FromBigEndian( inWord )	( inWord )
	#define	UByteSex_FromLittleEndian( inWord )	( UByteSex::Swap( inWord ) )
	#define	UByteSex_ToBigEndian( inWord )		( inWord )
	#define	UByteSex_ToLittleEndian( inWord )	( UByteSex::Swap( inWord ) )
#endif
};

#endif
		// _UBYTESEX_H

// ====================== //
// Disks travel in packs. //
// ====================== //
