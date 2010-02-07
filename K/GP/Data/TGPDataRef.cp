// ==============================
// Fichier:			TGPDataRef.cp
// Projet:			K
//
// CrŽŽ le:			13/2/2003
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
// The Original Code is TGPDataRef.cp.
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
// $Id: TGPDataRef.cp,v 1.5 2005/02/14 20:55:02 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Data/TGPDataRef.h>

// K Gen
#include <K/GP/Data/TGPData.h>
#include <K/GP/Data/TGPStringData.h>
#include <K/GP/Data/TGPBooleanData.h>
#include <K/GP/Data/Numbers/TGPSInt32Data.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  Constantes
// -------------------------------------------------------------------------- //
const string TGPDataRef::kNILString = "<nil>";

// -------------------------------------------------------------------------- //
//  * TGPDataRef( void )
// -------------------------------------------------------------------------- //
TGPDataRef::TGPDataRef( void )
	:
		mData( nil )
{
}

// -------------------------------------------------------------------------- //
//  * TGPDataRef( TGPData* )
// -------------------------------------------------------------------------- //
TGPDataRef::TGPDataRef( TGPData* inData )
	:
		mData( inData )
{
	// On incrŽmente le compteur de rŽfŽrences.
	if (mData)
	{
		mData->IncrementRef();
	}
}

// -------------------------------------------------------------------------- //
//  * TGPDataRef( TGPData& )
// -------------------------------------------------------------------------- //
TGPDataRef::TGPDataRef( TGPData& inData )
	:
		mData( &inData )
{
	inData.IsOnStack();
	inData.IncrementRef();
}

// -------------------------------------------------------------------------- //
//  * TGPDataRef( const string )
// -------------------------------------------------------------------------- //
TGPDataRef::TGPDataRef( const string inString )
	:
		mData( new TGPStringData( inString ) )
{
	// On incrŽmente le compteur de rŽfŽrences.
	mData->IncrementRef();
}

// -------------------------------------------------------------------------- //
//  * TGPDataRef( Boolean )
// -------------------------------------------------------------------------- //
TGPDataRef::TGPDataRef( Boolean inBoolean )
	:
		mData( new TGPBooleanData( inBoolean ) )
{
	// On incrŽmente le compteur de rŽfŽrences.
	mData->IncrementRef();
}

// -------------------------------------------------------------------------- //
//  * TGPDataRef( KSInt32 )
// -------------------------------------------------------------------------- //
TGPDataRef::TGPDataRef( KSInt32 inInteger )
	:
		mData( new TGPSInt32Data( inInteger ) )
{
	// On incrŽmente le compteur de rŽfŽrences.
	mData->IncrementRef();
}

// -------------------------------------------------------------------------- //
//  * TGPDataRef( const TGPDataRef& )
// -------------------------------------------------------------------------- //
TGPDataRef::TGPDataRef( const TGPDataRef& inCopy )
	:
		mData( inCopy.mData )
{
	// On incrŽmente le compteur de rŽfŽrences.
	if (mData)
	{
		mData->IncrementRef();	
	}
}

// -------------------------------------------------------------------------- //
//  * ~TGPDataRef( void )
// -------------------------------------------------------------------------- //
TGPDataRef::~TGPDataRef( void )
{
	if (mData)
	{
		// On dŽcrŽmente le nombre de rŽfŽrences sur les donnŽes, et on nettoie
		// si les donnŽes ne sont plus rŽfŽrencŽes.
		if (mData->DecrementRef())
		{
			delete mData;
		}
	}
}

// -------------------------------------------------------------------------- //
//  * operator = ( const TGPDataRef& )
// -------------------------------------------------------------------------- //
TGPDataRef&
TGPDataRef::operator = ( const TGPDataRef& inCopy )
{
	TGPData* theCopyData = inCopy.mData;
	if (mData != theCopyData)
	{
		if (mData)
		{
			// On dŽcrŽmente le nombre de rŽfŽrences sur les donnŽes, et on nettoie
			// si les donnŽes ne sont plus rŽfŽrencŽes.
			if (mData->DecrementRef())
			{
				delete mData;
			}
		}

		mData = theCopyData;
		if (theCopyData)
		{
			theCopyData->IncrementRef();
		}
	}
	
	return *this;
}

