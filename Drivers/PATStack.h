// ==============================
// File:            PATStack.h
// Project:         Einstein
//
// Copyright 2003-2013 by Paul Guyot (pguyot@kallisys.net).
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// ==============================
// $Id$
// ==============================

#ifndef _PATSTACK_H
#define _PATSTACK_H

#include <Newton.h>
#include <Protocols.h>

///
/// P-Class for the AppleTalk stack.
///
/// \author Paul Guyot <pguyot@kallisys.net>
/// \version $Revision: 117 $
///
/// \test   aucun test défini.
///
PROTOCOL PATStack : public TProtocol
{
public:
    static  PATStack*       New( char* inImplementationName );  // PROTOCOL ctor
            void            Delete( void );                     // PROTOCOL dtor

    ULong       Init(TUPort*, unsigned long);
    ULong       HandleRequest(unsigned char&, TAEvent*, TUMsgToken*, unsigned long);
    ULong       HandleReply(unsigned long&, unsigned long, unsigned long);
    /// OSErr NBPLookup( ATNBPRecHandle abRecord )
    ULong       NBPLookupNames(unsigned char*, unsigned long, unsigned long, unsigned long);
    ULong       NBPLookupNamesReceived(void);
    ULong       NBPLookupNamesGetResult(unsigned long, TAddress*, unsigned char*, unsigned char*, unsigned char*);
    ULong       NBPLookupNamesCancel(void);
    ULong       NBPLookupNamesClose(void);

    ULong       NBPRegisterName(unsigned char, unsigned char*);
    ULong       NBPUnRegisterName(unsigned char*);

    /// int atp_open( at_socket* socket )
    /// OSErr ATPOpenSOcket( AddrBlock addrRcvd, short* aptSocket )
    ULong       ATPOpenSocket(unsigned char);
    /// int atp_close( int fd )
    /// OSErr ATPCloseSocket( short atpSocket );
    ULong       ATPCloseSocket(void);
    /// int atp_sendreq( int fd, at_inet_t* dest, char* buf, int len, int userdata, int xo, int xo_relt, u_short* tid, at_resp_t* resp, at_retry_t* retry, int nowait )
    /// OSErr PSendRequest( ATPPBPtr thePBPtr, Boolean async )
    ULong       ATPSendRequest(TAddress*, TMemoryObject*, TMemoryObject*, unsigned char, unsigned char, unsigned char, unsigned long, unsigned long, unsigned char);
    /// OSErr PSendResponse( ATPBPtr thePBPtr, Boolean async )
    ULong       ATPSendRequestResponses(unsigned long&);
    ULong       ATPSendRequestGetResult(unsigned char);
    ULong       ATPSendRequestCancel(void);
    /// int ddp_open( at_socket* socket )
    /// OSErr DDPOpenSocket( short* theSocket, sktListener )
    ULong       DDPOpenSocket(unsigned char*, unsigned long, unsigned long, unsigned long);
    /// int ddp_close( int fd )
    /// OSErr DDPCloseSocket( short theSocket )
    ULong       DDPCloseSocket(void);
    /// OSErr DDPWrite( ATDDPRecHandle abRecord, Boolean doChecksum, Boolean async )
    ULong       DDPSend(TAddress*, unsigned char, CBufferList*);
    /// OSErr DDPRead( ATDDPRecHandle abRecord, Boolean retCksumErrs, Boolean async )
    ULong       DDPReceive(CBufferList*);
    ULong       DDPReceiveGetResults(unsigned char*, unsigned long*, TAddress*);
    /// OSErr DDPRdCancel( ATDDPRecHandle abRecord )
    ULong       DDPReceiveCancel(void);
};

#endif
        // _PATSTACK_H

// ============================================================================== //
// The problems of business administration in general, and database management in //
// particular are much to difficult for people that think in IBMese, compounded   //
// with sloppy english.                                                           //
//                 -- Edsger Dijkstra                                             //
// ============================================================================== //
