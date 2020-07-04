/*
	File:		Protocol.h

	Contains:	Definitions for Protocol.c

	Written by:	Ryan Robertson

	Copyright:	© 1996 by Apple Computer, Inc.  All rights reserved.

*/

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#ifdef forMac

#define kEncoding				0
#define kWriteCommandSwapSize	0
#define kReadCommandSwapSize	0
#define kReadStringSwapSize		0
#define DECL
 
#else

#include <windows.h>
#define kTimeoutInSecs			30
#define kEncoding				0
#define kWriteCommandSwapSize	4
#define kReadCommandSwapSize	4
#define kReadStringSwapSize		1
#define DECL FAR PASCAL

#endif

#include "DILCPIPE.h" 

//--------------------------------------------------------------------------------------------------------------
// COMMUNICATIONS DEFINITIONS
//--------------------------------------------------------------------------------------------------------------

#define kPipeTimeout			0

#define kHeloResponse			"Mini-MetaData\0"

#define kNewtonCancelled		0x0FFF	
#define kNewtonFinished			0x0FFE	

#define kHelloCommand			0x0FFD
#define kGoCommand				0x0FFC
#define kAckCommand				0x0FFB
#define kErrorCommand			0x0FF9

#define	kBufferEmpty			0x0FF8
#define kReadError				0x0FF7
#define kReadSuccess			0x0FF6
#define kWriteError				0x0FF5
#define kWriteSuccess			0x0FF4

//--------------------------------------------------------------------------------------------------------------
// INTERFACE PROTOTYPES
//--------------------------------------------------------------------------------------------------------------

void DECL			ListenCallback( CommErr errorValue, long refCon );
void DECL			DoConnection(void);
long DECL			ReadBuffer( char** bufferPtr, long* length );
long DECL			WriteCommand( long code );
void DECL			DoProtocol();
void DECL			DisconnectPipe();


#endif
