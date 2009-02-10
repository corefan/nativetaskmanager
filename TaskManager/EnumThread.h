#ifndef __ENUM_THREAD__H
#define __ENUM_THREAD__H


#include <string>
#include <vector>
//#include <Winternl.h>

using namespace std;


#ifdef _UNICODE
#define string wstring
#endif

namespace enumThread
{
	typedef   enum   _THREADINFOCLASS   
	{   
		ThreadBasicInformation=0,   
		ThreadTimes,   
		ThreadPriority,   
		ThreadBasePriority,   
		ThreadAffinityMask,   
		ThreadImpersonationToken,   
		ThreadDescriptorTableEntry,   
		ThreadEnableAlignmentFaultFixup,   
		ThreadEventPair_Reusable,   
		ThreadQuerySetWin32StartAddress,   
		ThreadZeroTlsCell,   
		ThreadPerformanceCount,   
		ThreadAmILastThread,   
		ThreadIdealProcessor,   
		ThreadPriorityBoost,   
		ThreadSetTlsArrayAddress,   
		ThreadIsIoPending,   
		ThreadHideFromDebugger,   
		ThreadBreakOnTermination,   
		MaxThreadInfoClass   
	}THREADINFOCLASS; 


	typedef struct _CLIENT_ID   
	{   
		HANDLE   UniqueProcess;   
		HANDLE   UniqueThread;   
	}CLIENT_ID;

	typedef struct _THREAD_BASIC_INFORMATION   
	{   //   Information   Class   0   
		LONG		ExitStatus;   
		PVOID		TebBaseAddress;   
		CLIENT_ID   ClientId;   
		LONG		AffinityMask;   
		LONG		Priority;   
		LONG		BasePriority;   
	}THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;


	typedef struct tagThreadInfo
	{
		string	m_strThreadID;				// 线程ID
		int		m_nThreadID;				

		string	m_strCreateTime;			// 线程开启时间
		string	m_strKernelTime;			// 线程内核时间
		string	m_strUserTime;				// 线程用户时间

		string	m_strPriority;				// 线程优先级
		int		m_nPriority;
		string	m_strStartAddr;				// 线程起始地址及模块名

		string	m_strStatus;				// 线程状态
		int		m_nStatus;

		string	m_strSwitchCount;			// 线程上下文换次数
		
	}ThreadInfo;

	class EnumThread
	{
	public:
		EnumThread(int nPID=0);
		virtual ~EnumThread(void);

		void Clear();
		void Init();
		void Enum();

		vector<ThreadInfo> &GetThreadInfo();
	private:
		void GetThreadID(ThreadInfo &threadInfo,DWORD dwThreadID);			// 获取线程ID
		void GetThreadTime(ThreadInfo &threadInfo,DWORD dwThreadID);		// 获取线程时间
		void GetTheadPriority(ThreadInfo &threadInfo,DWORD dwThreadID);		// 获取线程优先级
		void GetThreadStartAddress(ThreadInfo &threadInfo,DWORD dwThreadID);// 获取线程起始地址及模块
	private:
		vector<ThreadInfo>	m_vecThreadInfo;			// 线程信息容器
		int					m_nPID;						// 线程的父进程ID
	};		// end of class

};		// end of enumThread

#endif