//***************************************************************************
#if !defined(AFX_REGKEY_H__A3D3BEA1_AC84_11D3_B6C9_0080C8D668AC__INCLUDED_)
#define AFX_REGKEY_H__A3D3BEA1_AC84_11D3_B6C9_0080C8D668AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRegisterKey  
{
public:
	CRegisterKey();
	virtual ~CRegisterKey();

public:
	LONG RegOpen(HKEY hKeyRoot, LPCTSTR pszPath);
	LONG RegDel(LPCTSTR lpSubKey);
	void RegClose();

	LONG RegRead (LPCTSTR pszKey, DWORD& dwVal);
	LONG RegRead (LPCTSTR pszKey, CString& sVal);
	LONG RegRead (LPCTSTR pszKey, BYTE *pData, DWORD& dwLength);

	LONG RegWrite (LPCTSTR pszKey, DWORD dwVal);
	LONG RegWrite (LPCTSTR pszKey, LPCTSTR pszVal);
	LONG RegWrite (LPCTSTR pszKey, const BYTE *pData, DWORD dwLength);

protected:
	HKEY m_hKey;
	CString m_sPath;
};

#endif // !defined(AFX_REGKEY_H__A3D3BEA1_AC84_11D3_B6C9_0080C8D668AC__INCLUDED_)
