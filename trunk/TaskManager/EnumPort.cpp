#include "StdAfx.h"
#include "EnumPort.h"


using namespace enumPort;


EnumPort::EnumPort(DWORD dwProcessID)
	: m_dwPID(dwProcessID)
	, m_hModule(NULL)
{
	Init();
}

EnumPort::~EnumPort(void)
{
	Clear();
}


void EnumPort::Init()
{
	// 获取扩展函数的入口地址	
	m_hModule = ::LoadLibrary(_T("iphlpapi.dll"));
	m_pAllocateAndGetTcpExTableFromStack = 
			(PFNAllocateAndGetTcpExTableFromStack)::GetProcAddress(m_hModule, 
									"AllocateAndGetTcpExTableFromStack");
	
	m_pAllocateAndGetUdpExTableFromStack = 
			(PFNAllocateAndGetUdpExTableFromStack)::GetProcAddress(m_hModule, 
									"AllocateAndGetUdpExTableFromStack");

	
}

void EnumPort::Clear()
{
	if( m_hModule!=NULL )
	{
		::FreeLibrary(m_hModule);
		m_hModule=NULL;
	}
}

void EnumPort::Enum()
{
	// 清空
	m_vecPortInfo.clear();

	// 调用扩展函数，获取TCP扩展连接表和UDP扩展监听表
	PMIB_TCPEXTABLE pTcpExTable=NULL;
	PMIB_UDPEXTABLE pUdpExTable=NULL;

	// pTcpExTable和pUdpExTable所指的缓冲区自动由扩展函数在进程堆中申请
	if( m_pAllocateAndGetTcpExTableFromStack(&pTcpExTable,TRUE,::GetProcessHeap(),2,AF_INET)!=0 )
	{
		::OutputDebugString(_T("AllocateAndGetTcpExTableFromStack Failed!\n"));
		return;
	}

	if( m_pAllocateAndGetUdpExTableFromStack(&pUdpExTable,TRUE,::GetProcessHeap(),2,AF_INET)!=0 )
	{
		::OutputDebugString(_T("AllocateAndGetUdpExTableFromStack Failed!\n"));
		return;
	}


	// 获取指定进程打开的TCP信息
	for(DWORD i=0; i<pTcpExTable->dwNumEntries; ++i)
	{
		if( pTcpExTable->table[i].dwProcessId==m_dwPID )
		{
			PortInfo portInfo;
			portInfo.m_strType=_T("TCP");
			
			GetPortStatus(portInfo,pTcpExTable->table[i].dwState);				// tcp状态
			GetPortLocalIP(portInfo,pTcpExTable->table[i].dwLocalAddr);			// 本地IP
			GetPortLocalPort(portInfo,pTcpExTable->table[i].dwLocalPort);		// 本地端口
			GetPortRemoteIP(portInfo,pTcpExTable->table[i].dwRemoteAddr);		// 远程IP
			GetPortRemotePort(portInfo,pTcpExTable->table[i].dwRemotePort);		// 远程端口

			m_vecPortInfo.push_back(portInfo);
		}
	}

	// 获取指定进程打开的UDP信息
	for(DWORD i=0; i<pUdpExTable->dwNumEntries; ++i)
	{
		if( pUdpExTable->table[i].dwProcessId==m_dwPID )
		{
			PortInfo portInfo;
			portInfo.m_strType=_T("UDP");
			
			portInfo.m_strStatus=_T("*");										// udp状态
			GetPortLocalIP(portInfo,pUdpExTable->table[i].dwLocalAddr);			// 本地IP
			GetPortLocalPort(portInfo,pUdpExTable->table[i].dwLocalPort);		// 本地端口

			portInfo.m_strRemoteIP=_T("*.*.*.*");								// 远程IP
			portInfo.m_strRemotePort=_T("*");									// 远程端口
	
			m_vecPortInfo.push_back(portInfo);
		}
	}
	

	::LocalFree(pTcpExTable);
	::LocalFree(pUdpExTable);
}


void EnumPort::GetPortStatus(PortInfo &portInfo,DWORD dwStatus)
{
	// 状态
	switch (dwStatus)
	{
	case MIB_TCP_STATE_CLOSED:
		portInfo.m_strStatus=_T("关闭");
		break;
	case MIB_TCP_STATE_TIME_WAIT:
		portInfo.m_strStatus=_T("超时");
		break;
	case MIB_TCP_STATE_LAST_ACK:
		portInfo.m_strStatus=_T("握手");
		break;
	case MIB_TCP_STATE_CLOSING:
		portInfo.m_strStatus=_T("正在关闭");
		break;
	case MIB_TCP_STATE_CLOSE_WAIT:
		portInfo.m_strStatus=_T("关闭等待");
		break;
	case MIB_TCP_STATE_FIN_WAIT1:
		portInfo.m_strStatus=_T("等待");
		break;
	case MIB_TCP_STATE_ESTAB:
		portInfo.m_strStatus=_T("建立");
		break;
	case MIB_TCP_STATE_SYN_RCVD:
		portInfo.m_strStatus=_T("异步接收");
		break;
	case MIB_TCP_STATE_SYN_SENT:
		portInfo.m_strStatus=_T("异步传输");
		break;
	case MIB_TCP_STATE_LISTEN:
		portInfo.m_strStatus=_T("监听");
		break;
	case MIB_TCP_STATE_DELETE_TCB:
		portInfo.m_strStatus=_T("删除");
		break;
	default:
		portInfo.m_strStatus=_T("错误--未知状态");
		break;
	}
}

void EnumPort::GetPortLocalIP(PortInfo &portInfo,DWORD dwLocalIP)
{
	in_addr inadLocal={0};

	// 本地IP地址
	inadLocal.s_addr = dwLocalIP;

	TCHAR szTemp[32]={0};
	::MultiByteToWideChar(CP_ACP,0,inet_ntoa(inadLocal),-1,szTemp,32);
	portInfo.m_strLocalIP=szTemp;
}

void EnumPort::GetPortLocalPort(PortInfo &portInfo,DWORD dwLocalPort)
{
	TCHAR szTemp[16]={0};

	// 获取本地端口
	wsprintf(szTemp,_T("%u"),ntohs((unsigned short)(0x0000FFFF & dwLocalPort)));
	portInfo.m_strLocalPort=szTemp;
}

void EnumPort::GetPortRemoteIP(PortInfo &portInfo,DWORD dwRemoteIP)
{
	in_addr inadRemote={0};

	// 远程IP地址
	inadRemote.s_addr = dwRemoteIP;

	TCHAR szTemp[32]={0};
	::MultiByteToWideChar(CP_ACP,0,inet_ntoa(inadRemote),-1,szTemp,32);
	portInfo.m_strRemoteIP=szTemp;
}

void EnumPort::GetPortRemotePort(PortInfo &portInfo,DWORD dwPort)
{
	TCHAR szTemp[16]={0};

	// 获取远程端口
	if( lstrcmp(portInfo.m_strStatus.c_str(), _T("监听")) == 0 )
	{
		// 如果是处于监听状态则忽略
		dwPort=0;
	}
		
	wsprintf(szTemp,_T("%u"),ntohs((unsigned short)(0x0000FFFF & dwPort)));
	portInfo.m_strRemotePort=szTemp;
}