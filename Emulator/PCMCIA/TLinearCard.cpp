// ==============================
// File:			TLinearCard.cp
// Project:			Einstein
//
// Copyright 2003-2020 by Paul Guyot (pguyot@kallisys.net)
// and Matthias Melcher (einstein@matthiasm.com).
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

#include "TLinearCard.h"

// Einstein
#include "Emulator/TEmulator.h"
#include "Emulator/Log/TLog.h"
#include "Emulator/PCMCIA/TPCMCIAController.h"

// -------------------------------------------------------------------------- //
// Constantes
// -------------------------------------------------------------------------- //

const char *TLinearCard::kIdent = "TLinearCard";

//#define MATTS_2MB_CARD
//#define DIAG_1MB_CARD
#define APPLE_2MB_CARD

#ifdef MATTS_2MB_CARD
#include "FlashCIS.cpp"
#include "FlashDump.cpp"
#endif

#ifdef DIAG_1MB_CARD
#include "DiagCard.cpp"
#endif

#ifdef APPLE_2MB_CARD
#include "2MBCardApple.cpp"
#endif

const KUInt8 TLinearCard::kDefaultCISData[] = {
    // INTEL 2MD Linear Flash
    0x01, 0x03, // CISTPL_DEVICE
    0x52,   // Device Code = 5 (DTYPE_FLASH), WPS = 0 (WP Switch works), Speed = 2 (DSPEED_200NS)
    0x06,   // 2MB
    0xff,   // CISTPL_END
    0x1e, 0x06, // CISTPL_DEVICEGEO
    0x02,   // DGTPL_BUS (16 bit access)
    0x11,   // DGTPL_EBS erase block size = 2^16 - 64kB
    0x01,   // DGTPL_RBS minimum read size (1 byte)
    0x01,   // DGTPL_WBS minimum write block (1 byte)
    0x03,   // DGTPL_PART minimal partition size
    0x01,   // DGTPL_HWIL not interleaving
    0x18, 0x02, // CISTPL_JEDEC_C
    0x89,   // Intel
    0xa2,   // A2 28F008SA (1MBIT*8), A6 28F008S5, AA 28F016S5
    0x15, 0x50,  // CISTPL_VERS_1
    0x04,   // major version 4
    0x01,   // minor version 1
    0x69,   // text: manufacturer \0 product \0 product info 2 \0 product info 3 \0\ff
    // "intel SERIES2-02 \0 2H REGBASE 4000h DBBDRELP \0 COPYRIGHT intel CORPORATION 1991"
    0x6e, 0x74, 0x65, 0x6c, 0x00, 0x53, 0x45, 0x52, 0x49, 0x45, 0x53, 0x32, 0x2d, 0x30, 0x32, 0x20,
    0x00, 0x32, 0x48, 0x20, 0x52, 0x45, 0x47, 0x42, 0x41, 0x53, 0x45, 0x20, 0x34, 0x30, 0x30, 0x30,
    0x68, 0x20, 0x44, 0x42, 0x42, 0x44, 0x52, 0x45, 0x4c, 0x50, 0x00, 0x43, 0x4f, 0x50, 0x59, 0x52,
    0x49, 0x47, 0x48, 0x54, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x6c, 0x20, 0x43, 0x4f, 0x52, 0x50, 0x4f,
    0x52, 0x41, 0x54, 0x49, 0x4f, 0x4e, 0x20, 0x31, 0x39, 0x39, 0x31, 0x00, 0xff,
    0x1a, 0x06, // CISTPL_CONFIG
    0x01,    // TPCC_RMSZ=0 TPCC_RASZ=1
    0x00,   // TPCC_LAST
    0x00,    // TPCC_RADR Configuration Registers Base Address in Attribute Space
    0x40,   // TPCC_RADR high byte
    0x03,    // TPCC_RMSK two config registers present
    0xff,   // done
    0xff
};


