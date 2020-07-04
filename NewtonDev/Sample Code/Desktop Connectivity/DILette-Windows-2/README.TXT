/*
**      Newton Developer Technical Support Sample Code
**
**      DILette, minimal code to demonstrate use of the CDILs
**
**      by David Fedor, Newton Developer Technical Support
**
**      Copyright © 1996-1997 by Apple Computer, Inc.  All rights reserved.
**
**      You may incorporate this sample code into your applications without
**      restriction.  This sample code has been provided "AS IS" and the
**      responsibility for its operation is 100% yours.  You are not
**      permitted to modify and redistribute the source as "DTS Sample Code."
**      If you are going to re-distribute the source, we require that you
**      make it clear in the source that the code was descended from
**      Apple-provided sample code, but that you've made changes.
*/


This is the bare minimum code necessary to demonstrate use of the CDILs and
still run as a standalone application.  There is no UI at all in DILette.

DILette expects to use COM1 at 38400 baud and connect to a Newton device
which is running the SoupDrink package.  If you want to use a different
port or baud rate, change the parameters to CDPipeInit() in the source.

DILette will connect to the Newton device and send a command to the Newton
SoupDrink application so that SoupDrink will respond with "Hello".  DILette
then closes the connection.  If DILette believes everything worked properly,
it will display a "success" message and then exit.

If any errors occur, a message box will be displayed, telling which call
returned an error.

April 1997 - Modified to use the 1.0.2 DILs.