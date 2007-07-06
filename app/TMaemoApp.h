// ==============================
// File:			TMaemoApp.h
// Project:			Einstein
//
// Copyright 2003-2007 by Paul Guyot (pguyot@kallisys.net).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#ifndef _TMAEMOAPP_H
#define _TMAEMOAPP_H

#include <K/Defines/KDefinitions.h>

///
/// Class for Einstein program with Maemo.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
class TMaemoApp
{
public:
	///
	/// Default constructor.
	///
	TMaemoApp( void );

	///
	/// Destructor.
	///
	~TMaemoApp( void );

	///
	/// Entry point.
	///
	void Run( int argc, char* argv[] );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMaemoApp( const TMaemoApp& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TMaemoApp& operator = ( const TMaemoApp& inCopy );

	/// \name Variables

};

#endif
		// _TMAEMOAPP_H

// ======================================================================= //
// One good reason why computers can do more work than people is that they //
// never have to stop and answer the phone.                                //
// ======================================================================= //