// -------------------------------------------------------------------------- //
//  * TLinearCard( KUInt32 )
// -------------------------------------------------------------------------- //
TLinearCard::TLinearCard(const char* inFilename, const char* inFilenameCIS)
{
    if (!inFilename)
        inFilename = "C:/Users/micro/Downloads/MP2000-PCMCIA-Image_S.PILET/MP2000-PCMCIA-Image_S.PILET/Fodor94";
    if (!inFilenameCIS)
        inFilenameCIS = "C:/Users/micro/Downloads/MP2000-PCMCIA-Image_S.PILET/MP2000-PCMCIA-Image_S.PILET/Fodor94-CIS";

    FILE *f = fopen(inFilename, "rb");
    // FIXME: and what if we have an error?
    fseek(f, 0, SEEK_END);
    KUInt32 size = (KUInt32)ftell(f);
    fseek(f, 0, SEEK_SET);
    mSize = size;
    mMemoryMap = (KUInt8*)malloc(mSize);
    fread(mMemoryMap, 1, mSize, f);
    fclose(f);

#if 0
    mCISDataSize = sizeof(kCISData);
    mCISData = (KUInt8*)malloc(mCISDataSize);
    memmove(mCISData, kCISData, mCISDataSize);

    switch (mSize / 1024 / 1024) {
    case  2: mCISData[3] = 0x06; break;
    case  4: mCISData[3] = 0x0e; break;
    case  8: mCISData[3] = 0x1e; break;
    case 16: mCISData[3] = 0x3e; break;
    case 24: mCISData[3] = 0x5e; break;
    case 32: mCISData[3] = 0x7e; break;
    case 48: mCISData[3] = 0xbe; break;
    case 64: mCISData[3] = 0xfe; break;
    }
    //mCISData[3] = 1;
    //scale = *p & 7;
    //if (scale == 7)
    //    return -EINVAL;
    //device->dev[i].size = ((*p >> 3) + 1) * (512 << (scale * 2));
     //   2MB=06(6)
     //   8MB 1Eh
     //   16MB 3Eh
     //   24MB 5Eh
     //   32MB 7Eh
     //   48MB BEh
     //   64MB FEh
#else
    mCISDataSize = 1024;
    mCISData = (KUInt8*)malloc(mCISDataSize);
    f = fopen(inFilenameCIS, "rb");
    // FIXME: and what if we have an error?
    fread(mCISData, 1, mCISDataSize, f);
    for (int i = 0; i < 512; i++) {
        mCISData[i] = mCISData[(i ^ 1) * 2 +1];
    }
    fclose(f);

#endif
}

#if 0
TLinearCard::TLinearCard( KUInt32 inSize )
:   mSize(inSize)
{
    mMemoryMap = (KUInt8*)malloc(mSize);
    memset(mMemoryMap, 0xff, mSize);
#ifdef MATTS_2MB_CARD
    memcpy(mMemoryMap, FlashDump, mSize);
#endif
#ifdef DIAG_1MB_CARD
    memcpy(mMemoryMap, DiagCard, mSize/2);
#endif
#ifdef APPLE_2MB_CARD
    memcpy(mMemoryMap, Apple2MB, mSize);
#endif
}
#endif



// -------------------------------------------------------------------------- //
//  * ~TLinearCard( void )
// -------------------------------------------------------------------------- //
TLinearCard::~TLinearCard( void )
{
    if (mMemoryMap)
        ::free(mMemoryMap);
    if (mCISData)
        ::free(mCISData);
}

// -------------------------------------------------------------------------- //
//  * GetVPCPins( void )
// -------------------------------------------------------------------------- //
KUInt32
TLinearCard::GetVPCPins( void )
{
	if (GetLog())
	{
		GetLog()->LogLine( "TLinearCard::GetVPCPins" );
	}

    return TPCMCIAController::k1C00_CardIsPresent
         | TPCMCIAController::k1C00_Pin16_37        // RDY/-BSY, IREQ, INTRQ
//       | TPCMCIAController::k1C00_Pin33_24        // WP, Write Protect Slider
         | TPCMCIAController::k1C00_Pin62_45        // BVD2, -SPKR, -DASP [I/O]
         | TPCMCIAController::k1C00_Pin63_46        // BVD1, -STSCHG, -PDIAG [I/O]
    ;
}

// -------------------------------------------------------------------------- //
//  * SetVPCPins( KUInt32 )
// -------------------------------------------------------------------------- //
void
TLinearCard::SetVPCPins( KUInt32 inPins )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::SetVPCPins( %.4X )", (unsigned int) inPins );
	}
}

// -------------------------------------------------------------------------- //
//  * ReadAttr( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TLinearCard::ReadAttr( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::ReadAttr( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadAttrB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TLinearCard::ReadAttrB( KUInt32 inOffset )
{
    KUInt8 theResult = 0;

    inOffset = (inOffset/2)^1; // byte addresses are 32bit-flipped
    if (inOffset<mCISDataSize) {
        theResult = mCISData[ inOffset];
    } else {
        theResult = 0;
    }

    if (GetLog())
    {
        GetLog()->FLogLine( "TLinearCard::ReadAttrB( %.8X => %d = '%c' (%d))",
                           (unsigned int) inOffset,
                           inOffset>>3,
                           (theResult>31&&theResult<128)?theResult:'.',
                           theResult );
//      GetEmulator()->BreakInMonitor();
    }
    return theResult;
}

