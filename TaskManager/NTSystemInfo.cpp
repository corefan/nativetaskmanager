//////////////////////////////////////////////////////////////////////////////////////
//
// SystemInfo.cpp v2.1
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <process.h>
#include "NTSystemInfo.h"



//////////////////////////////////////////////////////////////////////////////////////
//
// SystemInfoUtils
//
//////////////////////////////////////////////////////////////////////////////////////
using namespace NT;


// From wide char string to CString
void SystemInfoUtils::LPCWSTR2CString( LPCWSTR strW, string& str )
{
#ifdef UNICODE
	// if it is already UNICODE, no problem
	str = strW;
#else
	str = _T("");

	TCHAR* actChar = (TCHAR*)strW;

	if ( actChar == _T('\0') )
		return;

	ULONG len = wcslen(strW) + 1;
	TCHAR* pBuffer = new TCHAR[ len ];
	TCHAR* pNewStr = pBuffer;

	while ( len-- )
	{
		*(pNewStr++) = *actChar;
		actChar += 2;
	}

	str = pBuffer;

	delete [] pBuffer;
#endif
}

// From wide char string to unicode
void SystemInfoUtils::Unicode2CString( UNICODE_STRING* strU, string& str )
{
	if ( *(DWORD*)strU != 0 )
		LPCWSTR2CString( (LPCWSTR)strU->Buffer, str );
	else
		str = _T("");
}

// From device file name to DOS filename
BOOL SystemInfoUtils::GetFsFileName( LPCTSTR lpDeviceFileName, string& fsFileName )
{
	BOOL rc = FALSE;

	TCHAR lpDeviceName[0x1000];
	TCHAR lpDrive[3] = _T("A:");

	// Iterating through the drive letters
	for ( TCHAR actDrive = _T('A'); actDrive <= _T('Z'); actDrive++ )
	{
		lpDrive[0] = actDrive;

		// Query the device for the drive letter
		if ( QueryDosDevice( lpDrive, lpDeviceName, 0x1000 ) != 0 )
		{
			// Network drive?
			if ( _tcsnicmp( _T("\\Device\\LanmanRedirector\\"), lpDeviceName, 25 ) == 0 )
			{
				//Mapped network drive 

				char cDriveLetter;
				DWORD dwParam;

				TCHAR lpSharedName[0x1000];

				if ( _stscanf(  lpDeviceName, 
								_T("\\Device\\LanmanRedirector\\;%c:%d\\%s"), 
								&cDriveLetter, 
								&dwParam, 
								lpSharedName ) != 3 )
						continue;

				_tcscpy( lpDeviceName, _T("\\Device\\LanmanRedirector\\") );
				_tcscat( lpDeviceName, lpSharedName );
			}
			
			// Is this the drive letter we are looking for?
			if ( _tcsnicmp( lpDeviceName, lpDeviceFileName, _tcslen( lpDeviceName ) ) == 0 )
			{
				fsFileName = lpDrive;
				fsFileName += (LPCTSTR)( lpDeviceFileName + _tcslen( lpDeviceName ) );

				rc = TRUE;

				break;
			}
		}
	}

	return rc;
}

// From DOS file name to device file name
BOOL SystemInfoUtils::GetDeviceFileName( LPCTSTR lpFsFileName, string& deviceFileName )
{
	BOOL rc = FALSE;
	TCHAR lpDrive[3];

	// Get the drive letter 
	// unfortunetaly it works only with DOS file names
	_tcsncpy( lpDrive, lpFsFileName, 2 );
	lpDrive[2] = _T('\0');

	TCHAR lpDeviceName[0x1000];

	// Query the device for the drive letter
	if ( QueryDosDevice( lpDrive, lpDeviceName, 0x1000 ) != 0 )
	{
		// Subst drive?
		if ( _tcsnicmp( _T("\\??\\"), lpDeviceName, 4 ) == 0 )
		{
			deviceFileName = lpDeviceName + 4;
			deviceFileName += lpFsFileName + 2;

			return TRUE;
		}
		else
		// Network drive?
		if ( _tcsnicmp( _T("\\Device\\LanmanRedirector\\"), lpDeviceName, 25 ) == 0 )
		{
			//Mapped network drive 

			char cDriveLetter;
			DWORD dwParam;

			TCHAR lpSharedName[0x1000];

			if ( _stscanf(  lpDeviceName, 
							_T("\\Device\\LanmanRedirector\\;%c:%d\\%s"), 
							&cDriveLetter, 
							&dwParam, 
							lpSharedName ) != 3 )
					return FALSE;

			_tcscpy( lpDeviceName, _T("\\Device\\LanmanRedirector\\") );
			_tcscat( lpDeviceName, lpSharedName );
		}

		_tcscat( lpDeviceName, lpFsFileName + 2 );

		deviceFileName = lpDeviceName;

		rc = TRUE;
	}

	return rc;
}

