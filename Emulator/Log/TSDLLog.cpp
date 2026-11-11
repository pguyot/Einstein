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

#include <cstring>
#include <deque>
#include <mutex>

namespace {
	std::mutex gCaptureMutex;
	std::deque<std::string> gCapturedLines;
	constexpr size_t kMaxCapturedLines = 300;
	SDL_LogOutputFunction gDefaultOutput = nullptr;
	void* gDefaultUserData = nullptr;

	void SDLCALL CaptureOutput(void* inUserData, int inCategory,
		SDL_LogPriority inPriority, const char* inMessage)
	{
		(void) inUserData;
		if (gDefaultOutput)
		{
			gDefaultOutput(gDefaultUserData, inCategory, inPriority, inMessage);
		}
		if (!inMessage || std::strstr(inMessage, "Handle event "))
		{
			return; // one line per SDL event would hide everything else
		}
		std::lock_guard<std::mutex> lock(gCaptureMutex);
		gCapturedLines.emplace_back(inMessage);
		while (gCapturedLines.size() > kMaxCapturedLines)
		{
			gCapturedLines.pop_front();
		}
	}
}

void
TSDLLog::InstallCapture(void)
{
	if (gDefaultOutput == nullptr)
	{
		SDL_GetLogOutputFunction(&gDefaultOutput, &gDefaultUserData);
		SDL_SetLogOutputFunction(CaptureOutput, nullptr);
	}
}

std::vector<std::string>
TSDLLog::RecentLines(size_t inMaxLines)
{
	std::lock_guard<std::mutex> lock(gCaptureMutex);
	size_t n = gCapturedLines.size() < inMaxLines ? gCapturedLines.size() : inMaxLines;
	return std::vector<std::string>(gCapturedLines.end() - n, gCapturedLines.end());
}

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
