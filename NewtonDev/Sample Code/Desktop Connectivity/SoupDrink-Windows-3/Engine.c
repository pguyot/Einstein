/* 
 * Engine.c													  
 *
 *	Written by:	Rob Langhorne, J. Christopher Bell, and David Fedor
 * 
 *	Copyright:	© 1995-1996 by Apple Computer, Inc.  All rights reserved.
 *
 * This file is used both by the MacOS and Windows versions of SoupDrink; it contains
 * nearly all the calls to the CDIL and FDIL code other than the initialization routines.
 *
 * SoupDrink() is the routine for soup drinking: uploading a soup to the desktop.
 * UploadNewName() sends a frame holding a "name card" to the Newton.
 * PrintEntry() and PrintTree() pretty-print the contents of a frame to a file.
 * 
 * Purpose:
 *   To demonstrate the basic parts of FDILs and CDILs. The basic engines currently in
 *   this code are the "soup drink" engine which uploads a Newton soup and the the
 *   "new name" engine which downloads a business card to the Newton names database.
 *
 *   The soup drinking is split into two sections to illustrate both synchronous and
 *   asynchronous reading. The first entry read is done asynchronously from this
 *   function, with the callback function SoupDrinkCallback (below) doing the rest of
 *   the communication synchronously.
 * 
 *   The protocol for the soup drink is:
 *      Newton                                                             Desktop
 *      ======                                                             =======
 *                                                                    <---- "DRNK"
 *     "DRNK" -->
 *     "ENTR"-->
 *     a frame (or a string if in spit mode)--->
 *                                                                    <---- "OK"
 *     [then repeat with ENTR command, or ...]
 *     "END "--> 
 *   
 *  NOTE: The Newton "SPIT" command is supported for compatibility with older versions of this
 *  sample. If the SPIT command was sent instead of DRNK, a text version of the entry
 *  (NOT a frame) was sent. This was relevant only to a bug in some versions of Newton 1.x.    
 */
 
#ifndef _windows
#define forMac		// Codewarrior doesn't have a handy way to do this.
#endif

 
#include <string.h>
#include <stdio.h>


#ifdef forMac	// ****** stuff for MAC-OS applications ******
#include <Types.h>
#include <memory.h>
#include "DILCPIPE.h" 
#include "HLFDIL.h"
#include "SoupDrink.h"
#define Timeout 600		// this is currently in ticks but should be in milliseconds


#else   		// ****** stuff for Windows applications *********
#include <windows.h>
#include "DILCPIPE.H" 
#include "HLFDIL.h"
#include "SOUPDRNK.H"
#define Timeout 20000	// time out in 20 seconds
typedef unsigned char   Str255[256];

// Below two lines are only needed in 1.0 Windows DILs...
#define CurrentTimeInSeconds() ((long)(GetTickCount() / 1000))
#define  kTimeoutInSecs 15

#endif



#include "Engine.h"
#define Encoding 0
#define Swapping 0

extern CDILPipe		*ourPipe;
extern void			*gThisObject;		// This is the only DIL frame active at one time in this app
extern int			gInputMode;			// drink mode (receive frames) or spit mode (receive strings)
extern char			gReceivedString[]; 		// received string; used in 'spit mode'
extern long			gReceivedStringLen;		// received string length (see above)
extern char 		gTempBuffer [256];		// a global string buffer for input dialogs, etc


// ieee reals have bytes in the opposite order on Windows versus Newton devices.
// This function will swap the bytes to convert between one way and the other.
// It can be used both when sending and when receiving doubles, since it is 
// symmetric, and it won't do anything except on Windows, to make code cleaner.

double FlipDoubleIfNecessary(double theDouble)
{
#ifndef _windows
	return theDouble;	// no flipping is necessary except on Windows
#else
	unsigned long tmpWord1;
	unsigned long tmpWord2;
	double tmpDouble;

	tmpWord1 = ((unsigned long *)&theDouble)[0];
	tmpWord2 = ((unsigned long *)&theDouble)[1];
	tmpWord1 =  ((tmpWord1 & 0x000000ff) << 24) |
				((tmpWord1 & 0x0000ff00) <<  8) |
				((tmpWord1 & 0x00ff0000) >>  8) |
				((tmpWord1 & 0xff000000) >> 24);
	tmpWord2 =  ((tmpWord2 & 0x000000ff) << 24) |
				((tmpWord2 & 0x0000ff00) <<  8) |
				((tmpWord2 & 0x00ff0000) >>  8) |
				((tmpWord2 & 0xff000000) >> 24);
	((unsigned long *)&tmpDouble)[0] = tmpWord2;
	((unsigned long *)&tmpDouble)[1] = tmpWord1;
	
	return tmpDouble;
#endif
}