//Get NT version
DWORD SystemInfoUtils::GetNTMajorVersion()
{
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;
   
   // Try calling GetVersionEx using the OSVERSIONINFOEX structure,
   // which is supported on Windows 2000.
   //
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi);

   if( bOsVersionInfoEx == 0 )
   {
      // If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO.

      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return FALSE;
   }

   return osvi.dwMajorVersion;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// INtDll
//
//////////////////////////////////////////////////////////////////////////////////////
INtDll::PNtQuerySystemInformation INtDll::NtQuerySystemInformation		= NULL;
INtDll::PNtQueryObject INtDll::NtQueryObject							= NULL;
INtDll::PNtQueryInformationThread	INtDll::NtQueryInformationThread	= NULL;
INtDll::PNtQueryInformationFile	INtDll::NtQueryInformationFile			= NULL;
INtDll::PNtQueryInformationProcess INtDll::NtQueryInformationProcess	= NULL;


DWORD INtDll::dwNTMajorVersion= SystemInfoUtils::GetNTMajorVersion();
BOOL INtDll::NtDllStatus = INtDll::Init();

BOOL INtDll::Init()
{
	// Get the NtDll function pointers
	NtQuerySystemInformation = (PNtQuerySystemInformation)
					GetProcAddress( GetModuleHandle( _T( "ntdll.dll" ) ),
                    "NtQuerySystemInformation" );

	NtQueryObject = (PNtQueryObject)
					GetProcAddress(	GetModuleHandle( _T( "ntdll.dll" ) ),
                    "NtQueryObject");

	NtQueryInformationThread = (PNtQueryInformationThread)
					GetProcAddress(	GetModuleHandle( _T( "ntdll.dll" ) ),
                    "NtQueryInformationThread" );

	NtQueryInformationFile = (PNtQueryInformationFile)
					GetProcAddress(	GetModuleHandle( _T( "ntdll.dll" ) ),
                    "NtQueryInformationFile" );

	NtQueryInformationProcess = (PNtQueryInformationProcess)
					GetProcAddress(	GetModuleHandle( _T( "ntdll.dll" ) ),
                    "NtQueryInformationProcess" );

	return  NtQuerySystemInformation	!= NULL &&
			NtQueryObject				!= NULL &&
			NtQueryInformationThread	!= NULL &&
			NtQueryInformationFile		!= NULL &&
			NtQueryInformationProcess	!= NULL;
}


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemProcessInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemProcessInformation::SystemProcessInformation()
{
	m_pBuffer = (UCHAR*)::VirtualAlloc ((void*)0x100000,
						BufferSize, 
						MEM_COMMIT,
						PAGE_READWRITE);

	Refresh();
}

SystemProcessInformation::~SystemProcessInformation()
{
	::VirtualFree( m_pBuffer, 0, MEM_RELEASE );
}

BOOL SystemProcessInformation::Refresh()
{
	// map
	m_mapProcessInfo.clear();
	m_pCurrentProcessInfo = NULL;

	if ( !NtDllStatus || m_pBuffer == NULL )
		return FALSE;
	
	// query the process information
	if ( INtDll::NtQuerySystemInformation( NTSystemProcessInformation, m_pBuffer, BufferSize, NULL ) != 0 )
		return FALSE;

	DWORD nCurID = GetCurrentProcessId(); //Current Process ID

	SYSTEM_PROCESS_INFORMATION* pSysProcess = (SYSTEM_PROCESS_INFORMATION *)m_pBuffer;
	do 
	{
		// fill the process information map
		m_mapProcessInfo[pSysProcess->ProcessId]=pSysProcess;

		// we found this process
		if ( pSysProcess->ProcessId == nCurID )
			m_pCurrentProcessInfo = pSysProcess;

		// get the next process information block
		if ( pSysProcess->NextEntryDelta != 0 )
			pSysProcess = (SYSTEM_PROCESS_INFORMATION*)((UCHAR*)pSysProcess + pSysProcess->NextEntryDelta);
		else
			pSysProcess = NULL;

	} while ( pSysProcess != NULL );


	return TRUE;
}

