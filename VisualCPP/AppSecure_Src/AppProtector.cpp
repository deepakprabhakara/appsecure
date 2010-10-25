#include "stdafx.h"
#include "AppSecure.h"
#include "AppProtector.h"
#include "AppSecureDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include <fstream.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <imagehlp.h>//include Imagehlp.lib into project
#pragma comment(lib,"imagehlp.lib")


#define DEFAULT_PROTFILENAME "SecureFile.EXE"
#define APPSECURE_CLA 0x85
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CAppProtector::CAppProtector()
{
	szProtectedFileName=NULL;
	szMyFileName=NULL;
	ui=NULL;
}


CAppProtector::CAppProtector(const char *lpszProtectedFileName,const char *lpszOriginalPEImageName, CAppSecureDlg* t)
{
	ui=t;
	/*
	lpszProtectedFileName is the output filename, which is "SecureFile.EXE" by default
	lpszOriginalPEImageName is the PE File to protect. This file will NOT be modified in anyway.
	*/
	szProtectedFileName=new char[lstrlen(lpszProtectedFileName)+1];
	lstrcpy(szProtectedFileName,lpszProtectedFileName);
	szMyFileName=new char[lstrlen(lpszOriginalPEImageName)+1];
	lstrcpy(szMyFileName,lpszOriginalPEImageName);
	/////////////////////////////////////////////////////////////////////////
	LOADED_IMAGE li;
	if(MapAndLoad(szMyFileName,0,&li,FALSE,TRUE)) RVAToFileOffset(li.FileHeader,li.FileHeader->OptionalHeader.AddressOfEntryPoint,li.FileHeader->FileHeader.NumberOfSections);
    else
    {
        AfxMessageBox("Load Failed !");
        exit(1);
    }
	UnMapAndLoad(&li);
	/////////////////////////////////////////////////////////////////////////
}

CAppProtector::~CAppProtector()
{
	delete[] szMyFileName;
	szMyFileName=NULL;
	delete[] szProtectedFileName;
	szProtectedFileName=NULL;
	ui=NULL;
}

DWORD CAppProtector::RVAToFileOffset(PIMAGE_NT_HEADERS pNtHdr, DWORD rva, unsigned int nNumberOfSections)
{
   HANDLE hFile=CreateFile(szMyFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,(HANDLE)0);
   HANDLE hFileMapping=CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);
   PVOID pMemoryMappedFileBase=(PCHAR)MapViewOfFile(hFileMapping,FILE_MAP_READ,0,0,0);
	
	if(pMemoryMappedFileBase==0)
	{
		//Error
		CloseHandle(hFileMapping);
		hFileMapping=0;
		CloseHandle(hFile);
		hFile=INVALID_HANDLE_VALUE;
		return ((DWORD)-1);
	}
     
    PIMAGE_SECTION_HEADER pSectHdr=IMAGE_FIRST_SECTION(pNtHdr);
	DWORD cbMaxOnDisk=0,startSectRVA=0,endSectRVA=0,dwResult=(DWORD)-1;
     
    for(unsigned i=0;i<nNumberOfSections;i++,pSectHdr++)
    {
        cbMaxOnDisk=min(pSectHdr->Misc.VirtualSize,pSectHdr->SizeOfRawData);
        startSectRVA=pSectHdr->VirtualAddress;
        endSectRVA=startSectRVA + cbMaxOnDisk;
         
        if((rva>=startSectRVA)&&(rva<endSectRVA)) dwResult=pSectHdr->PointerToRawData+(rva-startSectRVA);
    }
     	
	CloseHandle(hFileMapping);
    CloseHandle(hFile);
	if(dwResult<0) MessageBox(NULL,"Error calculating file offset (what type of PE file is this ?)","Error",MB_OK|MB_ICONQUESTION);
	unsigned int nNumberOfBytesToRead=((pSectHdr-nNumberOfSections)->Misc.VirtualSize-(rva-(pSectHdr-nNumberOfSections)->VirtualAddress));//We will be reading (rva-start of section) bytes after start of .text offset. We are reading VirtualSize bytes of data because that's really the size of actual code before padding it up by the linker
	 
	if(DumpSection(dwResult,nNumberOfBytesToRead,rva)) MessageBox(NULL,"Your application has been AppSecured","Done !",MB_OK|MB_ICONINFORMATION);;
    return dwResult;
}

