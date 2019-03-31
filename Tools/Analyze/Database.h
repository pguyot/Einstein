


#ifndef _DATABASE_H
#define _DATABASE_H

#include <stdint.h>
#include <map>


typedef enum {
	TYPE_NONE = 0,
	TYPE_ARM_CODE,
	TYPE_ARM_INLINE_DATA,
	TYPE_ARM_INLINE_TEXT
} EType;

class Entry
{
	EType pType = TYPE_NONE;
	char *pLabel = nullptr;
	bool pJumpTarget = false;
public:
	Entry();
	int write(FILE *out, uint32_t start, uint32_t end);
	void setLabel(const char *sym);
	const char *getLabel() { return pLabel; }
	void setType(EType t) { pType = t; }
	EType getType() { return pType; }
	void setAsJumpTarget() { pJumpTarget = true; }
	bool isJumpTarget() { return pJumpTarget; }
	void addComment(const char *pattern, ...) { return; } // TODO: implement this
};


class Database
{
	std::map<uint32_t, Entry> pDB;
public:
	Database();
	Entry *findEntryAt(uint32_t addr);
	Entry *operator()(uint32_t addr) { return findEntryAt(addr); }
	Entry &operator[](uint32_t addr);
	int write(FILE *out, uint32_t startSeg, uint32_t endSeg);
	const char *findLabelAt(uint32_t addr);
	const char *getLabelAt(uint32_t addr);
};


extern Database DB;

#endif /* _DATABASE_H */