SYSTEM_PROCESS_INFORMATION* SystemProcessInformation::GetSpecialProcessInfo(DWORD dwID)
{
	return m_mapProcessInfo[dwID];
}


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemThreadInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemThreadInformation::SystemThreadInformation( DWORD pID )
{
	m_processId = pID;

	Refresh();
}

SystemThreadInformation::~SystemThreadInformation()
{
	m_vecThreadInfo.clear();
}

BOOL SystemThreadInformation::Refresh()
{
	m_vecThreadInfo.clear();

	// query the process information
	DWORD dwBufSize=0x10000;
	UCHAR *pBuffer = (UCHAR*)::VirtualAlloc ((void*)0x100000,
						dwBufSize, 
						MEM_COMMIT,
						PAGE_READWRITE);

	if ( INtDll::NtQuerySystemInformation( NTSystemProcessInformation, pBuffer, dwBufSize, NULL ) != 0 )
		return FALSE;

	SYSTEM_PROCESS_INFORMATION* pSysProcess = (SYSTEM_PROCESS_INFORMATION *)pBuffer;
	do 
	{
		// we found this process
		if ( pSysProcess->ProcessId == m_processId )
		{
			for(int i=0; i<(int)pSysProcess->ThreadCount; ++i)
			{
				SYSTEM_THREAD ThreadInfo=pSysProcess->Threads[i];
				m_vecThreadInfo.push_back(ThreadInfo);
			}

			break;
		}

		// get the next process information block
		if ( pSysProcess->NextEntryDelta != 0 )
			pSysProcess = (SYSTEM_PROCESS_INFORMATION*)((UCHAR*)pSysProcess + pSysProcess->NextEntryDelta);
		else
			pSysProcess = NULL;

	} while ( pSysProcess != NULL );

	// 释放空间
	::VirtualFree( pBuffer, 0, MEM_RELEASE );

	return TRUE;
}

vector<SYSTEM_THREAD> &SystemThreadInformation::GetThreadInfo()
{
	return m_vecThreadInfo;
}
//////////////////////////////////////////////////////////////////////////////////////
//
// SystemHandleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemHandleInformation::SystemHandleInformation( DWORD pID )
{
	m_processId = pID;
	Refresh();
}

SystemHandleInformation::~SystemHandleInformation()
{
}


BOOL SystemHandleInformation::IsSupportedHandle( SYSTEM_HANDLE& handle )
{
	//Here you can filter the handles you don't want in the Handle list

	// Windows 2000 supports everything :)
	if ( dwNTMajorVersion >= 5 )
		return TRUE;

	//NT4 System process doesn't like if we bother his internal security :)
	if ( handle.ProcessID == 2 && handle.HandleType == 16 )
		return FALSE;

	return TRUE;
}

BOOL SystemHandleInformation::Refresh()
{
	m_vecHandleInfo.clear();

	DWORD size = 0x2000;
	DWORD needed = 0;
	DWORD i = 0;
	BOOL  ret = TRUE;
	string strType;


	if ( !INtDll::NtDllStatus )
		return FALSE;

	// Allocate the memory for the buffer
	SYSTEM_HANDLE_INFORMATION* pSysHandleInformation = 
		(SYSTEM_HANDLE_INFORMATION*)::VirtualAlloc( NULL, size, MEM_COMMIT, PAGE_READWRITE );

	if ( pSysHandleInformation == NULL )
		return FALSE;

	// Query the needed buffer size for the objects ( system wide )
	if ( INtDll::NtQuerySystemInformation( NTSystemHandleInformation, pSysHandleInformation, size, &needed ) != 0 )
	{
		if ( needed == 0 )
		{
			ret = FALSE;
			goto cleanup;
		}

		// The size was not enough
		::VirtualFree( pSysHandleInformation, 0, MEM_RELEASE );

		pSysHandleInformation = (SYSTEM_HANDLE_INFORMATION*)
			::VirtualAlloc( NULL, size = needed + 256, MEM_COMMIT, PAGE_READWRITE );
	}
	
	if ( pSysHandleInformation == NULL )
		return FALSE;

	// Query the objects ( system wide )
	if ( INtDll::NtQuerySystemInformation( NTSystemHandleInformation, pSysHandleInformation, size, NULL ) != 0 )
	{
		ret = FALSE;
		goto cleanup;
	}
	
	// Iterating through the objects
	for ( i = 0; i < pSysHandleInformation->Count; i++ )
	{
		if ( !IsSupportedHandle( pSysHandleInformation->Handles[i] ) )
			continue;
		
		// ProcessId filtering check
		if ( pSysHandleInformation->Handles[i].ProcessID == m_processId || m_processId == (DWORD)-1 )
		{
			//pSysHandleInformation->Handles[i].HandleType = (WORD)(pSysHandleInformation->Handles[i].HandleType % 256);
			HANDLE_INFORMATION sysHandleInfo;

			//sysHandleInfo.strPath=GetHandlePath(pSysHandleInformation->Handles[i]);
			GetHandleName(pSysHandleInformation->Handles[i],sysHandleInfo);
			GetHandleType(pSysHandleInformation->Handles[i],sysHandleInfo);
			GetHandleFlag(pSysHandleInformation->Handles[i].Flags,sysHandleInfo);
			GetHandleAddress(pSysHandleInformation->Handles[i].KernelAddress,sysHandleInfo);

			m_vecHandleInfo.push_back(sysHandleInfo);
		}
	}

cleanup:
	
	if ( pSysHandleInformation != NULL )
		::VirtualFree( pSysHandleInformation, 0, MEM_RELEASE );

	return ret;
}

