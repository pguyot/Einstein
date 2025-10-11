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

#include <FL/Fl.H>

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

	///
	/// Close the connection to the printer
	///
	KUInt32 Close(KUInt32 inDrvr) override;

	///
	/// Start to print a page
	///
	KUInt32 OpenPage(KUInt32 inDrvr) override;

	///
	/// Finish printing a page
	///
	KUInt32 ClosePage(KUInt32 inDrvr) override;

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
	typedef void (*Async_Handler)(TFLPrinterManager* This);

	KUInt32 CallAsync(Async_Handler cb);

	KUInt32 AsyncDtor();
	static void
	AsyncDtorCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncDtor());
	}

	KUInt32 AsyncDelete();
	static void
	AsyncDeleteCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncDelete());
	}

	KUInt32 AsyncOpen();
	static void
	AsyncOpenCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncOpen());
	}

	KUInt32 AsyncClose();
	static void
	AsyncCloseCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncClose());
	}

	KUInt32 AsyncOpenPage();
	static void
	AsyncOpenPageCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncOpenPage());
	}

	KUInt32 AsyncClosePage();
	static void
	AsyncClosePageCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncClosePage());
	}

	KUInt32 AsyncImageBand();
	static void
	AsyncImageBandCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncImageBand());
	}

	KUInt32 AsyncCancelJob();
	static void
	AsyncCancelJobCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncCancelJob());
	}

	KUInt32 AsyncGetPageInfo();
	static void
	AsyncGetPageInfoCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncGetPageInfo());
	}

	KUInt32 AsyncGetBandPrefs();
	static void
	AsyncGetBandPrefsCB(TFLPrinterManager* This)
	{
		This->mPromise->set_value(This->AsyncGetBandPrefs());
	}

	static constexpr KUInt8 kSubtype72dpi = 0;
	static constexpr KUInt8 kSubtype150dpi = 1;
	static constexpr KUInt8 kSubtype300dpi = 2;

	KUInt8 GetSubType(KUInt32 inDrvr);

	void AsyncSetScale();

	enum class State {
		Uninitialized,
		Allocated,
		Open,
		PageOpen
	};
	State mState { State::Uninitialized };

	Fl_Printer* mPrinter { nullptr };

	KUInt32 mDrvr { 0 };

	std::promise<KUInt32>* mPromise;
	std::future<KUInt32> mFuture;

	struct PrPageInfo {
		KUInt32 horizontalDPI; // DPI as a fixed point value
		KUInt32 verticalDPI;
		KUInt16 width; // page width in pixels
		KUInt16 height;
	} mPageInfo;

	struct PixelMap {
		KUInt32 baseAddr;
		KUInt16 rowBytes, pad; // 300, pads to long
		KUInt16 top, left, bottom, right;
		KUInt32 pixMapFlags; // 0x40000101, ptr, dot printer, 1 bit
		// Point      deviceRes;    // resolution of input device (0 indicates kDefaultDPI
		// UChar*      grayTable;    // gray tone table
	} mBand;

	KUInt8* mBandPixels { nullptr };
};

#endif
// _TFLPRINTERMANAGER_H

// ========================================= //
// TRANSACTION CANCELLED - FARECARD RETURNED //
// ========================================= //
