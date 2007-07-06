// ==============================
// File:			TScheduler.h
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

#ifndef _TSCHEDULER_H
#define _TSCHEDULER_H

#include <K/Defines/KDefinitions.h>

// K
#include <K/Misc/TDoubleLinkedList.h>

///
/// Class for the Relativity Scheduler.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision$
///
/// \test	aucun test défini.
///
class TScheduler
{
public:
	///
	/// Constructeur par défaut.
	///
	TScheduler( void );

	///
	/// Destructeur.
	///
	~TScheduler( void );

	///
	/// Add a task if not current.
	///
	void AddWhenNotCurrent( TTask* inTask );

	///
	/// Add a task.
	///
	void Add( TTask* inTask );

	///
	/// Remove a task.
	///
	void Remove( TTask* inTask );

	///
	/// Enter atomic.
	///
	void EnterAtomic( void );

	///
	/// Exit atomic.
	///
	void ExitAtomic( void );

	///
	/// Say we want schedule.
	/// NOP.
	///
	void WantSchedule( void );

	///
	/// Put the scheduler on hold.
	///
	void HoldSchedule( void );

	///
	/// Allow other tasks.
	///
	void AllowSchedule( void );

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TScheduler( const TScheduler& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TScheduler& operator = ( const TScheduler& inCopy );

	///
	/// Get the current task.
	///
	TTask* GetCurrentTask( void );

	/// \name Variables
	KUInt32				mHoldScheduleLevel;	///< Equivalent to gHoldScheduleLevel.
	TDoubleLinkedList	mTasks;				///< List of the tasks.
};

#endif
		// _TSCHEDULER_H

// ===================================================================== //
// The number of UNIX installations has grown to 10, with more expected. //
//         -- The Unix Programmer's Manual, 2nd Edition, June 1972       //
// ===================================================================== //
