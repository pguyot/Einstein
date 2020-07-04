/*
 *	File:		INTERFAC.c
 * 
 *	Contains:	Demonstration application for the CDIL
 *              This is the Windows version of INTERFAC.C - see Interface.c for Macintosh.
 *
 *	Written by:	Ryan Robertson
 * 
 *	Copyright:	© 1996 by Apple Computer, Inc.  All rights reserved.
 *
 *  Notes:
 *   This file contains all of the Windows specific code for the Mini-meta data application.
 *
 *   Nearly all of the DIL code can be found in PROTOCOL.c.  The only interesting
 *   DIL code in here can be found near the top - DoInitialize,
 *   and InitializePipe.
 *
 *   You may incorporate this sample code into your applications without
 *   restriction.  This sample code has been provided "AS IS" and the
 *   responsibility for its operation is 100% yours.  You are not
 *   permitted to modify and redistribute the source as "DTS Sample Code."
 *   If you are going to re-distribute the source, we require that you
 *   make it clear in the source that the code was descended from
 *   Apple-provided sample code, but that you've made changes.
 */ 

#include <Windows.h>   
#include <string.h>
#include <stdio.h>
#include <stdlib.h>        

#include "INTERFAC.h"
#include "Protocol.h"
#include "DILCPIPE.H"

//--------------------------------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//--------------------------------------------------------------------------------------------------------------

char 			szAppName [] = kApplicationName;
HANDLE FAR 		gXInst;
HWND			gWindow;
static HANDLE  	hInstance;
Boolean			gPipeBusy;				// tells whether we are in a connection/connect phase
CDILPipe		*gOurPipe;				// The pipe object

//--------------------------------------------------------------------------------------------------------------
// INTERFACE IMPLEMENTATION
//--------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------

void FAR PASCAL ConductErrorDialog( char* errorString )
{
	MessageBox( 0, errorString, kApplicationName, MB_ICONEXCLAMATION | MB_OK);
		
} // ConductErrorDialog

//--------------------------------------------------------------------------------------------------------------

LONG FAR PASCAL InitializePipe()
{
	CommErr				anErr;

	// create the pipe 
	gOurPipe = CDCreateCDILObject();
	if (!gOurPipe) {
		return kError_InitFailed;
	}

	CDSetApplication(gOurPipe, (short) gXInst);

	// Right now, the port is hard-coded.  It would probably be
	// good to let the user choose between com ports...
	anErr = CDPipeInit(gOurPipe, "MNP", "", "COM1:38400,n,8,1", "COM1", kDefaultBufferSize, kDefaultBufferSize);	
	if (anErr) {
		// if there was an error initializing the pipe then dispose of the CDIL object
		CDDisposeCDILObject( gOurPipe );
		gPipeBusy = false;
		
		return anErr;
	}
	
	return noErr;
}	// InitializePipe

//--------------------------------------------------------------------------------------------------------------


Boolean FAR PASCAL AboutDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
     switch (message)
          {
          case WM_INITDIALOG:
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                        case IDOK:
                        case IDCANCEL:
                           EndDialog (hDlg, 0) ;     
                           return TRUE ;
                    }
               break ;
          }
     return FALSE ;
} 	// AboutDlgProc

//--------------------------------------------------------------------------------------------------------------

// CreatNOpenFile will create and open a file to start dumping the data into
long FAR PASCAL CreateNOpenFile( char* fileName, Boolean* unused, HFILE* fileRefPtr )
{
	char*	winFileName = "\0\0\0\0\0\0\0\0\0\0\0\0";
	unsigned int i;

	// Turn the file name into a Windows 8.3 file name
	strncpy( winFileName, fileName, 8 );

	// strip all spaces from the filename and replace with an underscore
	for( i = 0; i < strlen(winFileName); i++ )
		if (winFileName[i] == ' ')
			winFileName[i] = '_';

	// finally, append the .TXT file type
	strcat( winFileName, ".TXT" );

	*fileRefPtr = _lcreat( winFileName, 0 );

	if ( *fileRefPtr == HFILE_ERROR )
		return true;

	return noErr;

}	// CreateNOpenFile

//--------------------------------------------------------------------------------------------------------------

long FAR PASCAL WriteToFile( HFILE fileRef, long* length, char* buffer )
{
	// need to add a line feed to the end of the line of text.  We must
	// also increment the buffer by one and add a new null terminator
	buffer[*length] = 0x0A;
	buffer[++(*length)] = 0;

	// return 1 (true) if _hwrite failed, otherwise return 0 (false)
	return _hwrite( fileRef, buffer, *length ) == -1;
}	// WriteToFile

//--------------------------------------------------------------------------------------------------------------

HFILE FAR PASCAL UpdateNCloseFile( HFILE fileRef, Boolean* unused )
{
 	return _lclose( fileRef );
}	// UpdateNCloseFile


//--------------------------------------------------------------------------------------------------------------

LONG FAR PASCAL DoTerminate()
{
	CommErr	anErr;
	
	anErr = CDDisposeCDIL();

	return anErr;
	
} // DoTerminate

//--------------------------------------------------------------------------------------------------------------

long FAR PASCAL  WndProc (HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
     static FARPROC lpfnAboutDlgProc;
     static FARPROC lpfnInputDlgProc;
     HMENU       	hMenu;
	 Boolean 		eom = false;
	 		 		
     switch (message)
          {
           case WM_CREATE:
               hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

               lpfnAboutDlgProc = MakeProcInstance ((FARPROC) AboutDlgProc,
                                                    hInstance) ;
 
               return 0 ;

         case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;

               switch (wParam)
                    {
                    case IDM_EXIT:
                        SendMessage (hwnd, WM_CLOSE, 0, 0L) ;
                        return 0 ;

					case IDM_CONNECT:
						if (gPipeBusy) {
							ConductErrorDialog( kAlreadyConnectedString );
							return 0;
						}

						gPipeBusy = true;
						DoConnection();
						return 0;

                    case IDM_ABOUT:
                        DialogBox (hInstance, "AboutBox", hwnd, lpfnAboutDlgProc) ;
                        return 0 ;
                    }
               break;

          case WM_DESTROY :
               PostQuitMessage(0);
               return 0;
          }
          
     return DefWindowProc (hwnd, message, wParam, lParam);
}	// WndProc

//--------------------------------------------------------------------------------------------------------------

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
{
     HWND     hwnd;
     MSG      msg;
     WNDCLASS wndclass;
	 CommErr  anErr;

     if (!hPrevInstance) {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, kApplicationName) ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
     }

     gXInst = hInstance;

     hwnd = CreateWindow (szAppName, kApplicationName,
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

	 gWindow = hwnd;

	// initialize the CDIL library
	anErr = CDInitCDIL() ;
	if (anErr) {
		ConductErrorDialog( kCDILInitErrorString );
		return msg.wParam;
	}
	 
	if (anErr == noErr)
		{
       		while (TRUE)
       		{
       			if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          		{   
          			if (msg.message == WM_QUIT)
          				break;
          		
          			TranslateMessage(&msg);
          			DispatchMessage(&msg);
          		}  
         		else
         		{	//	Perform idle processing
					if (gOurPipe)
						CDIdle(gOurPipe);
         		}
         	}

		DoTerminate();		
		}

    return msg.wParam;
	}

//--------------------------------------------------------------------------------------------------------------

