// =========== Header ===========
// File:                PAppleTalkStack.h
// Project:             Newton Includes
// Written by:          Paul Guyot (pguyot@pnm-consulting.com)
//
// Created on:          05/29/2000
// Internal version:    1
//
// Copyright:           © 2000 by Paul Guyot, P&M Consulting.
//                      All rights reserved worldwide.
// ===========

// =========== Change History ===========
// 05/29/2000   v1  [PG]    Creation of the file
// ===========

#ifndef __PAPPLETALKSTACK__
#define __PAPPLETALKSTACK__

// This file has been automatically generated from Hammer debugging information.
// Class definition for PAppleTalkStack

class PAppleTalkStack {
public:

    UndefRes    ATPCloseSocket(void);
    UndefRes    ATPOpenSocket(unsigned char);
    UndefRes    ATPSendRequestCancel(void);
    UndefRes    ATPSendRequestGetResult(unsigned char);
    UndefRes    ATPSendRequestResponses(unsigned long&);
    UndefRes    ATPSendRequest(TAddress*, TMemoryObject*, TMemoryObject*, unsigned char, unsigned char, unsigned char, unsigned long, unsigned long, unsigned char);
    UndefRes    DDPCloseSocket(void);
    UndefRes    DDPOpenSocket(unsigned char*, unsigned long, unsigned long, unsigned long);
    UndefRes    DDPReceiveCancel(void);
    UndefRes    DDPReceiveGetResults(unsigned char*, unsigned long*, TAddress*);
    UndefRes    DDPReceive(CBufferList*);
    UndefRes    DDPSend(TAddress*, unsigned char, CBufferList*);
    UndefRes    Delete(void);
    UndefRes    HandleReply(unsigned long&, unsigned long, unsigned long);
    UndefRes    HandleRequest(unsigned char&, TAEvent*, TUMsgToken*, unsigned long);
    UndefRes    Init(TUPort*, unsigned long);
    UndefRes    NBPLookupNamesCancel(void);
    UndefRes    NBPLookupNamesClose(void);
    UndefRes    NBPLookupNamesGetResult(unsigned long, TAddress*, unsigned char*, unsigned char*, unsigned char*);
    UndefRes    NBPLookupNamesReceived(void);
    UndefRes    NBPLookupNames(unsigned char*, unsigned long, unsigned long, unsigned long);
    UndefRes    NBPRegisterName(unsigned char, unsigned char*);
    UndefRes    NBPUnRegisterName(unsigned char*);
    UndefRes    New(void);
}; // PAppleTalkStack

#endif
        // __PAPPLETALKSTACK__

// ============================================================================== //
// Dear Emily:                                                                    //
//         I recently read an article that said, "reply by mail, I'll summarize." //
// What should I do?                                                              //
//                 -- Doubtful                                                    //
//                                                                                //
// Dear Doubtful:                                                                 //
//         Post your response to the whole net.  That request applies only to     //
// dumb people who don't have something interesting to say.  Your postings are    //
// much more worthwhile than other people's, so it would be a waste to reply by   //
// mail.                                                                          //
//                 -- Emily Postnews Answers Your Questions on Netiquette         //
// ============================================================================== //
