/*
	File:		Protocol.c

	Contains:	Implementation for the protocol

	Written by:	Ryan Robertson

	Copyright:	© 1995-1996 by Apple Computer, Inc.  All rights reserved.

   	Notes:
   	 This file contains all of the cross platform code DILS code.
   	 
     You may incorporate this sample code into your applications without
     restriction.  This sample code has been provided "AS IS" and the
     responsibility for its operation is 100% yours.  You are not
     permitted to modify and redistribute the source as "DTS Sample Code."
     If you are going to re-distribute the source, we require that you
     make it clear in the source that the code was descended from
     Apple-provided sample code, but that you've made changes.

*/

#ifndef forWindows
#define forMac		// Codewarrior doesn't have a handy way to do this.
#endif

#include "Protocol.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef forMac	// Mac specific definitions and included files

#include <Types.h>
#include <Memory.h>
#include <StandardFile.h>
#include "DILCPIPE.h" 
#include "Interface.h"
#define FILEREPLY 	StandardFileReply
#define FILEREF		short     
#define MALLOCCAST	

#else	// Windows specific definitions and included files

#include <windows.h>
#include "INTERFAC.h"  
#include <malloc.h>
#define CurrentTimeInSeconds() ((long)(GetTickCount() / 1000))
#define FILEREPLY  	Boolean
#define FILEREF		HFILE   
#define MALLOCCAST	(size_t)       

#endif

#define Fail(label) {goto label;}


//-------------------------------------------------------------------------------
// GLOBAL VARIABLES
//-------------------------------------------------------------------------------


extern CDILPipe		*gOurPipe;				// See Interface.c/INTERFAC.C for definitions of these variables
extern Boolean		gPipeBusy;


//-------------------------------------------------------------------------------
// INTERFACE IMPLEMENTATION
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------

void DECL DoConnection(void) 
{
	CommErr		anErr = noErr;
	
	// if the pipe initialization completes sucessfully then start the connection
	// InitializePipe is defined in the Interface.c file.
	anErr = InitializePipe();
	if ( anErr )
		ConductErrorDialog( kInitPipeErrorString );
	else {
		long endTime;

		anErr = CDPipeListen( gOurPipe, kDefaultTimeout, NULL, 0 );

		if (anErr) {
			// if there was an error initializing the pipe then dispose of the CDIL object
			CDDisposeCDILObject( gOurPipe );
			gOurPipe = nil;
			
			ConductErrorDialog( kConnectErrorString );
		} else {
#ifdef forMac
			anErr = CDPipeAccept(gOurPipe);
#else
			// This code doesn't need to be executed on MacOS, but is currently required for
			// Windows.  We need to loop, waiting for the connection state to change
			// to kCDIL_ConnectPending.
			anErr = true;

			// Loop until the Newton device connects
			endTime = CurrentTimeInSeconds() + kTimeoutInSecs;
			while (CurrentTimeInSeconds() < endTime ) {
				if (CDGetPipeState( gOurPipe ) == kCDIL_ConnectPending ) {
					anErr = CDPipeAccept( gOurPipe );
					break;
				} else
					CDIdle( gOurPipe );
			}
#endif

			// if there was an error then disconnect the pipe, otherwise start the protocol
			if (anErr) {
				CDDisposeCDILObject( gOurPipe );
				gOurPipe = nil;
				ConductErrorDialog( kAcceptFailedString );
			} else {
				DoProtocol();
				DisconnectPipe();
			}
		}
	}
	gPipeBusy = false;
}	// DoConnection

// -------------------------------------------------------------------------------

