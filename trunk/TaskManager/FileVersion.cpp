// // FileVersion.cpp: implementation of the CFileVersion class.
// by Lichengyi2002@msn.com
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileVersion.h"

#pragma comment(lib, "version")

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CFileVersion::CFileVersion() 
{ 
    m_lpVersionData = NULL;
    m_dwLangCharset = 0;
	m_hFile			= 0;

	memset(&m_ffd,0,sizeof(m_ffd));
}

CFileVersion::~CFileVersion() 
{ 
    Close();
} 

void CFileVersion::Close()
{
    delete[] m_lpVersionData; 
    m_lpVersionData = NULL;
    m_dwLangCharset = 0;

	if( m_hFile!=NULL )
	{
		::FindClose(m_hFile);
		m_hFile=NULL;
	}
}

BOOL CFileVersion::Open(LPCTSTR lpszModuleName)
{
    ASSERT(_tcslen(lpszModuleName) > 0);
    ASSERT(m_lpVersionData == NULL);

    // Get the version information size for allocate the buffer
    DWORD dwHandle;     
    DWORD dwDataSize = ::GetFileVersionInfoSize((LPTSTR)lpszModuleName, &dwHandle); 
    if ( dwDataSize == 0 ) 
        return FALSE;

    // Allocate buffer and retrieve version information
    m_lpVersionData = new BYTE[dwDataSize]; 
    if (!::GetFileVersionInfo((LPTSTR)lpszModuleName, dwHandle, dwDataSize, 
	                          (void**)m_lpVersionData) )
    {
        Close();
        return FALSE;
    }

    // Retrieve the first language and character-set identifier
    UINT nQuerySize;
    DWORD* pTransTable;
    if (!::VerQueryValue(m_lpVersionData, _T("\\VarFileInfo\\Translation"),
                         (void **)&pTransTable, &nQuerySize) )
    {
        Close();
        return FALSE;
    }

    // Swap the words to have lang-charset in the correct format
    m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));

	// Attributes and consturct time and modified time
	m_hFile=FindFirstFile(lpszModuleName,&m_ffd);


    return TRUE;
}

CString CFileVersion::QueryValue(LPCTSTR lpszValueName, 
                                 DWORD dwLangCharset /* = 0*/)
{
    // Must call Open() first
    ASSERT(m_lpVersionData != NULL);
    if ( m_lpVersionData == NULL )
        return (CString)_T("");

    // If no lang-charset specified use default
    if ( dwLangCharset == 0 )
        dwLangCharset = m_dwLangCharset;

    // Query version information value
    UINT nQuerySize;
    LPVOID lpData;
    CString strValue, strBlockName;
    strBlockName.Format(_T("\\StringFileInfo\\%08lx\\%s"), 
	                     dwLangCharset, lpszValueName);
    if ( ::VerQueryValue((void **)m_lpVersionData, strBlockName.GetBuffer(0), 
	                     &lpData, &nQuerySize) )
        strValue = (LPCTSTR)lpData;

    strBlockName.ReleaseBuffer();

    return strValue;
}

BOOL CFileVersion::GetFixedInfo(VS_FIXEDFILEINFO& vsffi)
{
    // Must call Open() first
    ASSERT(m_lpVersionData != NULL);
    if ( m_lpVersionData == NULL )
        return FALSE;

    UINT nQuerySize;
	VS_FIXEDFILEINFO* pVsffi;
    if ( ::VerQueryValue((void **)m_lpVersionData, _T("\\"),
                         (void**)&pVsffi, &nQuerySize) )
    {
        vsffi = *pVsffi;
        return TRUE;
    }

    return FALSE;
}

CString CFileVersion::GetFixedFileVersion()
{
    CString strVersion;
	VS_FIXEDFILEINFO vsffi;

    if ( GetFixedInfo(vsffi) )
    {
        strVersion.Format (_T("%u,%u,%u,%u"),HIWORD(vsffi.dwFileVersionMS),
            LOWORD(vsffi.dwFileVersionMS),
            HIWORD(vsffi.dwFileVersionLS),
            LOWORD(vsffi.dwFileVersionLS));
    }
    return strVersion;
}

CString CFileVersion::GetFixedProductVersion()
{
    CString strVersion;
	VS_FIXEDFILEINFO vsffi;

    if ( GetFixedInfo(vsffi) )
    {
        strVersion.Format (_T("%u,%u,%u,%u"), HIWORD(vsffi.dwProductVersionMS),
            LOWORD(vsffi.dwProductVersionMS),
            HIWORD(vsffi.dwProductVersionLS),
            LOWORD(vsffi.dwProductVersionLS));
    }
    return strVersion;
}


CString CFileVersion::GetFileAttributes()
{
	if( m_hFile==NULL )
		return _T("");

	CString strAttr;
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE )
	{
		strAttr+=_T(" 可存档的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED )
	{
		strAttr+=_T(" 压缩的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
	{
		strAttr+=_T(" 文件夹");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED )
	{
		strAttr+=_T(" 加密的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
	{
		strAttr+=_T(" 隐藏的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_NORMAL )
	{
		strAttr+=_T(" 正常的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED )
	{
		strAttr+=_T(" 不能被索引的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_OFFLINE )
	{
		strAttr+=_T(" 脱机的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY )
	{
		strAttr+=_T(" 只读的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM )
	{
		strAttr+=_T(" 系统的");
	}
	if( m_ffd.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY )
	{
		strAttr+=_T(" 临时的");
	}

	return strAttr;
}

CString CFileVersion::GetFileConsturctTime()
{
	if( m_hFile==NULL )
		return _T("");

	SYSTEMTIME sysTime,stUTC;
	::FileTimeToSystemTime(&m_ffd.ftCreationTime,&sysTime);
	::SystemTimeToTzSpecificLocalTime(NULL,&sysTime,&stUTC);

	CString strCreateTime;
	strCreateTime.Format(_T("%02d-%02d-%02d %02d:%02d:%02d"),
		stUTC.wYear,stUTC.wMonth,stUTC.wDay,stUTC.wHour,stUTC.wMinute,stUTC.wSecond);
	return strCreateTime;
}

CString CFileVersion::GetFileAccessTime()
{
	if( m_hFile==NULL )
		return _T("");

	SYSTEMTIME sysTime,stUTC;
	::FileTimeToSystemTime(&m_ffd.ftLastAccessTime,&sysTime);
	::SystemTimeToTzSpecificLocalTime(NULL, &sysTime, &stUTC);

	CString strAccessTime;
	strAccessTime.Format(_T("%02d-%02d-%02d %02d:%02d:%02d"),
		stUTC.wYear,stUTC.wMonth,stUTC.wDay,stUTC.wHour,stUTC.wMinute,stUTC.wSecond);
	return strAccessTime;
}

CString CFileVersion::GetFileModifiedTime()
{
	if( m_hFile==NULL )
		return _T("");

	SYSTEMTIME sysTime,stUTC;
	::FileTimeToSystemTime(&m_ffd.ftLastWriteTime,&sysTime);
	::SystemTimeToTzSpecificLocalTime(NULL, &sysTime, &stUTC);

	CString strModifyTime;
	strModifyTime.Format(_T("%02d-%02d-%02d %02d:%02d:%02d"),
		stUTC.wYear,stUTC.wMonth,stUTC.wDay,stUTC.wHour,stUTC.wMinute,stUTC.wSecond);
	return strModifyTime;
}