// Loader32Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "Loader32.h"
#include "Loader32Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <fstream.h>
#include <string.h>

#define APPSECURE_CLA 0x85

UINT MonitorEvents( LPVOID pParam );

//////////////////////////////
#include <imagehlp.h>
#pragma comment(lib,"imagehlp.lib")
//////////////////////////////

BOOL InitializeUs(DWORD*,DWORD*);
char szMyFileName[MAX_PATH+1];

//////////////////////This section needs to be changed program to program///////////////////////
#define CODE_DUMP "securekey.dat"
#define FILE_TO_LOAD "SecureFile.EXE"
#define nNumberOfRVAsToPatch 1
//////////////////////////////////////////////////////////////

struct PatchInfo
{
 char *RVA;//Relative virtual address
 unsigned long int nNumberOfPatches;//number of bytes to patch
 unsigned char *szPatchData;//If above bytes matched, these will be written over them.
 PatchInfo()
 {
	 RVA=NULL;
	 nNumberOfPatches=0;
	 szPatchData=NULL;
 }
 
};
//////////Patch Info data ///////////////////////
PatchInfo *PatchBytes=NULL;
STARTUPINFO startupinfo;
PROCESS_INFORMATION processinfo;
////////////////////////////////////
 void InitializeSP() {
	 memset(&startupinfo,0,sizeof(startupinfo));//fill startupinfo with NULL
	 startupinfo.cb = sizeof(startupinfo);
 }
/////////////////////////////////////
BOOL PatchProcess(PatchInfo*,int );
/////////////////////////////////////

BOOL LoadCrackedFile(LPCTSTR lpApplicationName,LPSTR  lpCmdLine)
{
	InitializeSP();
	BOOL bWhat=FALSE;
	CreateProcessA(lpApplicationName,lpCmdLine,0,0,0,CREATE_SUSPENDED,0,0,&startupinfo,&processinfo);//Create the thread as supended
	for(int i=0;i<nNumberOfRVAsToPatch;i++) bWhat=PatchProcess(PatchBytes,i);//process to be patched
	if(bWhat) ResumeThread(processinfo.hThread);
	else TerminateProcess(processinfo.hProcess,-1);
	return (bWhat);
}
  
BOOL PatchProcess(PatchInfo *PatchBytes,int index)
{
 BOOL ReturnVal=FALSE;
 DWORD BytesWritten=0;
 HANDLE ProcessHandle=OpenProcess(PROCESS_ALL_ACCESS,FALSE,processinfo.dwProcessId);
 if (ProcessHandle)
 {
		if(WriteProcessMemory(ProcessHandle,(PatchBytes+index)->RVA,(PatchBytes+index)->szPatchData,(PatchBytes+index)->nNumberOfPatches,&BytesWritten)) ReturnVal = TRUE;
	
  CloseHandle(ProcessHandle);
 }
 return ReturnVal;
}

BOOL InitializeUs(DWORD *nEntryPoint,DWORD *nPreferredLoadAddress)
{
	LOADED_IMAGE li;
	if (MapAndLoad(szMyFileName,0,&li,FALSE,TRUE)) 
	{
		*nEntryPoint=li.FileHeader->OptionalHeader.AddressOfEntryPoint;
		*nPreferredLoadAddress=li.FileHeader->OptionalHeader.ImageBase;
	}
    else
    {
        return FALSE;
    }
	UnMapAndLoad( &li );
	return TRUE;
}

CString CLoader32Dlg::AskTheUserToGetTheFile(LPCTSTR strLoadEXE)
{
		//If the read from registry was unsucessfull or the user likes to do things himself
		CString sFilter="Win32 Executable(*.EXE)|";
		sFilter+=FILE_TO_LOAD;
		sFilter+="||";
		CFileDialog dlg(TRUE,NULL,strLoadEXE,OFN_FILEMUSTEXIST,(LPCTSTR)sFilter,this);
		if(dlg.DoModal()==IDOK) return CString(dlg.GetPathName());
		return CString("");
}

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
// CLoader32Dlg dialog

CLoader32Dlg::CLoader32Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoader32Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoader32Dlg)
	m_StatusString = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_sCard = NULL;
}

void CLoader32Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoader32Dlg)
	DDX_Control(pDX, IDC_CONNECTBUTTON, m_ConnectButton);
	DDX_Control(pDX, IDC_STATUSEDIT, m_StatusCtrl);	
	DDX_Control(pDX, IDC_READERSCOMBO, m_ReaderComboCtrl);	
	DDX_Text(pDX, IDC_STATUSEDIT, m_StatusString);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLoader32Dlg, CDialog)
	//{{AFX_MSG_MAP(CLoader32Dlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECTBUTTON, OnConnectbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoader32Dlg message handlers

BOOL CLoader32Dlg::OnInitDialog()
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

void CLoader32Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLoader32Dlg::OnPaint() 
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
HCURSOR CLoader32Dlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLoader32Dlg::OnConnectbutton() 
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

	OnLoad();
	}
	else {    
		DisconnectCard();
	}

	UpdateData(FALSE);
}

