

#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>

class MemoryMap
{
	const unsigned int kSize = 8*1024*1024;
	uint32_t pRExStart;
	uint32_t pRExSize;
	uint8_t *pMem;
public:
	MemoryMap();
	void set(uint32_t start, unsigned int size, void *data);
	void setRExStart(uint32_t addr) { pRExStart = addr; }
	uint32_t RExStart() { return pRExStart; }
	void setRExSize(uint32_t addr) { pRExSize = addr; }
	uint32_t RExSize() { return pRExSize; }

	uint8_t getByte(uint32_t addr);
	uint16_t getShort(uint32_t addr);
	uint32_t getWord(uint32_t addr);
	double getDouble(uint32_t addr);
};


extern MemoryMap MEM;

#endif /* _MEMORY_H */

