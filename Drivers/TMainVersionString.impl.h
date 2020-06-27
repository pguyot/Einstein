// ==============================
// File:			TMainVersionString.impl.h
// Project:			Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net).
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

#ifndef _TMAINVERSIONSTRING_IMPL_H
#define _TMAINVERSIONSTRING_IMPL_H

#include "TVersionString.h"

///
/// Class for our version string.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL TMainVersionString : public TVersionString
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(TMainVersionString);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	TVersionString*	New( void );				// PROTOCOL ctor
	void			Delete( void );				// PROTOCOL dtor
	void			VersionString( UShort* outVersionString );
};

#endif
		// _TMAINVERSIONSTRING_IMPL_H

// ============================================================================== //
// The problems of business administration in general, and database management in //
// particular are much to difficult for people that think in IBMese, compounded   //
// with sloppy english.                                                           //
//                 -- Edsger Dijkstra                                             //
// ============================================================================== //
