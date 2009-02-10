/*#############################################################################
# ENUMNTSRV.CPP
#
# SCA Software International S.A.
# http://www.scasoftware.com
# scaadmin@scasoftware.com
#
# Copyright (c) 1999 SCA Software International S.A.
#
# Date: 05.12.1999.
# Author: Zoran M.Todorovic
#
# This software is provided "AS IS", without a warranty of any kind.
# You are free to use/modify this code but leave this header intact.
#
#############################################################################*/

#include "stdafx.h"
#include "resource.h"
#include <WinSvc.h>
#include "EnumNtSrv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace enumService;
//=============================================================================
// class TNtServiceInfo
//
//=============================================================================

EnumServiceInfo::EnumServiceInfo()
{
	/*m_strServiceName.Empty();
	m_strDisplayName.Empty();
	m_strBinaryPath.Empty();
	
	m_dwServiceType = 0;
	m_dwStartType = 0;
	m_dwErrorControl = 0;
	m_dwCurrentState = 0;
	*/
	m_hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON_SERVICE));
	_set_se_translator(MyTranslator);
}

EnumServiceInfo::EnumServiceInfo(const EnumServiceInfo& source)
{
	*this = source;
}

EnumServiceInfo::~EnumServiceInfo()
{
	// 销毁Icon
	Clear();
}

//EnumServiceInfo& EnumServiceInfo::operator=(const EnumServiceInfo& source)
//{
//	m_strServiceName	= source.m_strServiceName;
//	m_strDisplayName	= source.m_strDisplayName;
//	m_strBinaryPath		= source.m_strBinaryPath;
//	m_dwServiceType		= source.m_dwServiceType;
//	m_dwStartType		= source.m_dwStartType;
//	m_dwErrorControl	= source.m_dwErrorControl;
//	m_dwCurrentState	= source.m_dwCurrentState;
//
//	return *this;
//}

// Return a service type as a string
string EnumServiceInfo::GetServiceType(DWORD dwServiceType)
{
	// Winnt.h
	string str = _T("未知");
	if( dwServiceType & SERVICE_WIN32 ) 
	{
		if (dwServiceType & SERVICE_WIN32_OWN_PROCESS)
			str = _T("WIN32拥有进程");
		else if (dwServiceType & SERVICE_WIN32_SHARE_PROCESS)
			str = _T("WIN32共享进程");
		if (dwServiceType & SERVICE_INTERACTIVE_PROCESS)
			str += _T(" (交互进程)");
	}

	switch(dwServiceType) 
	{
		case SERVICE_KERNEL_DRIVER: 
			str = _T("内核驱动"); 
			break;

		case SERVICE_FILE_SYSTEM_DRIVER: 
			str = _T("文件系统驱动"); 
			break;
	};

	return str;
}

// Return a service start type as a string
LPCTSTR EnumServiceInfo::GetStartType(DWORD dwStartType)
{
	// Winnt.h
	TCHAR *types[] = 
	{
		_T("根启动"), // 0
		_T("系统启动"), // 1
		_T("自动启动"), // 2
		_T("手动启动"), // 3
		_T("关闭") // 4
	};

	return types[dwStartType];
}

// Return this service error control as a string
LPCTSTR EnumServiceInfo::GetErrorControl(DWORD dwErrorControl)
{
	// Winnt.h
	TCHAR *types[] = 
	{
		_T("ERROR_IGNORE"), // 0
		_T("ERROR_NORMAL"), // 1
		_T("ERROR_SEVERE"), // 2
		_T("ERROR_CRITICAL") // 3
	};

	return types[dwErrorControl];
}

// Return this service current state as a string
LPCTSTR EnumServiceInfo::GetCurrentState(DWORD dwCurrentState)
{
	// Winsvc.h
	TCHAR *types[] = 
	{
		_T("未知"),
		_T("停止"), // 1
		_T("正在启动"), // 2
		_T("正在停止"), // 3
		_T("运行"), // 4
		_T("正在继续"), // 5
		_T("正在暂停"), // 6
		_T("暂停") // 7
	};

	return types[dwCurrentState];
}

HICON EnumServiceInfo::GetServiceIcon(LPCTSTR lpszPath)
{
	//SHFILEINFO shfi={0};

	//__try
	//{
	//	// 获取图标
	//	int nRet=(int)SHGetFileInfo(lpszPath, 0,&shfi, sizeof(shfi), SHGFI_ICON );
	//	if( nRet>0 )
	//		__leave;
	//}
	//__finally
	//{
	//	
	//}

	//return shfi.hIcon;

	return m_hIcon;
}

ServiceInfoList &EnumServiceInfo::GetServiceInfo()
{
	return m_vecServiceInfo;
}


void EnumServiceInfo::Clear()
{
	/*for(deque<ServiceInfo>::iterator iter=m_vecServiceInfo.begin(); iter!=m_vecServiceInfo.end(); ++iter)
	{
		if( iter->m_hIcon!=NULL )
			::DestroyIcon(iter->m_hIcon);
	}*/

	m_vecServiceInfo.clear();
	::DestroyIcon(m_hIcon);
	//deque<T> veTemp;
	//veTemp.swap( vt );
}
// Enumerate services on this machine and return an STL list of service objects 
// dwType = bit OR of SERVICE_WIN32, SERVICE_DRIVER
// dwState = bit OR of SERVICE_ACTIVE, SERVICE_INACTIVE

