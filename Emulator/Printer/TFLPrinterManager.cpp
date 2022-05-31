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

#include "TFLPrinterManager.h"

#include "Emulator/TMemory.h"

#include <FL/Fl_Printer.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/fl_draw.H>

#include <stdlib.h>

#if TARGET_OS_WIN32
#include <Winsock2.h>
#include <string.h>
#else
#include <arpa/inet.h>
#include <strings.h>
#endif

// Einstein.

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

// -------------------------------------------------------------------------- //
//  * TFLPrinterManager(TLog*)
// -------------------------------------------------------------------------- //
TFLPrinterManager::TFLPrinterManager(TLog* inLog /* = nil */) :
		TPrinterManager(inLog)
{
	(void) mLog;
	setenv("CG_CONTEXT_SHOW_BACKTRACE", "1", 1);
}

/**
 Delete the printer manager and all allocated resources.
 */
TFLPrinterManager::~TFLPrinterManager()
{
	CallAsync(AsyncDtorCB);
	// TODO: deallocate all resources that were create in the emulator thread
}

/**
 Delete all resources that were allocated in the UI thread.
 */
KUInt32
TFLPrinterManager::AsyncDtor()
{
	// roll the state back to Allocated
	AsyncDelete();

	// delete the FLTK printer driver
	if (mPrinter)
	{
		delete mPrinter;
		mPrinter = nullptr;
		mState = State::Uninitialized;
	}

	return 0;
}

/**
 Close any active printer connection.

 Roll back the state of the connection to Allocated.
 */
void
TFLPrinterManager::Delete(KUInt32 inDrvr)
{
	mDrvr = inDrvr;

	// nothing to do
	if (mState == State::Uninitialized)
		return;

	// we are already at the requested state
	if (mState == State::Allocated)
		return;

	CallAsync(AsyncDeleteCB);
}

/**
 Close the printer driver.

 In the emulation, this does not actually delete anything.
 */
KUInt32
TFLPrinterManager::AsyncDelete()
{
	// Is the page still open?
	if (mState == State::PageOpen)
		AsyncClosePage();

	// Is the printer connection still open?
	if (mState == State::Open)
		AsyncClose();

	return 0;
}

/**
 Allocate the FLTK printer driver if needed and open the host printer dialog.
 */
KUInt32
TFLPrinterManager::Open(KUInt32 inDrvr)
{
	KPrintf("Open\n");
	mDrvr = inDrvr;
	return CallAsync(AsyncOpenCB);
}

/**
 Allocate and open the printer driver from the UI thread.
 */
KUInt32
TFLPrinterManager::AsyncOpen()
{
	KUInt32 ret = 0;

	// Allocate the FLTK printer driver if there is none yet.
	if (!mPrinter)
	{
		mPrinter = new Fl_Printer();
		mState = State::Allocated;
	}

	// Is the page still open?
	if (mState == State::PageOpen)
		AsyncClosePage();

	// Is the connection to the printer still open? Then we are done.
	if (mState == State::Open)
		return 0;

	// State must be Allocated at this point.
	// The following call will open a dialog box on all platforms that allows
	// the user to select a printer, the page size, etc. .
	int err = mPrinter->begin_job();
	if (err == 1)
	{
		ret = kPR_ERR_UserCancel;
	} else if (err >= 2)
	{
		ret = kPR_ERR_PrinterError;
	} else
	{
		ret = 0;
		mState = State::Open;
	}

	return ret;
}

/**
 Close the connection to the printer and flush all pages.
 */
KUInt32
TFLPrinterManager::Close(KUInt32 inDrvr)
{
	KPrintf("Close\n");
	mDrvr = inDrvr;
	return CallAsync(AsyncCloseCB);
}

/**
 Close the connection to the printer.
 This call may pop up dialog boxes, launch preview apps, etc., so it must be
 called from within the UI thread.
 */
