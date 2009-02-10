#ifndef __ENUM_PORT_H
#define __ENUM_PORT_H

#include "Winsock2.h"
#include <iphlpapi.h>
#include <vector>
#include <string>

using namespace std;


namespace enumPort
{
#pragma comment(lib,"WS2_32")
#pragma comment(lib,"Iphlpapi")

#ifdef _UNICODE
#define string wstring
#endif


	typedef struct 
	{
		DWORD   dwState;        	// ����״̬
		DWORD   dwLocalAddr;    	// ���ص�ַ
		DWORD   dwLocalPort;    	// ���ض˿�
		DWORD   dwRemoteAddr;   	// Զ�̵�ַ
		DWORD   dwRemotePort;   	// Զ�̶˿�
		DWORD	dwProcessId;		// ����ID��
	} MIB_TCPEXROW, *PMIB_TCPEXROW;

	typedef struct 
	{
		DWORD			dwNumEntries;
		MIB_TCPEXROW	table[ANY_SIZE];
	} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;

	typedef struct 
	{
		DWORD   dwLocalAddr;    	// ���ص�ַ
		DWORD   dwLocalPort;    	// ���ض˿�
		DWORD	dwProcessId;		// ����ID��
	} MIB_UDPEXROW, *PMIB_UDPEXROW;


	typedef struct 
	{
		DWORD			dwNumEntries;
		MIB_UDPEXROW	table[ANY_SIZE];
	} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;


	// ��չ����ԭ��
	typedef DWORD (WINAPI *PFNAllocateAndGetTcpExTableFromStack)(
		PMIB_TCPEXTABLE *pTcpTable, 
		BOOL bOrder,             
		HANDLE heap,
		DWORD zero,
		DWORD flags
		);

	typedef DWORD (WINAPI *PFNAllocateAndGetUdpExTableFromStack)(
		PMIB_UDPEXTABLE *pUdpTable,  
		BOOL bOrder,              
		HANDLE heap,
		DWORD zero,
		DWORD flags
		);


	typedef struct
	{
		string	m_strType;				// ��������
		string	m_strStatus;			// ����״̬
		string	m_strLocalIP;			// ���ӱ���IP
		string	m_strLocalPort;			// ���ӱ��ض˿�
		string	m_strRemoteIP;			// ����Զ��IP
		string	m_strRemotePort;		// ����Զ�̶˿�
	}PortInfo;


	class EnumPort
	{
	public:
		EnumPort(DWORD dwProcessID);
		virtual ~EnumPort(void);

	public:
		vector<PortInfo> &GetPortInfo()
		{
			return m_vecPortInfo;
		}
		

		void Clear();		// ���
		void Init();		// ��ʼ��
		void Enum();		// ��������ʱ��

	private:
		void GetPortStatus(PortInfo &portInfo,DWORD dwStatus);
		void GetPortLocalIP(PortInfo &portInfo,DWORD dwLocalIP);
		void GetPortLocalPort(PortInfo &portInfo,DWORD dwLocalPort);
		void GetPortRemoteIP(PortInfo &portInfo,DWORD dwRemoteIP);
		void GetPortRemotePort(PortInfo &portInfo,DWORD dwRemotePort);

	private:
		// ������չ����ָ��
		PFNAllocateAndGetTcpExTableFromStack m_pAllocateAndGetTcpExTableFromStack;
		PFNAllocateAndGetUdpExTableFromStack m_pAllocateAndGetUdpExTableFromStack;

		HMODULE					m_hModule;
		DWORD					m_dwPID;			// ָ������ID

		vector<PortInfo>		m_vecPortInfo;
	}; // end of class

}; // end of namespace 

#endif