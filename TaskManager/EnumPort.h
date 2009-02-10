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
		DWORD   dwState;        	// 连接状态
		DWORD   dwLocalAddr;    	// 本地地址
		DWORD   dwLocalPort;    	// 本地端口
		DWORD   dwRemoteAddr;   	// 远程地址
		DWORD   dwRemotePort;   	// 远程端口
		DWORD	dwProcessId;		// 进程ID号
	} MIB_TCPEXROW, *PMIB_TCPEXROW;

	typedef struct 
	{
		DWORD			dwNumEntries;
		MIB_TCPEXROW	table[ANY_SIZE];
	} MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;

	typedef struct 
	{
		DWORD   dwLocalAddr;    	// 本地地址
		DWORD   dwLocalPort;    	// 本地端口
		DWORD	dwProcessId;		// 进程ID号
	} MIB_UDPEXROW, *PMIB_UDPEXROW;


	typedef struct 
	{
		DWORD			dwNumEntries;
		MIB_UDPEXROW	table[ANY_SIZE];
	} MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;


	// 扩展函数原型
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
		string	m_strType;				// 链接类型
		string	m_strStatus;			// 链接状态
		string	m_strLocalIP;			// 链接本地IP
		string	m_strLocalPort;			// 链接本地端口
		string	m_strRemoteIP;			// 链接远程IP
		string	m_strRemotePort;		// 链接远程端口
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
		

		void Clear();		// 清空
		void Init();		// 初始化
		void Enum();		// 遍历进程时间

	private:
		void GetPortStatus(PortInfo &portInfo,DWORD dwStatus);
		void GetPortLocalIP(PortInfo &portInfo,DWORD dwLocalIP);
		void GetPortLocalPort(PortInfo &portInfo,DWORD dwLocalPort);
		void GetPortRemoteIP(PortInfo &portInfo,DWORD dwRemoteIP);
		void GetPortRemotePort(PortInfo &portInfo,DWORD dwRemotePort);

	private:
		// 定义扩展函数指针
		PFNAllocateAndGetTcpExTableFromStack m_pAllocateAndGetTcpExTableFromStack;
		PFNAllocateAndGetUdpExTableFromStack m_pAllocateAndGetUdpExTableFromStack;

		HMODULE					m_hModule;
		DWORD					m_dwPID;			// 指定进程ID

		vector<PortInfo>		m_vecPortInfo;
	}; // end of class

}; // end of namespace 

#endif