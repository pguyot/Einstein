// ==============================
// File:			TIOSPrinterManager.mm
// Project:			Einstein
//
// Copyright 2022 by Jesús A. Álvarez (zydeco@namedfork.net).
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

#include "TIOSPrinterManager.h"

#include "Emulator/TMemory.h"

@interface TIOSPrintPageRenderer : UIPrintPageRenderer
@property CGSize pageSize;
@property CGFloat printerDpi;
- (KUInt32)openPage;
- (KUInt32)closePage;
- (KUInt32)renderImageBand:(const KUInt8*)imageBits bytesPerRow:(KUInt16)rowBytes inRect:(CGRect)rect;
@end

TIOSPrinterManager::TIOSPrinterManager(TLog* inLog) :
		TPrinterManager(inLog)
{
	(void) mLog;
}

/**
 Delete the printer manager and all allocated resources.
 */
TIOSPrinterManager::~TIOSPrinterManager()
{
	mRenderer = nil;
}

/**
 Close any active printer connection.

 Roll back the state of the connection to Allocated.
 */
void
TIOSPrinterManager::Delete(KUInt32 inDrvr)
{
	mDrvr = inDrvr;

	// nothing to do
	if (mState == State::Uninitialized)
		return;

	// we are already at the requested state
	if (mState == State::Allocated)
		return;

	if (mState == State::Open)
		mState = State::Allocated;
}

/**
 iOS needs to know the number of pages before the printing UI is shown: everything must be printed
 */
KUInt32
TIOSPrinterManager::Open(KUInt32 inDrvr)
{
	mDrvr = inDrvr;

	// Fail if printing is not supported
	if (![UIPrintInteractionController isPrintingAvailable])
	{
		return kPR_ERR_NotFound;
	}

	// Is the connection to the printer still open? Then we are done.
	if (mState == State::Open)
		return 0;

	// Create page renderer
	mRenderer = [TIOSPrintPageRenderer new];

	mState = State::Open;

	return 0;
}

/**
 Close the connection to the printer and flush all pages.
 */
KUInt32
TIOSPrinterManager::Close(KUInt32 inDrvr)
{
	mDrvr = inDrvr;

	if (mRenderer == nil || mState != State::Open)
		return kPR_ERR_NewtonError;

	dispatch_sync(dispatch_get_main_queue(), ^{
		UIPrintInteractionController* printInteractionController = UIPrintInteractionController.sharedPrintController;
		printInteractionController.printPageRenderer = mRenderer;
		[printInteractionController presentAnimated:YES
								  completionHandler:^(UIPrintInteractionController* _Nonnull printInteractionController, BOOL completed, NSError* _Nullable error) {
									  // TODO: handle errors?
									  this->mRenderer = nil;
								  }];
	});

	return 0;
}

/**
 Start a new page in our printing process.
 This call may pop up dialogs (progress, error, etc.), so it must be called
 from the UI thread.
 */
KUInt32
TIOSPrinterManager::OpenPage(KUInt32 inDrvr)
{
	mDrvr = inDrvr;
	return [mRenderer openPage];
}

/**
 Finsh the current page.
 */
KUInt32
TIOSPrinterManager::ClosePage(KUInt32 inDrvr)
{
	mDrvr = inDrvr;
	return [mRenderer closePage];
}

KUInt32
TIOSPrinterManager::ImageBand(KUInt32 inDrvr, KUInt32 inBand, KUInt32 inRect)
{
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
	ret = [mRenderer renderImageBand:bits bytesPerRow:mBand.rowBytes inRect:CGRectMake(mBand.left, mBand.top, w, h)];
	free(bits);

	return ret;
}

void
TIOSPrinterManager::CancelJob(KUInt32 inDrvr, KUInt32 inAsyncCancel)
{
	(void) inDrvr;
	(void) inAsyncCancel;
}

