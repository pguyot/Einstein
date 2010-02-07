// ==============================
// Fichier:			TMutableStringRef.cp
// Projet:			K
// 
// CrŽŽ le:			1/10/2003
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
// The Original Code is TMutableStringRef.cp.
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
// $Id: TMutableStringRef.cp,v 1.3 2004/11/24 18:23:18 pguyot Exp $
// ===========

#include <K/Unicode/TMutableStringRef.h>

// -------------------------------------------------------------------------- //
//  * TMutableStringRef( TString* )
// -------------------------------------------------------------------------- //
TMutableStringRef::TMutableStringRef( TString* inString )
	:
		mString( inString )
{
	if (mString)
	{
		mString->IncrementRef();	
	}
}

// ------------------------------------------------------------------------- //
// 	* TMutableStringRef( const TMutableStringRef& )
// ------------------------------------------------------------------------- //
TMutableStringRef::TMutableStringRef( const TMutableStringRef& inCopy )
	:
		mString( inCopy.mString )
{
	if (mString)
	{
		mString->IncrementRef();	
	}
}

// -------------------------------------------------------------------------- //
//  * ~TMutableStringRef( void )
// -------------------------------------------------------------------------- //
TMutableStringRef::~TMutableStringRef( void )
{
	if (mString && mString->DecrementRef())
	{
		delete mString;
	}
}

// ------------------------------------------------------------------------- //
// 	* operator = ( const TMutableStringRef& )
// ------------------------------------------------------------------------- //
TMutableStringRef&
TMutableStringRef::operator = ( const TMutableStringRef& inCopy )
{
	if (mString != inCopy.mString)
	{
		if (mString && mString->DecrementRef())
		{
			delete mString;
		}

		mString = inCopy.mString;
		if (mString)
		{
			mString->IncrementRef();
		}
	}
	
	return *this;
}

// ======================================================================== //
// NOTE: No warranties, either express or implied, are hereby given. All    //
// software is supplied as is, without guarantee.  The user assumes all     //
// responsibility for damages resulting from the use of these features,     //
// including, but not limited to, frustration, disgust, system abends, disk //
// head-crashes, general malfeasance, floods, fires, shark attack, nerve    //
// gas, locust infestation, cyclones, hurricanes, tsunamis, local           //
// electromagnetic disruptions, hydraulic brake system failure, invasion,   //
// hashing collisions, normal wear and tear of friction surfaces, comic     //
// radiation, inadvertent destruction of sensitive electronic components,   //
// windstorms, the Riders of Nazgul, infuriated chickens, malfunctioning    //
// mechanical or electrical sexual devices, premature activation of the     //
// distant early warning system, peasant uprisings, halitosis, artillery    //
// bombardment, explosions, cave-ins, and/or frogs falling from the sky.    //
// ======================================================================== //
