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
		string	m_strThreadID;				// �߳�ID
		int		m_nThreadID;				

		string	m_strCreateTime;			// �߳̿���ʱ��
		string	m_strKernelTime;			// �߳��ں�ʱ��
		string	m_strUserTime;				// �߳��û�ʱ��

		string	m_strPriority;				// �߳����ȼ�
		int		m_nPriority;
		string	m_strStartAddr;				// �߳���ʼ��ַ��ģ����

		string	m_strStatus;				// �߳�״̬
		int		m_nStatus;

		string	m_strSwitchCount;			// �߳������Ļ�����
		
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
		void GetThreadID(ThreadInfo &threadInfo,DWORD dwThreadID);			// ��ȡ�߳�ID
		void GetThreadTime(ThreadInfo &threadInfo,DWORD dwThreadID);		// ��ȡ�߳�ʱ��
		void GetTheadPriority(ThreadInfo &threadInfo,DWORD dwThreadID);		// ��ȡ�߳����ȼ�
		void GetThreadStartAddress(ThreadInfo &threadInfo,DWORD dwThreadID);// ��ȡ�߳���ʼ��ַ��ģ��
	private:
		vector<ThreadInfo>	m_vecThreadInfo;			// �߳���Ϣ����
		int					m_nPID;						// �̵߳ĸ�����ID
	};		// end of class

};		// end of enumThread

#endif