/*
**      Newton Developer Technical Support Sample Code
**
**      Suite P, code to demonstrate use of the 2.0 DILs and provide basic Windows & Mac UI
**
**      by David Fedor, Newton Developer Technical Support
**
**      Copyright © 1997 by Newton, Inc.  All rights reserved.
**
**      You may incorporate this sample code into your applications without
**      restriction.  This sample code has been provided "AS IS" and the
**      responsibility for its operation is 100% yours.  You are not
**      permitted to modify and redistribute the source as "DTS Sample Code."
**      If you are going to re-distribute the source, we require that you
**      make it clear in the source that the code was descended from
**      Newton, Inc.-provided sample code, but that you've made changes.
*/

// This file contains the cross-platform definitions for SuiteP

#include "CDIL.h"
#include "PDIL.h"

#ifdef forMacOS
typedef char * LPSTR;
#endif

// calls from the platform-specific code to the cross-platform code
void DoStartupWork();
void DoShutdownWork();
void DoIdleWork();
void DoButtonHit(short whichBtn);

// calls from the cross-platform code to the platform-specific code
void Msg(LPSTR theMsg);                 // sends a message to the debugging listbox
void Msg2(LPSTR theMsg, long amt);      // similar, but doesn't add a newline at the end
void SetStatusText(LPSTR theStatus);    // sets the pipe status text
void SetBytesAvail(long bytes);         // shows how many bytes are waiting to be read
void ConfigureButton(short index, LPSTR text, short enabled);
void SetRadioBtnText(short index, LPSTR text);
short GetSelectedRadioBtn();            // which radio button is selected?

// calls within the cross-platform code
void DoAnAction();	// called by the idle routine when work needs to be done

