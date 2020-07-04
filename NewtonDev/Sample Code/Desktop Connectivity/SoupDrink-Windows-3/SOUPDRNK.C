/*     
 *	File:		SoupDrnk.C
 * 
 *	Contains:	Demonstration of the Newton Desktop Integration Libraries (DILs)
 *              Communication DILs (CDILs) and High Level Frames DILs (FDILs)
 *
 *              This is the Windows versin of SoupDrink.
 *              For the MacOS version of SoupDrink, see the file "SoupDrink.c"
 *
 *              Nearly all of the DIL code can be found in the file "engine.c".
 *              The only interesting DIL code in here is to initialize the
 *              DILs and allocate the pipe object
 * 
 *	Written by:	Rob Langhorne, J. Christopher Bell, and David Fedor
 * 
 *	Copyright:	© 1995-1996 by Apple Computer, Inc.  All rights reserved.
 *
 *  Disclaimer: This is nearly all Windows API code.  It is not intended
 *              to be an example of good Windows sample code, but it works to
 *              demonstrate and test how the Newton DILs work.
 *
 */ 

                               
#include <windows.h>   
#include <string.h>
#include <stdio.h>
#include <stdlib.h>        

#include "DILCPIPE.H"
#include "HLFDIL.h"
#include "SoupDrnk.h"
#include "Engine.h"

