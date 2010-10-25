// AppSecureDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AppSecure.h"
#include "AppSecureDlg.h"
#include "AppProtector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define DEFAULT_PROTFILENAME "SecureFile.EXE"

#include <fstream.h>

UINT MonitorEvents( LPVOID pParam );


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
// CAppSecureDlg dialog

CAppSecureDlg::CAppSecureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAppSecureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAppSecureDlg)
	m_StatusString = _T("");
	m_Count = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sCard = NULL;
}

void CAppSecureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAppSecureDlg)
	DDX_Control(pDX, IDC_CONNECTBUTTON, m_ConnectButton);
	DDX_Control(pDX, IDC_BUTTON1, m_BrowseButton);
	DDX_Control(pDX, IDC_MAINFRAME, m_MainFrameCtrl);	
	DDX_Control(pDX, IDC_STATUSEDIT, m_StatusCtrl);	
	DDX_Control(pDX, IDC_EDIT1, m_CountCtrl);	
	DDX_Control(pDX, IDC_READERSCOMBO, m_ReaderComboCtrl);	
	DDX_Text(pDX, IDC_STATUSEDIT, m_StatusString);
	DDX_Control(pDX, IDC_CHECK1, m_cb_1);
	DDX_Text(pDX, IDC_EDIT1, m_Count);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAppSecureDlg, CDialog)
	//{{AFX_MSG_MAP(CAppSecureDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECTBUTTON, OnConnectbutton)
	ON_BN_CLICKED(IDC_BUTTON1, OnDumpAndEncrypt)
	ON_BN_CLICKED(IDC_CHECK1, OnCheckBoxClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAppSecureDlg message handlers

BOOL CAppSecureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_MonitorThread = AfxBeginThread(MonitorEvents, this);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	int iSize = 0;
	char *szReaders = NULL;
	try 
	{
		m_sIOP.ListReaders(NULL, iSize);
		szReaders = new char[iSize];
		m_sIOP.ListReaders(szReaders, iSize);
	}
	catch(...)
	{
		MessageBox("Unable to list readers.  Make sure that the Smart Card Resource Manager is running, and that your readers are plugged in.",
		 		   "SimpleString Sample", MB_OK);
		m_ReaderComboCtrl.EnableWindow(false);
		m_ConnectButton.EnableWindow(false);
		if (szReaders != NULL)
			delete [] szReaders;
		return FALSE;
	}

	char *szCurrent = szReaders;

	while (strlen(szCurrent) > 0)
	{
		m_ReaderComboCtrl.AddString(szCurrent);
		szCurrent += strlen(szCurrent) + 1;
	}

	m_ReaderComboCtrl.SetCurSel(0);
	m_ReaderComboCtrl.SetFocus();

	delete [] szReaders;	

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAppSecureDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAppSecureDlg::OnPaint() 
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
HCURSOR CAppSecureDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAppSecureDlg::OnConnectbutton() 
{
	UpdateData(TRUE);
	CString strConnect;
    m_ConnectButton.GetWindowText(strConnect);

	if (strConnect == "&Connect")  {
   		CString strReader;
		m_ReaderComboCtrl.GetWindowText(strReader);

		try	{
			m_sCard = m_sIOP.Connect(strReader, false);
			m_ConnectButton.SetWindowText("&Disconnect");				
			m_ReaderComboCtrl.EnableWindow(false);


			m_VisaOP = m_sCard->GetVisaOP();

			BYTE aid[] = {0x01,0x02,0x03,0x04,0x05,0x06}; 

			try {
				m_VisaOP->SelectAID(aid,6);
			}
			catch(scu::Exception const &e)
			{
				ReportError("SelectAID: ", e.Description());
				return;
			}

			m_BrowseButton.ShowWindow(SW_SHOW);
			m_MainFrameCtrl.ShowWindow(SW_SHOW);
			if(m_cb_1.GetCheck()==1) m_CountCtrl.ShowWindow(SW_SHOW);
			m_cb_1.ShowWindow(SW_SHOW);

			m_StatusString = "Connected and card applet selected!";
			
		}
		catch(scu::Exception const &e)     {
			ReportError("Unable to connect to card: ", e.Description());
			return;
		}

	bCLA = (byte)0x85;
	bINS = (byte)0x40;
	bP1  = (byte)0;
	bP2  = (byte)0;


			BYTE pinnumber[] = {0x00,0x01,0x09,0x05}; 
		
	    try
        {
		    m_sCard->SendCardAPDU(bCLA,bINS,bP1,bP2,4,pinnumber,NULL,NULL);
        }
        catch(CSmartCard::Exception const &e)
	    {
            WORD sw =  e.Status();
            if (sw == 0x6300)
				ReportError("SendCardAPDU: ", "Wrong PIN.");
			else
				ReportError("SendCardAPDU: ", e.Description());

            return;
        }
	
		m_StatusString = "PIN Validated...";
	}
	else {    
		DisconnectCard();
	}
	
	UpdateData(FALSE);
}

void CAppSecureDlg::ReportError(CString strCommand, CString strMessage) 
{
	m_StatusString = strCommand + strMessage;
	UpdateData(FALSE);
}

void CAppSecureDlg::DisconnectCard() 
{
	delete m_sCard;
	m_ConnectButton.SetWindowText("&Connect");	
	m_sCard = NULL;
	m_StatusString = "Disconnected!";
	m_StatusCtrl.SetWindowText(m_StatusString);
	
	m_BrowseButton.ShowWindow(SW_HIDE);
	m_MainFrameCtrl.ShowWindow(SW_HIDE);

	m_ReaderComboCtrl.EnableWindow(true);

	m_CountCtrl.ShowWindow(SW_HIDE);
	m_cb_1.ShowWindow(SW_HIDE);	
}

UINT MonitorEvents( LPVOID pParam )
{
	CAppSecureDlg  *mdlg = (CAppSecureDlg*)pParam;

	CComboBox *cb = (CComboBox*)mdlg->GetDlgItem(IDC_READERSCOMBO);
	
    char*        szReaderList = NULL;
    DWORD        dwReaderListSize = 0, dwNewState, dwOldState;
    HRESULT      hr;
	SCARDCONTEXT hContext;
	
	hr = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
	mdlg->m_hContext = hContext;
    hr = SCardListReaders(hContext, NULL, szReaderList, &dwReaderListSize);

    szReaderList = new char[dwReaderListSize];

    hr = SCardListReaders(hContext, NULL, szReaderList, &dwReaderListSize);

    if (SCARD_S_SUCCESS != hr)
    {
        delete [] szReaderList;
        
        return -1;
    }
    
    int         iCount    = 0;
    int         i         = 0;
    char*       pszList   = szReaderList;

    // count the number of readers in the list
    while(pszList[0] != '\0')
    {
        iCount++;
        pszList+=strlen(pszList) + 1;
    }

    // create the readerState array
    SCARD_READERSTATE *readerState = new SCARD_READERSTATE[iCount];
    
    // initialize the data
    pszList = szReaderList;
    while(pszList[0] != '\0')
    {
        readerState[i].szReader = pszList;
        readerState[i].dwCurrentState = SCARD_STATE_UNAWARE;
        readerState[i].dwEventState   = SCARD_STATE_UNKNOWN;
        
        // increment counter
        i++;
        pszList+=strlen(pszList) + 1;
    }

	AttachThreadInput(GetCurrentThreadId(),GetWindowThreadProcessId(mdlg->m_hWnd, NULL),TRUE);
	

    hr = SCardGetStatusChange(hContext, INFINITE, readerState, iCount);
	int iReaderNum = cb->GetCurSel();

    while (true)
    {
        if ((readerState[iReaderNum].dwEventState & SCARD_STATE_EMPTY) == SCARD_STATE_EMPTY)
        {
			// Card Removed
			mdlg->DisconnectCard();

        }
        else if ((readerState[iReaderNum].dwEventState & SCARD_STATE_PRESENT) == SCARD_STATE_PRESENT)
        {
			// Card Inserted

        }
		mdlg->InvalidateRect(NULL);
        bool fEvent = false;
    
        do
        {
            
            // Only interested in state changes which don't involve
            // attaching/detaching the card.
            DWORD dwStateMask = ~(SCARD_STATE_INUSE   |
                                SCARD_STATE_EXCLUSIVE |
                                SCARD_STATE_UNAWARE   |
                                SCARD_STATE_IGNORE    |
                                SCARD_STATE_CHANGED);
            for (int j = 0; j < cb->GetCount(); j++)
                readerState[j].dwCurrentState = readerState[j].dwEventState;
            

            try
            {
                hr = SCardGetStatusChange(hContext, INFINITE, readerState, iCount);
            }
            catch(...)
            {
                // don't care what the error is -- just leave
                hr = SCARD_E_CANCELLED;
            }

            // SCardCancel allows the loop to exit
            // any thing other than NOERROR makes this loop exit
            if (!(hr == NOERROR))
            {
                // this is the default exit route for this function

                // don't delete the individual reader names -- they are owned by szReaderList

                delete [] readerState;
                delete [] szReaderList;

                return 0;
            }
            
            for (j = 0; j < cb->GetCount() && !fEvent; j++)
            {
                dwNewState = readerState[j].dwEventState & dwStateMask;
                dwOldState = readerState[j].dwCurrentState & dwStateMask;
                if (dwNewState != dwOldState)
                {
                    fEvent = true;
                    iReaderNum = j;
					// if it isn't the currently selected reader, we don't care!
					if (iReaderNum != cb->GetCurSel())
						fEvent = false;

                }
            }
        } while (!fEvent);

    }

    // this won't ever get executed, but just in case
    delete [] readerState;
    delete [] szReaderList;

  
	return 0;
}

void CAppSecureDlg::OnDumpAndEncrypt() 
{
	UpdateData(TRUE);

	if(m_cb_1.GetCheck()==1){
		int iTemp;
		sscanf( m_Count, "%d", &iTemp );

   if(iTemp>255 || iTemp<1) {
		AfxMessageBox("Count must be between 1 and 255. Please change and try again." ,MB_OK|MB_ICONSTOP);
		return;
	}
}

	CString strLoadEXE,strEXEPath;
	
	CFileDialog dlg(TRUE,NULL,strLoadEXE,OFN_FILEMUSTEXIST,"Win32 Executable file(*.EXE)|*.EXE||",this);
	if(dlg.DoModal()==IDOK) 
	{
		strLoadEXE=dlg.GetFileName();
		strEXEPath=dlg.GetPathName();
	}
	else return; //User cancelled
	CAppProtector oAppprot(DEFAULT_PROTFILENAME,strEXEPath,this);	

	UpdateData(FALSE);
DisconnectCard();
}

void CAppSecureDlg::OnCheckBoxClick() 
{
	if(m_cb_1.GetCheck()==1) m_CountCtrl.ShowWindow(SW_SHOW);
	else m_CountCtrl.ShowWindow(SW_HIDE);
}