CommErr SoupDrink() 
{
	char 		soupName[255];
	char 		response[5];
	long		length;
	objErr		fErr = 0;
	long		longLen;
	Boolean		eom;
	long		endTime;

	inputDialog((char *)soupName, "Names", "Enter Soup Name:");
	if ('\0' == *soupName)		/*	Probably a cancel	*/
		return 0;
	
	if (strlen(soupName) > 253)
		soupName[253] = 0;
	strcat((char *)soupName, "\4");
	
	fErr = (short) InitializePipe();
	if (fErr)
		return fErr;		// don't need to dispose pipe if error

	// make the connection!
	CHECKDILERROR((short) CDPipeListen(ourPipe, kDefaultTimeout, 0, 0));
#ifndef _windows
	CHECKDILERROR((short) CDPipeAccept(ourPipe));
#else
	// This code doesn't need to be done on the MacOS side, but is currently required
	// for Windows.  You need to loop a little bit for the connection state to be
	// set to kCDIL_ConnectPending.  It might not do that if there's nobody on the
	// other side of the cable, though!

	// Loop until the Newton device connects
	endTime = CurrentTimeInSeconds() + kTimeoutInSecs;	// time at which we should stop looping
	while (CurrentTimeInSeconds() < endTime) {
		if (CDGetPipeState(ourPipe) == kCDIL_ConnectPending) {
			CHECKDILERROR((short) CDPipeAccept(ourPipe));
			break;
		} else
			CDIdle(ourPipe);
	}

	if (CurrentTimeInSeconds() >= endTime) {	// did we time out?
		CDPipeDisconnect(ourPipe) ;
		return kOurTimeoutError;
	}
#endif		

	/*	Initiate conversation; tell the Newton that we want to do a 'DRNK' */
	length = 4;
	CHECKDILERROR((short) CDPipeWrite(ourPipe, (void *)"DRNK", &length, true,Swapping,Encoding,Timeout,0,0));
	
	/* send the name of the soup we want to search */
	length = strlen(soupName);
	CHECKDILERROR((short) CDPipeWrite(ourPipe, (void *)soupName, &length, true,Swapping,Encoding,Timeout,0,0));
	
	/* get the Newton response. Possibilities are:
	 *   "DRNK"  -- no problem; about to begin
	 *   "SPIT"  -- DRNK not supported.   ADSP and outputframe is a problem on some systems -
	 *              that is, 1.x Newton devices without the December 1995 system update (xxx333).
	 *              The Newton will spit strings representing the names. In this case,
	 *              you might want to 'flatten' the frames yourself using the CDILs but not FDILS.
	 *              See notes at top of file for more info.
	 *   "NONE"  -- that soup is not available.
	 */
	length = 4;
	CHECKDILERROR((short) CDPipeRead(ourPipe, (void *) response, &length, &eom,Swapping,Encoding,Timeout,0,0));
	response[4] = 0;
	if (strcmp(response, "DRNK") == 0)
		gInputMode = kDrinkMode;
	else if (strcmp(response, "SPIT") == 0)
		gInputMode = kSpitMode;
	else if (strcmp(response, "NONE") == 0) {
		PostAlertMessage("Could not open that soup on Newton.", "", "", "");
		CDPipeDisconnect(ourPipe);
		return(0);
		}
		
	length = 4;
	
	/* receive a response; "END " for no more entries, "ENTR" for another soup entry */
	CHECKDILERROR((short) CDPipeRead(ourPipe, (void *) response, &length, &eom,Swapping,Encoding,Timeout,0,0));
	
	response[4] = 0;						/* make it a c string */
	if (strcmp(response, "END ") == 0) {	/* if there are no more entries */
		CDPipeDisconnect(ourPipe);
		PostAlertMessage("There are no entries in this soup.", "", "", "");
		return(0);
		}
	if (strcmp(response, "ENTR") != 0) {
		PostAlertMessage("Received unknown command:", response, "", "");
		CDPipeDisconnect(ourPipe);
		return(0);
		}

	//	read frame as unbound data
	gThisObject = FDCreateObject(kDILFrame, nil);

	gReceivedStringLen = kMAXSTR;
	longLen = 4;
	if (gInputMode == kDrinkMode) {
		fErr = (short) FDget(gThisObject, kDILFrame, ourPipe, Timeout, (CDILPipeCompletionProcPtr)SoupDrinkCallback, 0);
	} else {
		// we're in "spit" mode; the newton says it can't send frames.
		fErr = (short) CDPipeRead(ourPipe, (void *) &gReceivedStringLen, &longLen, &eom,Swapping,Encoding,Timeout,0,0);
		if (fErr == 0)
			fErr = (short) CDPipeRead(ourPipe, (void *) &gReceivedString[0], &gReceivedStringLen, &eom,
										Swapping,Encoding,Timeout,(CDILPipeCompletionProcPtr)SoupDrinkCallback, 0);
	}
		
	if (fErr) {
		CDPipeDisconnect(ourPipe);
		PostAlertMessage("Read returned:",ErrorStrings(fErr, gTempBuffer) ,"", "");
		return 0;
		}

	return fErr;	
}


