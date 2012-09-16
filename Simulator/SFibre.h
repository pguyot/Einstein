// ==============================
// File:                        Simulator/SFibre.h
// Project:                     Simulator
//
// Copyright 2012 by Matthias Melcher (newton@matthiasm.com).
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

#ifndef SIMULATOR_SFIBRE_H
#define SIMULATOR_SFIBRE_H

#include <K/Defines/KDefinitions.h>
#include <K/Threads/TFibre.h>


namespace Sim {

	typedef void (*STask)();
	
	class SFibre : public TFibre {
	public:
		SFibre()
		: TFibre()
		{
		}
		KSInt32 Task(KSInt32 inReason=0, void* inUserData=0L)
		{
			STask task = (STask)inUserData;
			task();
			return 0;
		}
	};

  
} // namespace


#endif
// SIMULATOR_SFIBRE_H

