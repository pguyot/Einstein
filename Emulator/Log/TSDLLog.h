// ==============================
// File:			TSDLLog.h
// Project:			Einstein
//
// Copyright 2025 by Matthias Melcher,
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

#ifndef _TSDLLOG_H
#define _TSDLLOG_H

#include <K/Defines/KDefinitions.h>
#include "TLog.h"

///
/// Class for logging to stdout.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 140 $
///
/// \test	aucun test défini.
///
class TSDLLog
		: public TLog
{
public:
	///
	/// Default constructor.
	///
	TSDLLog(void);

private:
	///
	/// Log a line.
	///
	/// \param inLine	line to log.
	///
	virtual void DoLogLine(const char* inLine);
};

#endif
// _TSDLLOG_H

// ============================ //
// All constants are variables. //
// ============================ //
