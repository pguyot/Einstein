

#include "Database.h"

#include "Memory.h"
#include "Disarm.h"


Database DB;


Entry::Entry()
{
}


int Entry::write(FILE *out, uint32_t start, uint32_t end)
{
	char line[1024];
	bool written = false;
	for (auto &c: pCommentList) {
		::fprintf(out, "// /* 0x%08X-0x%08X */ // %s\n", start, end, c);
		written = true;
	}
	if (getLabel()) {
		::fprintf(out, "// /* 0x%08X-0x%08X */\n", start, end);
		::fprintf(out, "// /* 0x%08X-0x%08X */ %s:\n", start, end, getLabel());
		written = true;
	} else if (isJumpTarget()) {
		::fprintf(out, "// /* 0x%08X-0x%08X */ L%08X:\n", start, end, start);
		written = true;
	}
	switch (pType) {
		case TYPE_ARM_CODE:
			disarm(line, start, MEM.getWord(start));
			::fprintf(out, "// /* 0x%08X-0x%08X */     %s\n", start, end, line);
			written = true;
			break;
		case TYPE_ARM_INLINE_DATA:
			disarm_word(line, start, MEM.getWord(start));
			::fprintf(out, "// /* 0x%08X-0x%08X */     %s\n", start, end, line);
			written = true;
			break;
		case TYPE_ARM_INLINE_TEXT:
			// FIXME: output the text
			::fprintf(out, "// /* 0x%08X-0x%08X */     ARM_Text\n", start, end);
			written = true;
			break;
		default:
			break;
	}
	if (!written)
		::fprintf(out, "// /* 0x%08X-0x%08X */\n", start, end);
	return 0;
}


void Entry::setLabel(const char *sym)
{
	if (pLabel) ::free(pLabel);
	if (sym && *sym) {
		pLabel = strdup(sym);
	} else {
		pLabel = nullptr;
	}
}

void Entry::addComment(const char *pattern, ...)
{
	static char buf[4096];
	va_list va;
	va_start(va, pattern);
	vsnprintf(buf, 4096, pattern, va);
	va_end(va);
	pCommentList.push_back(strdup(buf));
}







Database::Database()
{
}


Entry *Database::findEntryAt(uint32_t addr)
{
	auto it = pDB.find(addr);
	if (it!=pDB.end()) {
		return &it->second;
	} else {
		return nullptr;
	}
}


Entry &Database::operator[](uint32_t addr)
{
	return pDB[addr];
}


int Database::write(FILE *out, uint32_t startSeg, uint32_t endSeg)
{
	uint32_t lastWritten = startSeg;
	for (auto it=pDB.lower_bound(startSeg); it!=pDB.end() && it->first<endSeg; ++it) {
		uint32_t startEntry = it->first, endEntry = endSeg;
		auto itNext = it; itNext++;
		if (itNext!=pDB.end() && itNext->first<endSeg) endEntry = itNext->first;
		if (lastWritten<startEntry)
			::fprintf(out, "// /* 0x%08X-0x%08X */\n", lastWritten, startEntry);

		it->second.write(out, startEntry, endEntry);

		// printf("%08X %s\n", it->first, it->second.getSymbol());
		lastWritten = endEntry;
	}
	// fill the segment to the last byte
	if (lastWritten<endSeg) {
		::fprintf(out, "// /* 0x%08X-0x%08X */\n", startSeg, endSeg);
	}

	return -1;
}


const char *Database::findLabelAt(uint32_t addr)
{
	const char *label = nullptr;
	Entry *e = findEntryAt(addr);
	if (e) label = e->getLabel();
	return label;
}

const char *Database::getLabelAt(uint32_t addr)
{
	static char genericLabel[12];
	const char *label = findLabelAt(addr);
	if (!label) {
		sprintf(genericLabel, "L%08X", addr);
		label = genericLabel;
	}
	return label;
}

