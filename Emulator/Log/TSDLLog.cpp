// ==============================
// File:			TSDLLog.cp
// Project:			Einstein
//
// Copyright 2025 by Matthias Melcher.
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

#include "TSDLLog.h"

#include <SDL3/SDL_log.h>

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TStdOutLog( void )
// -------------------------------------------------------------------------- //
TSDLLog::TSDLLog(void)
{
}

// -------------------------------------------------------------------------- //
//  * DoLogLine( const char* )
// -------------------------------------------------------------------------- //
void
TSDLLog::DoLogLine(const char* inLine)
{
	// Log a message with SDL_LOG_CATEGORY_APPLICATION and SDL_LOG_PRIORITY_INFO.
	SDL_Log(" %s", inLine);
}

// ======================================================================= //
// I am professionally trained in computer science, which is to say        //
// (in all seriousness) that I am extremely poorly educated.               //
//                 -- Joseph Weizenbaum, "Computer Power and Human Reason" //
// ======================================================================= //
