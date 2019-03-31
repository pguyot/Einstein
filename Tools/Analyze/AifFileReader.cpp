// ==============================
// File:			AifFileReader.cpp
// Project:			Einstein
//
// Copyright 2019 by Matthais Melcher (proteus@matthiasm.com).
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

#include "AifFileReader.h"

#include "Analyze.h"
#include "Database.h"
#include "Memory.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


struct AIFHdr {
	uint32_t a00, a04, a08, a0c, a10, roSize, rwSize, dbgSize;
	uint32_t zeroSize, a24, a28, a2c, a30, dataLoc, a38, a3c;
	uint32_t a40, a44, a48, a4c, a50, a54, a58, a5c;
	uint32_t a60, a64, a68, a6c, a70, a74, a78, a7c;
};

struct DBGHdr { // 0071FC4C
	uint16_t size, type;                        // 0x0024 0x0001
	uint8_t lang, flags, unused, dbgVersion;    // 0, 0, 0, 2
	uint32_t codeStart, dataStart;              // 0, 0c10800,
	uint32_t codeSize, dataSize;                // 0071a95c, 000052f0
	uint32_t fileInfo, debugSize, nSyms;        // 0, 001CADF0, 0000CE0F (*8)
};

struct Sym { // 71FCF0  05.00.00.04
	uint32_t sym_f_offset;
	uint32_t value;
};


/*
 An AIF image has the following layout:
 • Header
 • Read-only area
 • Read-write area
 • Debugging data (optional)
 • Self-relocation code (position-independent)
 • Relocation list. This is a list of byte offsets from the beginning of the AIF header,
 of words to be relocated, followed by a word containing -1. The relocation of non-word values is not supported.
 */

