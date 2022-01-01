// ==============================
// File:			TROMImage.cp
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

#include "TROMImage.h"

// ANSI C & POSIX
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#if TARGET_OS_WIN32
	#include <io.h>
#else
	#include <sys/uio.h>
	#include <unistd.h>
#endif

// K
#include <K/Misc/TMappedFile.h>
#include <K/Defines/UByteSex.h>
#include <K/Misc/CRC32.h>

// Einstein
#include "TFlatROMImageWithREX.h"
#include "TAIFROMImageWithREXes.h"
#include "Emulator/TMemoryConsts.h"
#include "Emulator/TMemory.h"
#include "Emulator/TARMProcessor.h"
#include "Emulator/JIT/Generic/TJITGeneric_Macros.h"
#include "Emulator/TEmulator.h"
#include "Emulator/Screen/TScreenManager.h"

#include "app/Version.h"

// -------------------------------------------------------------------------- //
//  * TROMImage( void )
// -------------------------------------------------------------------------- //
TROMImage::TROMImage( void )
	:
		mImage( NULL )
{
	// I'll create the mmap file later, when asked to.
}

// -------------------------------------------------------------------------- //
//  * ~TROMImage( void )
// -------------------------------------------------------------------------- //
TROMImage::~TROMImage( void )
{
    if (mImage) {
		::free( mImage );
		mImage = NULL;
	}
}

// -------------------------------------------------------------------------- //
//  * CreateImage( const char*, const KUInt8*, KUInt32 )
// -------------------------------------------------------------------------- //
void
TROMImage::CreateImage(const KUInt8* inBuffer)
{
	// Patch the version number
	// TODO: we could have a much more complete REX management that can add or remove packages
	//       from the REX file as needed, so that we can, for example, include the internet enabler.
	// TODO: at some point, we must also patch known ROMs for the current decade to fix the Y10k bug
    if (memcmp(inBuffer+0x00806b74, "2020.2", 7)==0) {
        char *d = (char*)inBuffer+0x00806b74;
        const char *vv = PROJECT_VER_MAJOR;
        // copy no more than four characters; should be the full year
        if (vv[0]) { *d++ = vv[0];
            if (vv[1]) { *d++ = vv[1];
                if (vv[2]) { *d++ = vv[2];
                    if (vv[3]) { *d++ = vv[3]; }
                }
            }
        }
        *d++ = '.';
        const char *vm = PROJECT_VER_MINOR;
        // copy no more than two characters for the minor version number
        if (vm[0]) { *d++ = vm[0];
            if (vm[1]) { *d++ = vm[1]; }
        }
        // end of text
        *d = 0;
	}
	
    SImage* theImagePtr = (SImage*)::calloc(1, sizeof(SImage));
	
	// inBuffer contains 16 MB consisting of the ROM followed by the REX.
	// Write this at the start of the image.
	
    // TODO: we can save the time for flipping the Einstein REX if we keep it in the desired byte order already

#if TARGET_RT_LITTLE_ENDIAN
		// Endian swap it first
		KUInt32* src = (KUInt32*) inBuffer;
		KUInt32* dest = (KUInt32*) theImagePtr;
    KUInt32* end = (KUInt32*) &inBuffer[TMemoryConsts::kROMEnd];
		do {
			*dest = UByteSex::Swap( *src );
			++dest;
		} while (++src < end);
#else
	(void) ::memcpy(theImagePtr, inBuffer, inBufferSize);
#endif

    mROMId = ComputeROMId(theImagePtr->fROM);

	JITClass::PatchROM((KUInt32*) theImagePtr->fROM, mROMId);
	
	// Compute the checksum.
	DoComputeChecksums(theImagePtr);

    mImage = theImagePtr;
}

// -------------------------------------------------------------------------- //
//  * GetLatestModDate( struct timespec*, const char* )
// -------------------------------------------------------------------------- //
int
TROMImage::GetLatestModDate(
				time_t* ioModDate,
				const char* inPath )
{
	int err;
	do {
		// Check the file exists.
		struct stat theInfos;
		err = ::stat( inPath, &theInfos );
		if (err < 0)
		{
			// The file (probably) doesn't exist.
			break;
		}
		
		// Compare with ioModDate and update it if what we have is newer.
		if (theInfos.st_mtime > *ioModDate)
		{
			*ioModDate = theInfos.st_mtime;
		}
	} while (false);
	
	return err;
}

// -------------------------------------------------------------------------- //
//  * ComputeChecksums( KUInt32 outChecksums[10] ) const
// -------------------------------------------------------------------------- //
void
TROMImage::ComputeChecksums( KUInt32 outChecksums[10] ) const
{
	// Copy checksum from the SImagePtr.
	KUInt32 index;
	for (index = 0; index < 10; index++)
	{
		outChecksums[index] = mImage->fInfo.fChecksums[index];
	}
}