KUInt32
TFLPrinterManager::AsyncClose()
{
	KUInt32 ret = 0;

	if (!mPrinter)
		return kPR_ERR_NewtonError;

	// Is the page still open?
	if (mState == State::PageOpen)
		AsyncClosePage();

	if (mState == State::Open)
	{
		mPrinter->end_job();
		mState = State::Allocated;
	}
	return ret;
}

/**
 Start a new page in our printing process.
 This call may pop up dialogs (progress, error, etc.), so it must be called
 from the UI thread.
 */
KUInt32
TFLPrinterManager::OpenPage(KUInt32 inDrvr)
{
	KPrintf("OpenPage\n");
	mDrvr = inDrvr;
	return CallAsync(AsyncOpenPageCB);
}

/**
 Start a new page.
 */
KUInt32
TFLPrinterManager::AsyncOpenPage()
{
	if (!mPrinter)
		return kPR_ERR_NewtonError;

	// Is the printer connection open?
	if (mState != State::Open)
		return kPR_ERR_NewtonError;

	mPrinter->begin_page();
	mState = State::PageOpen;
	AsyncSetScale();

	// FLTK expects more printing calls, but since we do this asynchronously,
	// we deactivate the driver for now and reactivate it when needed.
	Fl_Surface_Device::pop_current();

	return 0;
}

/**
 Finsh the current page.
 */
KUInt32
TFLPrinterManager::ClosePage(KUInt32 inDrvr)
{
	KPrintf("ClosePage\n");
	mDrvr = inDrvr;
	return CallAsync(AsyncClosePageCB);
}

/**
 Finish the current page in the UI thread.
 */
KUInt32
TFLPrinterManager::AsyncClosePage()
{
	if (!mPrinter)
		return kPR_ERR_NewtonError;

	// Is the printer connection open?
	if (mState != State::PageOpen)
		return kPR_ERR_NewtonError;

	// Make sure that this device is active before ending the page.
	Fl_Surface_Device::push_current(mPrinter);

	mPrinter->end_page();
	mState = State::Open;

	return 0;
}

KUInt32
TFLPrinterManager::ImageBand(KUInt32 inDrvr, KUInt32 inBand, KUInt32 inRect)
{
	KPrintf("ImageBand\n");
	mDrvr = inDrvr;
	(void) inRect;

	KUInt32 ret = 0;

	// TODO: we currently only support 1 bit bitmaps
	// TODO: we should give an error message for unsupported bitmap formats
	// #define    kPixMapStorage    0xC0000000  // to mask off the appropriate bits
	// #define    kPixMapHandle    0x00000000  // baseAddr is a handle
	// #define    kPixMapPtr      0x40000000  // baseAddr is a pointer
	// #define    kPixMapOffset    0x80000000  // baseAddr is an offset from the PixelMap
	// #define    kPixMapLittleEndian  0x20000000  // pixMap is little endian
	// #define    kPixMapAllocated  0x10000000  // pixMap "owns" the bits memory
	// #ifdef QD_Gray
	// #define    kPixMapGrayTable  0x08000000  // grayTable field exists
	// #define    kPixMapNoPad    0x04000000  // direct pixel format, no pad byte
	// #define    kPixMapByComponent  0x02000000  // direct pixel format, stored by component
	// #define    kPixMapAntiAlias  0x01000000  // antialiasing ink text
	// #endif
	// #define    kPixMapVersionMask  0x0000F000  // version of this struct
	// #define    kPixMapDeviceType  0x00000F00  // bits 8..11 are device type code
	// #define    kPixMapDevScreen  0x00000000  //   screen or offscreen bitmap
	// #define    kPixMapDevDotPrint  0x00000100  //   dot matrix printer
	// #define    kPixMapDevPSPrint  0x00000200  //   postscript printer
	// #define    kPixMapDepth    0x000000FF  // bits 0..7 are chunky pixel depth

	mMemory->FastReadBuffer(inBand, sizeof(mBand), (KUInt8*) &mBand);
	mBand.baseAddr = htonl(mBand.baseAddr);
	mBand.rowBytes = htons(mBand.rowBytes);
	mBand.top = htons(mBand.top);
	mBand.left = htons(mBand.left);
	mBand.bottom = htons(mBand.bottom);
	mBand.right = htons(mBand.right);
	mBand.pixMapFlags = htonl(mBand.pixMapFlags);

	int w = mBand.right - mBand.left;
	int h = mBand.bottom - mBand.top;
	int rowWords = (mBand.rowBytes + 3) / 4; // row bytes must be 32bit aligned
	int bytes = rowWords * 4 * h;
	KUInt8* bits = (KUInt8*) malloc(bytes);
	mMemory->FastReadBuffer(mBand.baseAddr, bytes, bits);

	int nBlack = 0;
	int nWhite = 0;
	mBandPixels = (KUInt8*) malloc(w * h);
	KUInt8* dst = mBandPixels;
	for (int y = 0; y < h; y++)
	{
		KUInt8* src = bits + y * rowWords * 4;
		KUInt8 b = 0;
		for (int x = 0; x < w; x++)
		{
			if ((x & 7) == 0)
				b = *src++;
			*dst++ = (b & 128) ? 0 : 255;
			if (b & 128)
				nBlack++;
			else
				nWhite++;
			b = b << 1;
		}
	}
	if (nBlack)
	{
		ret = CallAsync(AsyncImageBandCB);
	}
	free(bits);
	free(mBandPixels);

	return ret;
}