void SystemHandleInformation::GetHandleName(SYSTEM_HANDLE &sysHandle,HANDLE_INFORMATION &handleInfo)
{
	string strName;
	string strPath;
	//NT4 Stupid thing if it is the services.exe and I call GetName :((
	//if ( INtDll::dwNTMajorVersion == 4 && _tcsicmp( processName, _T("services.exe" ) ) == 0 )
	//	name = "";
	//else
	GetName( (HANDLE)sysHandle.HandleNumber, strName, sysHandle.ProcessID);
	if ( sysHandle.HandleType == SystemHandleInformation::OB_TYPE_FILE )
	{
		if ( SystemInfoUtils::GetFsFileName( strName.c_str(), strPath ) )
			strName = strPath;
	}

	handleInfo.strPath=strName;
}


void SystemHandleInformation::GetHandleType(SYSTEM_HANDLE &sysHandle,HANDLE_INFORMATION &handleInfo)
{
	string strTemp;

	GetTypeToken( (HANDLE)sysHandle.HandleNumber, strTemp, sysHandle.ProcessID  );

	/*
	switch(wHandleType)
	{
	case OB_TYPE_UNKNOWN:
		strTemp=_T("未知");
		break;
	case OB_TYPE_TYPE:
		strTemp=_T("TYPE");
		break;
	case OB_TYPE_DIRECTORY:
		strTemp=_T("路径");
		break;
	case OB_TYPE_SYMBOLIC_LINK:
		strTemp=_T("链接");
		break;
	case OB_TYPE_TOKEN:
		strTemp=_T("权限");
		break;
	case OB_TYPE_PROCESS:
		strTemp=_T("进程");
		break;
	case OB_TYPE_THREAD:
		strTemp=_T("线程");
		break;
	case OB_TYPE_UNKNOWN_7:
		strTemp=_T("未知7");
		break;
	case OB_TYPE_EVENT:
		strTemp=_T("事件");
		break;
	case OB_TYPE_EVENT_PAIR:
		strTemp=_T("PAIR");
		break;
	case OB_TYPE_MUTANT:
		strTemp=_T("MUTANT");
		break;
	case OB_TYPE_UNKNOWN_11:
		strTemp=_T("UNKNOWN_11");
		break;
	case OB_TYPE_SEMAPHORE:
		strTemp=_T("信号");
		break;
	case OB_TYPE_TIMER:
		strTemp=_T("定时");
		break;
	case OB_TYPE_PROFILE:
		strTemp=_T("PROFILE");
		break;
	case OB_TYPE_WINDOW_STATION:
		strTemp=_T("STATION");
		break;
	case OB_TYPE_DESKTOP:
		strTemp=_T("DESKTOP");
		break;
	case OB_TYPE_SECTION:
		strTemp=_T("关键段");
		break;
	case OB_TYPE_KEY:
		strTemp=_T("KEY");
		break;
	case OB_TYPE_PORT:
		strTemp=_T("端口");
		break;
	case OB_TYPE_WAITABLE_PORT:
		strTemp=_T("等待端口");
		break;
	case OB_TYPE_UNKNOWN_21:
		strTemp=_T("UNKNOWN_21");
		break;
	case OB_TYPE_UNKNOWN_22:
		strTemp=_T("UNKNOWN_22");
		break;
	case OB_TYPE_UNKNOWN_23:
		strTemp=_T("UNKNOWN_23");
		break;
	case OB_TYPE_UNKNOWN_24:
		strTemp=_T("UNKNOWN_24");
		break;
	case OB_TYPE_IO_COMPLETION:
		strTemp=_T("IO");
		break;
	case OB_TYPE_FILE:
		strTemp=_T("文件");
		break;
	default:
		{
			static TCHAR szError[32]={0};
			wsprintf(szError,_T("未知状态: %d"),wHandleType);
			strTemp=szError;
		}
		break;
	}
	*/

	handleInfo.strType=strTemp;
}


