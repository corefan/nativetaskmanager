#include "StdAfx.h"
#include "EnumWTS.h"


using namespace WTS;


//--------------------------------------------------------------------
// Windows Terminal Services wrappers
//
LPCTSTR EnumWTS::WTSGetConnectState(WTS_CONNECTSTATE_CLASS eClass)
{
	switch(eClass)
	{
	case WTSActive:         // User logged on to WinStation
		return(_T("活跃"));

	case WTSConnected:      // WinStation connected to client
		return(_T("已链接"));

	case WTSConnectQuery:   // In the process of connecting to client
		return(_T("访问"));

	case WTSShadow:         // Shadowing another WinStation
		return(_T("映像"));

	case WTSDisconnected:   // WinStation logged on without client
		return(_T("关闭"));

	case WTSIdle:           // Waiting for client to connect
		return(_T("空闲"));

	case WTSListen:         // WinStation is listening for connection
		return(_T("监听"));

	case WTSReset:          // WinStation is being reset
		return(_T("重置"));

	case WTSDown:           // WinStation is down due to error
		return(_T("失败"));

	case WTSInit:           // WinStation in initialization
		return(_T("初始化"));

	default:
		{ 
			static TCHAR szInfo[64];
			wsprintf(szInfo, _T("未知的状态:%u..."), eClass);
			return(szInfo);
		}
	}
}

EnumWTS::EnumWTS()
	:m_hServer(NULL)
{
	ZeroMemory(m_szComputerName,32*sizeof(TCHAR));
	DWORD dwLen=32;
	::GetComputerName(m_szComputerName,&dwLen);

	WTSOpenServer();
}

EnumWTS::~EnumWTS()
{
	if( m_hServer!=NULL )
	{
		WTSCloseServer();
	}
}



void EnumWTS::WTSOpenServer(LPTSTR szServer/* NULL */)
{
	if( szServer!=NULL )
	{
		lstrcpy(m_szComputerName,szServer);
	}

	try
	{
		m_hServer = ::WTSOpenServer(m_szComputerName);
	}
	catch(...)
	{
		TRACE1("WTSOpenServer Error:%d",::GetLastError());
	}
}

VOID EnumWTS::WTSCloseServer()
{
	try
	{
		::WTSCloseServer(m_hServer);
		m_hServer=NULL;
	}
	catch(...)
	{
		TRACE1("WTSCloseServer Error:%d",::GetLastError());
	}
}

BOOL EnumWTS::ProcessIdToSessionId(DWORD dwProcessId, DWORD *pSessionId)
{
	BOOL bReturn = FALSE;
	try
	{
		bReturn = ::ProcessIdToSessionId(dwProcessId, pSessionId);
	}
	catch(...)
	{
		TRACE1("ProcessIdToSessionId Error:%d",::GetLastError());
	}

	return(bReturn);
}

BOOL EnumWTS::WTSEnumerateProcesses()
{
	if( m_hServer==NULL )
		return FALSE;

	// 清空
	m_vecProcInfo.clear();
	BOOL bReturn = FALSE;

	PWTS_PROCESS_INFO pProcessInfo;
	DWORD dwCount;

	bReturn = ::WTSEnumerateProcesses(m_hServer, 0, 1, &pProcessInfo, &dwCount);
	if( bReturn!=0 )
	{
		WTSProcessInfo procInfo;
		TCHAR szTemp[32]={0};

		for(DWORD i=0; i<dwCount; ++i)
		{
			procInfo.m_dwPID=pProcessInfo[i].ProcessId;
			procInfo.m_dwSID=pProcessInfo[i].SessionId;
			procInfo.m_pUID=pProcessInfo[i].pUserSid;

			procInfo.m_strProccessName=pProcessInfo->pProcessName;

			wsprintf(szTemp,_T("%d"),pProcessInfo[i].ProcessId);
			procInfo.m_strPID=szTemp;
			wsprintf(szTemp,_T("%d"),pProcessInfo[i].SessionId);
			procInfo.m_strSessionID=szTemp;
			
			GetUserNameFromSid( pProcessInfo[i].pUserSid, szTemp);
			procInfo.m_strUserName=szTemp;

			// 插入数据
			m_vecProcInfo.push_back(procInfo);
		}

		WTSFreeMemory(pProcessInfo);
	}
	

	return(bReturn);
}


BOOL EnumWTS::WTSEnumerateSessions(HANDLE hServer, DWORD Reserved, DWORD Version, 
								   PWTS_SESSION_INFO* ppSessionInfo, DWORD* pCount)
{
	BOOL bReturn = FALSE;
	try
	{
		bReturn = ::WTSEnumerateSessions(hServer, Reserved, Version, ppSessionInfo, pCount);
	}
	catch(...)
	{
		TRACE1("WTSEnumerateSessions Error:%d",::GetLastError());
	}

	return(bReturn);
}


BOOL EnumWTS::WTSQuerySessionInformation(HANDLE hServer, DWORD SessionId, 
										 WTS_INFO_CLASS WTSInfoClass, LPTSTR *ppBuffer, DWORD *pBytesReturned)
{
	BOOL bReturn = FALSE;
	try
	{
		bReturn = ::WTSQuerySessionInformation(hServer, SessionId, WTSInfoClass, ppBuffer, pBytesReturned);
	}
	catch(...)
	{
		TRACE1("WTSQuerySessionInformation Error:%d",::GetLastError());
	}

	return(bReturn);
}


VOID EnumWTS::WTSFreeMemory(PVOID pMemory)
{
	try
	{
		::WTSFreeMemory(pMemory);
	}
	catch(...)
	{
		TRACE1("WTSFreeMemory Error:%d",::GetLastError());
	}
}

BOOL EnumWTS::GetUserNameFromSid(PSID pUserSid, LPTSTR pszUserName)
{
	if (pUserSid == NULL)
		return FALSE;

	lstrcpy(pszUserName,_T("??"));

	SID_NAME_USE snu;

	TCHAR	szUser[_MAX_PATH];
	TCHAR	szDomain[_MAX_PATH];
	DWORD	chUser		= _MAX_PATH;
	DWORD	chDomain	= _MAX_PATH;

	PDWORD	pcchUser	= &chUser;
	PDWORD	pcchDomain	= &chDomain;

	// Retrieve user name and domain name based on user’s SID.
	if( ::LookupAccountSid(NULL, pUserSid, szUser, pcchUser, szDomain, pcchDomain, &snu) )
	{
		wsprintf(pszUserName,_T("\\\\%s\\%s"), szDomain,szUser);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
} 


string EnumWTS::GetProcessUserName(DWORD dwPID)
{
	if( m_vecProcInfo.empty() )
		return _T("");

	for(vector<WTSProcessInfo>::const_iterator iter=m_vecProcInfo.begin(); iter!=m_vecProcInfo.end(); ++iter)
	{
		if( iter->m_dwPID==dwPID )
			return iter->m_strUserName;
	}

	return _T("");
}
//
//--------------------------------------------------------------------