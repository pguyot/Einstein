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
#include <FL/Fl_Bitmap.H>
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
		mState = State::None;
	}
}

/**
 Close any active printer connection.

 Safely finsh the printing process and prepare for another print run.
 */
void
TFLPrinterManager::Delete(KUInt32 inDrvr)
{
	KPrintf("Delete\n");
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
	KPrintf("Open\n");

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

	return ret;
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
	KPrintf("Close\n");

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
	(void) inDrvr;
	KPrintf("OpenPage\n");
	mPrinter->begin_page();
//	mPrinter->scale(0.1, 0.1);
	return noErr;
}

KUInt32
TFLPrinterManager::ClosePage(KUInt32 inDrvr)
{
	(void) inDrvr;
	KPrintf("ClosePage\n");
	mPrinter->end_page();
	return noErr;
}

KUInt32
TFLPrinterManager::ImageBand(KUInt32 inDrvr, KUInt32 inBand, KUInt32 inRect)
{
	(void) inDrvr;
	(void) inBand;
	(void) inRect;
	KPrintf("ImageBand\n");

	struct {
		KUInt32 baseAddr;
		KUInt16 rowBytes, pad;    // 300, pads to long
		KUInt16 top, left, bottom, right;
		KUInt32 pixMapFlags;	// 40000101
		//        Point      deviceRes;    // resolution of input device (0 indicates kDefaultDPI
		//        UChar*      grayTable;    // gray tone table
		//      } PixelMap;
	} band;
	mMemory->FastReadBuffer(inBand, sizeof(band), (KUInt8*)&band);
	band.baseAddr = htonl(band.baseAddr);
	band.rowBytes = htons(band.rowBytes);
	band.top = htons(band.top);
	band.left = htons(band.left);
	band.bottom = htons(band.bottom);
	band.right = htons(band.right);
	band.pixMapFlags = htonl(band.pixMapFlags);
	KPrintf("T:%d, L:%d, B:%d, R:%d\n", band.top, band.left, band.bottom, band.right);

	KUInt8 buf[64];
	mMemory->FastReadBuffer(inBand, 32, buf);
	for (int i=0; i<32; i++) KPrintf("%02x", buf[i]);
	KPrintf("\n");
	mMemory->FastReadBuffer(inRect, 32, buf);
	for (int i=0; i<32; i++) KPrintf("%02x", buf[i]);
	KPrintf("\n");


	Fl::lock();
	mPrinter->push_current(mPrinter);
	mPrinter->scale(72.0/300.0);

	int width, height;
	mPrinter->printable_rect(&width, &height);
	fl_color(FL_BLACK);
	fl_line_style(FL_SOLID, 2);
	fl_rect(band.left, band.top, band.right-band.left, band.bottom-band.top);

	int w = band.right - band.left;
	int h = band.bottom - band.top;
	int bytes = band.rowBytes * h;
	KUInt8 *bits = (KUInt8*)malloc(bytes);
	mMemory->FastReadBuffer(band.baseAddr, bytes, bits);

	Fl_Bitmap *bm = new Fl_Bitmap(bits, w, h);
	bm->draw(band.left, band.top);
	delete bm;
	free(bits);

//	fl_font(FL_COURIER, 12);
//	time_t now; time(&now); fl_draw(ctime(&now), 0, fl_height());

	mPrinter->pop_current();
	Fl::unlock();


//	KUInt32 tl, br;
//	mMemory->Read(inRect+0, tl);
//	mMemory->Read(inRect+4, br);
//	KPrintf("minRect: T:%d, L:%d, B:%d, R:%d\n",
//		tl >> 16, tl & (0xffff),
//		br >> 16, br & (0xffff));

	return noErr;
	// Image Band is sent many times until page is all printed
	//      struct Rect
	//      {
	//        short  top;
	//        short  left;
	//        short  bottom;
	//        short  right;
	//      };
	//      #define    kPixMapStorage    0xC0000000  // to mask off the appropriate bits
	//      #define    kPixMapHandle    0x00000000  // baseAddr is a handle
	//      #define    kPixMapPtr      0x40000000  // baseAddr is a pointer
	//      #define    kPixMapOffset    0x80000000  // baseAddr is an offset from the PixelMap
	//      #define    kPixMapLittleEndian  0x20000000  // pixMap is little endian
	//      #define    kPixMapAllocated  0x10000000  // pixMap "owns" the bits memory
	//      #ifdef QD_Gray
	//      #define    kPixMapGrayTable  0x08000000  // grayTable field exists
	//      #define    kPixMapNoPad    0x04000000  // direct pixel format, no pad byte
	//      #define    kPixMapByComponent  0x02000000  // direct pixel format, stored by component
	//      #define    kPixMapAntiAlias  0x01000000  // antialiasing ink text
	//      #endif
	//      #define    kPixMapVersionMask  0x0000F000  // version of this struct
	//      #define    kPixMapDeviceType  0x00000F00  // bits 8..11 are device type code
	//      #define    kPixMapDevScreen  0x00000000  //   screen or offscreen bitmap
	//      #define    kPixMapDevDotPrint  0x00000100  //   dot matrix printer
	//      #define    kPixMapDevPSPrint  0x00000200  //   postscript printer
	//      #define    kPixMapDepth    0x000000FF  // bits 0..7 are chunky pixel depth
	//
	//      #define    kOneBitDepth    1
	//      #define   kDefaultDPI      72      // default value for deviceRes fields
	//      #define   kVersionShift    12
	//
	//      #define    kPixMapVersion1    (0x0 << kVersionShift)
	//      #define    kPixMapVersion2    (0x1 << kVersionShift)
	//      typedef struct PixelMap
	//      {
	//        Ptr        baseAddr;
	//        short      rowBytes;    // 300, pads to long
	//        Rect      bounds;
	//        ULong      pixMapFlags;	// 40000101
	//        Point      deviceRes;    // resolution of input device (0 indicates kDefaultDPI
	//        UChar*      grayTable;    // gray tone table
	//      } PixelMap;

	//	KPrintf("PDImageBand\n");
	//	{
	//		KUInt32 tl, br;
	//		KUInt32 band = mProcessor->GetRegister(1);
	//		KUInt16 rowBytes, top, left, bottom, right;
	//		mMemory->Read(band + 4, tl);
	//		rowBytes = tl >> 16;
	//		top = (KUInt16) tl;
	//		mMemory->Read(band + 8, tl);
	//		left = tl >> 16;
	//		bottom = (KUInt16) tl;
	//		mMemory->Read(band + 8, tl);
	//		right = tl >> 16;
	//		KPrintf("PixelMap: RowBytes:%d T:%d, L:%d, B:%d, R:%d\n",
	//			rowBytes, top, left, bottom, right);
	//
	//		KUInt32 rect = mProcessor->GetRegister(2);
	//		mMemory->Read(rect, tl);
	//		mMemory->Read(rect, br);
	//		KPrintf("minRect: T:%d, L:%d, B:%d, R:%d\n",
	//			tl >> 16, tl & (0xffff),
	//			br >> 16, br & (0xffff));
	//	}
	//	mProcessor->SetRegister(0, 0); // no error
}

void
TFLPrinterManager::CancelJob(KUInt32 inDrvr, KUInt32 inAsyncCancel)
{
	(void) inDrvr;
	(void) inAsyncCancel;
	KPrintf("CancelJob\n");
}

void
TFLPrinterManager::GetPageInfo(KUInt32 inDrvr, KUInt32 inInfo)
{
	(void) inDrvr;
	(void) inInfo;
	KPrintf("GetPageInfo\n");
}

void
TFLPrinterManager::GetBandPrefs(KUInt32 inDrvr, KUInt32 inPrefs)
{
	(void) inDrvr;
	(void) inPrefs;
	KPrintf("GetBandPrefs\n");
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