// -------------------------------------------------------------------------- //
//  * operator = ( TGPData* )
// -------------------------------------------------------------------------- //
TGPDataRef&
TGPDataRef::operator = ( TGPData* inData )
{
	if (mData != inData)
	{
		if (mData)
		{
			// On dŽcrŽmente le nombre de rŽfŽrences sur les donnŽes, et on nettoie
			// si les donnŽes ne sont plus rŽfŽrencŽes.
			if (mData->DecrementRef())
			{
				delete mData;
			}
		}

		mData = inData;
		if (inData)
		{
			inData->IncrementRef();
		}
	}
	
	return *this;
}

// -------------------------------------------------------------------------- //
//  * operator = ( Boolean )
// -------------------------------------------------------------------------- //
TGPDataRef&
TGPDataRef::operator = ( Boolean inBoolean )
{
	if (mData)
	{
		// On dŽcrŽmente le nombre de rŽfŽrences sur les donnŽes, et on nettoie
		// si les donnŽes ne sont plus rŽfŽrencŽes.
		if (mData->DecrementRef())
		{
			delete mData;
		}
	}

	mData = new TGPBooleanData( inBoolean );
	mData->IncrementRef();
	
	return *this;
}

// -------------------------------------------------------------------------- //
//  * operator = ( KSInt32 )
// -------------------------------------------------------------------------- //
TGPDataRef&
TGPDataRef::operator = ( KSInt32 inInteger )
{
	if (mData)
	{
		// On dŽcrŽmente le nombre de rŽfŽrences sur les donnŽes, et on nettoie
		// si les donnŽes ne sont plus rŽfŽrencŽes.
		if (mData->DecrementRef())
		{
			delete mData;
		}
	}

	mData = new TGPSInt32Data( inInteger );
	mData->IncrementRef();
	
	return *this;
}

// -------------------------------------------------------------------------- //
//  * operator = ( string )
// -------------------------------------------------------------------------- //
TGPDataRef&
TGPDataRef::operator = ( string inString )
{
	if (mData)
	{
		// On dŽcrŽmente le nombre de rŽfŽrences sur les donnŽes, et on nettoie
		// si les donnŽes ne sont plus rŽfŽrencŽes.
		if (mData->DecrementRef())
		{
			delete mData;
		}
	}

	mData = new TGPStringData( inString );
	mData->IncrementRef();
	
	return *this;
}

// -------------------------------------------------------------------------- //
//  * AsString( TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
const string&
TGPDataRef::AsString( TGPEvaluationContext* inContext ) const
{
	if (mData)
	{
		return mData->AsString( inContext );
	} else {
		return kNILString;
	}
}

// -------------------------------------------------------------------------- //
//  * AsBoolean( TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
Boolean
TGPDataRef::AsBoolean( TGPEvaluationContext* inContext ) const
{
	if (mData)
	{
		return mData->AsBoolean( inContext );
	} else {
		return false;
	}
}

// -------------------------------------------------------------------------- //
//  * AsInteger( TGPEvaluationContext* ) const
// -------------------------------------------------------------------------- //
KSInt32
TGPDataRef::AsInteger( TGPEvaluationContext* inContext ) const
{
	if (mData)
	{
		return mData->AsInteger( inContext );
	} else {
		return 0;
	}
}

// -------------------------------------------------------------------------- //
//  * PrintToStream( ostream& ) const
// -------------------------------------------------------------------------- //
void
TGPDataRef::PrintToStream( ostream& inStream ) const
{
	// Affichage de la donnŽe.
	if (mData)
	{
		mData->PrintToStream( inStream );
	} else {
		(void) (inStream << "<null>");
	}	
}

// ======================================================================= //
// The number of arguments is unimportant unless some of them are correct. //
//                 -- Ralph Hartley                                        //
// ======================================================================= //
