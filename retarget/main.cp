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
	if (argc!=2) {
		fprintf(stderr, "USAGE: retarget \"scriptFile\"\n");
		return 0;
	}
	
	TSymbolList::List = new TSymbolList("/Users/matt/dev/Einstein/_Data_/symbols.txt");
	TMonitorCore m(TSymbolList::List);
//	TJITGenericRetarget t(0L, TSymbolList::List); // memory, symbols
	
	m.ExecuteScript(argv[1]);
	
//	t.OpenFiles("/Users/matt/dev/test");
//	t.TranslateFunction(0x00388520, 0x00388520+8, "ClassInfo__12TPSPAPDriverSFv");
//	t.CloseFiles();
    return 0;
}
