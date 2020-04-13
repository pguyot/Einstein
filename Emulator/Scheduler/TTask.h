// ==============================
// File:			TTask.h
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

#ifndef _TTASK_H
#define _TTASK_H

#include <K/Defines/KDefinitions.h>

// K
#include <K/Misc/TDoubleLinkedList.h>
#include <K/Threads/TThread.h>

///
/// Class for a task in Einstein.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
class TTask : public TDoubleLinkedElem
{
public:
	///
	/// Default constructor.
	///
	TTask( void );

	///
	/// Destructor.
	///
	~TTask( void );

	///
	/// Determine if we're the current task.
	///
	bool IsCurrentTask( void ) const;

	///
	/// Suspend for EnterAtomic.
	///
	void SuspendAtomic( void );

	///
	/// Resume for ExitAtomic.
	///
	void ExitAtomic( void );

	///
	/// Thread entry point.
	///
	void Run( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TTask( const TTask& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TTask& operator = ( const TTask& inCopy );

	/// \name Variables
	TThread<TTask>	mThread;			///< Thread associated with this task.
	bool			mAtomicSuspended;	///< If the task was suspended with
										///< SuspendAtomic.
};

#endif
		// _TTASK_H

// ============================================================ //
// Do you guys know what you're doing, or are you just hacking? //
// ============================================================ //