/*
**	Function Prototypes
*/
long FAR PASCAL  WndProc (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL  AboutDlgProc (HWND, UINT, UINT, LONG) ;
BOOL FAR PASCAL  InputDlgProc (HWND, UINT, UINT, LONG);

CommErr ReportError(CommErr fErr, char* mes);

/*
**	Global variables (some used by Engine.c)
*/
char szAppName [] = "SoupDrink" ;
char gMessageText [255];
HANDLE FAR gXInst;
HWND	gWindow;
static HANDLE  hInstance, inputInstance ;
FARPROC gInputProc;
int     gInputMode = 0;
char	gTempBuffer[1000];
char	gReceivedString[1000];
int		gReceivedStringLen;
void 	*ourPipe ;
void 	*gThisObject;

CommErr ReportError(CommErr fErr, char* mess)
{
	char  s[250];
	sprintf(s, "Error: %s ... err %d", mess, fErr);
	MessageBox (0, s, "SoupDrink", MB_ICONEXCLAMATION | MB_OK) ;
	
	return(fErr);                                                          
}

void PostAlertMessage(char*a, char*b, char*c, char*d)
{
	char s[2000];
	
	sprintf(s, "%s%s%s%s", a,b,c,d); 
	MessageBox (0, s, "SoupDrink", MB_ICONEXCLAMATION | MB_OK) ;
}

void inputDialog(char *soupname, char* defaultSoup, char* instruction) {
	if (DialogBoxParam (inputInstance, "IDD_INPUTDIALOG", NULL, gInputProc, (long) defaultSoup))
    	strcpy(soupname, gMessageText);
    else
    	*soupname = '\0';
}

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
     {
     HWND     hwnd ;
     MSG      msg ;
     WNDCLASS wndclass ;
	 CommErr  err;

     if (!hPrevInstance) 
          {
          wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
          wndclass.lpfnWndProc   = WndProc ;
          wndclass.cbClsExtra    = 0 ;
          wndclass.cbWndExtra    = 0 ;
          wndclass.hInstance     = hInstance ;
          wndclass.hIcon         = LoadIcon (hInstance, "SoupDrink") ;
          wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
          wndclass.hbrBackground = GetStockObject (WHITE_BRUSH) ;
          wndclass.lpszMenuName  = szAppName ;
          wndclass.lpszClassName = szAppName ;

          RegisterClass (&wndclass) ;
          }

   	 strcpy( gMessageText, "");		// clear out gMessageText
     gXInst = hInstance;

     hwnd = CreateWindow (szAppName, "SoupDrink",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          CW_USEDEFAULT, CW_USEDEFAULT,
                          NULL, NULL, hInstance, NULL) ;

     ShowWindow (hwnd, nCmdShow) ;
     UpdateWindow (hwnd) ;

	 gWindow = hwnd;

	 // Initialize the DILs
	 err = CDInitCDIL();
	 err = FDInitFDIL();

	 // Create a pipe
	 ourPipe = CDCreateCDILObject ();
	 CDSetApplication(ourPipe, (short) gXInst);

       while (TRUE)
       {
       		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
          	{   
          		if (msg.message == WM_QUIT)
          			break;
          		
          		TranslateMessage (&msg) ;
          		DispatchMessage (&msg) ;
          	}  
         	else
         	{			//	Perform idle processing
				if (ourPipe)
					CDIdle (ourPipe);
         	}
         }

	 //	Cleanup DIL
	if (ourPipe) {
		 err = CDPipeDisconnect(ourPipe);
		 CDDisposeCDILObject(ourPipe);
	}

	CDDisposeCDIL() ;

    return msg.wParam ;
    }

CommErr InitializePipe(void)
{
	CommErr err;

	// Right now, the port is hard-coded.  It would probably be
	// good to let the user choose between com ports...
	err = CDPipeInit(ourPipe, "MNP", "", "COM1:38400,n,8,1", "COM1", 1024, 1024);	

	if (err)
		return (int) ReportError(err, "CDPipeInit error");
}


long FAR PASCAL  WndProc (HWND hwnd, UINT message, UINT wParam, LONG lParam)
{
     static FARPROC lpfnAboutDlgProc ;
     static FARPROC lpfnInputDlgProc ;
     HMENU       	hMenu ;
	 Boolean 		eom = false;
	 CommErr		err;
	 		 		
     switch (message)
          {
           case WM_CREATE:
               hInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

               lpfnAboutDlgProc = MakeProcInstance ((FARPROC) AboutDlgProc,
                                                    hInstance) ;
 
               inputInstance = ((LPCREATESTRUCT) lParam)->hInstance ;

               lpfnInputDlgProc = MakeProcInstance ((FARPROC) InputDlgProc,
                                                    inputInstance) ;
			   gInputProc = lpfnInputDlgProc;
               return 0 ;

         case WM_COMMAND:
               hMenu = GetMenu (hwnd) ;

               switch (wParam)
                    {
                    case IDM_EXIT:
                         SendMessage (hwnd, WM_CLOSE, 0, 0L) ;
                         return 0 ;

                    case IDM_NEWNAME:         // Send a name card to the Newton device
						if (ourPipe==0)
							PostAlertMessage("Please make a connection first.","","","");
						else {
							err = UploadNewName();
							if (err)
								PostAlertMessage("An error was returned while creating a new name card.",ErrorStrings(err, gTempBuffer),"","");
                        }
                        return 0 ;  
                          
                    case IDM_READ:           // Read a soup from the Newton device
    					if (ourPipe) {   
							err = SoupDrink () ; // *** Do The Drinking!
							if (err)
								PostAlertMessage("An error was returned from that Soup Drink operation.", ErrorStrings(err, gTempBuffer), "", "");
      					}
      					break;

                    case IDM_ABOUT:
                          DialogBox (hInstance, "AboutBox", hwnd, lpfnAboutDlgProc) ;
                         return 0 ;
                    }
               break ;

          case WM_DESTROY :
               PostQuitMessage (0) ;
               return 0 ;
          }
     return DefWindowProc (hwnd, message, wParam, lParam) ;
}

BOOL FAR PASCAL AboutDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
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
}
 
BOOL FAR PASCAL  InputDlgProc (HWND hDlg, UINT message, UINT wParam, LONG lParam)
{
     switch (message)
          {
          case WM_INITDIALOG:
               SetDlgItemText(hDlg, IDC_EDIT1, (LPSTR)lParam);
               return TRUE ;

          case WM_COMMAND:
               switch (wParam)
                    {
                    case IDOK:
                    case IDCANCEL:
                         GetDlgItemText(hDlg, IDC_EDIT1, (LPSTR)&gMessageText, 255);
                         EndDialog (hDlg, (wParam==IDOK)) ;
                         return TRUE ;
                    }
               break ;
          }
     return FALSE ;
}
