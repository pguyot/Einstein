//
//  MemoryMap.cpp
//  Albert
//
//  Created by Matthias Melcher on 9/6/15.
//
//

#include "Memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>



MemoryMap MEM;


MemoryMap::MemoryMap() :
pRExStart(0),
pRExSize(0),
pMem(0)
{
	pMem = (uint8_t*)calloc(kSize, 1);
}


void MemoryMap::set(uint32_t start, unsigned int size, void *data)
{
	// FIXME: we assume that the blocks falls within the first 8MB or the address space
	memmove(pMem+start, data, size);
}


uint8_t MemoryMap::getByte(uint32_t addr)
{
	static const uint8_t lut[] = { 0xde, 0xad, 0xbe, 0xef };
	if (addr<kSize) {
		return pMem[addr];
	} else {
		return lut[addr&3];
	}
}


uint16_t MemoryMap::getShort(uint32_t addr)
{
	static const uint16_t lut[] = { 0xdead, 0xadbe, 0xbeef, 0xefde };
	if (addr<kSize-1) {
		return (((uint16_t)pMem[addr])<<8)|pMem[addr+1];
	} else {
		return lut[addr&3];
	}
}


uint32_t MemoryMap::getWord(uint32_t addr)
{
	if (addr<kSize-3) {
		return (((uint32_t)pMem[addr])<<24)
		| (((uint32_t)pMem[addr+1])<<16)
		| (((uint32_t)pMem[addr+2])<<8)
		| ((uint32_t)pMem[addr+3]);
	} else {
		return 0xdeadbeef;
	}
}


double MemoryMap::getDouble(uint32_t addr)
{
	union { double d; uint8_t b[8]; } v;
	if (addr<kSize-7) {
		if (htonl(1)==1) {
			memmove(v.b, pMem+addr, 8);
		} else {
			uint8_t *s = pMem+addr+7, *d = v.b;
			for (int i=0;i<8;i++) { *d++ = *s--; }
		}
		return v.d;
	} else {
		return 3.14159265; // some random value
	}
}