// -------------------------------------------------------------------------- //
//  * ReadIO( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TLinearCard::ReadIO( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::ReadIO( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadIOB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TLinearCard::ReadIOB( KUInt32 inOffset )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::ReadIOB( %.8X )", (unsigned int) inOffset );
	}
	
	return 0;
}

// -------------------------------------------------------------------------- //
//  * ReadMem( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt32
TLinearCard::ReadMem( KUInt32 inOffset )
{
    KUInt32 v;

    switch (mState) {
        case kReadArray:
            if (inOffset<mSize) {
                v = (mMemoryMap[inOffset]<<24) | (mMemoryMap[inOffset+1]<<16) | (mMemoryMap[inOffset+2]<<8) | (mMemoryMap[inOffset+3]<<0);
            } else {
                v = 0;
            }
            break;
        default:
            v = (mStatusRegister<<24)|(mStatusRegister<<16);
            break;
    }

	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::ReadMem( %.8X ) = %.8X", (unsigned int) inOffset, v );
	}
	
	return v;
}

// -------------------------------------------------------------------------- //
//  * ReadMemB( KUInt32 )
// -------------------------------------------------------------------------- //
KUInt8
TLinearCard::ReadMemB( KUInt32 inOffset )
{
    KUInt8 v;

    switch (mState) {
        case kReadArray:
            if (inOffset<mSize) {
                v = mMemoryMap[inOffset];
            } else {
                v = 0;
            }
            break;
        default:
            v = mStatusRegister;
            break;
    }

	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::ReadMemB( %.8X ) = %.2X", (unsigned int) inOffset, v );
	}

	return v;
}

// -------------------------------------------------------------------------- //
//  * WriteAttr( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteAttr( KUInt32 inOffset, KUInt32 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::WriteAttr( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteAttrB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteAttrB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::WriteAttrB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIO( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteIO( KUInt32 inOffset, KUInt32 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::WriteIO( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteIOB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteIOB( KUInt32 inOffset, KUInt8 inValue )
{
	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::WriteIOB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
}

