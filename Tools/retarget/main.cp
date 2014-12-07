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
	char script_file[PATH_MAX];
	
//	int i;
//	for (i=0; i<argc; i++) {
//		fprintf(stderr, "Arg %d = >>%s<<\n", i, argv[i]);
//	}
	
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
			strcpy(script_file, argv[3]);
			f = fopen(script_file, "rb");
			if (!f) {
				fprintf(stderr, "ERROR: can't open script_file \"%s\"\n", script_file);
				return -1;
			} else {
				fclose(f);
			}
			err = 0;
		}
	}
	
	if (err) {
		fprintf(stderr, "USAGE: retarget -s symbols_file script_file\n");
		return -1;
	}
	
	TSymbolList::List = new TSymbolList(symbols_file);
	TMonitorCore m(TSymbolList::List);
	
	char script_dir[PATH_MAX];
	char current_dir[PATH_MAX];
	
	strcpy(script_dir, script_file);
	char *slash = strrchr(script_dir, '/');
	if (slash) {
		slash[1] = 0;
	} else {
		strcpy(script_dir, ".");
	}
	getcwd(current_dir, PATH_MAX);
	chdir(script_dir);
	
	bool result = m.ExecuteScript(script_file);
	
	if (result==false) {
		fprintf(stderr, "ERROR: script error in \"%s\"\n", argv[3]);
		return -1;
	}

	chdir(current_dir);

    return 0;
}