/**
 Render the pixel data into the printer buffer.
 */
KUInt32
TFLPrinterManager::AsyncImageBand()
{
	if (!mPrinter)
		return kPR_ERR_NewtonError;

	// Is the printer connection open?
	if (mState != State::PageOpen)
		return kPR_ERR_NewtonError;

	int x = mBand.left;
	int y = mBand.top;
	int w = mBand.right - mBand.left;
	int h = mBand.bottom - mBand.top;
	// KPrintf("T:%d, L:%d, B:%d, R:%d\n", band.top, band.left, band.bottom, band.right);

	Fl_Printer::push_current(mPrinter);
	AsyncSetScale();
	mPrinter->origin(0, 0);

	fl_push_no_clip();
	fl_color(FL_BLACK);

	fl_draw_image_mono(mBandPixels, x, y, w, h);

	// Visualize the bands
	//	fl_rect(x, y, h, h);
	//	fl_rect(x+w-h-1, y, h, h);

	fl_pop_clip();

	Fl_Printer::pop_current();

	return 0;
}

void
TFLPrinterManager::CancelJob(KUInt32 inDrvr, KUInt32 inAsyncCancel)
{
	(void) inDrvr;
	(void) inAsyncCancel;
	// KPrintf("CancelJob\n");
}

void
TFLPrinterManager::GetPageInfo(KUInt32 inDrvr, KUInt32 inInfo)
{
	KPrintf("GetPageInfo\n");
	mDrvr = inDrvr;

	mMemory->FastReadBuffer(inInfo, sizeof(mPageInfo), (KUInt8*) &mPageInfo);
	mPageInfo.horizontalDPI = htonl(mPageInfo.horizontalDPI);
	mPageInfo.verticalDPI = htonl(mPageInfo.verticalDPI);
	mPageInfo.width = htons(mPageInfo.width);
	mPageInfo.height = htons(mPageInfo.height);

	CallAsync(AsyncGetPageInfoCB);

	mPageInfo.horizontalDPI = htonl(mPageInfo.horizontalDPI);
	mPageInfo.verticalDPI = htonl(mPageInfo.verticalDPI);
	mPageInfo.width = htons(mPageInfo.width);
	mPageInfo.height = htons(mPageInfo.height);
	mMemory->FastWriteBuffer(inInfo, sizeof(mPageInfo), (KUInt8*) &mPageInfo);
}

