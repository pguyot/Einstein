// ==============================
// Fichier:			TGPNodeConst.cp
// Projet:			K
//
// CrŽŽ le:			25/2/2003
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
// The Original Code is TGPNodeConst.cp.
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
// $Id: TGPNodeConst.cp,v 1.7 2005/02/14 20:55:04 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNodeConst.h>

// ISO C++
#include <sstream>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPNodeConst( TGPDataRef& )
// -------------------------------------------------------------------------- //
TGPNodeConst::TGPNodeConst( TGPDataRef& inValue )
	:
		mValue( inValue ),
		mType( inValue.GetClass() )
{
	// On utilise sstream.
	ostringstream someStream;
	(void) (someStream << "const<" << mType.GetName() << "-" << inValue << ">");
	
	mLispName = someStream.str();
}

// -------------------------------------------------------------------------- //
//  * TGPNodeConst( const TGPType&, TGPDataRef& )
// -------------------------------------------------------------------------- //
TGPNodeConst::TGPNodeConst( const TGPType& inType, TGPDataRef& inValue )
	:
		mValue( inValue ),
		mType( inType )
{
	// On utilise sstream.
	ostringstream someStream;
	(void) (someStream << "const<" << inType.GetName() << "-" << inValue << ">");
	
	mLispName = someStream.str();
}

// -------------------------------------------------------------------------- //
//  * GetType( void ) const
// -------------------------------------------------------------------------- //
const TGPType&
TGPNodeConst::GetType( void ) const
{
	return mType;
}

// -------------------------------------------------------------------------- //
//  * GetLispName( void ) const
// -------------------------------------------------------------------------- //
TStringRef
TGPNodeConst::GetLispName( void ) const
{
	return mLispName;
}

// -------------------------------------------------------------------------- //
//  * IsConst( void ) const
// -------------------------------------------------------------------------- //
Boolean
TGPNodeConst::IsConst( void ) const
{
	return true;
}

// -------------------------------------------------------------------------- //
//  * Evaluate( TGPNodeRef*, TGPDataRef*, vector<TGPDataRef>&, ... )
// -------------------------------------------------------------------------- //
TGPNodeRef*
TGPNodeConst::Evaluate(
				TGPNodeRef* /* inNodeRef */,
				TGPDataRef* ioResult,
				vector<TGPDataRef>& /* ioStorage */,
				TGPEvaluationContext* /* inContext */,
				KUIntPtr* /* ioCookie */ )
{
	*ioResult = mValue;
	
	return nil;
}

// =============================================================== //
// Per buck you get more computing action with the small computer. //
//                 -- R.W. Hamming                                 //
// =============================================================== //
