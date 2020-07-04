/*   
**      Newton Developer Technical Support Sample Code
**
**      SoupDrink, a sample of the Newton Desktop Integration Libraries (DILs)
**
**      by David Fedor, Rob Langhorne and J. Christopher Bell
**
**      Copyright © 1995-1996 by Apple Computer, Inc.  All rights reserved.
**
**      You may incorporate this sample code into your applications without
**      restriction.  This sample code has been provided "AS IS" and the
**      responsibility for its operation is 100% yours.  You are not
**      permitted to modify and redistribute the source as "DTS Sample Code."
**      If you are going to re-distribute the source, we require that you
**      make it clear in the source that the code was descended from
**      Apple-provided sample code, but that you've made changes.
*/
 
This is a complete Windows application which demonstrates use of the CDILs
and the FDILs.  It is designed to communicate with the SoupDrink-Newton
package, which is a separate project.

You can obtain the DIL libraries, headers and documentation from 
http://dev.info.apple.com/newton/tools/dils.html

Most of the interesting DIL code can be found in the file Engine.c, which
holds the cross-platform code which makes most of the DIL API calls.

The SoupDrink application has two test scripts.  The first, "Read Soup",
will read the entire contents of any soup on the Newton device.  It does
this by sending a command to the Newton device indicating the name of the
soup to read, and then repeatedly reading frames which are sent from the
Newton application, converting them to text, and writing them to the file
SoupData.out.

The second test script, "New Name", constructs a name card frame for the
built-in Names application, and sends it to the Newton device.  The Newton
SoupDrink application adds it to the Names soup.


Note that the SoupDrink application always uses COM1.  You should provide
an interface for the user to select which port is to be used.  If you are
sending large binary objects, or are experiencing transmission problems,
it may be necessary to lower the baud rate to 9600 baud.

Also note that there are two makefiles; one for Windows 3.1 (SOUP16.MAK)
and one for 32-bit versions of Windows (SOUPDRNK.MAK).  Both versions
should work under Windows 95.

There is only one difference between Windows code and MacOS code: after
calling CDPipeListen, you must wait for a short period to let the pipe
state change to kCDIL_ConnectPending.  There is an example of how to
perform this loop in engine.c.  It is #ifdef'd such that it will not
execute on a Macintosh, but it will not cause harm if it does run.



NOTE: The "SPIT" command is supported for compatibility with older versions of
this sample. If the SPIT command was sent instead of DRNK, a text version of
the entry (NOT a frame) was sent. This was relevant only to a bug in some
versions of Newton 1.x. See engine.c for more info.
