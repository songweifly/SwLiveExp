///////////////////////////////////////////////////////////////////////////////////////////
// �ļ�����  RegisterKey.cpp
// ����������ע�����
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RegisterKey.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegisterKey::CRegisterKey()
{
	m_hKey = NULL;
}

CRegisterKey::~CRegisterKey()
{
	RegClose();
}

void CRegisterKey::RegClose()
{
	if(m_hKey)
	{
		RegCloseKey (m_hKey);
		m_hKey = NULL;
	}
}

//��
LONG CRegisterKey::RegOpen(HKEY hKeyRoot,LPCTSTR pszPath)
{
	DWORD dw;
	m_sPath = pszPath;

	if(RegOpenKeyEx(hKeyRoot, pszPath, 0, KEY_ALL_ACCESS, &m_hKey) != ERROR_SUCCESS)
		RegCreateKeyEx(hKeyRoot,pszPath,0L,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL,&m_hKey,&dw);

	return RegOpenKeyEx(hKeyRoot, pszPath, 0, KEY_ALL_ACCESS, &m_hKey);
}

//��ע���
LONG CRegisterKey::RegRead(LPCTSTR pszKey,BYTE *pData,DWORD &dwLength)
{
	//ASSERT(m_hKey);
	//ASSERT(pszKey);

	if(m_hKey == NULL || pszKey == NULL)
		return 0;
	
	DWORD dwType;
	return RegQueryValueEx(m_hKey,(LPCTSTR)pszKey,NULL,&dwType,pData,&dwLength);
}

//��ע���
LONG CRegisterKey::RegRead(LPCTSTR pszKey,CString &sVal)
{
	//ASSERT(m_hKey);
	//ASSERT(pszKey);

	if(m_hKey == NULL || pszKey == NULL)
		return 0;
	
	DWORD dwType;
	DWORD dwSize		= 200;
	TCHAR string[200]	= {0};
	
	LONG IReturn = RegQueryValueEx(m_hKey,(LPCTSTR)pszKey,NULL,&dwType,(BYTE *)string,&dwSize);
	
	if(IReturn == ERROR_SUCCESS)
		sVal = string;

	return IReturn;
}

//��ע���
LONG CRegisterKey::RegRead(LPCTSTR pszKey,DWORD &dwVal)
{
	//ASSERT(m_hKey);
	//ASSERT(pszKey);

	if(m_hKey == NULL || pszKey == NULL)
		return 0;
	
	DWORD dwType;
	DWORD dwSize = sizeof (DWORD);
	DWORD dwDest;
	
	LONG LRet = RegQueryValueEx(m_hKey,(LPCTSTR)pszKey,NULL,&dwType,(BYTE *)&dwDest,&dwSize);
	
	if(LRet == ERROR_SUCCESS)
		dwVal = dwDest;
	return LRet;
}

//дע���
LONG CRegisterKey::RegWrite(LPCTSTR pszKey,const BYTE *pData,DWORD dwLength)
{
	//ASSERT(m_hKey);
	//ASSERT(pszKey);
	//ASSERT(AfxIsValidAddress (pData,dwLength,FALSE));

	//ASSERT(pData&&dwLength>0);
	//ASSERT(AfxIsValidAddress(pData,dwLength,FALSE));

	return RegSetValueEx(m_hKey,pszKey,0L,REG_BINARY,pData,dwLength);
}

//дע���
LONG CRegisterKey::RegWrite(LPCTSTR pszKey,LPCTSTR pszVal)
{
	//ASSERT(m_hKey);
	//ASSERT(pszKey);
	//ASSERT(pszVal);

	//ASSERT(AfxIsValidAddress(pszVal,strlen(pszVal),FALSE));

	return RegSetValueEx(m_hKey,pszKey,0L,REG_SZ,(CONST BYTE *)pszVal,wcslen(pszVal)+1);
}

//дע���
LONG CRegisterKey::RegWrite(LPCTSTR pszKey,DWORD dwVal)
{
	//ASSERT(m_hKey);
	//ASSERT(pszKey);

	if(m_hKey == NULL || pszKey == NULL)
		return 0;
	
	return RegSetValueEx(m_hKey,pszKey,0L,REG_DWORD,(CONST BYTE *)&dwVal,sizeof(DWORD));
}

LONG CRegisterKey::RegDel(LPCTSTR lpSubKey)
{
	//ASSERT(m_hKey);

	//ASSERT(lpSubKey);

	return RegDeleteValue(m_hKey,lpSubKey);
}