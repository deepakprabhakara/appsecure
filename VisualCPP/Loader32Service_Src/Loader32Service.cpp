/*
Source to the Loader32Service program.
This operates on the SAME core as the Dialog based Loader32 program
*/
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>
#include <string.h>

#include <imagehlp.h>//include Imagehlp.lib into project
#pragma comment(lib,"imagehlp.lib")

#include "iop.h"

using namespace iop;

BOOL InitializeUs(DWORD*,DWORD*);
char szMyFileName[MAX_PATH+1];

//////////////////////////////
#define nNumberOfRVAsToPatch 1
//////////////////////////////
#define CODE_DUMP "securekey.dat"
#define FILE_TO_LOAD "SecureFile.exe"

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

BOOL LoadCrackedFile(LPSTR lpApplicationName,LPSTR  lpCmdLine)
{
 	//////////////////Patching data global or else we have to get messy with pointers /////////////////////// 
	InitializeSP();
	///////////////////////////////////////////////////////////////////////////////////////////////
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
 //We are actually emulating a debugger.Note that Write/ReadProcessMemory are NOT the fastest APIs, but VERY good at messing into other programs
 HANDLE ProcessHandle=OpenProcess(PROCESS_ALL_ACCESS,FALSE,processinfo.dwProcessId);
 if (ProcessHandle)
 {
  if(WriteProcessMemory(ProcessHandle,(PatchBytes+index)->RVA,(PatchBytes+index)->szPatchData,(PatchBytes+index)->nNumberOfPatches,&BytesWritten)) ReturnVal = TRUE;
  CloseHandle(ProcessHandle);//Close process handle
 }
 return ReturnVal;
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	CIOP m_sIOP;
	CVisaOP *m_VisaOP; 
	CSmartCard *m_sCard;
	BYTE bCLA;
	BYTE bINS;
	BYTE bP1;
	BYTE bP2;

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
		if (szReaders != NULL)
			delete [] szReaders;
		return 0;
	}

	char *szCurrent = szReaders;


		try	{
			m_sCard = m_sIOP.Connect(szReaders, false);


			m_VisaOP = m_sCard->GetVisaOP();

			BYTE aid[] = {0x01,0x02,0x03,0x04,0x05,0x06}; 

			try {
				m_VisaOP->SelectAID(aid,6);
			}
			catch(CSmartCard::Exception const /*&e*/)
			{
				return 0;
			}
			
		}
		catch(CSmartCard::Exception const /*&e*/)     {
			return 0;
		}

//Validate PIN
	bCLA = (byte)0x85;
	bINS = (byte)0x40;
	bP1  = (byte)0;
	bP2  = (byte)0;


			BYTE pinnumber[] = {0x00,0x01,0x09,0x05}; 
		
	    try
        {
		    m_sCard->SendCardAPDU(bCLA,bINS,bP1,bP2,4,pinnumber,NULL,NULL);
        }
        catch(CSmartCard::Exception const /*&e*/)
	    {
            return 0;
        }

// validate
		

	DWORD dwSize=NULL,dwByteCount=NULL;
	//////////////////////////Open file contaning section data///////////////////
	HANDLE hFile=CreateFile(CODE_DUMP,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE) {
		return 0;
	}
	dwSize=GetFileSize(hFile,NULL);
	//Create a dynamic array
	PatchBytes=new PatchInfo[1];
	PatchBytes->szPatchData=new unsigned char[dwSize+100];

	//Send APDU to card

	bCLA = (byte)0x85;
	bINS = (byte)0x20;
	bP1  = (byte)0;
	bP2  = (byte)0;

                try {
				    m_sCard->SendCardAPDU(bCLA,bINS,bP1,bP2,NULL,NULL,100,PatchBytes->szPatchData);
                }
                catch(CSmartCard::Exception const /*&e*/)
	            {
                    return 0;
                }
			

delete [] szReaders;	
//End APDU

	if(PatchBytes==NULL&&PatchBytes->szPatchData==NULL) 
	{
		exit(0);
	}
	//Fill the array up with CODE_DUMP contents and initialize 
	ReadFile(hFile,PatchBytes->szPatchData + 100,dwSize,&dwByteCount,NULL);//szPatchData now contains encrypted data

PatchBytes->nNumberOfPatches=dwSize+100;

	CloseHandle(hFile);


	DWORD nEntryPoint=-1,nPreferredLoadAddress=-1;

	////////////////////////////////////////////////////
	
	hFile=CreateFile(FILE_TO_LOAD,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if(hFile==INVALID_HANDLE_VALUE) {
		return 0;
	}
	CloseHandle(hFile);

	//////////////////////////////////////////////////////
	strcpy(szMyFileName,FILE_TO_LOAD);
	InitializeUs(&nEntryPoint,&nPreferredLoadAddress);
	///////////////////////////////////////////////////////
	PatchBytes->RVA=(char *)(nEntryPoint+nPreferredLoadAddress);//AddressOfEntryPoint of program
	if(dwByteCount!=dwSize) 
	{
		return 0;
	}
	//Call the loader
	if(!LoadCrackedFile(FILE_TO_LOAD,"")) return 0;//MessageBox("\nSorry ! Program could not be patched! Will terminate thread.");
    //Clean up our dynamic array
	delete[] PatchBytes->szPatchData;
	delete[] PatchBytes;


	return 0;
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