void SystemHandleInformation::GetHandleFlag(DWORD dwHandleFlag,HANDLE_INFORMATION &handleInfo)
{
	string strTemp;

	switch(dwHandleFlag)
	{
	case 0x01:	// PROTECT_FROM_CLOSE
		strTemp=_T("保护");
		break;
	case 0x02:
		strTemp=_T("继承");
		break;	// INHERIT
	default:
		{
			static TCHAR szError[32]={0};
			wsprintf(szError,_T("未知状态: %d"),dwHandleFlag);
			strTemp=szError;
		}
		break;
	}

	handleInfo.strFlag=strTemp;
}


void SystemHandleInformation::GetHandleAddress(DWORD dwHandleAddr,HANDLE_INFORMATION &handleInfo)
{
	static TCHAR szTemp[32]={0};

	wsprintf(szTemp,_T("%p"),dwHandleAddr);
	handleInfo.strAddr=szTemp;
}

HANDLE SystemHandleInformation::OpenProcess( DWORD processId )
{
	// Open the process for handle duplication
	return ::OpenProcess( PROCESS_DUP_HANDLE, TRUE, processId );
}

HANDLE SystemHandleInformation::DuplicateHandle( HANDLE hProcess, HANDLE hRemote )
{
	HANDLE hDup = NULL;

	// Duplicate the remote handle for our process
	::DuplicateHandle( hProcess, hRemote,	GetCurrentProcess(), &hDup,	0, FALSE, DUPLICATE_SAME_ACCESS );

	return hDup;
}

//Information functions
BOOL SystemHandleInformation::GetTypeToken( HANDLE h, string& str, DWORD processId )
{
	ULONG size = 0x2000;
	UCHAR* lpBuffer = NULL;
	BOOL ret = FALSE;

	HANDLE handle;
	HANDLE hRemoteProcess = NULL;
	BOOL remote = processId != GetCurrentProcessId();
	
	if ( !NtDllStatus )
		return FALSE;

	if ( remote )
	{
		// Open the remote process
		hRemoteProcess = OpenProcess( processId );
		
		if ( hRemoteProcess == NULL )
			return FALSE;

		// Duplicate the handle
		handle = DuplicateHandle( hRemoteProcess, h );
	}
	else
		handle = h;

	// Query the info size
	INtDll::NtQueryObject( handle, 2, NULL, 0, &size );

	lpBuffer = new UCHAR[size];

	// Query the info size ( type )
	if ( INtDll::NtQueryObject( handle, 2, lpBuffer, size, NULL ) == 0 )
	{
		str = _T("");
		SystemInfoUtils::LPCWSTR2CString( (LPCWSTR)(lpBuffer+0x60), str );

		ret = TRUE;
	}

	if ( remote )
	{
		if ( hRemoteProcess != NULL )
			CloseHandle( hRemoteProcess );

		if ( handle != NULL )
			CloseHandle( handle );
	}
	
	if ( lpBuffer != NULL )
		delete [] lpBuffer;

	return ret;
}

BOOL SystemHandleInformation::GetType( HANDLE h, WORD& type, DWORD processId )
{
	string strType;

	type = OB_TYPE_UNKNOWN;

	if ( !GetTypeToken( h, strType, processId ) )
		return FALSE;

	return GetTypeFromTypeToken( strType.c_str(), type );
}