// -------------------------------------------------------------------------- //
//  * DoComputeChecksums( SImage* )
// -------------------------------------------------------------------------- //
void
TROMImage::DoComputeChecksums( SImage* inImage )
{
	const KUInt8* thePointer = inImage->fROM;

	// Find RExes.
	KUInt32 theBaseSize;
	KUInt32 theRexBases[4];
	KUInt32 theRexSizes[4];
	
	KUInt32 nbRexes = LookForREXes(
						thePointer,
						&theBaseSize,
						theRexBases,
						theRexSizes );
	for (; nbRexes < 4; nbRexes++)
	{
		theRexBases[nbRexes] = 0;
		theRexSizes[nbRexes] = 0;
	}

	// Compute base checksum.
	ComputeSegmentChecksums( thePointer, theBaseSize, &inImage->fInfo.fChecksums[0] );
	
	// Compute REX checksums.
	KUInt32 indexRexes;
	for (indexRexes = 0; indexRexes < 4; indexRexes++)
	{
		ComputeSegmentChecksums(
			thePointer + theRexBases[indexRexes],
			theRexSizes[indexRexes],
			&inImage->fInfo.fChecksums[(2*indexRexes)+2] );
	}
}

// -------------------------------------------------------------------------- //
//  * ComputeSegmentChecksums( KUInt32 outChecksums[10] )
// -------------------------------------------------------------------------- //
void
TROMImage::ComputeSegmentChecksums(
						const KUInt8* inPointer,
						KUInt32 inSize,
						KUInt32 outChecksums[2] )
{
	// if the segment is empty, the checksums are -1, -1.
	if (inSize == 0)
	{
		outChecksums[0] = 0xFFFFFFFF;
		outChecksums[1] = 0xFFFFFFFF;
	} else {
		KUInt32* theCursor = (KUInt32*) inPointer;
		KUInt32* theEnd = (KUInt32*) (inPointer + inSize);
		KUInt32 highBits = 0;
		KUInt32 lowBits = 0;
		do
		{
			KUInt32 value = *theCursor++;
			lowBits += value & 0x0000FFFF;
			highBits += value >> 16;
		} while (theCursor != theEnd);
		
		outChecksums[0] = highBits;
		outChecksums[1] = lowBits;
	}
}

// -------------------------------------------------------------------------- //
//  * LookForREXes( const KUInt8*, KUInt32*, KUInt32*, KUInt32* )
// -------------------------------------------------------------------------- //
KUInt32
TROMImage::LookForREXes(
					const KUInt8* inBuffer,
					KUInt32* outBaseSize,
					KUInt32 outRexBases[4],
					KUInt32 outRexSizes[4] )
{
	KUInt32* swappedROM = (KUInt32*) inBuffer;
	// The size of the base ROM.
	KUInt32 theBaseSize = swappedROM[0x3C/4];
	if (theBaseSize >= 0x00800000)
	{
		(void) ::fprintf(
					stderr,
					"Couldn't determine base ROM size (bad ROM?)\n" );
		::abort();
	}

	*outBaseSize = theBaseSize;
	
	// Look for rexes just afterwards.
	KUInt32 nbRexes = 0;
	KUInt32 rexCursor = theBaseSize;
	do {
		if (rexCursor > 0x00800000 - 0x20)
		{
			break;
		}
		if ((swappedROM[rexCursor / 4] == 'RExB')
			&& (swappedROM[(rexCursor / 4) + 1] == 'lock'))
		{
			// Found a rex.
			KUInt32 theRexSize = swappedROM[(rexCursor + 0x18) / 4];
			if (
				(theRexSize < 0x20)
				|| (rexCursor + theRexSize > 0x00800000))
			{
				(void) ::fprintf(
							stderr,
							"Invalid size for REx %i at %.8X\n",
							(int) nbRexes,
							(unsigned int) rexCursor);
				::abort();
			}
			
			outRexBases[nbRexes] = rexCursor;
			outRexSizes[nbRexes++] = theRexSize;
			rexCursor += theRexSize;
		} else {
			break;
		}
	} while (nbRexes < 4);
	
	// Look for rexes at 0x00800000.
	if ((nbRexes < 4) && (rexCursor < 0x00800000))
	{
		rexCursor = 0x00800000;

		do {
			if ((swappedROM[rexCursor / 4] == 'RExB')
				&& (swappedROM[(rexCursor / 4) + 1] == 'lock'))
			{
				// Found a rex.
				KUInt32 theRexSize = swappedROM[(rexCursor + 0x18) / 4];
				if (
					(theRexSize < 0x20)
					|| (rexCursor + theRexSize > 0x01000000))
				{
					(void) ::fprintf(
								stderr,
								"Invalid size for REx %i at %.8X\n",
								(int) nbRexes,
								(unsigned int) rexCursor);
					::abort();
				}
				// Patch the REx to have a sequential ID, or NewtonOS will
				// be very confused an erase the user's Flash image.
				swappedROM[(rexCursor / 4) + 7] = nbRexes;
				
				outRexBases[nbRexes] = rexCursor;
				outRexSizes[nbRexes++] = theRexSize;
				rexCursor += theRexSize;
			} else {
				break;
			}
		} while (true);
	}

	return nbRexes;
}