/********************************************************************************
 * SoupDrinkCallback
 *
 * SoupDrinkCallback is the drinking soup 'read' callback function
 */
/* for 16 bit Windows... void FAR _loadds  */
#ifdef forWin16
void FAR _loadds SoupDrinkCallback (CommErr errorValue, void *pData, Size Count, long refCon, long lFlags)
#else
static void      SoupDrinkCallback (CommErr errorValue, void *pData, Size Count, long refCon, long lFlags)
#endif 
{
	FILE 			*fp;		/* file reference */
	slotDefinition *list = nil;
	long			fErr = 0, len = 3;
	long			longLen;
	long			length;
	char 			response[255];
	Boolean			eom;
	short			x;

	if (errorValue)
	{
		PostAlertMessage("Error reading first frame:", ErrorStrings(errorValue, gTempBuffer), "", "");
		FDDisposeObject(gThisObject);
		CDPipeDisconnect(ourPipe);
		return;
	}
	
	/*	open file for entry (append)		*/
	fp = fopen("SoupData.out", "w+");
	if (!fp)
	{
		PostAlertMessage("Could not open disk file for output.", "", "", "");
		FDDisposeObject(gThisObject);
		CDPipeDisconnect(ourPipe);
		return;
	}
	
	do {	/*	traverse frames		*/
		if (gInputMode == kSpitMode) /* we are just receiving strings, not frames */
			{
			gReceivedString[gReceivedStringLen] = 0;	/* turn it into c string */
			fprintf(fp, gReceivedString);				/* send the string to the file */
			fprintf(fp, "\n");							/* send a carriage return char */
			}
		else {		
			/*	We should have an unbound frame	list */
			list = FDGetUnboundList(gThisObject);	
			if (!list)							/* if not, we got an empty soup frame?; fail! */
				break;
				
			/*	traverse list		*/
			for (x= 0; x < list->peerCnt; x++)
				printTree(fp, &list[x], 1);
	
			fputc('\n', fp);
			fputc('\n', fp);
			fputc('\n', fp);
	
			FDFreeUnboundList(gThisObject, list);	
			list = nil;
		}
			
		if (fErr)
			PostAlertMessage("Unbound List stuff returned an error:", ErrorStrings(fErr, gTempBuffer), "", "");
		else {
			/*	read next frame	*/
			len = 3;
			fErr = CDPipeWrite(ourPipe, (void *)"OK\4", &len, true,Swapping,Encoding,Timeout,0,0) ; /* acknowledge receipt! */
			if (!fErr) {
				length = 4;
				
				/* receive a response; "END " for no more entries, "ENTR" for another soup entry */
				fErr = (short) CDPipeRead(ourPipe, (void *) response, &length, &eom,Swapping,Encoding,Timeout,0,0);
				if (fErr) {
					PostAlertMessage("Read returned an error:",ErrorStrings(fErr, gTempBuffer) ,"", "");
					break;
				}
					
				response[4] = 0;						/* make it a c string */
				if (strcmp(response, "END ") == 0)		/* if there are no more entries */
					break;
				if (strcmp(response, "ENTR") != 0) {
					PostAlertMessage("Received unknown command:", response, "", "");
					break;
				}

				// there's another entry coming down; read it in and loop
				gReceivedStringLen = kMAXSTR;
				longLen = 4; 
		
				if (gInputMode == kDrinkMode)
					fErr = FDget(gThisObject, kDILFrame, ourPipe, 500*60, 0, 0);
				else {
					fErr = CDPipeRead(ourPipe, (void *) &gReceivedStringLen, &longLen, &eom,Swapping,Encoding,Timeout,0,0);
					fErr = CDPipeRead(ourPipe, (void *) &gReceivedString[0], &gReceivedStringLen, &eom,Swapping,Encoding,Timeout,0,0);
				}
			}
		}		
	} while (!fErr && gThisObject);
	

	/* Clean up */
	if (fErr)
		PostAlertMessage("At end of soupdrink, got an error:", ErrorStrings(fErr, gTempBuffer), "", "");
	
	fflush(fp);		
	fclose(fp);
	if (list)
		FDFreeUnboundList(gThisObject, list);
	FDDisposeObject(gThisObject);
	CDPipeDisconnect(ourPipe);
}




