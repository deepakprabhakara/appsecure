//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPPROTECTOR_H__)
#define AFX_APPPROTECTOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CAppSecureDlg;
//#include "SimpleStringDlg.h"

class CAppProtector  
{
public:
	CAppProtector();
	CAppProtector(const char *lpszProtectedFileName,const char *lpszOriginalPEImageName,CAppSecureDlg* t);
	virtual ~CAppProtector();

	CAppSecureDlg* ui;

private:
	char *szProtectedFileName;
	char *szMyFileName;

	DWORD GetOutputLength(DWORD lInputLong);
	BOOL DumpSection(DWORD nFileOffset,DWORD dwNumberOfBytesToRW,DWORD dwRVA);
	DWORD RVAToFileOffset(PIMAGE_NT_HEADERS pNtHdr,DWORD rva,unsigned nNumberOfSections);
};

#endif // !defined(AFX_APPPROTECTOR_H__)