void CLoader32Dlg::ReportError(CString strCommand, CString strMessage) 
{
	m_StatusString = strCommand + strMessage;
	UpdateData(FALSE);
}

void CLoader32Dlg::DisconnectCard() 
{
	delete m_sCard;
	m_ConnectButton.SetWindowText("&Connect");	
	m_sCard = NULL;
	m_StatusString = "Disconnected!";
	m_StatusCtrl.SetWindowText(m_StatusString);
	m_ReaderComboCtrl.EnableWindow(true);

}

UINT MonitorEvents( LPVOID pParam )
{
	CLoader32Dlg  *mdlg = (CLoader32Dlg*)pParam;

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

void CLoader32Dlg::OnLoad() 
{
	DWORD dwSize=NULL,dwByteCount=NULL;
	/*
	First of all, let's set the Current Directory to our own and NOT of the calling process.
	Else ReadFile() etc will not find the associated files if we call this program form anywhere else
	*/
	char *lpszCurrentDirectory=new char[MAX_PATH+1]; //We will be needing this once only
	GetModuleFileName(NULL,lpszCurrentDirectory,MAX_PATH);
	*(strrchr(lpszCurrentDirectory,int('\\')))='\0';
	SetCurrentDirectory(lpszCurrentDirectory); //Check the returned BOOL value
	delete lpszCurrentDirectory;
	/*	Done setting the current directory to our very comfortable abode ;)	*/

	//////////////////////////Open file contaning section data///////////////////
	HANDLE hFile=CreateFile(CODE_DUMP,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE) {
		MessageBox("\nEncryption key file is missing ?");
		return;
	}
	dwSize=GetFileSize(hFile,NULL);
	//Create a dynamic array
	PatchBytes=new PatchInfo[1];
	PatchBytes->szPatchData=new unsigned char[dwSize+100];

	//Send APDU to card

	UpdateData(TRUE);

	bCLA = (byte)APPSECURE_CLA;
	bINS = (byte)0x20;
	bP1  = (byte)0;
	bP2  = (byte)0;

                try {
				    m_sCard->SendCardAPDU(bCLA,bINS,bP1,bP2,NULL,NULL,100,PatchBytes->szPatchData);
                }
                catch(CSmartCard::Exception const &e)
	            {
            WORD sw =  e.Status();
            if (sw == 0x6910)
				ReportError("SendCardAPDU: ", "License Expired");
			else
				ReportError("SendCardAPDU: ", e.Description());                
                return;
                }

				m_StatusString = "Code section read from card.";
					
	UpdateData(FALSE);

//End APDU

	if(PatchBytes==NULL&&PatchBytes->szPatchData==NULL) 
	{
		MessageBox("\nFailed to create dynamic Memory buffer ! Exiting ....");
		exit(0);
	}
	//Fill the array up with CODE_DUMP contents and initialize 
	ReadFile(hFile,PatchBytes->szPatchData + 100,dwSize,&dwByteCount,NULL);//szPatchData now contains encrypted data

PatchBytes->nNumberOfPatches=dwSize+100;

	CloseHandle(hFile);


	DWORD nEntryPoint=-1,nPreferredLoadAddress=-1;

	CString strLoadEXE="SecureFile.EXE";
	////////////////////////////////////////////////////
	
	hFile=CreateFile(FILE_TO_LOAD,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if(hFile==INVALID_HANDLE_VALUE) {
		MessageBox("\nThere was an error while trying to load the program.\nError during file validation.","Please browse for the file",MB_OK|MB_ICONINFORMATION);
		strLoadEXE=AskTheUserToGetTheFile((LPCTSTR)strLoadEXE);

		hFile=CreateFile(strLoadEXE,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if(hFile==INVALID_HANDLE_VALUE) {
		MessageBox("\nThere was an error again while trying to load the program.\nError during file validation.","Skipping. Please try again",MB_OK|MB_ICONINFORMATION);
		return;
	}

	}
	CloseHandle(hFile);

	//////////////////////////////////////////////////////
	strcpy(szMyFileName,strLoadEXE);
	InitializeUs(&nEntryPoint,&nPreferredLoadAddress);
	///////////////////////////////////////////////////////
	PatchBytes->RVA=(char *)(nEntryPoint+nPreferredLoadAddress);//AddressOfEntryPoint of program
	char szMsg[512];
	if(dwByteCount!=dwSize) 
	{
		wsprintf(szMsg,"\n\nThere was an error reading the file to memory !\n(Bytes read = %d Bytes to be read = %d)\n",dwByteCount,dwSize);
		MessageBox(szMsg);
		return;
	}
	//Call the loader
	if(!LoadCrackedFile((LPCTSTR)strLoadEXE,"")) MessageBox("\nSorry ! Program could not be patched! Will terminate thread.");
    //Clean up our dynamic array
	delete[] PatchBytes->szPatchData;
	delete[] PatchBytes;
	exit(0);//
}