// -------------------------------------------------------------------------- //
//  * WriteMem( KUInt32, KUInt32 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteMem( KUInt32 inOffset, KUInt32 inValue )
{
//    KUInt32 v = (mMemoryMap[inOffset]<<24) | (mMemoryMap[inOffset+1]<<16) | (mMemoryMap[inOffset+2]<<8) | (mMemoryMap[inOffset+3]<<0);
//    KUInt32 p = GetPCMCIAController()->GetReg2000();
//    const char *states[] = {
//        "Read", "Status", "Write", "Erase"
//    };
//    printf("TFlashWrite: pc: %08x  lr: %08x\n",
//           GetPCMCIAController()->GetEmulator()->GetProcessor()->GetRegister(15),
//           GetPCMCIAController()->GetEmulator()->GetProcessor()->GetRegister(14));
//    printf("TFlashWrite: a:%08x v:%08x, current:%08x, flags:%08x %s\n", inOffset, inValue, v, p, states[mState]);

	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::WriteMem( %.8X, %.8X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
    if (mState==kWriteArray) {
        if (GetPCMCIAController()->GetReg2000() & 0x0800) {
            mMemoryMap[inOffset+2] = (KUInt8)(inValue>>24);
            mMemoryMap[inOffset+3] = (KUInt8)(inValue>>16);
        } else {
            mMemoryMap[inOffset+0] = (KUInt8)(inValue>>24);
            mMemoryMap[inOffset+1] = (KUInt8)(inValue>>16);
        }
        mState = kReadStatusRegister;
    } else if (mState==kEraseSetup) {
        if (inValue==0xd0d00000) {
            KUInt32 start = inOffset & ~(0x00020000-1);
            KUInt32 end = inOffset + 0x00020000;
            for (KUInt32 addr = start; addr<end; addr+=4) {
                mMemoryMap[addr+0] = 0xFF;
                mMemoryMap[addr+1] = 0xFF;
                mMemoryMap[addr+2] = 0xFF;
                mMemoryMap[addr+3] = 0xFF;
            }
            mStatusRegister = 0x80;
        } else {
            // set error state (read and erase error)
            mStatusRegister = 0xb0;
        }
        mState = kReadStatusRegister;
    } else {
        if (inValue==0xffff0000) {
            mState = kReadArray;
        } else if (inValue==0x50500000) {
            mStatusRegister = 0x80;
        } else if (inValue==0x70700000) {
            mState = kReadStatusRegister;
        } else if (inValue==0x40400000) {
            mState = kWriteArray;
        } else if (inValue==0x20200000) {
            mState = kEraseSetup;
        } else {
            if (GetLog())
                GetLog()->FLogLine( "TLinearCard::WriteMem: UNKNOWN COMMAND\n");
        }
    }
}

// -------------------------------------------------------------------------- //
//  * WriteMemB( KUInt32, KUInt8 )
// -------------------------------------------------------------------------- //
void
TLinearCard::WriteMemB( KUInt32 inOffset, KUInt8 inValue )
{
//    KUInt8 v = mMemoryMap[inOffset];
//    KUInt32 p = GetPCMCIAController()->GetReg2000();
//    const char *states[] = {
//        "Read", "Status", "Write", "Erase"
//    };
//    printf("TFlashWriteB: pc: %08x  lr: %08x\n",
//           GetPCMCIAController()->GetEmulator()->GetProcessor()->GetRegister(15),
//           GetPCMCIAController()->GetEmulator()->GetProcessor()->GetRegister(14));
//    printf("TFlashWriteB: a:%08x v:%02x, current:%02x, flags:%08x %s\n", inOffset, inValue, v, p, states[mState]);

	if (GetLog())
	{
		GetLog()->FLogLine( "TLinearCard::WriteMemB( %.8X, %.2X )",
			(unsigned int) inOffset,
			(unsigned int) inValue );
	}
    if (mState==kWriteArray) {
        if (GetPCMCIAController()->GetReg2000() & 0x0800) {
            mMemoryMap[inOffset] = inValue;
        } else {
            mMemoryMap[inOffset] = inValue;
        }
        mState = kReadStatusRegister;
    } else if (mState==kEraseSetup) {
        if (inValue==0xd0) {
            // this code is untested and should never be called by NewtonOS
            KUInt32 start = inOffset & ~(0x00020000-1);
            KUInt32 end = inOffset + 0x00020000;
            for (KUInt32 addr = start; addr<end; addr+=4) {
                if (inOffset&1) {
                    mMemoryMap[addr+1] = 0xFF;
                    mMemoryMap[addr+3] = 0xFF;
                } else {
                    mMemoryMap[addr+0] = 0xFF;
                    mMemoryMap[addr+2] = 0xFF;
                }
            }
            mStatusRegister = 0x80;
        } else {
            // set error state (read and erase error)
            mStatusRegister = 0xb0;
        }
        mState = kReadStatusRegister;
    } else {
        if (inValue==0xff) {
            mState = kReadArray;
        } else if (inValue==0x50) {
            mStatusRegister = 0x80;
        } else if (inValue==0x70) {
            mState = kReadStatusRegister;
        } else if (inValue==0x40) {
            mState = kWriteArray;
        } else if (inValue==0x20) {
            mState = kEraseSetup;
        } else {
            if (GetLog())
                GetLog()->FLogLine( "TLinearCard::WriteMemB: UNKNOWN COMMAND\n");
        }
    }
}


/**
 * Static method to generate a PCMCIA Card Image file.
 * 
 * The PCMCIA Card Image starts with the data section, followed by the CIS,
 * followed by an optional graphic (not yet), and more data that may be 
 * relevant. The format is extendable.
 * 
 * The file ends in a lookup table. See ImageInfo.
 * 
 * \param inOutFilename the final image will be written to this file
 * \param inDataFilename read PCMCIA Data block from this file, size is usually a multiple of 1MB
 * \param inCISFilename read CIS data from this file and descramble it
 */
KSInt32 TLinearCard::ComposeImageFile(const char* inOutFilename, const char* inDataFilename, const char* inCISFilename)
{
    KSInt32 err = 0;
    FILE* fOut = nullptr;
    FILE* fData = nullptr;
    FILE* fCIS = nullptr;
    KUInt8* dataBuffer = nullptr;
    KUInt8* cisBuffer = nullptr;
    ImageInfo imageInfo = { 0 };
    imageInfo.pType = 0; // increment this when adding infomation to the file format
    strcpy(imageInfo.pIdent, kIdent);

    // --- create the image file
    fOut = fopen(inOutFilename, "wb");
    if (!fOut) { err = kErrCantCreateOutFile; goto cleanup; }

    // --- read the Data part
    fData = fopen(inDataFilename, "rb");
    if (!fData) { err = kErrCantOpenDataFile; goto cleanup; }
    fseek(fData, 0, SEEK_END);  // simple way to find file size
    imageInfo.pDataSize = ftell(fData);
    fseek(fData, 0, SEEK_SET);
    if (imageInfo.pDataSize == 0) { err = kErrCorruptDataFile; goto cleanup; }
    if (imageInfo.pDataSize > 128 * 1024 * 1024) { err = kErrDataFileTooBig; goto cleanup; }

    // copy the data over verbatim
    dataBuffer = (KUInt8*)malloc(imageInfo.pDataSize);
    if (!dataBuffer) { err = kErrOutOfMemory; goto cleanup; }
    if (fread(dataBuffer, imageInfo.pDataSize, 1, fData)!=1) { err = kErrCorruptDataFile; goto cleanup; }
    if (fwrite(dataBuffer, imageInfo.pDataSize, 1, fOut)!=1) { err = kErrIncompleteImageFile; goto cleanup; }

    // --- read the CIS data
    // TODO: we can derive the CIS data (well, 'some' CIS data from the code above, in case this CIS is missing or corrupt)
    imageInfo.pCISStart = imageInfo.pDataSize;
    fCIS = fopen(inCISFilename, "rb");
    if (!fCIS) { err = kErrCantOpenCISFile; goto cleanup; }
    fseek(fCIS, 0, SEEK_END);  // simple way to find file size
    KUInt32 rawCISSize = ftell(fCIS);
    fseek(fCIS, 0, SEEK_SET);
    if (rawCISSize == 0) { err = kErrCorruptCISFile; goto cleanup; }
    if (rawCISSize > 4096) rawCISSize = 4096;

    // read CIS data
    cisBuffer = (KUInt8*)malloc(rawCISSize);
    if (!cisBuffer) { err = kErrOutOfMemory; goto cleanup; }
    if (fread(cisBuffer, rawCISSize, 1, fCIS) != 1) { err = kErrCorruptCISFile; goto cleanup; }

    // find byte order and descramble (usually x1x0x3x2, but could be 0123...)
    if (cisBuffer[3] == 1 && cisBuffer[1] > 0) {
        for (int i=0; i<rawCISSize/2; ++i)
            cisBuffer[i] = cisBuffer[(i ^ 1) * 2 + 1];
        rawCISSize /= 2;
    } else {
        // TODO: we are cheating here a bit. Some diagnostic data would be nice, so we can add other descramblers.
        err = kErrCorruptCISFile; goto cleanup;
    }

    // find the end of the CIS data (we can output all card information here, if needed)
    // (even if the CIS is corrupt, this loop will not run forever)
    imageInfo.pCISSize = rawCISSize;
    for (int i=0;i<rawCISSize;) {
        // get tuple type
        KUInt8 tt = cisBuffer[i++];
        KUInt8 st = cisBuffer[i++];
        switch (tt) {
        case 0x01: // CISTPL_DEVICE
            imageInfo.pType = cisBuffer[i] >> 4; 
            // TODO: cisBuffer[i+1] contains the encoded size of the Flash. Do we want to verify that this is the same as Data size?
            break;
        case 0x15: // CISTPL_VERS_1
            // majr, minor version, manufacturer \0 product \0 product info 2 \0 product info 3 \0\ff 
            break;
        case 0xff: 
            imageInfo.pCISSize = i; 
            i = rawCISSize; // end the loop
            break;
        }
        i += st;
    }
    if (fwrite(cisBuffer, imageInfo.pCISSize, 1, fOut) != 1) { err = kErrIncompleteImageFile; goto cleanup; }

    // add our image info block to the end of the file
    if (fwrite(&imageInfo, sizeof(imageInfo), 1, fOut) != 1) { err = kErrIncompleteImageFile; goto cleanup; }

    // close all files and deallocate buffers
cleanup:
    if (fOut) fclose(fOut);
    if (fData) fclose(fData);
    if (fCIS) fclose(fData);
    if (dataBuffer) ::free(dataBuffer);
    if (cisBuffer) ::free(cisBuffer);
    return err;
}


// ======================================================================= //
// Flash Card                                                              //
//                                                                         //
// A memory card using flash RAM chips that can be rewritten and maintain  //
// the storage of information when power is removed. Commonly referred to  //
// as a storage card. Not to be confused with a CompactFlash (CF) card,    //
// which is similar in function but different in size and implementation.  //
//                                                                         //
//  - Newton Glossary                                                      //
// ======================================================================= //
