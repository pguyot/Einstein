// ==============================
// File:			TDiag.h
// Project:			Einstein
//
// Copyright 2021, Matthias Melcher, einstein@messagepad.org.
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

#ifndef _TDIAG_H
#define _TDIAG_H


// POSIX & ANSI C
#include <stdio.h>

// K
#include <K/Defines/KDefinitions.h>


class TEmulator;


/// 
/// \brief Print diagnostic output.
/// 
/// This class provides a collection of diagnostic tools that print
/// information about the current state of the emulator.
/// 
class TDiag
{
public:
	/// 
	/// \brief List all installed drivers.
	/// 
	/// The emulator uses drivers for audio output, the screen and touchpad,
	/// for the serial and network ports. There can also be various JIT
	/// machine code interpreters.
	/// 
	static void PrintDrivers(TEmulator* inEmulator);

	///
	/// \brief Print the status of all threads.
	/// 
	/// Einstein uses a number of threads to emulate NewtonOS. This
	/// method listst all (supported) threads, and if they were launched,
	/// if they are waiting, or running.
	/// 
	static void PrintThreadStates(TEmulator* inEmulator);
};

#endif
// _TDIAG_H

// =================================================== //
// Staff meeting in the conference room in %d minutes. //
// =================================================== //