/*
 * UploadNewName
 *
 * This function connects to a Newton and sends it a "name" for the names application.
 */
CommErr UploadNewName ()
{
	CommErr		fErr = 0;
	void		*entry, *name;
	void		*phones;
	long		cardType = 4, len;
	char		pClass[] = "person";
	char		fName[] = "Howard";
	char		lName[] = "Duck";
	char		phoneNo[] = "555-1212";
	char 		addr[]	= "123 Sesame Street";
	char		town[] = "Anytown";
	char		state[] = "CA";
	char		zip[] = "95014";
	char		sName[] = "Duck";
	char 		response[5];
	long		length;
	Boolean		eom;
	long		endTime;
	double		mydouble;
	
	
	fErr = (short) InitializePipe();
	if (fErr)
		return fErr;	// don't need to dispose pipe if error

	// make the connection!
	CHECKDILERROR((short) CDPipeListen(ourPipe, kDefaultTimeout, 0, 0 ) );
#ifndef _windows
	CHECKDILERROR((short) CDPipeAccept(ourPipe));
#else
	// This code doesn't need to be done on the MacOS side, but is currently required
	// for Windows.  You need to loop a little bit for the connection state to be
	// set to kCDIL_ConnectPending.  It might not do that if there's nobody on the
	// other side of the cable, though!

	// Loop until the Newton device connects
	endTime = CurrentTimeInSeconds() + kTimeoutInSecs;	// time at which we should stop looping
	while (CurrentTimeInSeconds() < endTime) {
		if (CDGetPipeState(ourPipe) == kCDIL_ConnectPending) {
			CHECKDILERROR((short) CDPipeAccept(ourPipe));
			break;
		} else
			CDIdle(ourPipe);
	}

	if (CurrentTimeInSeconds() >= endTime) {	// did we time out?
		CDPipeDisconnect(ourPipe) ;
		return kOurTimeoutError;
	}
#endif

	/*	Create an object		*/

	// in the future, errors should delete name, phones, entry objects.
	
	name = FDCreateObject(kDILFrame, NULL);	
	CHECKDILERROR((short) FDbindSlot(name, "Class", (void *)&pClass, kDILSymbol, strlen(pClass), -1, NULL));
	CHECKDILERROR((short) FDbindSlot(name, "first", (void *)&fName, kDILString, strlen(fName), -1, NULL));
	CHECKDILERROR((short) FDbindSlot(name, "last", (void *)&lName, kDILString, strlen(lName), -1, NULL));

	phones = FDCreateObject(kDILArray, NULL);
	CHECKDILERROR((short) FDbindSlot(phones, "HomePhone", (void *)&phoneNo, kDILString, strlen(phoneNo), -1, NULL));

	entry = FDCreateObject(kDILFrame, NULL);
	CHECKDILERROR((short) FDbindSlot(entry, "cardType", (void *)&cardType, kDILInteger, sizeof(int), -1, NULL)) ;
	CHECKDILERROR((short) FDbindSlot(entry, "Name", (void *)name, kDILFrame, 0, -1, NULL));
	CHECKDILERROR((short) FDbindSlot(entry, "Address", (void *)&addr, kDILString, strlen(addr), -1, NULL)) ;
	CHECKDILERROR((short) FDbindSlot(entry, "City", (void *)&town, kDILString, strlen(town), -1, NULL)) ;
	CHECKDILERROR((short) FDbindSlot(entry, "Region", (void *)&state, kDILString, strlen(state), -1, NULL)) ;
	CHECKDILERROR((short) FDbindSlot(entry, "Postal_Code", (void *)&zip, kDILString, strlen(zip), -1, NULL)) ;
	CHECKDILERROR((short) FDbindSlot(entry, "phones", (void *)phones, kDILArray, 0, -1, NULL)) ;						
	CHECKDILERROR((short) FDbindSlot(entry, "sorton", (void *)&sName, kDILString, strlen(sName), 0, "Name")) ;	

	// and just to show how it works, we add a slot which holds a real number.
	mydouble = FlipDoubleIfNecessary(3.14159);		// ieee reals have bytes in the opposite order...
	CHECKDILERROR((short) FDbindSlot(entry, "pi", (void *)&mydouble, kDILBinaryObject, 8, 8, "Real"));

	/*	Initiate conversation	*/
	len = 5;
	CHECKDILERROR((short) CDPipeWrite (ourPipe, (void *)"NAME\4", &len, true, Swapping,Encoding,Timeout, 0, 0)) ;
	
	/*	Send a frame		*/
	CHECKDILERROR((short) FDput(entry, kDILFrame, ourPipe)) ;
	
	FDDisposeObject(phones);
	FDDisposeObject(name);
	FDDisposeObject(entry);
	
	// wait for the ack back from the Newton (or for 30 seconds) before we disconnect
	length = 2;
	CHECKDILERROR((short) CDPipeRead(ourPipe, (void *) response, &length, &eom,Swapping,Encoding,Timeout,0,0));

	CDPipeDisconnect(ourPipe) ;
	
	return fErr;	

}


