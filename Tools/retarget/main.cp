//
//  main.cpp
//  retarget
//
//  Created by Matthias Melcher on 11/26/14.
//
//

#include <iostream>

#include "TMonitorCore.h"
#include "TJITGenericRetarget.h"
#include "TSymbolList.h"


extern KUInt32 gMemPageLUT[];
extern KUInt32 gMemPage[][4096];
extern KUInt32 gMemPageN;


bool MemoryRead( KUInt32 inAddress, KUInt32& outWord )
{
	int i;
	for (i=0; i<gMemPageN; i++) {
		if (gMemPageLUT[i]<=inAddress && gMemPageLUT[i]+4096>inAddress) {
			KUInt32 offset = (inAddress&4095)>>2;
			outWord = gMemPage[i][offset];
			return false;
		}
	}
	return true;
}


int main(int argc, const char * argv[])
{
	int err = 1;
	char symbols_file[PATH_MAX];
	
	strcpy(symbols_file, "symbols.txt");
	if (argc==4) {
		if (strcmp(argv[1], "-s")==0) {
			strcpy(symbols_file, argv[2]);
			FILE *f = fopen(symbols_file, "rb");
			if (!f) {
				fprintf(stderr, "ERROR: can't open symbols_file \"%s\"\n", symbols_file);
				return -1;
			} else {
				fclose(f);
			}
			f = fopen(argv[3], "rb");
			if (!f) {
				fprintf(stderr, "ERROR: can't open script_file \"%s\"\n", argv[3]);
				return -1;
			} else {
				fclose(f);
			}
		}
	}
	
	if (err) {
		fprintf(stderr, ">>%s<<\n", argv[1]);
		fprintf(stderr, ">>%s<<\n", argv[2]);
		fprintf(stderr, "USAGE: retarget -s symbols_file script_file\n");
		return -1;
	}
	
	TSymbolList::List = new TSymbolList(argv[3]);
	TMonitorCore m(TSymbolList::List);
	
	bool result = m.ExecuteScript(argv[1]);
	
	if (result==false) {
		fprintf(stderr, "ERROR: script erro in \"%s\"\n", argv[3]);
		return -1;
	}
	
    return 0;
}
