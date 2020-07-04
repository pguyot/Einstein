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
**      Apple Computer, Inc.-provided sample code, but that you've made changes.
*/

// This file contains the implementation of the UI classes, and
// also has the C routines called from the cross-platform code.

#include "stdafx.h"
#include "SuiteP.h"
#include "SuitePDlg.h"
#include "SuitePX.h"

CDialog *gTheDialog;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuitePDlg dialog

CSuitePDlg::CSuitePDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSuitePDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSuitePDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSuitePDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSuitePDlg)
	DDX_Control(pDX, IDC_OUTPUT, m_outputBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSuitePDlg, CDialog)
	//{{AFX_MSG_MAP(CSuitePDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON8, OnButton8)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuitePDlg message handlers

BOOL CSuitePDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	
	gTheDialog = this;	// stash a pointer to ourselves for the routines called by the cross-platform code
	SendDlgItemMessage(IDC_RADIO1, BM_SETCHECK, TRUE, 0);	// default to the first port
	DoStartupWork();
	
	// *** There's probably a better way to do this in a dialog, but I can't find it right now...
	m_myTimer = SetTimer(0, 100, NULL);	// get a timer message 10 times a second, to call CD_Idle.
	
	return TRUE;
}

void CSuitePDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSuitePDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSuitePDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BOOL CSuitePDlg::DestroyWindow() 
{
	DoShutdownWork();
	KillTimer(m_myTimer);
	
	return CDialog::DestroyWindow();
}

void CSuitePDlg::OnTimer(UINT nIDEvent) 
{
	DoIdleWork();
	
	CDialog::OnTimer(nIDEvent);
}

void CSuitePDlg::OnButton1() { DoButtonHit(0); }
void CSuitePDlg::OnButton2() { DoButtonHit(1); }
void CSuitePDlg::OnButton3() { DoButtonHit(2); }
void CSuitePDlg::OnButton4() { DoButtonHit(3); }
void CSuitePDlg::OnButton5() { DoButtonHit(4); }
void CSuitePDlg::OnButton6() { DoButtonHit(5); }
void CSuitePDlg::OnButton7() { DoButtonHit(6); }
void CSuitePDlg::OnButton8() { DoButtonHit(7); }

// this sends the given text to the debug output always adds a linefeed at the end.
void Msg(LPSTR theMsg)
{
	gTheDialog->SendDlgItemMessage(IDC_OUTPUT, EM_REPLACESEL, 0, (LPARAM) theMsg);
	gTheDialog->SendDlgItemMessage(IDC_OUTPUT, EM_REPLACESEL, 0, (LPARAM) "\r\n");
}

// Msg2 works like Msg, but doesn't append a linefeed, and the caller specifies the buffer length
void Msg2(LPSTR theMsg, long amt)
{
#define maxMsg2BufSize 80
	char tmpbuf[maxMsg2BufSize];

	if (amt==-1)	// they want us to figure the length; it's null terminated.
		amt = strlen(theMsg);

	while (amt>0) {
		strncpy(tmpbuf, theMsg, min(maxMsg2BufSize,amt));
		tmpbuf[min(maxMsg2BufSize,amt)]=0;
		gTheDialog->SendDlgItemMessage(IDC_OUTPUT, EM_REPLACESEL, 0, (LPARAM) tmpbuf);
		amt=amt-maxMsg2BufSize;
	}
}

void SetStatusText(LPSTR theStatus)
{
	gTheDialog->SendDlgItemMessage(IDC_STATUS, WM_SETTEXT, 0, (LPARAM) theStatus);
}

void SetRadioBtnText(short index, LPSTR text)	// note we're assuming the buttons are sequential...
{
	if ((index>=0) && (index<4))
		if (text)
			gTheDialog->SendDlgItemMessage(IDC_RADIO1+index, WM_SETTEXT, 0, (LPARAM) text);
		else
			gTheDialog->GetDlgItem(IDC_RADIO1+index)->ShowWindow(0);	// hide if the caption is a null string
}

// find out which radio button is selected.
short GetSelectedRadioBtn()
{
	int i;
	
	// note we're assuming the buttons are sequential...
	for (i=0; i<4; i++)
		if (1 & gTheDialog->SendDlgItemMessage(IDC_RADIO1+i, BM_GETSTATE, 0, 0))
			return i;
	
	return 0;	// if something weird is going on, just say they selected the first one
}

void SetBytesAvail(long bytes)
{
	char bip[30];
	if (bytes)
		wsprintf(bip, "%ld", bytes);
	else
		bip[0]='\0';	// don't say "0 bytes"; use a blank string instead
	gTheDialog->SendDlgItemMessage(IDC_BIP, WM_SETTEXT, 0, (LPARAM) bip);
}

void ConfigureButton(short index, LPSTR text, short enabled)
{
	int dlgitem = index + IDC_BUTTON1;	// note we're assuming the buttons are sequential...
	
	if (text) {
		gTheDialog->SendDlgItemMessage(dlgitem, WM_SETTEXT, 0, (LPARAM) text);
		gTheDialog->GetDlgItem(dlgitem)->EnableWindow(enabled);
	} else
		gTheDialog->GetDlgItem(dlgitem)->ShowWindow(0);	// hide if the caption is a null string
	
}
