/*
**      Newton Developer Technical Support Sample Code
**
**      Suite P, code to demonstrate use of the 2.0 DILs and provide basic Windows & Mac UI
**
**      by David Fedor, Newton Developer Technical Support
**
**      Copyright © 1997 by Apple Computer, Inc.  All rights reserved.
**
**      You may incorporate this sample code into your applications without
**      restriction.  This sample code has been provided "AS IS" and the
**      responsibility for its operation is 100% yours.  You are not
**      permitted to modify and redistribute the source as "DTS Sample Code."
**      If you are going to re-distribute the source, we require that you
**      make it clear in the source that the code was descended from
**      Apple Computer Inc.-provided sample code, but that you've made changes.
*/

// This file defines the application class.  Not much here.

#include "stdafx.h"
#include "SuiteP.h"
#include "SuitePDlg.h"
#include "SuitePX.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSuitePApp

BEGIN_MESSAGE_MAP(CSuitePApp, CWinApp)
	//{{AFX_MSG_MAP(CSuitePApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuitePApp construction

CSuitePApp::CSuitePApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSuitePApp object

CSuitePApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSuitePApp initialization

BOOL CSuitePApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CSuitePDlg dlg;
	m_pMainWnd = &dlg;
	
	int nResponse = dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CSuitePApp::OnIdle(LONG lCount) 
{
	DoIdleWork();
	
	return CWinApp::OnIdle(lCount);
}