void	printEntry (FILE * fp, slotDefinition *thisEntry)
{	
	if (thisEntry->slotName)
		fprintf( fp, "%s : ", thisEntry->slotName);
	
	switch (thisEntry->varType)
	{
		case kDILCharacter:			/*	ASCII Character					*/
			fprintf( fp, "'%c'\n", *(char *)thisEntry->var);
			break;
			
		case kDILUnicodeCharacter:	/*	Unicode Character				*/
			{
				char c = *(((char *)thisEntry->var)+1);
				fprintf( fp,  "$%c\n", (char *)c);
			}
			break;
			
		case kDILString:			/*	String (null-terminated)		*/
			if (thisEntry->oClass)
				// there's a special class defined for this string; show it.
				fprintf( fp, "\"%s\" (class '%s)\n", (char *)thisEntry->var, (char *)thisEntry->oClass);
			else
				// generic string; no special class.
				fprintf( fp,  "\"%s\"\n", (char *)thisEntry->var);
			break;
			
		case kDILBoolean:			/*	Boolean							*/
			if (*(Boolean *)thisEntry->var)
				fprintf( fp,  "TRUE\n");
			else
				fprintf( fp,  "FALSE\n");
			break;
			
		case kDILImmediate:			/*	Indeterminate Immediate type	*/
		case kDILInteger:			/*	Integer (4 byte)				*/
			fprintf( fp, "#%ld\n", *(long *)thisEntry->var);
			break;
			
		case kDILPlainArray:		/*	Anonymous Array					*/
			fprintf (fp, "PLAINARRAY\n");
			break;
			
		case kDILArray:				/*	Named Array						*/
			if (thisEntry->oClass)
				fprintf (fp, "NAMEDARRAY (class '%s)\n", thisEntry->oClass);
			else
				fprintf (fp, "NAMEDARRAY (No class???)\n");
			break;
		
		case kDILFrame:				/*	Frame							*/
			fprintf (fp, "FRAME \n");
			break;
		
		case kDILSmallRect:			/*	Small rect						*/
			fprintf (fp, "SMALLRECT\n");
			break;
		
		case kDILPrecedent:			/*	Repeated Item					*/
			fprintf (fp, "PRECEDENT %s\n", (char *)thisEntry->var);
			break;
		
		case kDILSymbol:			/*	Object Symbol					*/
			fprintf( fp,  "'%s\n", (char *)thisEntry->var);
			break;
		
		case kDILBinaryObject:		/*	Small Binary Object ( < 32K )	*/
			if (thisEntry->oClass)
				if (0==strcmp(thisEntry->oClass,"Real")) {
					double tmpDouble;
					tmpDouble = FlipDoubleIfNecessary(*(double *)thisEntry->var);
					fprintf( fp,  "%f\n", tmpDouble);
				}
				else
					fprintf( fp,  "<%s, length %ld>\n", thisEntry->oClass, (ulong *) thisEntry->length);
			else
				fprintf( fp,  "<binary, length %ld>\n", (ulong *) thisEntry->length);
			break;
		
		case kDILNIL:				/*	nil object						*/
			fprintf (fp, "NIL\n");
			break;
		
		case kDILBLOB:				/*	Large Binary Object				*/
			fprintf (fp, "BLOB\n");
			break;
		
		default:					// ~~~~~ ignore these
			fprintf (fp, "UNKNOWN TYPE\n");
			break;
	}
}

