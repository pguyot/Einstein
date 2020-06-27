// ==============================
// File:			PEinsteinInTranslator.impl.h
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

#ifndef _PEINSTEININTRANSLATOR_IMPL_H
#define _PEINSTEININTRANSLATOR_IMPL_H

#include "PInTranslator.h"

///
/// Class for the debugger NS in translator.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 117 $
///
/// \test	aucun test défini.
///
PROTOCOL PEinsteinInTranslator : public PInTranslator
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(PEinsteinInTranslator);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	PInTranslator*	New( void );
	void			Delete( void );

	ULong			Init(void*);
	ULong			Idle( void );
	ULong			FrameAvailable( void );
	Ref				ProduceFrame( int );
};

#endif
		// _PEINSTEININTRANSLATOR_IMPL_H

// ==================================================== //
// /earth is 98% full ... please delete anyone you can. //
// ==================================================== //
