// ==============================
// Fichier:			TGPNode.cp
// Projet:			K
//
// CrŽŽ le:			16/1/2003
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
// The Original Code is TGPNode.cp.
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

#include <K/Defines/KDefinitions.h>
#include <K/GP/Tree/Nodes/TGPNode.h>

// ISO C++
#include <iostream>
#include <stdexcept>

// K Libs
#include <K/GP/TGPEvaluationContext.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPNode( KUInt32 )
// -------------------------------------------------------------------------- //
TGPNode::TGPNode( KUInt32 inArity )
	:
		mArity( inArity )
{
}

// -------------------------------------------------------------------------- //
//  * TGPNode( const TGPNode& )
// -------------------------------------------------------------------------- //
TGPNode::TGPNode( const TGPNode& inCopy )
	:
		mArity( inCopy.mArity )
{
}

// -------------------------------------------------------------------------- //
//  * ~TGPNode( void )
// -------------------------------------------------------------------------- //
TGPNode::~TGPNode( void )
{
}

// -------------------------------------------------------------------------- //
//  * operator()(const TGPNode*, const TGPNode*)
// -------------------------------------------------------------------------- //
bool
less<TGPNode*>::operator()(const TGPNode* x, const TGPNode* y)
{
	return (x->GetLispName() < y->GetLispName());
}

// -------------------------------------------------------------------------- //
//  * IsConst( void ) const
// -------------------------------------------------------------------------- //
Boolean
TGPNode::IsConst( void ) const
{
	return true;
}

// ========================================================================== //
// On two occasions I have been asked [by members of Parliament!], "Pray, Mr. //
// Babbage, if you put into the machine wrong figures, will the right answers //
// come out?"  I am not able rightly to apprehend the kind of confusion of    //
// ideas that could provoke such a question.                                  //
//                 -- Charles Babbage                                         //
// ========================================================================== //
