/******************************************************************************
 * Engine.h
 *
 *  Header file for Engine.c, part of the SoupDrink DIL sample application
 *
 *	Written by:	Rob Langhorne, David Fedor, and J. Christopher Bell
 * 
 *	Copyright:	© 1995-1996 by Apple Computer, Inc.  All rights reserved.
 */

typedef enum UnicodeStuff {
	kUnicode = 0,				// not implemented
	kMacRomanEncoding,			// Macintosh Roman
	kPCRomanEncoding = 3		// PC Roman
} UnicodeStuff; // UnicodeEncodingStuffWhichShouldBeDefinedElsewhere;
enum {kDrinkMode = 0, kSpitMode};
#define kMAXSTR 255


long	SoupDrink () ;
CommErr	UploadNewName ();
void	printTree (FILE * fp, slotDefinition *thisEntry, short depth);
char*	ErrorStrings(CommErr theErr, char* theString);

#ifdef forWin16
void FAR _loadds SoupDrinkCallback (CommErr errorValue, void *pData, Size Count, long refCon, long lFlags);
#else
static void   SoupDrinkCallback (CommErr errorValue, void *pData, Size Count, long refCon, long lFlags);
#endif 


#ifndef CHECKDILERROR
#define CHECKDILERROR(x); {CommErr retVal = (x); if (retVal) { CDPipeDisconnect (ourPipe) ; return (retVal); }}
#endif
