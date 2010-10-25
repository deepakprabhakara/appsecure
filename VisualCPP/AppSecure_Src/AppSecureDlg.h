
#if !defined(AFX_APPSECUREDLG_H__A8197647_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_)
#define AFX_APPSECUREDLG_H__A8197647_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define BUFFER_SIZE  100
#define READER_SIZE  100

#include "iop.h"

using namespace iop;

/////////////////////////////////////////////////////////////////////////////
// CAppSecureDlg dialog

class CAppSecureDlg : public CDialog
{
// Construction
public:
	CAppSecureDlg(CWnd* pParent = NULL);	// standard constructor
	SCARDCONTEXT m_hContext;

	void DisconnectCard();

// Dialog Data
	//{{AFX_DATA(CAppSecureDlg)
	enum { IDD = IDD_APPSECURE_DIALOG };
	CEdit	m_StatusCtrl;
	CEdit   m_CountCtrl;
	CButton	m_ConnectButton;
	CButton	m_BrowseButton;
	CString	m_StatusString;
	CString m_ReaderCombo;
    CComboBox m_ReaderComboCtrl;
	CStatic m_MainFrameCtrl;
	CButton m_cb_1;
	CString	m_Count;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppSecureDlg)
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
	//{{AFX_MSG(CAppSecureDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnectbutton();
	afx_msg void OnDumpAndEncrypt();
	afx_msg void OnCheckBoxClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPSECUREDLG_H__A8197647_5E72_11D3_9CD9_00C04F5AD6B1__INCLUDED_)
