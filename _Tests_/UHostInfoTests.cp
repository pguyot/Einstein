// ==============================
// Fichier:			UHostInfoTests.cp
// Projet:			Einstein
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			22/5/2005
// Tabulation:		4 espaces
// 
// Copyright:		© 2005 by Paul Guyot.
// 					Tous droits réservés pour tous pays.
// ===========
// $Id: UHostInfoTests.cp 147 2005-09-29 20:17:58Z paul $
// ===========

#include <K/Defines/KDefinitions.h>

#include "UHostInfoTests.h"

// ANSI C & POSIX
#include <stdio.h>

// K
#include <K/Unicode/UUTF16CStr.h>

// Einstein
#include "Emulator/Host/THostInfo.h"

#if TARGET_OS_WIN32
	#include "Emulator/Host/UserInfoDefinitionsLF.h"
#else
	#include "Emulator/Host/UserInfoDefinitions.h"
#endif

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * HostInfoTest( void )
// -------------------------------------------------------------------------- //
void
UHostInfoTests::HostInfoTest( void )
{
	const THostInfo* theHostInfo = THostInfo::GetHostInfo();
	const KUInt16* theFirstName = theHostInfo->GetUserInfo( kUserInfo_FirstName );
	const KUInt16* theLastName = theHostInfo->GetUserInfo( kUserInfo_LastName );
	
	char buffer[512];
	UUTF16CStr::ToUTF8( theFirstName, (KUInt8*) buffer );
	(void) ::printf( "first = >%s<\n", buffer );

	UUTF16CStr::ToUTF8( theLastName, (KUInt8*) buffer );
	(void) ::printf( "last = >%s<\n", buffer );
}

// ============================================================================== //
//         A master programmer passed a novice programmer one day.  The master    //
// noted the novice's preoccupation with a hand-held computer game.  "Excuse me", //
// he said, "may I examine it?"                                                   //
//         The novice bolted to attention and handed the device to the master.    //
// "I see that the device claims to have three levels of play: Easy, Medium,      //
// and Hard", said the master.  "Yet every such device has another level of play, //
// where the device seeks not to conquer the human, nor to be conquered by the    //
// human."                                                                        //
//         "Pray, great master," implored the novice, "how does one find this     //
// mysterious setting?"                                                           //
//         The master dropped the device to the ground and crushed it under foot. //
// And suddenly the novice was enlightened.                                       //
//                 -- Geoffrey James, "The Tao of Programming"                    //
// ============================================================================== //