BOOL CAppProtector::DumpSection(DWORD nFileOffset, DWORD dwNumberOfBytesToRW, DWORD /*dwRVA*/)
{
		ui->UpdateData(TRUE);
	/*
	szMyFileName contains the original EXE filename which we are to protect.
	We will read the WHOLE contents of szMyFileName into a unsigned char array - ucOriginalBuffer.
	ucOriginalBuffer is a 1:1 mapping of the file on disc, and by directly loading the file into memory, we will be boosting program performance.
	Now all furthur operations will be solely memory based.
	*/
	HANDLE hFile=CreateFile(szMyFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile) 
	{
		AfxMessageBox(("File open error!\n["+(CString)szMyFileName+"]") ,MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}	
	DWORD dwBufferSize=GetFileSize(hFile,NULL),dwBytesRead=0;
	if(dwBufferSize==-1) return FALSE;//Could not get file size
	//Let's get a buffer dwBufferSize bytes long
	unsigned char *ucOriginalBuffer=new unsigned char[dwBufferSize]; //Check for exceptions
	if(!ucOriginalBuffer) {MessageBox(NULL,"There was an error in dynamically allocating memory block.","Not enough memory",MB_OK|MB_ICONSTOP); return FALSE;}
	BOOL bResult=ReadFile(hFile,ucOriginalBuffer,dwBufferSize,&dwBytesRead,NULL);
	CloseHandle(hFile);
	if(!bResult||(dwBufferSize!=dwBytesRead)) {MessageBox(NULL,"There was an error in reading the file contents into memory.","File I/O Error",MB_OK|MB_ICONSTOP);delete[] ucOriginalBuffer;return FALSE;}
	//Let's get a buffer dwNumberOfBytesToRW bytes long, which will contain the code section
	unsigned char *ucTempHexBuffer=new unsigned char[dwNumberOfBytesToRW]; //Check for exceptions
	if(!ucTempHexBuffer) {MessageBox(NULL,"There was an error in dynamically allocating memory block.","Not enough memory",MB_OK|MB_ICONSTOP);delete[] ucOriginalBuffer;return FALSE;}
	ZeroMemory(ucTempHexBuffer,dwNumberOfBytesToRW);
	//Now let's copy dwNumberOfBytesToRW bytes from ucOriginalBuffer starting from nFileOffset into ucTempHexBuffer.
	CopyMemory(ucTempHexBuffer,(ucOriginalBuffer+nFileOffset),dwNumberOfBytesToRW);
	//Now let's NULL out the code section from ucOriginalBuffer.
	ZeroMemory((ucOriginalBuffer+nFileOffset),dwNumberOfBytesToRW);
	//ucOriginalBuffer now contains the "protected" executable file's contents, and ucTempHexBuffer the code section.

	//Let's write them out to disc - SecureFile.EXE and code section to egate card
	//Write SecureFile.EXE
	if(lstrcmp(szProtectedFileName,"")==0) lstrcpy(szProtectedFileName,DEFAULT_PROTFILENAME);
	hFile=CreateFile(szProtectedFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile) 
	{
		AfxMessageBox(("File open error!\n["+(CString)szProtectedFileName+"]") ,MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	bResult=WriteFile(hFile,ucOriginalBuffer,dwBufferSize,&dwBytesRead,NULL);
	if((!bResult)||(dwBufferSize!=dwBytesRead)) //Read dwBytesRead as dwBytesWritten
	{
		AfxMessageBox(("File write error!\n["+(CString)szProtectedFileName+"]") ,MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	CloseHandle(hFile);

//Load license
	ui->bCLA = (byte)APPSECURE_CLA;
	ui->bINS = (byte)0x30;
	ui->bP1  = (byte)0;
	ui->bP2  = (byte)0;
int license_count=ui->m_cb_1.GetCheck();

signed int iTemp=-127; //-127 means no license restrictions.

if(license_count==1) {
	sscanf( ui->m_Count, "%d", &iTemp ); //iTemp is now between 1 and 255
	//Convert this to the range -126 to 128.
	iTemp=iTemp-127;
}
		try 
		{
			ui->bDataBuffer[0]=(byte)iTemp;
			ui->m_sCard->SendCardAPDU(ui->bCLA,ui->bINS,ui->bP1,ui->bP2,1,ui->bDataBuffer,NULL,NULL);
		}
		catch(scu::Exception const &e)
		{
			ui->ReportError("SendCardAPDU: ", e.Description());
			return FALSE;
		}
		
		ui->m_StatusString = "License send to card...";

//end license

//Send APDU to card

	ui->bCLA = (byte)APPSECURE_CLA;
	ui->bINS = (byte)0x10;
	ui->bP1  = (byte)0;
	ui->bP2  = (byte)0;

		try 
		{
			ui->m_sCard->SendCardAPDU(ui->bCLA,ui->bINS,ui->bP1,ui->bP2,100,ucTempHexBuffer,NULL,NULL);
		}
		catch(scu::Exception const &e)
		{
			ui->ReportError("SendCardAPDU: ", e.Description());
			return FALSE;
		}
		
		ui->m_StatusString = "Code section sent to card...";
		ui->UpdateData(FALSE);

//End send APDU


//Write code_sec.key
	HANDLE code_secFile=CreateFile("securekey.dat.",GENERIC_WRITE,FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!code_secFile) 
	{
		AfxMessageBox(("File open error!\n[\"securekey.dat\"]") ,MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	DWORD dwBytesWritten=0;
	bResult=WriteFile(code_secFile,ucTempHexBuffer+100,dwNumberOfBytesToRW-100,&dwBytesWritten,NULL);
	if((!bResult)||(dwNumberOfBytesToRW-100!=dwBytesWritten))
	{
		AfxMessageBox(("File write error! Skipping file.\n[\"securekey.dat\"]") ,MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	CloseHandle(code_secFile);

	if(ucTempHexBuffer!=NULL) delete[] ucTempHexBuffer;
	ucTempHexBuffer=NULL;
	if(ucOriginalBuffer!=NULL) delete[] ucOriginalBuffer;
	ucOriginalBuffer=NULL;
	return TRUE;
}

DWORD CAppProtector::GetOutputLength(DWORD lInputLong)
{
	DWORD lVal=lInputLong%8;// find out if uneven number of bytes at the end
	if(lVal!=0) return (lInputLong + 8 - lVal);
	return lInputLong;
}
