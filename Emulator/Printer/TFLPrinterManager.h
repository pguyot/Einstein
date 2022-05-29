// ==============================
// File:      TFLPrinterManager.h
// Project:      Einstein
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

#ifndef _TFLPRINTERMANAGER_H
#define _TFLPRINTERMANAGER_H

#include "Emulator/Printer/TPrinterManager.h"

#include <future>

class TLog;
class TMemory;
class Fl_Printer;

///
/// Class to handle a single printer.
/// This class is subclassed by host implementations.
///
class TFLPrinterManager : public TPrinterManager
{
public:
	///
	/// Constructor from a log.
	///
	/// \param inLog        log interface (can be null)
	///
	TFLPrinterManager(TLog* inLog = nil);

	///
	/// Destructor.
	///
	~TFLPrinterManager() override;

	///
	/// Printer is no longer used
	///
	void Delete(KUInt32 inDrvr) override;

	///
	/// Open a connection to the host printer
	///
	KUInt32 Open(KUInt32 inDrvr) override;
	void SyncOpen();

	///
	/// Close the connection to the printer
	///
	KUInt32 Close(KUInt32 inDrvr) override;
	void SyncClose();

	///
	/// Start to print a page
	///
	KUInt32 OpenPage(KUInt32 inDrvr) override;
	// void SyncOpenPage();

	///
	/// Finish printing a page
	///
	KUInt32 ClosePage(KUInt32 inDrvr) override;
	// void SyncClosePage();

	///
	/// NewtonOS sends a band of pixel data to be printed
	///
	KUInt32 ImageBand(KUInt32 inDrvr, KUInt32 inBand, KUInt32 inRect) override;

	///
	/// User or Newton wants to cancel the current print job.
	///
	void CancelJob(KUInt32 inDrvr, KUInt32 inAsyncCancel) override;

	///
	/// Fill in the Info structure about our paper size and print resolution
	///
	void GetPageInfo(KUInt32 inDrvr, KUInt32 inInfo) override;

	///
	/// Fill in the structure to have bands delivered a certain way
	///
	void GetBandPrefs(KUInt32 inDrvr, KUInt32 inPrefs) override;

protected:
	static constexpr KUInt8 kSubtype72dpi = 0;
	static constexpr KUInt8 kSubtype150dpi = 1;
	static constexpr KUInt8 kSubtype300dpi = 2;

	KUInt8 GetSubType(KUInt32 inDrvr);

	void SetScale(KUInt32 inDrvr);

	enum class State {
		Uninitialized,
		Allocated,
		Open,
		PageOpen
	};
	State mState = State::Uninitialized;

	Fl_Printer* mPrinter = nullptr;

	KUInt32 mDrvr = 0;

	std::promise<KUInt32>* mPromise;
	std::future<KUInt32> mFuture;
};

#endif
// _TFLPRINTERMANAGER_H

// ========================================= //
// TRANSACTION CANCELLED - FARECARD RETURNED //
// ========================================= //
