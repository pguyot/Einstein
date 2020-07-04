/*
**      Newton Developer Technical Support Sample Code
**
**      Suite P, code to demonstrate use of the 2.0 DILs and provide basic Windows & Mac UI
**
**      by David Fedor, Newton Developer Technical Support
**
**      Copyright © 1997 by Apple Computer, Inc All rights reserved.
**
**      You may incorporate this sample code into your applications without
**      restriction.  This sample code has been provided "AS IS" and the
**      responsibility for its operation is 100% yours.  You are not
**      permitted to modify and redistribute the source as "DTS Sample Code."
**      If you are going to re-distribute the source, we require that you
**      make it clear in the source that the code was descended from
**      Apple-provided sample code, but that you've made changes.
*/

// This file contains the definition of the Dialog class

/////////////////////////////////////////////////////////////////////////////
// CSuitePDlg dialog

class CSuitePDlg : public CDialog
{
// Construction
public:
	CSuitePDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSuitePDlg)
	enum { IDD = IDD_SUITEP_DIALOG };
	CEdit	m_outputBox;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSuitePDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	UINT m_myTimer;

	// Generated message map functions
	//{{AFX_MSG(CSuitePDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnButton8();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