int readARMImageFile(const char *path)
{
	FILE *f = fopen(path, "rb");
	if (!f) {
		fprintf(stderr, "Error: can't open AIF file \"%s\": %s\n", path, ::strerror(errno));
		return 30;
	}
	// read ro and rw data
	struct AIFHdr hdr;
	fread(&hdr, sizeof(hdr), 1, f);
	unsigned int ROMSize = htonl(hdr.roSize) + htonl(hdr.rwSize);
	void *data = calloc(ROMSize, 1);
	fread(data, ROMSize, 1, f);
	MEM.set(0, ROMSize, data);
	MEM.setRExStart(ROMSize);
	free(data);

	// read symbol table
	struct DBGHdr dbg;
	fread(&dbg, sizeof(dbg), 1, f);
	int i, nSym = htonl(dbg.nSyms);
	struct Sym *sym = (struct Sym *)calloc(nSym*8, 1);
	fread(sym, nSym, 8, f);
	size_t symBase = ftell(f);

	// and apply symboltable
	for (i=0; i<nSym; i++) {
		//            uint8_t flags = htonl(sym[i].sym_f_offset)>>24; // 1=?? 3=code, 5=data, 7=pointer
		uint32_t offset = htonl(sym[i].sym_f_offset) & 0x00FFFFFF;
		uint32_t value = htonl(sym[i].value);
		uint32_t flags = (htonl(sym[i].sym_f_offset) & 0xFF000000) >> 24;
		fseek(f, symBase+offset, SEEK_SET);
		int n = fgetc(f);
		char name[1024];
		fread(name, n, 1, f);
		name[n] = 0;
		if (flags==1 && value<0x00800000 && name[0]=='k' && isupper(name[1])) {
			// ignore symbols in ROM with flags set to 01 and starting with k[A..Z]
			// they are constant values and not labels.
		} else {
			DB[value].setLabel(name);
		}
	}
	DB[0x00000000].setLabel("Reset");
	DB[0x00000004].setLabel("_UndefinedInstruction");
	DB[0x00000008].setLabel("_SoftwareInterrupt");
	DB[0x0000000C].setLabel("_AbortPrefetch");
	DB[0x00000010].setLabel("_AbortData");
	DB[0x00000014].setLabel("_not_used");
	DB[0x00000018].setLabel("_IRQ");
	DB[0x0000001C].setLabel("_FIQ");

	DB[0x02000000].setLabel("kFlashBank1");
	DB[0x02400000].setLabel("kFlashBank1End");
	DB[0x04000000].setLabel("kRAMStart");
	DB[0x0F000000].setLabel("kHardwareBase");
	DB[0x0F001000].setLabel("kHdWr_P0F001000");
	DB[0x0F001800].setLabel("kHdWr_04RAMSize");
	DB[0x0F001C00].setLabel("kHdWr_08RAMSize");
	DB[0x0F002000].setLabel("kHdWr_P0F002000");
	DB[0x0F043000].setLabel("kHdWr_P0F043000");
	DB[0x0F043800].setLabel("kHdWr_P0F043800");
	DB[0x0F048000].setLabel("kHdWr_P0F048000");
	DB[0x0F052C00].setLabel("kHdWr_P0F052C00");
	DB[0x0F053000].setLabel("kHdWr_P0F053000");
	DB[0x0F053400].setLabel("kHdWr_P0F053400");
	DB[0x0F053800].setLabel("kHdWr_P0F053800");
	DB[0x0F054400].setLabel("kHdWr_P0F054400");
	DB[0x0F054800].setLabel("kHdWr_P0F054800");
	DB[0x0F055000].setLabel("kHdWr_P0F055000");
	DB[0x0F080000].setLabel("kHdWr_DMAChan1Base");
	DB[0x0F08FC00].setLabel("kHdWr_DMAChan1End");
	DB[0x0F08FC00].setLabel("kHdWr_DMAAssgmnt");
	DB[0x0F090000].setLabel("kHdWr_DMAChan2Base");
	DB[0x0F098000].setLabel("kHdWr_DMAChan2End");
	DB[0x0F098000].setLabel("kHdWr_DMAEnableStat");
	DB[0x0F098400].setLabel("kHdWr_DMADisable");
	DB[0x0F098800].setLabel("kHdWr_DMAWordStat");
	DB[0x0F110000].setLabel("kHdWr_P0F110000");
	DB[0x0F110400].setLabel("kHdWr_HighSpeedClck");
	DB[0x0F111400].setLabel("kHdWr_P0F111400");
	DB[0x0F180400].setLabel("kHdWr_P0F180400");
	DB[0x0F181000].setLabel("kHdWr_CalendarReg");
	DB[0x0F181400].setLabel("kHdWr_AlarmReg");
	DB[0x0F181800].setLabel("kHdWr_Ticks");
	DB[0x0F182000].setLabel("kHdWr_MatchReg0");
	DB[0x0F182400].setLabel("kHdWr_MatchReg1");
	DB[0x0F182800].setLabel("kHdWr_MatchReg2");
	DB[0x0F182C00].setLabel("kHdWr_MatchReg3");
	DB[0x0F183000].setLabel("kHdWr_IntPresent");
	DB[0x0F183400].setLabel("kHdWr_IntCtrlReg");
	DB[0x0F183800].setLabel("kHdWr_IntClear");
	DB[0x0F183C00].setLabel("kHdWr_FIQMaskReg");
	DB[0x0F184000].setLabel("kHdWr_IntEDReg1");
	DB[0x0F184400].setLabel("kHdWr_IntEDReg2");
	DB[0x0F184800].setLabel("kHdWr_IntEDReg3");
	DB[0x0F184C00].setLabel("kHdWr_P0F184C00");
	DB[0x0F185000].setLabel("kHdWr_P0F185000");
	DB[0x0F18C000].setLabel("kHdWr_GPIO_RReg");
	DB[0x0F18C400].setLabel("kHdWr_GPIO_EReg");
	DB[0x0F18C800].setLabel("kHdWr_GPIO_CReg");
	DB[0x0F18CC00].setLabel("kHdWr_P0F18CC00");
	DB[0x0F18D000].setLabel("kHdWr_P0F18D000");
	DB[0x0F18D400].setLabel("kHdWr_P0F18D400");
	DB[0x0F18D800].setLabel("kHdWr_P0F18D800");
	DB[0x0F18DC00].setLabel("kHdWr_P0F18DC00");
	DB[0x0F18E800].setLabel("kHdWr_IOPower1");
	DB[0x0F18EC00].setLabel("kHdWr_IOPower2");
	DB[0x0F1C0000].setLabel("kExternalSerialBase");
	DB[0x0F1D0000].setLabel("kInfraredSerialBase");
	DB[0x0F1E0000].setLabel("kBuiltInSerialBase");
	DB[0x0F1F0000].setLabel("kModemSerialBase");
	DB[0x0F200000].setLabel("kSerialEnd");
	DB[0x0F240000].setLabel("kHdWr_ExtDataAbt1");
	DB[0x0F240400].setLabel("kHdWr_ExtDataAbt2");
	DB[0x0F240800].setLabel("kHdWr_ExtDataAbt3");
	DB[0x0F241000].setLabel("kHdWr_BankCtrlReg");
	DB[0x0F241800].setLabel("kHdWr_P0F241800");
	DB[0x0F242400].setLabel("kHdWr_P0F242400");
	DB[0x0F243000].setLabel("kROMSerialChip");
	DB[0x0F247000].setLabel("kHdWr_P0F247000");
	DB[0x0F280000].setLabel("kHdWr_P0F280000");
	DB[0x0F280400].setLabel("kHdWr_P0F280400");
	DB[0x0F280800].setLabel("kHdWr_P0F280800");
	DB[0x0F283000].setLabel("kHdWr_P0F283000");
	DB[0x0F284000].setLabel("kHdWr_P0F284000");
	DB[0x10000000].setLabel("kFlashBank2");
	DB[0x10400000].setLabel("kFlashBank2End");
	DB[0x30000000].setLabel("kPCMCIA0Base");
	DB[0x40000000].setLabel("kPCMCIA1Base");
	DB[0x50000000].setLabel("kPCMCIA2Base");
	DB[0x60000000].setLabel("kPCMCIA3Base");

	fclose(f);

	return 0;
}


int readREXFile(const char *path)
{
	// open the file
	FILE *f = fopen(path, "rb");
	if (!f) {
		fprintf(stderr, "Can't open REx file \"%s\": %s\n", path, strerror(errno));
		return -1;
	}

	fseeko(f, 0, SEEK_END);
	unsigned int RExSize = (unsigned int)fseeko(f, 0, SEEK_SET);
	void *data = calloc(RExSize, 1);
	fread(data, RExSize, 1, f);
	MEM.set(MEM.RExStart(), RExSize, data);
	MEM.setRExSize(RExSize);
	free(data);


	fclose(f);

	return 0;
}


// ============================================================================== //
// I've ended up calling my cat Brexit. It wakes me up meowing like crazy every
// morning because it wants to go out, but as soon as I open the door, it just
// sits there undecided and then looks angry when I put it outside.
//   -- French Europe Minister Nathalie Loiseau
// ============================================================================== //