KSInt32 TROMImage::ComputeROMId(KUInt8 *inROMPtr)
{
    // Identify the ROM by taking the CRC32 of the ROM and internal REX.

    // The manufacturer of the ROM may change, but the remaining content is the same
    KUInt32 tmpManufacturer[1];
    memcpy(tmpManufacturer, inROMPtr+0x000013fC, sizeof(tmpManufacturer));
    memset(inROMPtr+0x000013fC, 0, sizeof(tmpManufacturer));

    // Also, make a copy of the diagnostics and checksums (they are unset in the developer ROM)
    KUInt32 tmpDiagCheckTag[12];
    memcpy(tmpDiagCheckTag, inROMPtr+0x00018420, sizeof(tmpDiagCheckTag));
    memset(inROMPtr+0x00018420, 0, sizeof(tmpDiagCheckTag));

    // Get a neutral CRC32 of the ROM minus the variables
    KUInt32 crc = GetCRC32(inROMPtr, 0x00800000);
//    FILE *f = fopen("/Users/matt/img", "wb");
//    fwrite(inROMPtr, 1, 0x00800000, f);
//    fclose(f);

    // Now restore the variable content
    memcpy(inROMPtr+0x000013fC, tmpManufacturer, sizeof(tmpManufacturer));
    memcpy(inROMPtr+0x00018420, tmpDiagCheckTag, sizeof(tmpDiagCheckTag));

    KSInt32 romID = kUnknownROM;
    switch (crc) {
        case 0x2bab2cee: // MP2x00(US): 2.1(711000)-1, can be updated to 2.1/710031
            romID = k717006;
            break;
        case 0x62081e10: // eMate 300(US): v2.2.00-0(737041) can be updated to v2.1/737246
            romID = kEMate300ROM;
            break;
		case 0xa9862ccc: // MP2100(D): (747129)  (747260)
			romID = kMP2x00DROM;
			break;
		case 0x176d233b: // Watson (MP2100 French)
			romID = kWatsonROM;
			break;
		default:
            KPrintf("Unknown ROM with CRC 0x%08x. No patches will be applied.\n", crc);
            break;
    }
    return romID;
}


#if TARGET_OS_WIN32
static int strcasecmp(const char *a, const char *b) { return stricmp(a, b); }
#endif

#if TARGET_UI_FLTK

# include <FL/filename.H>

#else

#define FL_PATH_MAX PATH_MAX
const char *fl_filename_name(const char *path)
{
    const char *win = strrchr(path, '\\');
    const char *bsd = strrchr(path, '/');
    const char *name = (win>bsd) ? win : bsd;
    if (name==nullptr) name = path;
    return name;
}
const char *fl_filename_ext(const char *path)
{
    const char *name = fl_filename_name(path);
    const char *dot = strrchr(path, '/');
    if (dot>name)
        return dot;
    else
        return path + strlen(path);
}
void fl_filename_setext(char *buf, int size, const char *ext)
{
    char *d = (char*)fl_filename_ext(buf);
    strcpy(d, ext);
}
#endif

TROMImage *TROMImage::LoadROMAndREX(const char *theROMImagePath, Boolean useMonitor, Boolean useBuiltinERex)
{
    (void)useMonitor;
    TROMImage *theROMImage = nullptr;

    // If we use the builtin REX, set the REX path to null
    // If we want an external file, take the ROM path with the filename "Einstein.rex"
    char *theREX1Path = nullptr;
    char theREX1PathBuffer[FL_PATH_MAX];
    if (useBuiltinERex) {
        theREX1Path = nullptr;
    } else {
        strcpy(theREX1PathBuffer, theROMImagePath);
        char *rexName = (char *) fl_filename_name(theREX1PathBuffer);
        if (rexName) {
            strcpy(rexName, "Einstein.rex");
        }
        theREX1Path = theREX1PathBuffer;
    }

    // Read an .aif image
    const char *ext = fl_filename_ext(theROMImagePath);
    if ( ext && strcasecmp(ext, ".aif")==0 ) {
        char theREX0Path[FL_PATH_MAX];
        strcpy(theREX0Path, theROMImagePath);
        fl_filename_setext(theREX0Path, FL_PATH_MAX, ".rex");
        theROMImage = new TAIFROMImageWithREXes(theROMImagePath, theREX0Path, theREX1Path);
        return theROMImage;
    }

    // Or is it the pair of "Senior CirrusNoDebug image" and "Senior CirrusNoDebug high"?
    const char *name = fl_filename_name(theROMImagePath);
    if (   name
        &&  (   strncmp(name, "Senior Cirrus", 13)==0
             || strncmp(name, "Senior DCirrus", 14)==0 )
        && strstr(name, "image"))
    {
        char theREX0Path[FL_PATH_MAX];
        strcpy(theREX0Path, theROMImagePath);
        char *image = strstr(theREX0Path, "image");
        strcpy(image, "high");
        theROMImage = new TAIFROMImageWithREXes(theROMImagePath, theREX0Path, theREX1Path);
        return theROMImage;
    }

    // If it's none of the above, just load a file verbatim and hope it's a ROM
    theROMImage = new TFlatROMImageWithREX(theROMImagePath, theREX1Path);
    return theROMImage;
}


// ====================================================== //
// Is a computer language with goto's totally Wirth-less? //
// ====================================================== //
