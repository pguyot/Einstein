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

#ifdef TARGET_OS_WIN32
#include <string.h>
#else
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
}

/**
 Delete the printer manager and the printer driver if allocated.

 Safely finsh the printing process and delete the FLTK driver
 from the main thread.
 */
TFLPrinterManager::~TFLPrinterManager()
{
	// Tell the main thread to delete the printer driver at the next occasion.
	if (mPrinter)
	{
		// Is the page still open?
		if (mState == State::PageOpen)
			ClosePage(mDrvr);

		// Is the printer still active?
		if (mState == State::Open)
			Close(mDrvr);

		// Is the FLTK printer driver allocated?
		Fl::awake(
			[](void* inPrinter) { delete (Fl_Printer*) inPrinter; },
			mPrinter);
		mPrinter = nullptr;
		mState = State::Uninitialized;
	}
}

/**
 Close any active printer connection.

 Safely finsh the printing process and prepare for another print run.
 */
void
TFLPrinterManager::Delete(KUInt32 inDrvr)
{
	// KPrintf("Delete\n");
	if (mPrinter)
	{
		// Is the page still open?
		if (mState == State::PageOpen)
			ClosePage(inDrvr);

		// Is the printer still active?
		if (mState == State::Open)
			Close(inDrvr);
	}
}

/**
 Allocate the FLTK printer driver if needed and open the host printer dialog.

 Must be done from the main thread.
 */
KUInt32
TFLPrinterManager::Open(KUInt32 inDrvr)
{
	// KPrintf("Open\n");

	KUInt32 ret = noErr;

	if (!mPrinter)
	{
		mDrvr = inDrvr;
		Fl::lock();
		mPrinter = new Fl_Printer();
		Fl::unlock();
		mState = State::Allocated;
	}

	// Is the page still open?
	if (mState == State::PageOpen)
		ClosePage(inDrvr);

	if (mState != State::Open)
	{
		mPromise = new std::promise<KUInt32>;
		mFuture = mPromise->get_future();

		Fl::awake(
			[](void* userdata) {
				TFLPrinterManager* self = (TFLPrinterManager*) userdata;
				self->SyncOpen();
			},
			this);

		mFuture.wait();
		ret = mFuture.get();
		if (ret == 1)
		{
			ret = kPR_ERR_UserCancel;
		} else if (ret >= 2)
		{
			ret = kPR_ERR_PrinterError;
		} else
		{
			ret = noErr;
			mState = State::Open;
		}

		delete mPromise;
	}
	SetScale(inDrvr);
	return ret;
}

