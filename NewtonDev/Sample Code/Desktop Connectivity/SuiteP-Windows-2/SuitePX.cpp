/*
**      Newton Developer Technical Support Sample Code
**
**      Suite P, code to demonstrate use of the 2.0 DILs and provide basic Windows & Mac UI
**
**      by David Fedor, Newton Developer Technical Support
**
**      Copyright © 1997 by Apple Computer, Inc.  All rights reserved.
**
**      You may incorporate this sample code into your applications without
**      restriction.  This sample code has been provided "AS IS" and the
**      responsibility for its operation is 100% yours.  You are not
**      permitted to modify and redistribute the source as "DTS Sample Code."
**      If you are going to re-distribute the source, we require that you
**      make it clear in the source that the code was descended from
**      Apple Computer, Inc.-provided sample code, but that you've made changes.
*/

// This file contains the cross-platform code for SuiteP, including all
// of the DIL calls.

#ifdef forMacOS
	#include "string.h"
	#include <stdio.h>		// for sprintf()
	#define NEWLINESTRING "\r"
#else
	#include "stdafx.h"     // the VC++ makefile could have /Yu"stdafx.h" but it didn't handle the enclosing #ifdef right.
                            // This seems to be a bug in VC++ as far as I can tell...
	#include "SuiteP.h"
	#define NEWLINESTRING "\r\n"
#endif

/////////////////////////////////////////////////////////////////////////////
// Cross-platform code from here on down!
//
// Below is the code which "really" does the DIL work, as opposed to the
// platform-specific and/or UI code.
/////////////////////////////////////////////////////////////////////////////

#include "SuitePX.h"	// the cross-platform includes

void MsgErr(DIL_Error err, LPSTR msg);	// prints out the given error code and message
void SoupDrinkContinue();


// user-visible descriptions of the states of a CDIL Pipe
static LPSTR stateMsgs[] = {"Unknown","Uninitialized","Disconnected","Listening",
                            "Connect Pending","Connected","Disconnect Pending"};


// Define the order and functionality of the buttons on the dialog.
// The text of the buttons are in button_captions, terminated with a NULL.
// The UI currently has a maximum of 8 buttons, but that's not too hard to change...
static LPSTR button_captions[] = {"SoupDrink", "New Name SD", "New Name PDIL", "PDSurvey", "RootSlots", "Get Clip", "Disconnect", NULL};
enum button_defs {btn_SoupDrink, btn_NewNameSD, btn_NewNamePDIL, btn_PDSurvey, btn_RootSlots, btn_GetClip, btn_Disconnect};


// what action we want to do when we get idle time
enum idleActions {doNothing, doSoupDrinkStart, doSoupDrinkContinue, doNewNameSD, doNewNamePDIL, doPDSurvey, doRootSlots, doGetClip};
int idleAction=doNothing;		// default to doing nothing

// we currently just use one global pipe; we could maintain several if we wanted...
CD_Handle pipe;

