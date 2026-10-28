/*
	File:		Template.impl.h

	Contains:	Latern driver implementation header for PCMCIA Template Ethernet card

				You may incorporate this sample code into your applications without
				restriction.  This sample code has been provided "AS IS" and the
				responsibility for its operation is 100% yours.  You are not
				permitted to modify and redistribute the source as "DDK Sample Code."
				If you are going to re-distribute the source, we require that you
				make it clear in the source that the code was descended from
				Apple-provided sample code, but that you've made changes.

	Copyright:	� 1997 by Apple Computer, Inc., all rights reserved.

	Obtained from NIE2.0 F1C2 build. 9/4/97
*/


#ifndef __TEMPLATE_IMPL_H
#define __TEMPLATE_IMPL_H


// The Lantern driver protocols, declared for gcc (Apple's LanternDriver.proto.h
// and LanternDriverAPI.proto.h are not part of the public NCT headers).
#include "LanternDriver.h"

#ifndef __BUFFERLIST_H
#include "BufferList.h"
#endif

#ifndef __NEWTONTIME_H
#include "NewtonTime.h"
#endif



#define kPacketAddrSize 					(6)
#define kTimerTime							(100)				// Milliseconds
#define kMinTimerTime						(10)				// Shortest tick Einstein may ask for
#define kMaxTimerTime						(1000)				// Longest tick Einstein may ask for
#define kMaxRxPerTick						(16)				// Packets handed to Lantern per timer tick
//
//	TTemplateDriver	Template Ethernet Driver driver
//

PROTOCOL TNE2000Sample : public TLanternDriver					// Protocol class
	PROTOCOLVERSION(1.0)										// Protocol class version
{
	public:

		//	Protocol Class Interface
		
		PROTOCOL_IMPL_HEADER_MACRO(TNE2000Sample);				// Magic stuff, don't touch (for ProtocolGen)

		TLanternDriver*	New(void);								// Protocol class constructor, called when the card is inserted
		void			Delete();								// Protocol class destructor, called when the card is removed

		//	Task services

		NewtonErr		Init(void);								// Driver initialization 
		NewtonErr		Enable();								// Start/resume card operations (startup/newton power on)
		NewtonErr		Disable();								// Stop/suspend card operations (shutdown/newton power off)
		void			InterruptHandler();						// Called to handle a card interrupt

	private:

		//	Client Services (from event handlers of TLanternDriverAPI)

		void			SendBuffer(Ptr thePacket, Size size);	// Transmit a buffer
		void			SendCBufferList(CBufferList* thePacket);// Transmit a CBufferList
		void			GetDeviceAddress(UByte* addr, Size size); // Return the device address
		void			AddMulticastAddress(UByte* addr);		// Add a multicast address
		void			DelMulticastAddress(UByte* addr);		// Delete a multicast address
		void			GetLinkIntegrity(void);					// Get the link integrity flag
	
		// Optional services
		
		void 			SetPromiscuous(ULong promiscuous);		// Set to receive in promiscous mode
		void 			GetThroughput(void);					// Get a throughput statistic
		
		// Private Template Services

		void			TimerExpired(void);						// Called when driver timer expires
		void			ReceivePackets(void);					// Hand waiting packets to Lantern
		
		//	Template routines

		//	Internal variables
		
		ULong			fStatus;								// Internal status

		UChar			fEthernetAddr[kPacketAddrSize];			// Ethernet address
		Boolean			fLinkIntegrity;							// Link integrity staus
		Boolean			fTimerPosted;							// Is a timer event posted.
		TTime*			fTickTime;								// TIme for the next timer event
		
	  #ifdef forDebug

		void			DumpBuffer(UChar* buffer, ULong size, ULong startingAddr);	// Dump buffer data

		EventTraceCauseDesc*	fEventTraceCauseDescs;			// Event trace cause descriptors
		TEventCollector*		fEventTraceCollector;			// Event trace collector

	  #endif
};



//
//	Internal status definitions
//

enum kStatus
{
	kStatusNoStatus						=	0x00000000,			// No status yet
	kStatusCardPowerOn					=	0x00000001,			// Card power is truned on by us (need to turn off later)
};


#endif



//==========================================================================================


//==========================================================================================
