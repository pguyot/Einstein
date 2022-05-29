// ==============================
// File:			TPrinterManager.h
// Project:			Einstein
//
// Copyright 2022 by Matthias Melcher (einstein@messagepad.org).
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

#ifndef _TPRINTERMANAGER_H
#define _TPRINTERMANAGER_H

#include <K/Defines/KDefinitions.h>

class TLog;
class TMemory;

#define ERRBASE_PRINTING (-44000) // Printing errors
#define PR_ERROR_BASE (ERRBASE_PRINTING)
#define kPR_ERR_PrinterError (PR_ERROR_BASE)
#define kPR_ERR_NewtonError (PR_ERROR_BASE - 1)
#define kPR_ERR_NotFound (PR_ERROR_BASE - 2)
#define kPR_ERR_Busy /* out-box should try again! */ (PR_ERROR_BASE - 3)
#define kPR_ERR_UserCancel (PR_ERROR_BASE - 4)
#define kPR_ERR_LostContact (PR_ERROR_BASE - 5)
#define kPR_ERR_TooComplex (PR_ERROR_BASE - 6)

///
/// Class to handle a single printer.
/// This class is subclassed by host implementations.
///
class TPrinterManager
{
public:
	///
	/// Constructor from a log.
	///
	/// \param inLog        log interface (can be null)
	///
	TPrinterManager(TLog* inLog = nil);

	///
	/// Destructor.
	///
	virtual ~TPrinterManager();

	///
	/// Printer is no longer used
	///
	virtual void Delete(KUInt32 inDrvr);

	///
	/// Open a connection to the host printer
	///
	virtual KUInt32 Open(KUInt32 inDrvr);

	///
	/// Close the connection to the printer
	///
	virtual KUInt32 Close(KUInt32 inDrvr);

	///
	/// Start to print a page
	///
	virtual KUInt32 OpenPage(KUInt32 inDrvr);

	///
	/// Finish printing a page
	///
	virtual KUInt32 ClosePage(KUInt32 inDrvr);

	///
	/// NewtonOS sends a band of pixel data to be printed
	///
	virtual KUInt32 ImageBand(KUInt32 inDrvr, KUInt32 inBand, KUInt32 inRect);

	///
	/// User or Newton wants to cancel the current print job.
	///
	virtual void CancelJob(KUInt32 inDrvr, KUInt32 inAsyncCancel);

	///
	/// NewtonOS wants to know if the current problem was solved
	///
	virtual KUInt32 IsProblemResolved(KUInt32 inDrvr);

	///
	/// Fill in the Info structure about our paper size and print resolution
	///
	virtual void GetPageInfo(KUInt32 inDrvr, KUInt32 inInfo);

	///
	/// Fill in the structure to have bands delivered a certain way
	///
	virtual void GetBandPrefs(KUInt32 inDrvr, KUInt32 inPrefs);

	///
	/// For Fax connections only
	///
	virtual KUInt32 FaxEndPage(KUInt32 inDrvr, KUInt32 inPageCount);

	///
	/// We need to be able to read and write NewtonOS memory.
	///
	void
	SetMemory(TMemory* inMemory)
	{
		mMemory = inMemory;
	}

protected:
	TMemory* mMemory = nullptr; ///< Reference to emulated memory.

	TLog* mLog = nullptr; ///< Reference to the log.
};

#endif
// _TPRINTERMANAGER_H

// ========================================= //
// TRANSACTION CANCELLED - FARECARD RETURNED //
// ========================================= //