// This is the data for a protocol extension which compiles and evaluates a NewtonScript 
// expression, optionally calls the result with some parameters, and returns the result
// to the caller.  With this you can do your own pseudo-Inspector, or generate NewtonScript 
// on the fly to run on the device!
const unsigned char CompNRunData[] = 
{0x02, 0x06, 0x05, 0x07, 0x05, 0x63, 0x6C, 0x61, 0x73, 0x73, 0x07, 0x0C, 0x69, 0x6E, 0x73, 0x74, 
 0x72, 0x75, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x73, 0x07, 0x08, 0x6C, 0x69, 0x74, 0x65, 0x72, 0x61, 
 0x6C, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6D, 0x65, 0x07, 0x07, 0x6E, 0x75, 
 0x6D, 0x41, 0x72, 0x67, 0x73, 0x00, 0x32, 0x03, 0x5F, 0x09, 0x02, 0x7B, 0x18, 0x38, 0xA4, 0x20, 
 0xA7, 0x00, 0x07, 0x19, 0x27, 0x00, 0xEC, 0xC9, 0x7C, 0x1A, 0x91, 0x1B, 0x29, 0xA6, 0x24, 0xA7, 
 0x00, 0x07, 0x7E, 0x30, 0xA6, 0x27, 0x00, 0x08, 0xA7, 0x00, 0x07, 0x22, 0x7C, 0x1C, 0xC7, 0x00, 
 0x17, 0x6F, 0x00, 0x31, 0x7C, 0x1C, 0x91, 0x7E, 0x31, 0x5F, 0x00, 0x32, 0x7E, 0x1D, 0x8A, 0xA5, 
 0x07, 0x00, 0x07, 0x5F, 0x00, 0x52, 0x7F, 0x00, 0x07, 0x7C, 0x1E, 0x91, 0x6F, 0x00, 0x4B, 0x1F, 
 0x00, 0x07, 0x28, 0x5F, 0x00, 0x4C, 0x22, 0x1D, 0x8A, 0xA5, 0x07, 0x00, 0x07, 0x1F, 0x00, 0x08, 
 0x7D, 0x27, 0x00, 0x1A, 0x7B, 0x1F, 0x00, 0x09, 0x3B, 0x02, 0x04, 0x0A, 0x09, 0x03, 0x07, 0x0F, 
 0x52, 0x65, 0x61, 0x64, 0x43, 0x6F, 0x6D, 0x6D, 0x61, 0x6E, 0x64, 0x44, 0x61, 0x74, 0x61, 0x07, 
 0x06, 0x65, 0x76, 0x74, 0x2E, 0x65, 0x78, 0x07, 0x03, 0x73, 0x72, 0x63, 0x07, 0x07, 0x43, 0x6F, 
 0x6D, 0x70, 0x69, 0x6C, 0x65, 0x07, 0x04, 0x61, 0x72, 0x67, 0x73, 0x07, 0x05, 0x41, 0x72, 0x72, 
 0x61, 0x79, 0x07, 0x05, 0x64, 0x65, 0x62, 0x75, 0x67, 0x07, 0x10, 0x43, 0x75, 0x72, 0x72, 0x65, 
 0x6E, 0x74, 0x45, 0x78, 0x63, 0x65, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x08, 0x0A, 0x00, 0x43, 0x00, 
 0x52, 0x00, 0x75, 0x00, 0x6E, 0x00, 0x00, 0x07, 0x0C, 0x57, 0x72, 0x69, 0x74, 0x65, 0x43, 0x6F, 
 0x6D, 0x6D, 0x61, 0x6E, 0x64, 0x0A, 0x00, 0xFF, 0x00, 0x10, 0x00, 0x04};


