// ==============================
// Fichier:			TStringRef.cp
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
// The Original Code is TStringRef.cp.
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
// $Id: TStringRef.cp,v 1.9 2005/02/24 06:49:58 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/Unicode/TStringRef.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TStringRef( void )
// -------------------------------------------------------------------------- //
TStringRef::TStringRef( void )
	:
		mString( nil )
{
}

// -------------------------------------------------------------------------- //
//  * TStringRef( TString* )
// -------------------------------------------------------------------------- //
TStringRef::TStringRef( TString* inString )
	:
		mString( inString )
{
	if (mString)
	{
		mString->IncrementRef();	
	}
}

// -------------------------------------------------------------------------- //
//  * TStringRef( TString& )
// -------------------------------------------------------------------------- //
TStringRef::TStringRef( TString& inString )
	:
		mString( &inString )
{
	inString.IsOnStack();
	inString.IncrementRef();
}

// ------------------------------------------------------------------------- //
// 	* TStringRef( const TStringRef& )
// ------------------------------------------------------------------------- //
TStringRef::TStringRef( const TStringRef& inCopy )
	:
		mString( inCopy.mString )
{
	if (mString)
	{
		mString->IncrementRef();
	}
}

// ------------------------------------------------------------------------- //
// 	* TStringRef( const TMutableStringRef& )
// ------------------------------------------------------------------------- //
TStringRef::TStringRef( const TMutableStringRef& inCopy )
	:
		mString( inCopy.GetString() )
{
	if (mString)
	{
		mString->IncrementRef();
	}
}

// ------------------------------------------------------------------------- //
// 	* TStringRef( const char* )
// ------------------------------------------------------------------------- //
TStringRef::TStringRef( const char* inString )
	:
		mString( new TString( inString ) )
{
	if (mString)
	{
		mString->IncrementRef();
	}
}

// ------------------------------------------------------------------------- //
// 	* TStringRef( const string& )
// ------------------------------------------------------------------------- //
TStringRef::TStringRef( const string& inString )
	:
		mString( new TString( inString.c_str() ) )
{
	if (mString)
	{
		mString->IncrementRef();
	}
}

// -------------------------------------------------------------------------- //
//  * ~TStringRef( void )
// -------------------------------------------------------------------------- //
TStringRef::~TStringRef( void )
{
	if (mString && mString->DecrementRef())
	{
		delete mString;
	}
}

// ------------------------------------------------------------------------- //
// 	* operator = ( TString* )
// ------------------------------------------------------------------------- //
TStringRef&
TStringRef::operator = ( TString* inCopy )
{
	if (mString != inCopy)
	{
		if (mString && mString->DecrementRef())
		{
			delete mString;
		}

		mString = inCopy;
		if (mString)
		{
			mString->IncrementRef();
		}
	}
	
	return *this;
}

// ------------------------------------------------------------------------- //
// 	* operator = ( const char* )
// ------------------------------------------------------------------------- //
TStringRef&
TStringRef::operator = ( const char* inString )
{
	if (mString && mString->DecrementRef())
	{
		delete mString;
	}

	mString = new TString( inString );
	if (mString)
	{
		mString->IncrementRef();
	}
	
	return *this;
}

// ------------------------------------------------------------------------- //
// 	* operator = ( const string& )
// ------------------------------------------------------------------------- //
TStringRef&
TStringRef::operator = ( const string& inString )
{
	if (mString && mString->DecrementRef())
	{
		delete mString;
	}

	mString = new TString( inString.c_str() );
	if (mString)
	{
		mString->IncrementRef();
	}
	
	return *this;
}

// ------------------------------------------------------------------------- //
// 	* operator = ( const TStringRef& )
// ------------------------------------------------------------------------- //
TStringRef&
TStringRef::operator = ( const TStringRef& inCopy )
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

// ------------------------------------------------------------------------- //
// 	* operator = ( const TMutableStringRef& )
// ------------------------------------------------------------------------- //
TStringRef&
TStringRef::operator = ( const TMutableStringRef& inCopy )
{
	if (mString != inCopy.GetString())
	{
		if (mString && mString->DecrementRef())
		{
			delete mString;
		}

		mString = inCopy.GetString();
		if (mString)
		{
			mString->IncrementRef();
		}
	}
	
	return *this;
}

// ============================================== //
// You can't go home again, unless you set $HOME. //
// ============================================== //