// Enumerate services on this machine and return a pointer to an array of objects.
// Caller is responsible to delete this pointer using delete [] ...
// dwType = bit OR of SERVICE_WIN32, SERVICE_DRIVER
// dwState = bit OR of SERVICE_ACTIVE, SERVICE_INACTIVE
void EnumServiceInfo::EnumServices(DWORD dwType, DWORD dwState)
{
	Clear();

	// Maybe check if dwType and dwState have at least one constant specified
	SC_HANDLE scman = ::OpenSCManager(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE);
	if (scman) 
	{
		ENUM_SERVICE_STATUS service, *lpservice;
		BOOL rc;
		DWORD bytesNeeded,servicesReturned,resumeHandle = 0;

		rc = ::EnumServicesStatus(scman,dwType,dwState,&service,sizeof(service),
			&bytesNeeded,&servicesReturned,&resumeHandle);

		if ( (rc == FALSE) && (::GetLastError() == ERROR_MORE_DATA) )
		{
			DWORD bytes = bytesNeeded + sizeof(ENUM_SERVICE_STATUS);
			lpservice = new ENUM_SERVICE_STATUS [bytes];

			::EnumServicesStatus(scman,dwType,dwState,lpservice,bytes,
				&bytesNeeded,&servicesReturned,&resumeHandle);

			char Buffer[1024]={0};         // Should be enough for service info

			QUERY_SERVICE_CONFIG *lpqch = (QUERY_SERVICE_CONFIG*)Buffer;

			for (DWORD ndx = 0; ndx < servicesReturned; ndx++) 
			{
				ServiceInfo	serviceInfo;

				serviceInfo.m_strServiceName	= lpservice[ndx].lpServiceName;
				serviceInfo.m_strDisplayName	= lpservice[ndx].lpDisplayName;

				serviceInfo.m_dwServiceType		= lpservice[ndx].ServiceStatus.dwServiceType;
				serviceInfo.m_strServiceType	= GetServiceType(lpservice[ndx].ServiceStatus.dwServiceType);

				serviceInfo.m_dwCurrentState	= lpservice[ndx].ServiceStatus.dwCurrentState;
				serviceInfo.m_strCurrentState	= GetCurrentState(lpservice[ndx].ServiceStatus.dwCurrentState);
				
				// 打开服务句柄
				SC_HANDLE sh = ::OpenService(scman,lpservice[ndx].lpServiceName,SERVICE_QUERY_CONFIG);

				// 查询信息
				if ( ::QueryServiceConfig(sh,lpqch,sizeof(Buffer),&bytesNeeded) ) 
				{
					serviceInfo.m_strBinaryPath		= lpqch->lpBinaryPathName;

					serviceInfo.m_dwStartType		= lpqch->dwStartType;
					serviceInfo.m_strStartType		= GetStartType(lpqch->dwStartType);

					serviceInfo.m_dwErrorControl	= lpqch->dwErrorControl;
					//serviceInfo.m_strError

					// 整合路径为长路径
					serviceInfo.m_strBinaryPath.Replace(_T("\""),_T(""));
					serviceInfo.m_strBinaryPath.Replace(_T("/"),_T("\\"));
					// 删除空格后参数信息
					int nDotIndex=serviceInfo.m_strBinaryPath.ReverseFind(_T('.'));
					if( nDotIndex!=-1 )
					{
						int nSpaceIndex=serviceInfo.m_strBinaryPath.Find(_T(' '),nDotIndex);
						if( nSpaceIndex!=-1 )
						{
							serviceInfo.m_strBinaryPath=serviceInfo.m_strBinaryPath.Left(nSpaceIndex);
						}
					}

					// 获取服务程序图标
					serviceInfo.m_hIcon			= GetServiceIcon(serviceInfo.m_strBinaryPath);

				}


				// 获取介绍信息
				LPSERVICE_DESCRIPTION pDesc=(LPSERVICE_DESCRIPTION)malloc(4096);
				
				if( pDesc!=NULL )
				{
					if( ::QueryServiceConfig2(sh,SERVICE_CONFIG_DESCRIPTION,(LPBYTE)pDesc,4096,&bytesNeeded) )
					{
						// 捕获系统异常 某些服务的描述没有导致返回为空（不是NULL）
						try
						{
							serviceInfo.m_strDescription=pDesc->lpDescription;
						}
						catch(EXCEPTION_POINTERS *pInfo)
						{
#ifdef _DEBUG
							afxDump << "caught C++ exception " << hex << "\n" << 
								"ExceptionAddress: " << pInfo->ExceptionRecord->ExceptionAddress << "\n"
								"ExceptionCode: " << pInfo->ExceptionRecord->ExceptionCode << "\n";
#endif
							//::OutputDebugString(_T("Error"));
							pInfo=NULL;
						}
						catch(...)
						{
							afxDump << "Enumerate Service Error!\n";
						}
					}
					
					free(pDesc);
					pDesc=NULL;
				}
				m_vecServiceInfo.push_back(serviceInfo);

				::CloseServiceHandle(sh);
			}
			delete [] lpservice;

		}
		::CloseServiceHandle(scman);
	}
}




/*#############################################################################
# End of file ENUMNTSRV.CPP
#############################################################################*/
