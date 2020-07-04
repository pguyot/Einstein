/*
 *	File:		DILette.c
 * 
 *	Contains:	Minimal demonstration application for the CDILs
 *
 *	Written by:	David Fedor, Newton Developer Technical Support
 * 
 *	Copyright:	© 1996-1997 by Apple Computer, Inc.  All rights reserved.
 *
 *  Notes:
 *     This is intended to show the minimum necessary to write an application
 *     which uses the DILs.  It is not a "friendly" application and has no
 *     UI whatsoever... the better to provide a simple demonstration!
 *
 *     DILette is intended to be used with the Newton package "SoupDrink".
 *     When run, DILette will make a connection and send down the string
 *     "HELO".  It will expect the Newton side to respond with "Hello".
 *     When this response has been received, DILette will disconnect.
 *
 *     If any errors are encountered on this side, a message will be displayed
 *     in a message box, and the pipe will be disconnected.
 *
 *     You may incorporate this sample code into your applications without
 *     restriction.  This sample code has been provided "AS IS" and the
 *     responsibility for its operation is 100% yours.  You are not
 *     permitted to modify and redistribute the source as "DTS Sample Code."
 *     If you are going to re-distribute the source, we require that you
 *     make it clear in the source that the code was descended from
 *     Apple-provided sample code, but that you've made changes.
 */ 

// standard windows include files
#include <windows.h>
#include <string.h>

#include "DILCPipe.h"

// the generic catch-all error handler, which alerts the user, disconnects and returns.
#define HANDLEERROR(err, msg) if (err) { MessageBox(0,msg,"Dilette",MB_OK | MB_ICONSTOP); if (ourPipe) { CDPipeDisconnect(ourPipe); CDDisposeCDILObject(ourPipe); } return 0; }

// this is needed for Windows to see what time it is (for CDPipeAccept)
#define CurrentTimeInSeconds() ((long)(GetTickCount() / 1000))

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	CommErr		fErr;
	CDILPipe	*ourPipe=0;
	long		length;
	Boolean		eom;
	char		response[100];
	long		endTime;

	// initialize the CDIL library
	fErr = CDInitCDIL() ; 
	HANDLEERROR(fErr, "Error initializing CDILs");

	// create the pipe
	ourPipe = CDCreateCDILObject();
	HANDLEERROR(ourPipe == 0, "Error creating the pipe");

	// set the application instance
	CDSetApplication(ourPipe, hInstance);

	// initialize the pipe and say which port to use
	fErr = (short) CDPipeInit(ourPipe, "MNP", "", "COM1:38400,n,8,1", "COM1", kDefaultBufferSize, kDefaultBufferSize);
	HANDLEERROR(fErr, "Error initializing the pipe");

	MessageBox(0, "Hit 'Connect' in SoupDrink on your Newton device now, and click OK here.", "Dilette", MB_OK);
	
	// listen for the Newton device's connection
	fErr = CDPipeListen(ourPipe, kDefaultTimeout, 0, 0);
	HANDLEERROR(fErr, "Error calling CDPipeListen... check the cables... is the Newton application running?");

	// accept the connection from the Newton device

	// This code doesn't need to be done on the MacOS side, but is currently required
	// for Windows.  You need to loop a little bit for the connection state to be
	// set to kCDIL_ConnectPending.  It might not do that if there's nobody on the
	// other side of the cable, though!

	// Loop until the Newton device connects
	endTime = CurrentTimeInSeconds() + 15;	// time that we should stop listening
	while (CurrentTimeInSeconds() < endTime) {
		if (CDGetPipeState(ourPipe) == kCDIL_ConnectPending) {
			fErr = CDPipeAccept(ourPipe);
			HANDLEERROR(fErr, "Error calling CDPipeAccept");
			break;
		} else
			CDIdle(ourPipe);
	}
	if (CurrentTimeInSeconds() >= endTime) {	// did we time out?
		CDPipeDisconnect(ourPipe);
		HANDLEERROR(-28708, "Timeout while waiting for a Newton connection...");
	}

	// now send a string down the pipe!
	length = 5;		// # of characters to transmit
	fErr = CDPipeWrite(ourPipe, "HELO\4", &length, true,0,0,1000,0,0);
	HANDLEERROR(fErr, "Error calling CDPipeWrite");
	
	// and read the response.
	length = 5;		// # of characters to read
	fErr = CDPipeRead(ourPipe, response, &length, &eom,0,0,15000,0,0);
	HANDLEERROR(fErr, "Error calling CDPipeRead");
	
	// see if we got the expected response, just for the heck of it.
	response[5] = 0;
	if (strcmp(response, "Hello") != 0)
		HANDLEERROR(1, "Unexpected response from SoupDrink (it didn't respond 'Hello')");

	// At this point, we're in business, talking back and forth with the Newton
	// device.  We could do anything at this point, including use the FDIL calls
	// (FDput, etc) to send frames back and forth.  However, we'll just shut down
	// the connection and clean up... we're done!
	CDPipeDisconnect(ourPipe);
	CDDisposeCDILObject(ourPipe);
	CDDisposeCDIL();
	
	// let the user know that it all worked as expected.
	MessageBox(0, "Success - everything worked.", "Dilette", MB_OK);
	return 0;
}