void DECL DoProtocol()
{
	FILEREPLY		 	fileReply;	   		// a fileReply is only used on MacOS, but to make
											// this code cross platform we also need it on Windows.
											// On Windows it is defined as a boolean that is unused
	FILEREF				fileRef = 0;
	long				length;
	char*				bufferPtr = NULL;
	long				fBufferResult;
	long				anErr;

	// Send kHelloCommand to the Newton
	fBufferResult = WriteCommand( kHelloCommand );
	if (fBufferResult == kWriteError)
		Fail(FailWrite); 
		
	// Make sure the we have connected to the Mini-MetaData app on the Newton	
	fBufferResult = ReadBuffer( &bufferPtr, &length );
    switch (fBufferResult) {
    	case kReadSuccess:			
    		if ( strcmp( kHeloResponse, bufferPtr ) ) {
    			Fail(FailWrongApp);
    		}
    		break;									
    	case kNewtonCancelled:
			Fail(NewtonCancelled);
    	case kReadError:
			Fail(FailRead);
    }		
		
	// Read the filename to save the incoming data to
	fBufferResult = ReadBuffer( &bufferPtr, &length );
    switch (fBufferResult) {		
    	case kNewtonCancelled:
    		Fail(NewtonCancelled)						
    	case kReadError:
			Fail(FailRead);
    }
	
	// create and open the file, then start dumping data into it.
	anErr = CreateNOpenFile( bufferPtr, &fileReply, &fileRef );
	if ( anErr == noErr ) {
		fBufferResult = WriteCommand( kGoCommand );
		
		if (fBufferResult == kWriteError) {
			UpdateNCloseFile( fileRef, &fileReply );
			Fail(FailWrite);
		}
		
		// Loop until there is either an error, or until the Newton sends a cancel command or a finished command
		while( true ) {
			CDIdle( gOurPipe );
			fBufferResult = ReadBuffer( &bufferPtr, &length );
			
   			switch (fBufferResult) {
    			case kReadSuccess:
    				anErr = WriteToFile( fileRef, &length, bufferPtr );
    				
    				// if there was an error writing to the file, close the file, display an error and return.
    				if (anErr)
    					 Fail(FailWriteFile);
    				
    				
    				// send an kAckCommand, if there was an error then handle it.
    				fBufferResult = WriteCommand( kAckCommand );
    				if (fBufferResult == kWriteError) {
    					UpdateNCloseFile( fileRef, &fileReply );
    					Fail(FailWrite);
    				}
    				break;
    			case kNewtonCancelled:
    				UpdateNCloseFile( fileRef, &fileReply );
    				Fail(NewtonCancelled);
    			case kNewtonFinished:
    				ConductErrorDialog( kDownloadWasSuccessful );
    			    UpdateNCloseFile( fileRef, &fileReply );
					free( bufferPtr );
					bufferPtr = NULL;
					return;	
    			case kReadError:
    				UpdateNCloseFile( fileRef, &fileReply );
					Fail(FailRead);
  			}	
		}
	}
	
	// This code is only executed if the CreateNOpenFile failed to create and open a file.
	WriteCommand( kErrorCommand );
	free( bufferPtr );
	return;
	
	// These are the Goto locations that are jumped to using the Fail() macro.
	FailWrite:
	    WriteCommand( kErrorCommand );
		ConductErrorDialog( kBufferWriteErrorString );
		free( bufferPtr );
		return;
		
	FailRead:
	    WriteCommand( kErrorCommand );
		ConductErrorDialog( kBufferReadErrorString );
		free( bufferPtr );
		return;
		
	NewtonCancelled:
		ConductErrorDialog( kNewtonCancelledString );
		free( bufferPtr );
		return;
		
	FailWriteFile:
	    ConductErrorDialog( kFileWriteErrorString );
    	WriteCommand( kErrorCommand );
    	UpdateNCloseFile( fileRef, &fileReply );
    	free( bufferPtr );
		return;
		
	FailWrongApp:
	    ConductErrorDialog( kWrongAppString	 );
    	free( bufferPtr );
		return;
		
}	// DoProtocol


//-------------------------------------------------------------------------------

long DECL ReadBuffer( char** bufferPtr, long* length )
{
	Boolean 	eom;
	CommErr		anErr;
	long		command;

	// read the first four bytes, this will either be a command code or a string length
	*length = 4;
	anErr = CDPipeRead( gOurPipe, &command, length, &eom, kReadCommandSwapSize, kEncoding, kPipeTimeout, 0, 0 );
	if (anErr) 
		return kReadError;
	
	
	// interpret the command code and act on it.  If the data was not a command code, then it
	// is a string length, so read in the string
	if (command == kNewtonCancelled)
		return kNewtonCancelled;
	else if (command == kNewtonFinished)
		return kNewtonFinished;
	else if (command) {
		*length = command;
		
		if ((*bufferPtr))
			free( *bufferPtr );

		// allocate a buffer large enough for the data, plus one for the null character		
		if ( !(*bufferPtr = malloc( MALLOCCAST(2 * ((*length)+1) ))) ) {
			ConductErrorDialog( kNoMemoryString );
			return kReadError;
		}

		anErr = CDPipeRead( gOurPipe, *bufferPtr, length, &eom, kReadStringSwapSize, kEncoding, kPipeTimeout, 0, 0 );
		if (anErr)
			return kReadError;
				
		(*bufferPtr)[(*length)] = '\0';									// Null terminate the string
		return kReadSuccess;
	}
	
	return kReadError;
}	// ReadBuffer

// -------------------------------------------------------------------------------

long DECL WriteCommand( long command )
{
	CommErr			anErr;
	Boolean			eom = false;
	long			length = 4;

	// write out our command code
	anErr = CDPipeWrite( gOurPipe, &command, &length, eom, kWriteCommandSwapSize, kEncoding, kPipeTimeout, 0 ,0 );
	if (anErr) {
		return kWriteError;
	}
	
	return kWriteSuccess;
}	// WriteCommand

// -------------------------------------------------------------------------------

void DECL DisconnectPipe()
{
	CommErr 	anErr;
		
	anErr = CDPipeDisconnect( gOurPipe );
	anErr = CDDisposeCDILObject( gOurPipe );
	gOurPipe = nil;
	
	gPipeBusy = false;	
}	// DisconnectPipe

// -------------------------------------------------------------------------------