//========================================================================================
// printTree

// printTree pretty prints the output, traversing the simple "tree" of data
//========================================================================================
void printTree (FILE * fp, slotDefinition *thisEntry, short depth)
{
	short i;
	short y;
	
	for (i = 0; i < depth; i++)
		fputc('\t', fp);
		
	printEntry(fp, thisEntry);
	
	if (thisEntry->children)
		for (y= 0; y < thisEntry->childCnt; y++)
			printTree(fp, &(thisEntry->children[y]), depth + 1);
}


/* ErrorStrings
 * 
 * Purpose: return error string based on error number from the CDIL or FDIL
 */
char* ErrorStrings(CommErr theErr, char* theString)
{
	switch (theErr)
		{
		case kOurTimeoutError:  strcpy((char*) theString, 
			(char*) "Timeout error. (Determined by SoupDrink)"); break;
		case -97:  strcpy((char*) theString, 
			(char*) "The port is busy. Quit the relevant application or restart if necessary (from ATalk Driver)");  break;

		// CDIL errors (see DILCPipe.h)
		case -28701:  strcpy((char*) theString, 
			(char*) "Error on memory allocation.");  break;
		case -28702:  strcpy((char*) theString, 
			(char*) "DIL pipe was set to a bad state.");  break;
		case -28703:  strcpy((char*) theString, 
			(char*) "An unknown exception has occurred.");  break;
		case -28704:  strcpy((char*) theString, 
			(char*) "The queue of asynchronous calls is full.");  break;
		case -28705:  strcpy((char*) theString, 
			(char*) "Pipe has not been initialized.");  break;
		case -28706:  strcpy((char*) theString, 
			(char*) "Parameter passed in was invalid.");  break;
		case -28707:  strcpy((char*) theString, 
			(char*) "Pipe is not ready for operation.");  break;
		case -28708:  strcpy((char*) theString, 
			(char*) "Timeout during DIL operation.");  break;

		// FDIL errors (see HLFDIL.h)
		case -28801:  strcpy((char*) theString, 
			(char*) "Out of heap memory");  break;
		case -28802:  strcpy((char*) theString, 
			(char*) "Out of temporary or other memory");  break;
		case -28803:  strcpy((char*) theString, 
			(char*) "Unknown slot");  break;
		case -28804:  strcpy((char*) theString, 
			(char*) "Slot size exceeded");  break;
		case -28805:  strcpy((char*) theString, 
			(char*) "Slot size is required");  break;
		case -28806:  strcpy((char*) theString, 
			(char*) "Unexpected data type");  break;

		// other errors
		case -28003:  strcpy((char*) theString, 
			(char*) "The communication operation was aborted.");  break;
		case -28009:  strcpy((char*) theString, 
			(char*) "Bad connection detected");  break;
		case -28017:  strcpy((char*) theString, 
			(char*) "Out of memory");  break;
		case -28029:  strcpy((char*) theString, 
			(char*) "Cannot connect to modem; no response.");  break;
		case -28030:  strcpy((char*) theString, 
			(char*) "Disconnection detected.");  break;
		case -28100:  strcpy((char*) theString, 
			(char*) "Disconnect occurred while reading.");  break;
		case -28101:  strcpy((char*) theString, 
			(char*) "An error occurred while reading.");  break;
		case -28102:  strcpy((char*) theString, 
			(char*) "The communication tool was not found");  break;
		case -28103:  strcpy((char*) theString, 
			(char*) "Bad modem tool version");  break;
		default: sprintf(theString, "%ld", theErr);
		}
	return (char*) theString;
}