BOOL SystemHandleInformation::GetTypeFromTypeToken( LPCTSTR typeToken, WORD& type )
{
	const WORD count = 27;
	string constStrTypes[count] = 
	{ 
		_T(""), _T(""), _T("Directory"), _T("SymbolicLink"), _T("Token"),
		_T("Process"), _T("Thread"), _T("Unknown7"), _T("Event"), _T("EventPair"), _T("Mutant"), 
		_T("Unknown11"), _T("Semaphore"), _T("Timer"), _T("Profile"), _T("WindowStation"),
		_T("Desktop"), _T("Section"), _T("Key"), _T("Port"), _T("WaitablePort"), 
		_T("Unknown21"), _T("Unknown22"), _T("Unknown23"), _T("Unknown24"),
		_T("IoCompletion"), _T("File") 
	};

	type = OB_TYPE_UNKNOWN;

	for ( WORD i = 1; i < count; i++ )
		if ( constStrTypes[i] == typeToken )
		{
			type = i;
			return TRUE;
		}
		
	return FALSE;
}

BOOL SystemHandleInformation::GetName( HANDLE handle, string& str, DWORD processId )
{
	WORD type = 0;

	if ( !GetType( handle, type, processId  ) )
		return FALSE;

	return GetNameByType( handle, type, str, processId );
}

BOOL SystemHandleInformation::GetNameByType( HANDLE h, WORD type, string& str, DWORD processId )
{
	ULONG size = 0x2000;
	UCHAR* lpBuffer = NULL;
	BOOL ret = FALSE;

	HANDLE handle;
	HANDLE hRemoteProcess = NULL;
	BOOL remote = processId != GetCurrentProcessId();
	DWORD dwId = 0;
	
	if ( !NtDllStatus )
		return FALSE;

	if ( remote )
	{
		hRemoteProcess = OpenProcess( processId );
		
		if ( hRemoteProcess == NULL )
			return FALSE;

		handle = DuplicateHandle( hRemoteProcess, h );
	}
	else
		handle = h;

	// let's be happy, handle is in our process space, so query the infos :)
	switch( type )
	{
	case OB_TYPE_PROCESS:
		//GetProcessId( handle, dwId );
		
		//str.Format( _T("PID: 0x%X"), dwId );
			
		ret = TRUE;
		goto cleanup;
		break;

	case OB_TYPE_THREAD:
		//GetThreadId( handle, dwId );

		//str.Format( _T("TID: 0x%X") , dwId );
				
		ret = TRUE;
		goto cleanup;
		break;

	case OB_TYPE_FILE:
		//ret = GetFileName( handle, str );

		// access denied :(
		//if ( ret && str == _T("") )
		//	goto cleanup;
		break;

	};

	INtDll::NtQueryObject ( handle, 1, NULL, 0, &size );

	// let's try to use the default
	if ( size == 0 )
		size = 0x2000;

	lpBuffer = new UCHAR[size];

	if ( INtDll::NtQueryObject( handle, 1, lpBuffer, size, NULL ) == 0 )
	{
		SystemInfoUtils::Unicode2CString( (UNICODE_STRING*)lpBuffer, str );
		ret = TRUE;
	}
	
cleanup:

	if ( remote )
	{
		if ( hRemoteProcess != NULL )
			CloseHandle( hRemoteProcess );

		if ( handle != NULL )
			CloseHandle( handle );
	}

	if ( lpBuffer != NULL )
		delete [] lpBuffer;
	
	return ret;
}


vector<SystemHandleInformation::HANDLE_INFORMATION> &SystemHandleInformation::GetHandleInfo()
{
	return m_vecHandleInfo;
}


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemPerformanceInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemPerformanceInformation::SystemPerformanceInformation()
{
	memset(&m_sysPerformanceInfo,0,sizeof(m_sysPerformanceInfo));
}

SystemPerformanceInformation::~SystemPerformanceInformation()
{
}

