// ==============================
// Fichier:			UScreenTests.h
// Projet:			Einstein
// Ecrit par:		Paul Guyot (pguyot@kallisys.net)
// 
// Créé le:			13/2/2005
// Tabulation:		4 espaces
// 
// Copyright:		© 2005 by Paul Guyot.
// 					Tous droits réservés pour tous pays.
// ===========
// $Id: UScreenTests.h 126 2005-02-24 06:55:53Z paul $
// ===========

#ifndef _USCREENTESTS_H
#define _USCREENTESTS_H

#include <K/Defines/KDefinitions.h>

class TMemory;

///
/// Class for screen manager tests.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 126 $
///
/// \test	aucun test défini.
///
class UScreenTests
{
public:
	///
	/// Test the X11 screen manager by drawing lines.
	///
	static void TestX11( void );

private:
	///
	/// Draw a single point on the pixmap.
	///
	static void	DrawPoint(
					TMemory* inMemoryIntf,
					KUInt32 inBaseAddy,
					KUInt32 inRowBytes,
					int inXcoord,
					int inYcoord,
					int inColor );
};

#endif
		// _USCREENTESTS_H

// =========================================================================== //
// Many of the convicted thieves Parker has met began their                    //
// life of crime after taking college Computer Science courses.                //
//                 -- Roger Rapoport, "Programs for Plunder", Omni, March 1981 //
// =========================================================================== //
