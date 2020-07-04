/*
	File:		INTERFAC.h

	Contains:	Definitions for INTERFAC.c

	Written by:	Ryan Robertson

	Copyright:	© 1996 by Apple Computer, Inc.  All rights reserved.

*/

#ifndef __INTERFACE_H
#define __INTERFACE_H
  
#include "DILCPIPE.H"
  
#define kApplicationName 		"Mini-MetaData"
    
//--------------------------------------------------------------------------------------------------------------
// MENU DEFINITIONS
//--------------------------------------------------------------------------------------------------------------

#define IDM_EXIT    			11

#define IDM_CONNECT				21

#define IDM_ABOUT   			31


//--------------------------------------------------------------------------------------------------------------
// DIALOG DEFINITIONS
//--------------------------------------------------------------------------------------------------------------

#define IDD_DIALOG1             101
#define IDC_EDIT1               1000


//--------------------------------------------------------------------------------------------------------------
// STRING DEFINITIONS
//--------------------------------------------------------------------------------------------------------------

#define kInitPipeErrorString		"There was an error initializing the pipe."
#define kConnectErrorString			"There was an error waiting for the connection.  Check your cables and try again."
#define kAcceptFailedString			"The connection was not accepted."

#define kWrongAppString				"The wrong application is trying to connect."
#define kBufferReadErrorString 		"There was a problem receiving data.  The connection has been closed."
#define kBufferWriteErrorString		"There was a problem sending data.  The connection has been closed."
#define kNewtonCancelledString		"The Newton has cancelled the transaction.  The connection has been closed."
#define kFileWriteErrorString		"There was a problem writing to the file.  The connection has been closed."
#define kNoMemoryString				"There was a problem allocating the buffer.  The connection has been closed."
#define kDownloadWasSuccessful		"The export was successful."

#define kCDILInitErrorString		"There was an error initializing the CDIL library."
#define kAlreadyConnectedString		"You are already connected or waiting for a connection."

//--------------------------------------------------------------------------------------------------------------
// ERROR DEFINITIONS
//--------------------------------------------------------------------------------------------------------------

#define noErr					0
#define kError_InitFailed		129
	
//--------------------------------------------------------------------------------------------------------------
// INTERFACE PROTOTYPES
//--------------------------------------------------------------------------------------------------------------

void		FAR PASCAL		ConductErrorDialog( char* errorString );
long		FAR PASCAL		InitializePipe();
Boolean		FAR PASCAL 		AboutDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);
long		FAR PASCAL 		CreateNOpenFile( char* fileName, Boolean* unused, HFILE* fileRefPtr );
long		FAR PASCAL 		WriteToFile( HFILE fileRef, long* length, char* buffer );
HFILE		FAR PASCAL 		UpdateNCloseFile( HFILE fileRef, Boolean* unused );
long		FAR PASCAL		DoTerminate( void );
long		FAR PASCAL 		WndProc (HWND hwnd, UINT message, UINT wParam, LONG lParam);

#endif