BOOL SystemPerformanceInformation::Refresh()
{
	DWORD dwNumberBytes = sizeof(SYSTEM_PERFORMANCE_INFORMATION);
	DWORD dwReturnLength= 0;

	if ( INtDll::NtQuerySystemInformation(NTSystemPerformanceInformation,
			&m_sysPerformanceInfo,dwNumberBytes,&dwReturnLength) !=0 )
	{
		return FALSE;
	}
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemTimeInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemTimeInformation::SystemTimeInformation()
{
	memset(&m_sysTime,0,sizeof(m_sysTime));
}

SystemTimeInformation::~SystemTimeInformation()
{
}

BOOL SystemTimeInformation::Refresh()
{
	DWORD dwNumberBytes = sizeof(SYSTEM_PROCESSOR_TIMES);
	DWORD dwReturnLength= 0;

	if ( INtDll::NtQuerySystemInformation(NTSystemProcessorCounters,
			&m_sysTime,dwNumberBytes,&dwReturnLength) !=0 )
	{
		return FALSE;
	}
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemPageFileInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemPageFileInformation::SystemPageFileInformation()
{
	memset(&m_pageInfo,0,sizeof(SYSTEM_PAGEFILE_INFORMATION));
}

SystemPageFileInformation::~SystemPageFileInformation()
{
}

BOOL SystemPageFileInformation::Refresh()
{
	DWORD dwNumberBytes = sizeof(SYSTEM_PAGEFILE_INFORMATION);
	DWORD dwReturnLength= 0;

	if ( INtDll::NtQuerySystemInformation(NTSystemPageFileInformation,
			&m_pageInfo,dwNumberBytes,&dwReturnLength) !=0 )
	{
		return FALSE;
	}
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////
//
// SystemCacheInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemCacheInformation::SystemCacheInformation()
{
	memset(&m_cacheInfo,0,sizeof(SYSTEM_CACHE_INFORMATION));
}

SystemCacheInformation::~SystemCacheInformation()
{
}

BOOL SystemCacheInformation::Refresh()
{
	DWORD dwNumberBytes = sizeof(SYSTEM_CACHE_INFORMATION);
	DWORD dwReturnLength= 0;

	if ( INtDll::NtQuerySystemInformation(NTSystemCacheInformation,
			&m_cacheInfo,dwNumberBytes,&dwReturnLength) !=0 )
	{
		return FALSE;
	}
	
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemModuleInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemModuleInformation::SystemModuleInformation( DWORD pID, BOOL bRefresh )
{
	m_processId = pID;

	if ( bRefresh )
		Refresh();
}

void SystemModuleInformation::GetModuleListForProcess( DWORD processID )
{
	DWORD i = 0;
	DWORD cbNeeded = 0;
	HMODULE* hModules = NULL;
	MODULE_INFO moduleInfo;

	// Open process to read to query the module list
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );

	if ( hProcess == NULL )
		goto cleanup;

	//Get the number of modules
	if ( !(*m_EnumProcessModules)( hProcess, NULL, 0, &cbNeeded ) )
		goto cleanup;

	hModules = new HMODULE[ cbNeeded / sizeof( HMODULE ) ];

	//Get module handles
    if ( !(*m_EnumProcessModules)( hProcess, hModules, cbNeeded, &cbNeeded ) )
		goto cleanup;
	
	for ( i = 0; i < cbNeeded / sizeof( HMODULE ); i++ )
	{
		moduleInfo.ProcessId = processID;
		moduleInfo.Handle = hModules[i];
		
		//Get module full paths
		if ( (*m_GetModuleFileNameEx)( hProcess, hModules[i], moduleInfo.FullPath, _MAX_PATH ) )
			m_ModuleInfos.AddTail( moduleInfo );
	}

cleanup:
	if ( hModules != NULL )
		delete [] hModules;

	if ( hProcess != NULL )
		CloseHandle( hProcess );
}

BOOL SystemModuleInformation::Refresh()
{
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// SystemWindowInformation
//
//////////////////////////////////////////////////////////////////////////////////////

SystemWindowInformation::SystemWindowInformation( DWORD pID, BOOL bRefresh )
{
	m_processId = pID;

	if ( bRefresh )
		Refresh();
}

BOOL SystemWindowInformation::Refresh()
{
	m_WindowInfos.RemoveAll();

	// Enumerating the windows
	EnumWindows( EnumerateWindows, (LPARAM)this );
	
	return TRUE;
}

BOOL CALLBACK SystemWindowInformation::EnumerateWindows( HWND hwnd, LPARAM lParam )
{
	SystemWindowInformation* _this = (SystemWindowInformation*)lParam;
	WINDOW_INFO wi;

	wi.hWnd = hwnd;
	GetWindowThreadProcessId(hwnd, &wi.ProcessId ) ;

	// Filtering by process ID
	if ( _this->m_processId == -1 || _this->m_processId == wi.ProcessId )
	{
		GetWindowText( hwnd, wi.Caption, MaxCaptionSize );

		// That is we are looking for
		if ( GetLastError() == 0 )
			_this->m_WindowInfos.AddTail( wi );
	}

	return TRUE;
};
