//////////////////////////////////////////////////////////////////////
// Author :- Nish 
// ServiceHelper.cpp: implementation of the CServiceHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ServiceHelper.h"

CServiceHelper::CServiceHelper()
{
	m_scm=OpenSCManager(0,0,SC_MANAGER_ALL_ACCESS);
}

CServiceHelper::~CServiceHelper()
{
	CloseServiceHandle(m_scm);
}

SERVICEINFO::SERVICEINFO()
{
	lpServiceName=NULL;
	lpDisplayName=NULL;
	lpBinaryPathName=NULL;
	bAutoStart=false;
}


// 创建服务
BOOL CServiceHelper::Create()
{
	BOOL suc=false;
	SC_HANDLE tmp=0;

	__try
	{
		if(m_serviceinfo.lpServiceName && 
			m_serviceinfo.lpDisplayName && 
			m_serviceinfo.lpBinaryPathName)
		{	
			tmp=CreateService(m_scm,m_serviceinfo.lpServiceName,
				m_serviceinfo.lpDisplayName,
				SERVICE_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS,
				m_serviceinfo.bAutoStart?SERVICE_AUTO_START:SERVICE_DEMAND_START,
				SERVICE_ERROR_NORMAL,
				m_serviceinfo.lpBinaryPathName,
				0,0,0,0,0);
			if(tmp)
			{
				suc=TRUE;
			}
			
		}
	}
	__finally
	{
		if( tmp!=NULL )
			CloseServiceHandle(tmp);
	}

	return suc;
}

// 删除服务
BOOL CServiceHelper::Delete()
{
	BOOL suc=FALSE;
	SC_HANDLE tmp=0;

	__try
	{
		if(m_serviceinfo.lpServiceName)
		{	
			tmp=OpenService(m_scm,m_serviceinfo.lpServiceName,
				SERVICE_ALL_ACCESS);
			if(tmp)
			{
				if(DeleteService(tmp))
				{
					suc=TRUE;
				}
			}
		}
	}
	__finally
	{
		if( tmp!=NULL )
			CloseServiceHandle(tmp);
	}

	return suc;

}

// 启动服务
BOOL CServiceHelper::Start()
{
	BOOL suc=FALSE;
	SC_HANDLE tmp=0;

	__try
	{
		if(m_serviceinfo.lpServiceName)
		{	
			tmp=OpenService(m_scm,m_serviceinfo.lpServiceName,
				SERVICE_ALL_ACCESS);
			if(tmp)
			{
				if(StartService(tmp,0,NULL))
				{
					suc=TRUE;
				}
			}
		}
	}
	__finally
	{
		if( tmp!=NULL )
			CloseServiceHandle(tmp);
	}

	return suc;
}

// 停止服务
BOOL CServiceHelper::Stop()
{
	BOOL suc=FALSE;
	SC_HANDLE tmp=0;

	__try
	{
		if(m_serviceinfo.lpServiceName)
		{	
			tmp=OpenService(m_scm,m_serviceinfo.lpServiceName,
				SERVICE_ALL_ACCESS);
			if(tmp)
			{
				SERVICE_STATUS m_SERVICE_STATUS;
				
				if(ControlService(tmp,
					SERVICE_CONTROL_STOP,
					&m_SERVICE_STATUS))
				{
					suc=TRUE;
				}
				
			}
		}
	}
	__finally
	{
		if( tmp!=NULL )
			CloseServiceHandle(tmp);
	}

	return suc;

}

// 暂停服务
BOOL CServiceHelper::Pause()
{
	BOOL suc=FALSE;
	SC_HANDLE tmp=0;

	__try
	{
		if(m_serviceinfo.lpServiceName)
		{	
			tmp=OpenService(m_scm,m_serviceinfo.lpServiceName,
				SERVICE_ALL_ACCESS);
			if(tmp)
			{
				SERVICE_STATUS m_SERVICE_STATUS;
				
				if(ControlService(tmp,
					SERVICE_CONTROL_PAUSE,
					&m_SERVICE_STATUS))
				{
					suc=TRUE;
				}
				
			}
		}
	}
	__finally
	{
		if( tmp!=NULL )
			CloseServiceHandle(tmp);
	}

	return suc;

}

// 继续服务 
BOOL CServiceHelper::Continue()
{
	BOOL suc=FALSE;
	SC_HANDLE tmp=0;

	__try
	{
		if(m_serviceinfo.lpServiceName)
		{	
			tmp=OpenService(m_scm,m_serviceinfo.lpServiceName,
				SERVICE_ALL_ACCESS);
			if(tmp)
			{
				SERVICE_STATUS m_SERVICE_STATUS;
				
				if( ControlService(tmp,
					SERVICE_CONTROL_CONTINUE,
					&m_SERVICE_STATUS) )
				{
					suc=TRUE;
				}
				
			}
		}
	}
	__finally
	{
		if( tmp!=NULL )
			CloseServiceHandle(tmp);
	}


	return suc;
}

// 改变服务启动类型
BOOL CServiceHelper::ChangeStartType(DWORD dwType)
{
	BOOL suc=FALSE;
	SC_HANDLE tmp=0;

	__try
	{
		if(m_serviceinfo.lpServiceName)
		{	
			tmp=OpenService(m_scm,m_serviceinfo.lpServiceName,
				SERVICE_CHANGE_CONFIG);

			if( ChangeServiceConfig(tmp,SERVICE_NO_CHANGE,dwType,SERVICE_NO_CHANGE,
				NULL,NULL,NULL,NULL,NULL,NULL,NULL) )
			{
				suc=TRUE;
			}
		}
	}
	__finally
	{
		if( tmp!=NULL )
			CloseServiceHandle(tmp);
	}

	return suc;
}

// 改变服务介绍
BOOL CServiceHelper::ChangeDescription(LPTSTR lpDesc)
{
	BOOL suc=FALSE;
	SC_HANDLE tmp=0;

	__try
	{
		if( m_serviceinfo.lpServiceName )
		{	
			tmp=OpenService(m_scm,m_serviceinfo.lpServiceName,
				SERVICE_CHANGE_CONFIG);

			SERVICE_DESCRIPTION sdBuf;
			sdBuf.lpDescription=lpDesc;

			if( ChangeServiceConfig2(tmp,SERVICE_CONFIG_DESCRIPTION,&sdBuf) )
			{
				suc=TRUE;
			}
		}
	}
	__finally
	{
		if( tmp!=NULL )
			CloseServiceHandle(tmp);
	}
	return suc;
}


void CServiceHelper::SetServiceName(LPCTSTR str)
{
	m_serviceinfo.lpServiceName = str;
}

void CServiceHelper::SetServiceDisplayName(LPCTSTR str)
{
	m_serviceinfo.lpDisplayName = str;
}

void CServiceHelper::SetServicePath(LPCTSTR str)
{
	m_serviceinfo.lpBinaryPathName = str;
}

void CServiceHelper::SetAutoStart(BOOL b)
{
	m_serviceinfo.bAutoStart = b;
}