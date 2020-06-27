// ==============================
// File:			TMainStartupDriver.impl.h
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

#ifndef _TMAINSTARTUPDRIVER_IMPL_H
#define _TMAINSTARTUPDRIVER_IMPL_H

#include "TStartupDriver.h"

///
/// Class for our startup driver.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
PROTOCOL TMainStartupDriver : public TStartupDriver
	PROTOCOLVERSION(1.0)
{
public:
	PROTOCOL_IMPL_HEADER_MACRO(TMainStartupDriver);

	CAPABILITIES( ("copyright" "Paul Guyot") ("version" "1.0") )

	///
	/// Entry point.
	///
	void Init( void );
};

#endif
		// _TMAINSTARTUPDRIVER_IMPL_H

// =============================================================== //
// If it's worth hacking on well, it's worth hacking on for money. //
// =============================================================== //
