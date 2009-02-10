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
	// ��ȡ��չ��������ڵ�ַ	
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
	// ���
	m_vecPortInfo.clear();

	// ������չ��������ȡTCP��չ���ӱ��UDP��չ������
	PMIB_TCPEXTABLE pTcpExTable=NULL;
	PMIB_UDPEXTABLE pUdpExTable=NULL;

	// pTcpExTable��pUdpExTable��ָ�Ļ������Զ�����չ�����ڽ��̶�������
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


	// ��ȡָ�����̴򿪵�TCP��Ϣ
	for(DWORD i=0; i<pTcpExTable->dwNumEntries; ++i)
	{
		if( pTcpExTable->table[i].dwProcessId==m_dwPID )
		{
			PortInfo portInfo;
			portInfo.m_strType=_T("TCP");
			
			GetPortStatus(portInfo,pTcpExTable->table[i].dwState);				// tcp״̬
			GetPortLocalIP(portInfo,pTcpExTable->table[i].dwLocalAddr);			// ����IP
			GetPortLocalPort(portInfo,pTcpExTable->table[i].dwLocalPort);		// ���ض˿�
			GetPortRemoteIP(portInfo,pTcpExTable->table[i].dwRemoteAddr);		// Զ��IP
			GetPortRemotePort(portInfo,pTcpExTable->table[i].dwRemotePort);		// Զ�̶˿�

			m_vecPortInfo.push_back(portInfo);
		}
	}

	// ��ȡָ�����̴򿪵�UDP��Ϣ
	for(DWORD i=0; i<pUdpExTable->dwNumEntries; ++i)
	{
		if( pUdpExTable->table[i].dwProcessId==m_dwPID )
		{
			PortInfo portInfo;
			portInfo.m_strType=_T("UDP");
			
			portInfo.m_strStatus=_T("*");										// udp״̬
			GetPortLocalIP(portInfo,pUdpExTable->table[i].dwLocalAddr);			// ����IP
			GetPortLocalPort(portInfo,pUdpExTable->table[i].dwLocalPort);		// ���ض˿�

			portInfo.m_strRemoteIP=_T("*.*.*.*");								// Զ��IP
			portInfo.m_strRemotePort=_T("*");									// Զ�̶˿�
	
			m_vecPortInfo.push_back(portInfo);
		}
	}
	

	::LocalFree(pTcpExTable);
	::LocalFree(pUdpExTable);
}


void EnumPort::GetPortStatus(PortInfo &portInfo,DWORD dwStatus)
{
	// ״̬
	switch (dwStatus)
	{
	case MIB_TCP_STATE_CLOSED:
		portInfo.m_strStatus=_T("�ر�");
		break;
	case MIB_TCP_STATE_TIME_WAIT:
		portInfo.m_strStatus=_T("��ʱ");
		break;
	case MIB_TCP_STATE_LAST_ACK:
		portInfo.m_strStatus=_T("����");
		break;
	case MIB_TCP_STATE_CLOSING:
		portInfo.m_strStatus=_T("���ڹر�");
		break;
	case MIB_TCP_STATE_CLOSE_WAIT:
		portInfo.m_strStatus=_T("�رյȴ�");
		break;
	case MIB_TCP_STATE_FIN_WAIT1:
		portInfo.m_strStatus=_T("�ȴ�");
		break;
	case MIB_TCP_STATE_ESTAB:
		portInfo.m_strStatus=_T("����");
		break;
	case MIB_TCP_STATE_SYN_RCVD:
		portInfo.m_strStatus=_T("�첽����");
		break;
	case MIB_TCP_STATE_SYN_SENT:
		portInfo.m_strStatus=_T("�첽����");
		break;
	case MIB_TCP_STATE_LISTEN:
		portInfo.m_strStatus=_T("����");
		break;
	case MIB_TCP_STATE_DELETE_TCB:
		portInfo.m_strStatus=_T("ɾ��");
		break;
	default:
		portInfo.m_strStatus=_T("����--δ֪״̬");
		break;
	}
}

void EnumPort::GetPortLocalIP(PortInfo &portInfo,DWORD dwLocalIP)
{
	in_addr inadLocal={0};

	// ����IP��ַ
	inadLocal.s_addr = dwLocalIP;

	TCHAR szTemp[32]={0};
	::MultiByteToWideChar(CP_ACP,0,inet_ntoa(inadLocal),-1,szTemp,32);
	portInfo.m_strLocalIP=szTemp;
}

void EnumPort::GetPortLocalPort(PortInfo &portInfo,DWORD dwLocalPort)
{
	TCHAR szTemp[16]={0};

	// ��ȡ���ض˿�
	wsprintf(szTemp,_T("%u"),ntohs((unsigned short)(0x0000FFFF & dwLocalPort)));
	portInfo.m_strLocalPort=szTemp;
}

void EnumPort::GetPortRemoteIP(PortInfo &portInfo,DWORD dwRemoteIP)
{
	in_addr inadRemote={0};

	// Զ��IP��ַ
	inadRemote.s_addr = dwRemoteIP;

	TCHAR szTemp[32]={0};
	::MultiByteToWideChar(CP_ACP,0,inet_ntoa(inadRemote),-1,szTemp,32);
	portInfo.m_strRemoteIP=szTemp;
}

void EnumPort::GetPortRemotePort(PortInfo &portInfo,DWORD dwPort)
{
	TCHAR szTemp[16]={0};

	// ��ȡԶ�̶˿�
	if( lstrcmp(portInfo.m_strStatus.c_str(), _T("����")) == 0 )
	{
		// ����Ǵ��ڼ���״̬�����
		dwPort=0;
	}
		
	wsprintf(szTemp,_T("%u"),ntohs((unsigned short)(0x0000FFFF & dwPort)));
	portInfo.m_strRemotePort=szTemp;
}