///////////////////////////////////////////////////////////////////////////////////////////////////
// MsgErr is like Msg() or Msg2(); it prints out the given (optional) error code and message
void MsgErr(DIL_Error err, LPSTR msg)	
{
	char tmpbuf[20];
	sprintf(tmpbuf, "(%d)", err);
	if (msg)
		Msg2(msg, -1);
	Msg(tmpbuf);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void DoStartupWork()
{
	char tmpbuf[80];
	long buflen;
	short i, portnum;
	DIL_Error err=kDIL_NoError;

	pipe = 0;

	CD_Startup();
	FD_Startup();
	PD_Startup();

	// set the captions for the pushbuttons - maximum of 8 supported by this UI
	i=0;
	while ((i<8) && (button_captions[i])) {
		ConfigureButton(i, button_captions[i], 1);
		i++;
	}
	while (i<8) {		// hide the buttons we're not using right now
		ConfigureButton(i, NULL, 0);
		i++;
	}
	
	// set the port names - maximum of 4 choices supported by this UI.
	// If AppleTalk is available, we always put it at position 0 in the list.
	// This way we can consider it almost just like another serial port.
	i=0;
	if (CD_HasADSP()==kDIL_NoError) {	// this might change to a boolean; at least I hope so...
		SetRadioBtnText(i, "AppleTalk");		// we'll always use AppleTalk as selection 0
		i++;
	}
	portnum=0;
	while ((i<4) && (err==kDIL_NoError)) {
		buflen=79;
		err = CD_GetSerialPortName(portnum, tmpbuf, &buflen);
		if (kDIL_NoError==err) {
			SetRadioBtnText(i, tmpbuf);
			i++;
			portnum++;
		}
	}
	while (i<4) {
		SetRadioBtnText(i, NULL);	// this'll make the radio button invisible
		i++;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DoShutdownWork()
{
	if (pipe) {
		CD_Disconnect(pipe);
		CD_Dispose(pipe);
		pipe=0;	
	}

	PD_Shutdown();
	FD_Shutdown();
	CD_Shutdown();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DoIdleWork()
{
	CD_State state;
	long bytesAvail;
	// keep track of state changes so the dialog isn't constantly repainting itself.
	// laststate is initialized to kCD_Connected because it can't possibly start out that way...
	static CD_State laststate=kCD_Connected;

	if (pipe) {
		CD_Idle(pipe);
		state = CD_GetState(pipe);  // this does an implicit idle, but I still call CD_Idle just to make it clear.
		if (state!=laststate)
			SetStatusText(stateMsgs[state]);
		laststate = state;
		
		// display how many bytes are waiting in the pipe to be read by us
		CD_BytesAvailable(pipe, &bytesAvail);
		SetBytesAvail(bytesAvail);

		// is someone trying to connect to us while we're listening?
		if (state==kCD_ConnectPending) {
			Msg("Accepting...");
			CD_Accept(pipe);
		}

		// Call a routine if we're connected and in the middle of doing something.
		// This is how this app does asynchronous operations. Simple but effective!
		// The kCD_DisconnectPending state is allowed here since the Newton device
		// may have disconnected before we got around to reading all of the data it sent.
		if (((state==kCD_Connected) || (state==kCD_DisconnectPending)) && (idleAction != doNothing))
			DoAnAction();
	} else {
		SetStatusText("(no pipe exists)");
		SetBytesAvail(0);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void StartListening(int talkToSoupDrink)	// starts listening on the selected port
{
	short which;
	DIL_Error err;
	
	if (pipe) {
		Msg("(Disconnecting and disposing the previously active pipe first)");
		CD_Disconnect(pipe);
		CD_Dispose(pipe);
		pipe=0;
	}
	
	which = GetSelectedRadioBtn();
	
	// AppleTalk, if supported, is always the first item in the list.
	if (CD_HasADSP()==kDIL_NoError)	// this might change to a boolean; at least I hope so...
		which--;	// so that "which" will match the serial port numbers

	if (which== -1)
		if (talkToSoupDrink)
			err = CD_CreateADSP(&pipe, NULL, "SoupDrink");
		else
			err = CD_CreateADSP(&pipe, NULL, NULL);
	else
		err = CD_CreateMNPSerial(&pipe, which, 38400);

	if (err==kDIL_NoError) {
		err = CD_StartListening(pipe);	// listen for a connection
		if (err!=kDIL_NoError)
			MsgErr(err, "Couldn't listen for a connection.");	
	} else MsgErr(err, "Couldn't create the pipe.");
	
	if (err!=kDIL_NoError) {
		CD_Disconnect(pipe);
		CD_Dispose(pipe);
		pipe=0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// These three routines are called back from the FDILs and PDILs to read and write
// data and get pipe status; we just turn around and call the CDIL to have it do the work.
DIL_Error cdilReader(void* buf, long amt, void* userData)
{
	return CD_Read((CD_Handle)userData, buf, amt);
}

DIL_Error cdilStatus(long* bytesAvailable, void* userData)
{
	return CD_BytesAvailable((CD_Handle)userData, bytesAvailable);
}

DIL_Error cdilWriter(const void* buf, long amt, void* userData)
{
	if (amt== -1)
		return CD_FlushOutput((CD_Handle)userData);
	else
		return CD_Write((CD_Handle)userData, buf, amt);
}


// This "writing" procedure doesn't actually write out the data; it just
// counts how many bytes *would* have been written out.  This is useful
// for seeing how large a flattened frame will be when it is written.
DIL_Error countBytesWriter(const void* buf, long amt, void* userData)
{
	long *lenSoFar = (long *) userData;
	*lenSoFar = amt + *lenSoFar;
	return kDIL_NoError;
}

// This is a writing procedure which just sends the data to our output text box.
// It is useful particularly for FD_PrintObject...
DIL_Error debugOutWriter(const void* buf, long amt, void* userData)
{
	Msg2((LPSTR)buf, amt);
	return kDIL_NoError;
}


//////////////////////////////////////////////////////////////////////////////////////////
// A structure for use by bufferReader and its callers.
// The caller should initialize offset to 0 when starting the read operation
typedef struct {
	unsigned char *theData;
	long offset;
} bufferReaderStruct;

// bufferReader is used with FD_Unflatten when you've a pointer to the flattened buffer
DIL_Error bufferReader(void* buf, long amt, void* userData)
{
	bufferReaderStruct *ourData = (bufferReaderStruct *) userData;

	memcpy(buf, ourData->theData + ourData->offset, amt);	// copy the data
	ourData->offset = ourData->offset + amt;				// move our pointer along

	return kDIL_NoError;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Part 1 of the SoupDrink suite.  This starts off the "conversation" with the SoupDrink package,
// and then calls SoupDrinkContinue() to handle reading the data and looping through the entries.
void SoupDrinkStart()
{
	char cmd[5];

	CD_Write(pipe, "DRNKNames\4", 10);	// tell the SoupDrink app we want to 'drink' the names soup
	CD_Read(pipe, &cmd, 4);				// read four bytes of command - it should be "DRNK"
	cmd[4]=0;
	if (0==strcmp(cmd, "DRNK")) {
		SoupDrinkContinue();		// we're all set; start the drinking process.
	} else {
		Msg("Didn't get 'DRNK' response; disconnecting...");
		CD_Disconnect(pipe);
		CD_Dispose(pipe);
		pipe=0;
	}
}

// Part 2 of the SoupDrink suite.  This routine handles one soup entry, and asks for the next.
void SoupDrinkContinue()
{
	FD_Handle theEntry=kFD_NIL;
	long framelength;
	char cmd[5];
	CD_State state;
	DIL_Error res;

	res=CD_Read(pipe, &cmd, 4);			// read four bytes of command - it should be "ENTR" or "END "
	cmd[4]=0;							// null terminate it so we can call strcmp()
	state = CD_GetState(pipe);
	
	if ((res==0) && (0==strcmp(cmd, "END "))) {
		Msg("All done; everything was successful!");
		CD_Disconnect(pipe);
		CD_Dispose(pipe);
		pipe=0;
		idleAction = doNothing;				// stop the cycle; we're done!
	} else if ((res==0) && ((kCD_Connected == state) || (kCD_DisconnectPending == state)) && (0==strcmp(cmd, "ENTR"))) {
		CD_Read(pipe, &framelength, 4);		// read the 4 bytes of length before the soup entry
		theEntry = FD_Unflatten(cdilReader, pipe);			// read the frame from the pipe
		CD_Write(pipe, "OK\4",3);			// tell SoupDrink to send the next frame, while we're dumping the object.

		FD_PrintObject(theEntry, NEWLINESTRING, debugOutWriter, 0);		// dump it out as text
		Msg(NEWLINESTRING);					// print a blank line between the frames
		FD_DeepDispose(theEntry);			// don't leak memory!

		idleAction = doSoupDrinkContinue;	// tell DoIdleWork() to call us the next time it can
	}
	else {
		if (res)
			Msg("An error occurred while reading the next command from the Newton device.");
		else if (kCD_Connected == state) {
			Msg2("Unknown response from the Newton device: got '",-1);
			Msg2(cmd,-1);
			Msg2("'; expected 'ENTR' or 'END '...",-1);
		} else
			Msg("The connection dropped unexpectedly; didn't get the 'END ' command.");
		CD_Disconnect(pipe);
		CD_Dispose(pipe);
		pipe=0;
		idleAction = doNothing;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Creates a FDIL frame, with subframes, which is a name card suitable for adding to the Names soup
FD_Handle CreateNameFrame()
{
	FD_Handle name = FD_MakeFrame();
	FD_SetClass(name, FD_MakeSymbol("person"));
	FD_SetFrameSlot(name, "first", FD_MakeString("John"));
	FD_SetFrameSlot(name, "last", FD_MakeString("Doe"));

	FD_Handle phones = FD_MakeArray(0, "phones");
	FD_Handle thePhoneNum = FD_MakeString("555-1212");
	FD_SetClass(thePhoneNum, FD_MakeSymbol("HomePhone"));
	FD_AppendArraySlot(phones, thePhoneNum);

	FD_Handle entry = FD_MakeFrame();
	FD_SetClass(entry, FD_MakeSymbol("person"));
	FD_SetFrameSlot(entry, "cardType", FD_MakeInt(3));
	FD_SetFrameSlot(entry, "Name", name);
	FD_SetFrameSlot(entry, "Address", FD_MakeString("5 Infinite Loop"));
	FD_SetFrameSlot(entry, "City", FD_MakeString("Cupertino"));
	FD_SetFrameSlot(entry, "Region", FD_MakeString("CA"));
	FD_SetFrameSlot(entry, "Postal_Code", FD_MakeString("95014"));
	FD_SetFrameSlot(entry, "phones", phones);
	FD_SetFrameSlot(entry, "sorton", FD_MakeString("Doe, John"));	// is this the right format?

	return entry;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// talk to the SoupDrink sample code, and tell it to add an entry to the names soup.
void NewNameSD()
{
	FD_Handle name=kFD_NIL;
	char okmsg[2];
	long framelen;
	
	Msg("Sending a new name card down to the Newton device.");

	CD_Write(pipe, "NAME\4", 5);			// tell the SoupDrink app an entry is coming at it
	
	// This is a bit odd. Before you send a frame down to a non-streaming endpoint, you
	// need to send the length of the flattened frame.  So either you have a buffer
	// big enough to hold the flattened frame, flatten it, then send the length and
	// then send the buffered frame, or else you flatten it twice - once where the
	// data isn't stored (it's just finding out the length) and once for real.
	// This second method is easier, safer and takes less memory, though it's slower.
	// (This isn't a DIL bug; this is a lazyness of the ROM...)
	name = CreateNameFrame();
	framelen = 0;
	FD_Flatten(name, countBytesWriter, &framelen);		// find out how large the frame is
	framelen = DIL_CANONICAL_32(framelen);				// byteswap it if necessary for this platform
	CD_Write(pipe, &framelen, 4);						// send the length of the frame
	FD_Flatten(name, cdilWriter, pipe);					// now send the frame itself
	
	Msg("Frame sent; waiting for an OK back to confirm delivery...");
	CD_Read(pipe, &okmsg, 2);				// wait for the "OK" back from the Newton device before disconnecting

	Msg("Done.");
	FD_DeepDispose(name);		// kill the name frame and all subobjects
	CD_Disconnect(pipe);
	CD_Dispose(pipe);
	pipe=0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// use PDIL routines to talk to the built-in "Dock" or "Connection" application, to add an entry to the names soup
void NewNamePDIL()
{
	PD_Handle pd;
	FD_Handle name=kFD_NIL;		// these are set to kFD_NIL so that we can call FD_Dispose
	FD_Handle tmp=kFD_NIL;		// whether or not we actually change their values.
	FD_Handle defStore=kFD_NIL;
	DIL_Error err;
	long entryID;
	
	// create a PDIL session, which will use our cdil pipe and i/o callback functions
	err = PD_CreateSession(&pd, cdilReader, cdilStatus, cdilWriter, pipe, NULL, 0);
	if (err==kDIL_NoError) {
		Msg("Connected to the device");
		err = PD_SetCurrentStore(pd, kFD_NIL);		// select the default store.
		if (err == kDIL_NoError) {
			err = PD_SetCurrentSoup(pd, FD_MakeString("Names"));
			if (err == kDIL_NoError) {
				name = CreateNameFrame();
				err = PD_AddEntry(pd, name, &entryID);
				if (err != kDIL_NoError)
					Msg("Couldn't add the new name card.");
				else
					Msg("New name card added successfully!");
			} else Msg("Couldn't select the names soup.");
		} else Msg("Couldn't set the current store.");
	} else Msg("Couldn't create the PDIL session.");
	
	FD_DeepDispose(name);		// kill the name frame and all subobjects
	FD_DeepDispose(tmp);
	PD_Dispose(pd);
	CD_Disconnect(pipe);
	CD_Dispose(pipe);
	pipe=0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Finds out what stores and soups are on the device, printing out their names
// This shows some more complex fdil calls (getting a slot out of a frame, converting
// it to a C string, etc.)
void PDSurvey()
{
	PD_Handle pd;
	FD_Handle name=kFD_NIL;		// these are set to kFD_NIL so that we can call FD_Dispose
	FD_Handle tmp=kFD_NIL;		// whether or not we actually change their values.
	FD_Handle defStore=kFD_NIL;
	DIL_Error err;
#define PDSurveyBufLen 200
	char      tmpBuf[PDSurveyBufLen];
	long i, numItems;
	
	// create a PDIL session
	err = PD_CreateSession(&pd, cdilReader, cdilStatus, cdilWriter, pipe, NULL, 0);
	if (err==kDIL_NoError) {
		Msg2("Connected to Newton Device named: ", -1);
		err = PD_GetNewtonName(pd, &tmp);
		if (err == kDIL_NoError) {
			FD_GetString(tmp, tmpBuf, PDSurveyBufLen);	// copy the string to our temporary buffer
			Msg(tmpBuf);
		} else
			Msg("(name unavailable)");
				
		FD_DeepDispose(tmp); tmp=kFD_NIL;
		err = PD_GetAllStores(pd, &tmp);
		if (err == kDIL_NoError) {
			Msg("==== Stores:");
			numItems = FD_GetLength(tmp);
			sprintf(tmpBuf, "There are %ld stores available.", numItems);
			for (i=0; i< numItems; i++) {
				name = FD_GetFrameSlot(FD_GetArraySlot(tmp,i), "name");
				if (!FD_IsNIL(name)) {
					FD_GetString(name, tmpBuf, PDSurveyBufLen);	// copy the string to our temporary buffer
					Msg(tmpBuf);
				} else
					Msg("(no name available for this store)");
			}
		} else
			Msg("Couldn't get the list of stores.");
			
		err = PD_SetCurrentStore(pd, kFD_NIL);		// select the default store.
		if (err != kDIL_NoError)
			Msg("Couldn't set the current store.");
		
		FD_DeepDispose(tmp); tmp=kFD_NIL;
		err = PD_GetAllSoups(pd, &tmp);
		if (err == kDIL_NoError) {
			Msg("");
			Msg("==== Soups:");

			numItems = FD_GetLength(tmp);
			sprintf(tmpBuf, "There are %ld soups on the default store.", numItems);
			Msg(tmpBuf);
			for (i=0; i< numItems; i++) {
				name = FD_GetArraySlot(FD_GetArraySlot(tmp,i), 0);
				if (!FD_IsNIL(name)) {
					FD_GetString(name, tmpBuf, PDSurveyBufLen);	// copy the string to our temporary buffer
					Msg(tmpBuf);
				} else
					Msg("(no name available for this soup)");
			}
			Msg("Done.");
		} else
			Msg("Couldn't get the list of soups on the default store.");
		

	} else Msg("Couldn't create the PDIL session.");
	
	FD_DeepDispose(name);		// kill the name frame and all subobjects
	FD_DeepDispose(tmp);
	PD_Dispose(pd);
	CD_Disconnect(pipe);
	CD_Dispose(pipe);
	pipe=0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Uses a protocol extension to compile and run a NewtonScript command that finds out how many
// slots are in GetRoot().  This is an example of code that must be executed on the Newton device 
// itself because of the large amount of data involved. It's trivial when executed on the device
// but impossible to do from the desktop.  (There are better uses for protocol extensions,
// of course...)
// The NTK source code for the protocol extension is available in another sample project, "CompNRun".
// NOTE: this routine sometimes makes my MP2000 reboot when the connection is shut down.  
// Information on why this happens, and fixes/workarounds, will be forthcoming...
void RootSlots()
{
	PD_Handle pd;
	DIL_Error err;
	FD_Handle res=kFD_NIL;
	FD_Handle ext=kFD_NIL;
	FD_Handle args=kFD_NIL;
	FD_Handle tmp=kFD_NIL;
	bufferReaderStruct bufStruct;	// for unflattening from a buffer
	long code = DIL_CANONICAL_32('CRun');	// this is what we're calling the protocol extension.
											// Don't use codes which are all lower-case; those are reserved.
	
	// create a PDIL session, which will use our cdil pipe and i/o callback functions
	err = PD_CreateSession(&pd, cdilReader, cdilStatus, cdilWriter, pipe, NULL, 0);
	if (err==kDIL_NoError) {
		Msg("Connected to the device");
		// use bufferReader to unflatten the protocol extension frame.  This could
		// also be done by reading the protocol extension from a file or resource.
		bufStruct.theData = (unsigned char *) CompNRunData;
		bufStruct.offset = 0;
		ext = FD_Unflatten(bufferReader, &bufStruct);
		err = FD_GetError();

		if (err==kDIL_NoError) {
			Msg("Installing the extension...");
			err = PD_LoadExtension(pd, code, ext);
			if (err==kDIL_NoError) {
				// Now write some NewtonScript code and call it!  You can do anything here; the sky's the limit...
				Msg("Calling the extension...");
				args = FD_MakeFrame();
				FD_SetFrameSlot(args, "src", FD_MakeString("length(GetRoot())"));
				FD_SetFrameSlot(args, "debug", kFD_True);		// so that exception data gets returned to us
				err = PD_CallExtension(pd, code, args, &res);

				if (err==kDIL_NoError) {
					// CRun returns an array of two elements.  The first gives status, and the second
					// element is the return value or exception frame.
					if ((FD_IsArray(res)) && (FD_GetLength(res)==2)) {
						tmp = FD_GetArraySlot(res, 0);
						if (FD_IsNIL(tmp)) {	// if tmp is nil, everything worked; nothing threw.
							Msg("Return value from the protocol extension:");
							// Just print the result.  Typically you'd do something more interesting with it.
							FD_PrintObject(FD_GetArraySlot(res, 1), NEWLINESTRING, debugOutWriter, 0);
						} else if (FD_Equal(tmp, FD_MakeInt(0)))
							Msg("That code couldn't be compiled.");
						else if (FD_Equal(tmp, FD_MakeInt(1)))
							Msg("Evaluating that code threw an exception.");
						else if (FD_Equal(tmp, FD_MakeInt(2)))
							Msg("Calling that routine threw an exception."); // we won't see this; we're not calling a routine
						else {
							Msg("Unexpected response from the extension: ");
							FD_PrintObject(res, NEWLINESTRING, debugOutWriter, 0);
						}
						Msg(NEWLINESTRING);
					} else Msg("Unexpected return value from the extension.");
				} else MsgErr(err, "Couldn't call the extension.");
				PD_RemoveExtension(pd, code);		// be tidy.  Shouldn't have to do this, but it's nice to do.
			} else MsgErr(err, "Couldn't install the extension.");
		} else MsgErr(err, "Couldn't unflatten the extension");
	} else MsgErr(err, "Couldn't create the PDIL session.");

	PD_Dispose(pd);
	FD_DeepDispose(tmp);
	FD_DeepDispose(args);
	FD_DeepDispose(ext);
	FD_DeepDispose(res);
	CD_Disconnect(pipe);
	CD_Dispose(pipe);
	pipe=0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Calls the global function GetClipboard() on the Newton device, to get the contents of the
// clipboard.  Note that this function only exists on Newton 2.1 devices... on Newton 2.0, it
// will throw.
// This routine could (and probably should) verify that it's a 2.1 device before calling
// the function.
// NOTE: this routine sometimes makes my MP2000 reboot when the connection is shut down.  
// Information on why this happens, and fixes/workarounds, will be forthcoming...
void GetClip()
{
	PD_Handle pd;
	DIL_Error err;
	FD_Handle res=kFD_NIL;
	FD_Handle args=kFD_NIL;
	FD_Handle tmp=kFD_NIL;
	
	// create a PDIL session, which will use our cdil pipe and i/o callback functions
	err = PD_CreateSession(&pd, cdilReader, cdilStatus, cdilWriter, pipe, NULL, 0);
	if (err==kDIL_NoError) {
		Msg("Connected to the device.  Getting the clipboard...");
		args = FD_MakeArray(0, "Array");		// we don't need to pass any arguments to GetClipboard.
		err = PD_CallGlobalFunction(pd, "GetClipboard", args, &res);
		if (err==kDIL_NoError) {
			if (FD_IsNIL(res))
				Msg("There is nothing on the clipboard.");
			else
				FD_PrintObject(res, NEWLINESTRING, debugOutWriter, 0);
		} else MsgErr(err, "Couldn't call the function.  (Is this a 2.1 Newton device? GetClipboard didn't exist in 2.0...");
	} else MsgErr(err, "Couldn't create the PDIL session.");

	Msg(NEWLINESTRING);
	PD_Dispose(pd);
	FD_DeepDispose(tmp);
	FD_DeepDispose(args);
	FD_DeepDispose(res);
	CD_Disconnect(pipe);
	CD_Dispose(pipe);
	pipe=0;
}


// determine what to do when the user clicks on a button
void DoButtonHit(short whichBtn)
{
	switch (whichBtn) {
		case btn_SoupDrink:
			Msg("Listening... press 'Connect' on the SoupDrink slip.");
			StartListening(1);
			idleAction=doSoupDrinkStart;
			break;
		case btn_NewNameSD:
			Msg("Listening... press 'Connect' on the SoupDrink slip.");
			StartListening(1);
			idleAction=doNewNameSD;
			break;
		case btn_NewNamePDIL:
			Msg("Listening... press 'Connect' on the built-in Dock/Connection slip.");
			StartListening(0);
			idleAction=doNewNamePDIL;
			break;
		case btn_PDSurvey:
			Msg("Listening... press 'Connect' on the built-in Dock/Connection slip.");
			StartListening(0);
			idleAction=doPDSurvey;
			break;
		case btn_RootSlots:
			Msg("Listening... press 'Connect' on the built-in Dock/Connection slip.");
			StartListening(0);
			idleAction=doRootSlots;
			break;
		case btn_GetClip:
			Msg("Listening... press 'Connect' on the built-in Dock/Connection slip.");
			StartListening(0);
			idleAction=doGetClip;
			break;
		case btn_Disconnect:
			if (!pipe)
				Msg("There is no pipe allocated, so I can't disconnect anything.");
			else {
				if (kCD_Disconnected==CD_GetState(pipe))
					Msg("The pipe is already disconnected.  Disposing it...");
				else
					Msg("Disconnecting and disposing the pipe...");
				CD_Disconnect(pipe);
				CD_Dispose(pipe);
				pipe=0;
				idleAction=doNothing;
				Msg("The pipe has been disconnected and disposed.");
			}
			break;
		default:
			Msg("That button doesn't have any action associated with it.");
			idleAction=doNothing;  // this shouldn't ever happen...
	}
}

// this is called by the DoIdleWork() routine when it notices someone trying to connect to us.
void DoAnAction()
{
	int theAction = idleAction;	// make a copy and clear it, to prevent accidental looping
	idleAction=doNothing;

	switch (theAction) {
		case doSoupDrinkStart: 
			SoupDrinkStart();
			break;
		
		case doSoupDrinkContinue:
			SoupDrinkContinue();
			break;
		
		case doNewNameSD: 
			NewNameSD();
			break;
		
		case doNewNamePDIL:
			NewNamePDIL();
			break;

		case doPDSurvey:
			PDSurvey();
			break;
		
		case doRootSlots:
			RootSlots();
			break;

		case doGetClip:
			GetClip();
			break;
			
		default:	// shouldn't ever get here!
			Msg("Uh oh, I don't know what to do now!  Unknown/unhandled action...");
	}
}
