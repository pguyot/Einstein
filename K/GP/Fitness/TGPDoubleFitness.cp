// ==============================
// Fichier:			TGPDoubleFitness.cp
// Projet:			K
// 
// CrŽŽ le:			4/9/2003
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
// The Original Code is TGPDoubleFitness.cp.
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
// $Id: TGPDoubleFitness.cp,v 1.4 2005/02/14 20:55:03 pguyot Exp $
// ===========

#include <K/Defines/KDefinitions.h>
#include <K/GP/Fitness/TGPDoubleFitness.h>

// K
#include <K/Math/TRandomGenerator.h>

using namespace std;

// -------------------------------------------------------------------------- //
//  * TGPDoubleFitness( double )
// -------------------------------------------------------------------------- //
TGPDoubleFitness::TGPDoubleFitness( double inValue )
	:
		mValue( inValue )
{
	// This space for rent.
}

// -------------------------------------------------------------------------- //
//  * operator < ( const TGPFitness& ) const
// -------------------------------------------------------------------------- //
bool
TGPDoubleFitness::operator < ( const TGPFitness& inAlterFitness ) const
{
	return (mValue < ((const TGPDoubleFitness&) inAlterFitness).mValue);
}

// -------------------------------------------------------------------------- //
//  * PrintToStream( ostream& inStream ) const
// -------------------------------------------------------------------------- //
void
TGPDoubleFitness::PrintToStream( ostream& inStream ) const
{
	(void) (inStream << mValue);
}

// -------------------------------------------------------------------------- //
//  * Clone( void ) const
// -------------------------------------------------------------------------- //
TGPFitness*
TGPDoubleFitness::Clone( void ) const
{
	return new TGPDoubleFitness( *this );
}

// -------------------------------------------------------------------------- //
//  * operator += ( const TGPFitness& )
// -------------------------------------------------------------------------- //
TGPFitness&
TGPDoubleFitness::operator += ( const TGPFitness& inAlterFitness )
{
	mValue += (( const TGPDoubleFitness& ) inAlterFitness).mValue;
	return *this;
}

// -------------------------------------------------------------------------- //
//  * operator /= ( KUInt32 )
// -------------------------------------------------------------------------- //
TGPFitness&
TGPDoubleFitness::operator /= ( KUInt32 inDivider )
{
	mValue /= ((double) inDivider);
	return *this;
}

// -------------------------------------------------------------------------- //
//  * Random( TRandomGenerator&, const TGPFitness& ) const
// -------------------------------------------------------------------------- //
TGPFitness*
TGPDoubleFitness::Random(
					TRandomGenerator& inRandomGenerator,
					const TGPFitness& inAlterBound ) const
{
	double alterValue = ((const TGPDoubleFitness&) inAlterBound).mValue;
	return new TGPDoubleFitness(
					inRandomGenerator.DistributionDouble_1(
						alterValue, mValue) );
}

// ======================================================================= //
// The day-to-day travails of the IBM programmer are so amusing to most of //
// us who are fortunate enough never to have been one -- like watching     //
// Charlie Chaplin trying to cook a shoe.                                  //
// ======================================================================= //
