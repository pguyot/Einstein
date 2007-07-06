// ==============================
// Fichier:			UMemoryTests.h
// Projet:			Einstein
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			8/3/2005
// Tabulation:		4 espaces
// 
// Copyright:		© 2005 by Paul Guyot.
// 					Tous droits réservés pour tous pays.
// ===========
// $Id: UMemoryTests.h 151 2006-01-13 16:15:33Z paul $
// ===========

#ifndef _UMEMORYTESTS_H
#define _UMEMORYTESTS_H

#include <K/Defines/KDefinitions.h>

///
/// Class for memory related tests.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class UMemoryTests
{
public:
	///
	/// Perform accesses that check both endianness and alignment with read
	/// accesses.
	///
	static void ReadROMTest( void );

	///
	/// Perform accesses that check both endianness and alignment with read
	/// and write accesses.
	///
	static void ReadWriteRAMTest( void );

	///
	/// Perform flash accesses.
	///
	static void FlashTest( void );
};

#endif
		// _UMEMORYTESTS_H

// ===================================================================== //
// "The eleventh commandment was `Thou Shalt Compute' or `Thou Shalt Not //
// Compute' -- I forget which."                                          //
//                 -- Epigrams in Programming, ACM SIGPLAN Sept. 1982    //
// ===================================================================== //