void
TFLPrinterManager::SetScale(KUInt32 inDrvr)
{
	switch (GetSubType(inDrvr))
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

void
TFLPrinterManager::SyncOpen()
{
	int ret = mPrinter->begin_job();
	mPromise->set_value(ret);
}

KUInt32
TFLPrinterManager::Close(KUInt32 inDrvr)
{
	// KPrintf("Close\n");

	KUInt32 ret = noErr;

	if (!mPrinter)
		return kPR_ERR_NewtonError;

	// Is the page still open?
	if (mState == State::PageOpen)
		ClosePage(inDrvr);

	if (mState == State::Open)
	{
		mPromise = new std::promise<KUInt32>;
		mFuture = mPromise->get_future();

		Fl::awake(
			[](void* userdata) {
				TFLPrinterManager* self = (TFLPrinterManager*) userdata;
				self->SyncClose();
			},
			this);

		mFuture.wait();

		mState = State::Allocated;

		delete mPromise;
	}

	return ret;
}

void
TFLPrinterManager::SyncClose()
{
	mPrinter->end_job();
	mPromise->set_value(noErr);
}

KUInt32
TFLPrinterManager::OpenPage(KUInt32 inDrvr)
{
	// KPrintf("OpenPage\n");

	mPrinter->begin_page();
	mPrinter->push_current(mPrinter);
	SetScale(inDrvr);
	mPrinter->pop_current();
	return noErr;
}

KUInt32
TFLPrinterManager::ClosePage(KUInt32 inDrvr)
{
	(void) inDrvr;
	// KPrintf("ClosePage\n");

	mPrinter->end_page();
	return noErr;
}

KUInt32
TFLPrinterManager::ImageBand(KUInt32 inDrvr, KUInt32 inBand, KUInt32 inRect)
{
	(void) inDrvr;
	(void) inBand;
	(void) inRect;
	// KPrintf("ImageBand\n");

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
	struct PixelMap {
		KUInt32 baseAddr;
		KUInt16 rowBytes, pad; // 300, pads to long
		KUInt16 top, left, bottom, right;
		KUInt32 pixMapFlags; // 0x40000101, ptr, dot printer, 1 bit
		// Point      deviceRes;    // resolution of input device (0 indicates kDefaultDPI
		// UChar*      grayTable;    // gray tone table
	} band;

	mMemory->FastReadBuffer(inBand, sizeof(band), (KUInt8*) &band);
	band.baseAddr = htonl(band.baseAddr);
	band.rowBytes = htons(band.rowBytes);
	band.top = htons(band.top);
	band.left = htons(band.left);
	band.bottom = htons(band.bottom);
	band.right = htons(band.right);
	band.pixMapFlags = htonl(band.pixMapFlags);

	// KPrintf("T:%d, L:%d, B:%d, R:%d\n", band.top, band.left, band.bottom, band.right);

	Fl::lock();
	Fl_Printer::push_current(mPrinter);
	SetScale(inDrvr);
	mPrinter->origin(0, 0);

	fl_push_no_clip();
	fl_color(FL_BLACK);

	int w = band.right - band.left;
	int h = band.bottom - band.top;
	int rowWords = (band.rowBytes + 3) / 4; // row bytes must be 32bit aligned
	int bytes = rowWords * 4 * h;
	KUInt8* bits = (KUInt8*) malloc(bytes);
	mMemory->FastReadBuffer(band.baseAddr, bytes, bits);

	int nBlack = 0;
	int nWhite = 0;
	KUInt8* gray = (KUInt8*) malloc(w * h);
	KUInt8* dst = gray;
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
		Fl_RGB_Image* img = new Fl_RGB_Image(gray, w, h, 1);
		img->draw(band.left, band.top);
		delete img;
	}
	free(bits);
	free(gray);

	fl_pop_clip();

	Fl_Printer::pop_current();
	Fl::unlock();

	return noErr;
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
	// KPrintf("GetPageInfo\n");

	struct PrPageInfo {
		KUInt32 horizontalDPI; // DPI as a fixed point value
		KUInt32 verticalDPI;
		KUInt16 width; // page width in pixels
		KUInt16 height;
	} pageInfo;
	mMemory->FastReadBuffer(inInfo, sizeof(pageInfo), (KUInt8*) &pageInfo);

	switch (GetSubType(inDrvr))
	{
		case kSubtype72dpi:
			pageInfo.horizontalDPI = htonl(72 << 16);
			pageInfo.verticalDPI = htonl(72 << 16);
			break;
		case kSubtype150dpi:
			pageInfo.horizontalDPI = htonl(150 << 16);
			pageInfo.verticalDPI = htonl(150 << 16);
			break;
		case kSubtype300dpi:
			pageInfo.horizontalDPI = htonl(300 << 16);
			pageInfo.verticalDPI = htonl(300 << 16);
			break;
	}
	int wdt = 0, hgt = 0;
	mPrinter->printable_rect(&wdt, &hgt);
	pageInfo.width = htons(wdt);
	pageInfo.height = htons(hgt);
	mMemory->FastWriteBuffer(inInfo, sizeof(pageInfo), (KUInt8*) &pageInfo);
}

void
TFLPrinterManager::GetBandPrefs(KUInt32 inDrvr, KUInt32 inPrefs)
{
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