void
TIOSPrinterManager::GetPageInfo(KUInt32 inDrvr, KUInt32 inInfo)
{
	mDrvr = inDrvr;

	mMemory->FastReadBuffer(inInfo, sizeof(mPageInfo), (KUInt8*) &mPageInfo);
	mPageInfo.horizontalDPI = ntohl(mPageInfo.horizontalDPI);
	mPageInfo.verticalDPI = ntohl(mPageInfo.verticalDPI);
	mPageInfo.width = ntohs(mPageInfo.width);
	mPageInfo.height = ntohs(mPageInfo.height);

	int dpi = 300;
	switch (GetSubType(mDrvr))
	{
		case kSubtype72dpi:
			dpi = 72;
			break;
		case kSubtype150dpi:
			dpi = 150;
			break;
		case kSubtype300dpi:
			dpi = 300;
			break;
	}

	mPageInfo.horizontalDPI = (dpi << 16);
	mPageInfo.verticalDPI = (dpi << 16);
	mRenderer.printerDpi = dpi;

	// Read paper size from user defaults
	// It's too soon to read from UIKit, and impossible to read from NewtonOS
	NSUserDefaults* prefs = [NSUserDefaults standardUserDefaults];
	CGSize printableSize = CGSizeFromString([prefs stringForKey:@"printable_size"]);
	if (printableSize.width <= 0 || printableSize.height <= 0)
	{
		// Default iOS reported size for US Letter
		printableSize = { .width = 8.0, .height = 9.88897 };
	}

	mPageInfo.width = floor(dpi * printableSize.width);
	mPageInfo.height = floor(dpi * printableSize.height);
	mRenderer.pageSize = CGSizeMake(mPageInfo.width, mPageInfo.height);

	// convert to guest endianness
	mPageInfo.horizontalDPI = htonl(mPageInfo.horizontalDPI);
	mPageInfo.verticalDPI = htonl(mPageInfo.verticalDPI);
	mPageInfo.width = htons(mPageInfo.width);
	mPageInfo.height = htons(mPageInfo.height);
	mMemory->FastWriteBuffer(inInfo, sizeof(mPageInfo), (KUInt8*) &mPageInfo);
}

void
TIOSPrinterManager::GetBandPrefs(KUInt32 inDrvr, KUInt32 inPrefs)
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
}

KUInt8
TIOSPrinterManager::GetSubType(KUInt32 inDrvr)
{
	KUInt8 subtype = 255;
	mMemory->ReadB(inDrvr + 31, subtype);
	return subtype;
}

@implementation TIOSPrintPageRenderer {
	NSMutableArray<UIImage*>* pages;
	CGContextRef currentCtx;
	CGColorSpaceRef gColorSpace, bwColorSpace;
}

- (instancetype)init
{
	if ((self = [super init]))
	{
		pages = [NSMutableArray new];
		currentCtx = NULL;
		_pageSize = CGSizeZero;
		gColorSpace = CGColorSpaceCreateDeviceGray();
		uint8_t colors[] = { 255, 0 };
		bwColorSpace = CGColorSpaceCreateIndexed(gColorSpace, 1, colors);
	}
	return self;
}

- (void)dealloc
{
	if (currentCtx)
	{
		CGContextRelease(currentCtx);
	}
	CGColorSpaceRelease(bwColorSpace);
	CGColorSpaceRelease(gColorSpace);
}

- (void)drawPageAtIndex:(NSInteger)pageIndex inRect:(CGRect)printableRect
{
	[pages[pageIndex] drawAtPoint:printableRect.origin];
}

- (NSInteger)numberOfPages
{
	return pages.count;
}

- (KUInt32)openPage
{
	NSAssert(currentCtx == NULL, @"openPage without closing previous page!");

	// create context for new page
	size_t width = floor(_pageSize.width);
	size_t height = floor(_pageSize.height);
	currentCtx = CGBitmapContextCreate(NULL, width, height, 8, width, gColorSpace, kCGImageAlphaNone);
	CGContextSetGrayFillColor(currentCtx, 1.0, 1.0);
	CGContextFillRect(currentCtx, CGRectMake(0, 0, width, height));
	return 0;
}

- (KUInt32)closePage
{
	CGImageRef imageRef = CGBitmapContextCreateImage(currentCtx);
	[pages addObject:[UIImage imageWithCGImage:imageRef scale:_printerDpi / 72.0 orientation:UIImageOrientationUp]];
	CGImageRelease(imageRef);
	CGContextRelease(currentCtx);
	currentCtx = NULL;

	return 0;
}

- (KUInt32)renderImageBand:(const KUInt8*)imageBits bytesPerRow:(KUInt16)rowBytes inRect:(CGRect)rect
{
	// create image from current band
	CGDataProviderRef dataProvider = CGDataProviderCreateWithData(NULL, imageBits, rect.size.height * rowBytes, NULL);
	CGImageRef image = CGImageCreate(rect.size.width, rect.size.height, 1, 1, rowBytes, bwColorSpace, kCGImageAlphaNone, dataProvider, NULL, false, kCGRenderingIntentDefault);
	CGDataProviderRelease(dataProvider);

	// draw in current page context
	rect.origin.y = _pageSize.height - rect.origin.y;
	CGContextDrawImage(currentCtx, rect, image);
	CGImageRelease(image);
	return 0;
}

@end