KUInt32
TFLPrinterManager::AsyncGetPageInfo()
{
	double scale = 1.0;

	switch (GetSubType(mDrvr))
	{
		case kSubtype72dpi:
			mPageInfo.horizontalDPI = (72 << 16);
			mPageInfo.verticalDPI = (72 << 16);
			scale = 1.0;
			break;
		case kSubtype150dpi:
			mPageInfo.horizontalDPI = (150 << 16);
			mPageInfo.verticalDPI = (150 << 16);
			scale = 150.0 / 72.0;
			break;
		case kSubtype300dpi:
			mPageInfo.horizontalDPI = (300 << 16);
			mPageInfo.verticalDPI = (300 << 16);
			scale = 300.0 / 72.0;
			break;
	}
	int wdt = 0, hgt = 0;
	mPrinter->printable_rect(&wdt, &hgt);

	// Check if the viewport is available and was already scaled correctly.
	if (mState != State::PageOpen)
	{
		wdt *= scale;
		hgt *= scale;
	}

	mPageInfo.width = wdt;
	mPageInfo.height = hgt;
	//	KPrintf(" %d %d\n", wdt, hgt);

	return 0;
}

void
TFLPrinterManager::GetBandPrefs(KUInt32 inDrvr, KUInt32 inPrefs)
{
	// The driver sets the default to a 50 pixel band
	//	typedef struct DotPrinterPrefs {
	//		long		minBand;				/* smallest useable band			*/
	//		long		optimumBand;			/* a good size to try to default	*/
	//		Boolean		asyncBanding;			/* true if band data sent async		*/
	//		Boolean		wantMinBounds;			/* true if minrect is useful		*/
	//	} DotPrinterPrefs;
	(void) inDrvr;
	(void) inPrefs;
	// KPrintf("GetBandPrefs\n");
}

/**
 Call FLTK printer driver functions exclusively from the main UI thread.

 The printer manager gets called from the CPU emulator thread, but FLTK
 printer calls must be made exclusively from the main UI thread. This
 method tells FLTK to execute some call as soon as there are no other UI
 calls to process. The method then waits until the call is finshed.
 */
KUInt32
TFLPrinterManager::CallAsync(Async_Handler cb)
{
	mPromise = new std::promise<KUInt32>;
	mFuture = mPromise->get_future();
	Fl::awake((Fl_Awake_Handler) cb, this);
	mFuture.wait();
	KUInt32 ret = mFuture.get();
	delete mPromise;
	return ret;
}

void
TFLPrinterManager::AsyncSetScale()
{
	switch (GetSubType(mDrvr))
	{
		case kSubtype72dpi:
			mPrinter->scale(1.0); // FLTK default is 72dpi
			break;
		case kSubtype150dpi:
			mPrinter->scale(72.0 / 150.0); // FLTK default is 72dpi
			break;
		case kSubtype300dpi:
			mPrinter->scale(72.0 / 300.0); // FLTK default is 72dpi
			break;
	}
}

KUInt8
TFLPrinterManager::GetSubType(KUInt32 inDrvr)
{
	KUInt8 subtype = 255;
	mMemory->ReadB(inDrvr + 31, subtype);
	return subtype;
}

// ============================================================================= //
// As in Protestant Europe, by contrast, where sects divided endlessly into      //
// smaller competing sects and no church dominated any other, all is different   //
// in the fragmented world of IBM.  That realm is now a chaos of conflicting     //
// norms and standards that not even IBM can hope to control.  You can buy a     //
// computer that works like an IBM machine but contains nothing made or sold by  //
// IBM itself.  Renegades from IBM constantly set up rival firms and establish   //
// standards of their own.  When IBM recently abandoned some of its original     //
// standards and decreed new ones, many of its rivals declared a puritan         //
// allegiance to IBM's original faith, and denounced the company as a divisive   //
// innovator.  Still, the IBM world is united by its distrust of icons and       //
// imagery.  IBM's screens are designed for language, not pictures.  Graven      //
// images may be tolerated by the luxurious cults, but the true IBM faith relies //
// on the austerity of the word.                                                 //
//                 -- Edward Mendelson, "The New Republic", February 22, 1988    //
// ============================================================================= //
