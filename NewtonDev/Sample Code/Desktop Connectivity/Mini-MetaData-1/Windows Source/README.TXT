/*
**      Newton Developer Technical Support Sample Code
**
**      Mini-MetaData, code to demonstrate use of the CDIL
**
**      by Ryan Robertson, Newton Developer Technical Support
**
**      Copyright © 1996 by Apple Computer, Inc.  All rights reserved.
**
**      You may incorporate this sample code into your applications without
**      restriction.  This sample code has been provided "AS IS" and the
**      responsibility for its operation is 100% yours.  You are not
**      permitted to modify and redistribute the source as "DTS Sample Code."
**      If you are going to re-distribute the source, we require that you
**      make it clear in the source that the code was descended from
**      Apple-provided sample code, but that you've made changes.
*/


This is a complete Windows application which demonstrates use of the CDIL.  It
is designed to communicate with the Newton Mini-MetaData package (metadata.pkg),
which is included with this sample code.

Most of the interesting DIL code can be found in the file Protocol.c. This file
holds the cross-platform code which makes most of the DIL API calls.

Note that the Mini-MetaData application always uses COM1.  You should provide
an interface for the user to select which port is to be used.

Also note that there are two makefiles; one for Windows 3.1 (MMD16.MAK)
and one for 32-bit versions of Windows (MMD32.MAK).  Both versions
should work under Windows 95.

There is only one major difference between Windows code and MacOS code: 
after calling CDPipeListen, you must wait for a short period to let the
pipe state change to kCDIL_ConnectPending.  There is an example of how to
perform this loop in Protocol.c.  It is #ifdef'd such that it will not
execute on a Macintosh, but it will not cause harm if it does run.

See the Newton Technology Journal article "Mini-MetaData" for an in-depth
discussion of how the sample works.
