// ==============================
// Fichier:			TNewtonTraceMonitor.cp
// Projet:			K
//
// Créé le:			08/09/2002
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
// The Original Code is TNewtonTraceMonitor.cp.
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

#include <K/Defines/KDefinitions.h>
#include <K/Tests/TNewtonTraceMonitor.h>

// ANSI C
#include <stdlib.h>
#include <stdio.h>

// K
#include <K/Misc/RelocHack.h>

// NewtonOS
#include <Objects.h>

// ------------------------------------------------------------------------- //
//  * Constants
// ------------------------------------------------------------------------- //
const char* const TNewtonTraceMonitor::kName = "TNewtonTraceMonitor";

// ------------------------------------------------------------------------- //
//  * TNewtonTraceMonitor( unsigned int )
// ------------------------------------------------------------------------- //
TNewtonTraceMonitor::TNewtonTraceMonitor( unsigned int inMode )
	:
		TTraceMonitor( inMode ),
		TNameServerSingleton()
{
	RelocVTable( &__VTABLE__19TNewtonTraceMonitor );
}

// ------------------------------------------------------------------------- //
//  * ~TNewtonTraceMonitor( void )
// ------------------------------------------------------------------------- //
TNewtonTraceMonitor::~TNewtonTraceMonitor( void )
{
	// This space for rent.
}

// ------------------------------------------------------------------------- //
//  * GetTraceMonitor( void )
// ------------------------------------------------------------------------- //
TNewtonTraceMonitor*
TNewtonTraceMonitor::GetTraceMonitor( unsigned int inMode )
{
	TNewtonTraceMonitor* theResult = (TNewtonTraceMonitor*) GetObject( kName );

	if (theResult == nil)
	{
		// Création de l'objet.
		theResult = new TNewtonTraceMonitor( inMode );
		
		if (theResult == nil)
			OutOfMemory();
		
		// Enregistrement.
		(void) CreateObject( theResult, kName );
	}
	
	return theResult;
}

// ------------------------------------------------------------------------- //
//  * DeleteObject( void )
// ------------------------------------------------------------------------- //
void
TNewtonTraceMonitor::RemoveTraceMonitor( void )
{
	(void) TNameServerSingleton::DeleteObject( kName );
}

// ====================================================================== //
// Prof:    So the American government went to IBM to come up with a data //
//          encryption standard and they came up with ...                 //
// Student: EBCDIC!"                                                      //
// ====================================================================== //
