// SimpleStringDlg.h : header file
//

#if !defined(AFX_LOADER32DLG_H__A8197647_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_)
#define AFX_LOADER32DLG_H__A8197647_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUFFER_SIZE  100
#define READER_SIZE  100

#include "iop.h"

using namespace iop;

/////////////////////////////////////////////////////////////////////////////
// CLoader32Dlg dialog

class CLoader32Dlg : public CDialog
{
// Construction
public:
	CLoader32Dlg(CWnd* pParent = NULL);	// standard constructor
	SCARDCONTEXT m_hContext;
	CString AskTheUserToGetTheFile(LPCTSTR strLoadEXE=NULL); //Our own little member function

	void DisconnectCard();

// Dialog Data
	//{{AFX_DATA(CLoader32Dlg)
	enum { IDD = IDD_APPSECURE_DIALOG };
	CEdit	m_StatusCtrl;
	CButton	m_ConnectButton;
	CString	m_StatusString;
	CString m_ReaderCombo;
    CComboBox m_ReaderComboCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoader32Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	HICON m_hIcon;
	CWinThread *m_MonitorThread;

	CIOP m_sIOP;
	CVisaOP *m_VisaOP; 
	CSmartCard *m_sCard;
	WORD SW;
	BYTE bCLA;
	BYTE bINS;
	BYTE bP1;
	BYTE bP2;
	BYTE bDataBuffer[BUFFER_SIZE];

	void ReportError(CString, CString);
	// Generated message map functions
	//{{AFX_MSG(CLoader32Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnectbutton();
	afx_msg void OnLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADERDLG_H__A8197647_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_)
