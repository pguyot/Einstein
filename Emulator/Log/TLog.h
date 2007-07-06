// ==============================
// File:			TLog.h
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

#ifndef _TLOG_H
#define _TLOG_H

#include <K/Defines/KDefinitions.h>

// K
#include <K/Threads/TMutex.h>

///
/// Classe to log emulator stuff.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 151 $
///
/// \test	aucun test défini.
///
class TLog
{
public:
	///
	/// Default constructor.
	///
	TLog( void );

	///
	/// Destructor.
	///
	virtual ~TLog( void );

	///
	/// Log a line (synchronously).
	///
	/// \param inLine	line to log.
	///
	void	LogLine( const char* inLine );

	///
	/// Log a line (synchronously).
	///
	/// \param inFormat	format to log stuff to.
	///
	void	FLogLine( const char* inLine, ... );

	///
	/// Enable the log.
	///
	void	Enable( void )
		{
			mEnabled = true;
		}

	///
	/// Disable the log.
	///
	void	Disable( void )
		{
			mEnabled = false;
		}

protected:
	///
	/// Lock the mutex.
	///
	void	LockMutex( void )
		{
			mMutex->Lock();
		}

	///
	/// Unlock the mutex.
	///
	void	UnlockMutex( void )
		{
			mMutex->Unlock();
		}

private:
	///
	/// Constructeur par copie volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TLog( const TLog& inCopy );

	///
	/// Opérateur d'assignation volontairement indisponible.
	///
	/// \param inCopy		objet à copier
	///
	TLog& operator = ( const TLog& inCopy );

	///
	/// Log a line.
	///
	/// \param inLine	line to log.
	///
	virtual void	DoLogLine( const char* inLine ) = 0;

	TMutex*	mMutex;			///< Mutex to log the lines synchronously.
	KUInt32 mEnabled;		///< If the log is enabled.
};

#endif
		// _TLOG_H

// ====================================================================== //
// In any problem, if you find yourself doing an infinite amount of work, //
// the answer may be obtained by inspection.                              //
// ====================================================================== //
