#include "StdAfx.h"
#include "Toolhelp.h"
#include "EnumThread.h"
#include "AdjustPrivilege.h"
#include "StackWalker.h"
#include "NtSystemInfo.h"

using namespace enumThread;


#ifdef _UNICODE
#define wmemset memset
#endif

extern "C" LONG (__stdcall *ZwQueryInformationThread)
		(   
			IN	HANDLE				ThreadHandle,   
			IN	THREADINFOCLASS		ThreadInformationClass,   
			OUT	PVOID				ThreadInformation,   
			IN	ULONG				ThreadInformationLength,   
			OUT	PULONG				ReturnLength				OPTIONAL   
		)=NULL; 


EnumThread::EnumThread(int nPID/*nPID=0*/)
	: m_nPID(nPID)
{
}

EnumThread::~EnumThread(void)
{
	Clear();
}

void EnumThread::Init()
{

	HINSTANCE hNtDLL=::GetModuleHandle(_T("ntdll"));   

	(FARPROC &)ZwQueryInformationThread=::GetProcAddress(hNtDLL,"ZwQueryInformationThread");

	::FreeLibrary(hNtDLL);


	{
		using namespace NT;
		SystemThreadInformation threadInfo(m_nPID);
		vector<SYSTEM_THREAD> vecThread=threadInfo.GetThreadInfo();

		string strTemp;
		TCHAR szTemp[16]={0};
		for(vector<SYSTEM_THREAD>::iterator iter=vecThread.begin(); iter!=vecThread.end(); ++iter)
		{
			ThreadInfo threadInfo;

			GetThreadID(threadInfo,(DWORD)iter->ClientId.UniqueThread);			// 获取线程ID
			GetThreadTime(threadInfo,(DWORD)iter->ClientId.UniqueThread);		// 获取线程时间
			GetTheadPriority(threadInfo,(DWORD)iter->ClientId.UniqueThread);		// 获取线程优先级
			GetThreadStartAddress(threadInfo,(DWORD)iter->ClientId.UniqueThread);// 获取线程起始地址和模块

			
			// 获取线程状态
			switch(iter->State)
			{
			case StateInitialized:
				strTemp=_T("初始化:");
				break;
			case StateReady:
				strTemp=_T("准备:");;
				break;
			case StateRunning:
				strTemp=_T("运行:");;
				break;
			case StateStandby:
				strTemp=_T("挂起:");;
				break;
			case StateTerminated:
				strTemp=_T("关闭:");;
				break;
			case StateWait:
				strTemp=_T("等待:");;
				break;
			case StateTransition:
				strTemp=_T("转变:");;
				break;
			case StateUnknown:
				strTemp=_T("未知:");;
				break;
			default:
				strTemp=_T("未知:");;
				break;
			}
		
			switch(iter->WaitReason)
			{
			case Executive:
				strTemp+=_T("Executive");
				break;
			case FreePage:
				strTemp+=_T("FreePage");
				break;
			case PageIn:
				strTemp+=_T("PageIn");
				break;
			case PoolAllocation:
				strTemp+=_T("PoolAllocation");
				break;
			case DelayExecution:
				strTemp+=_T("DelayExecution");
				break;
			case Suspended:
				strTemp+=_T("Suspended");
				break;
			case UserRequest:
				strTemp+=_T("UserRequest");
				break;
			case WrExecutive:
				strTemp+=_T("WrExecutive");
				break;
			case WrFreePage:
				strTemp+=_T("WrFreePage");
				break;
			case WrPageIn:
				strTemp+=_T("WrPageIn");
				break;
			case WrPoolAllocation:
				strTemp+=_T("WrPoolAllocation");
				break;
			case WrDelayExecution:
				strTemp+=_T("WrDelayExecution");
				break;
			case WrSuspended:
				strTemp+=_T("WrSuspended");
				break;
			case WrUserRequest:
				strTemp+=_T("WrUserRequest");
				break;
			case WrEventPair:
				strTemp+=_T("WrEventPair");
				break;
			case WrQueue:
				strTemp+=_T("WrQueue");
				break;
			case WrLpcReceive:
				strTemp+=_T("WrLpcReceive");
				break;
			case WrLpcReply:
				strTemp+=_T("WrLpcReply");
				break;
			case WrVertualMemory:
				strTemp+=_T("WrVertualMemory");
				break;
			case WrPageOut:
				strTemp+=_T("WrPageOut");
				break;
			case WrRendezvous:
				strTemp+=_T("WrRendezvous");
				break;
			case Spare2:
				strTemp+=_T("Spare2");
				break;
			case Spare3:
				strTemp+=_T("Spare3");
				break;
			case Spare4:
				strTemp+=_T("Spare4");
				break;
			case Spare5:
				strTemp+=_T("Spare5");
				break;
			case Spare6:
				strTemp+=_T("Spare6");
				break;
			case WrKernel:
				strTemp+=_T("WrKernel");
				break;
			default:
				strTemp+=_T("Unknown");
				break;
			}
			threadInfo.m_strStatus=strTemp;
			threadInfo.m_nStatus=iter->WaitReason;
			
			// 获取线程上下文切换次数
			wsprintf(szTemp,_T("%d"),iter->ContextSwitchCount);
			threadInfo.m_strSwitchCount=szTemp;

			m_vecThreadInfo.push_back(threadInfo);
		}
	}

}

void EnumThread::Clear()
{
	m_vecThreadInfo.clear();
}


void EnumThread::Enum()
{
	// 清空
	Clear();

	Init();

	/*
	THREADENTRY32 th32={sizeof(th32)};
	ToolHelp theThread(TH32CS_SNAPTHREAD,m_nPID);

	for(BOOL bOK=theThread.ThreadFirst(&th32); bOK; bOK=theThread.ThreadNext(&th32))
	{
		if( th32.th32OwnerProcessID==m_nPID )
		{
			ThreadInfo threadInfo;

			GetThreadID(threadInfo,th32.th32ThreadID);			// 获取线程ID
			GetThreadTime(threadInfo,th32.th32ThreadID);		// 获取线程时间
			GetTheadPriority(threadInfo,th32.th32ThreadID);		// 获取线程优先级
			GetThreadStartAddress(threadInfo,th32.th32ThreadID);// 获取线程起始地址和模块

			m_vecThreadInfo.push_back(threadInfo);
		}
	}
	*/

}

vector<ThreadInfo> &EnumThread::GetThreadInfo()
{
	return m_vecThreadInfo;
}


void EnumThread::GetThreadID(ThreadInfo &threadInfo,DWORD dwThreadID)
{
	TCHAR szTemp[16]={0};
	wsprintf(szTemp,_T("%d"),dwThreadID);

	threadInfo.m_nThreadID=dwThreadID;
	threadInfo.m_strThreadID=szTemp;
}

void EnumThread::GetThreadTime(ThreadInfo &threadInfo,DWORD dwThreadID)
{
	TCHAR szTemp[32]={0};
	HANDLE hThread=NULL;

	hThread=::OpenThread(THREAD_QUERY_INFORMATION,FALSE,dwThreadID);
	if( hThread==NULL )
		return;

	// 获取创建时间 内核时间 用户时间
	FILETIME tCreate,tExit,tKernel,tUser;
	FILETIME tLocalCreate,tLocalKernel,tLocalUser;
	SYSTEMTIME sysCreate,sysKernel,sysUser;
	::GetThreadTimes(hThread,&tCreate,&tExit,&tKernel,&tUser);			// 获取线程相关时间
	::CloseHandle(hThread);

	::FileTimeToLocalFileTime(&tCreate,&tLocalCreate);
	::FileTimeToLocalFileTime(&tKernel,&tLocalKernel);
	::FileTimeToLocalFileTime(&tUser,&tLocalUser);

	::FileTimeToSystemTime(&tLocalCreate,&sysCreate);						// 转换为系统时间
	::FileTimeToSystemTime(&tKernel,&sysKernel);
	::FileTimeToSystemTime(&tUser,&sysUser);

	wsprintf(szTemp,_T("%02d/%02d/%02d %02d:%02d:%02d"),sysCreate.wYear,sysCreate.wMonth,sysCreate.wDay,
		sysCreate.wHour,sysCreate.wMinute,sysCreate.wSecond);
	threadInfo.m_strCreateTime=szTemp;

	memset(szTemp,0,32);
	wsprintf(szTemp,_T("%02d:%02d:%02d.%03d"),sysKernel.wHour,sysKernel.wMinute,sysKernel.wSecond,
		sysKernel.wMilliseconds);
	threadInfo.m_strKernelTime=szTemp;

	memset(szTemp,0,32);
	wsprintf(szTemp,_T("%02d:%02d:%02d.%03d"),sysUser.wHour,sysUser.wMinute,sysUser.wSecond,
		sysUser.wMilliseconds);
	threadInfo.m_strUserTime=szTemp;
}

void EnumThread::GetTheadPriority(ThreadInfo &threadInfo,DWORD dwThreadID)
{
	TCHAR szTemp[32]={0};
	HANDLE hThread=NULL;

	hThread=::OpenThread(THREAD_QUERY_INFORMATION,FALSE,dwThreadID);
	if( hThread==NULL )
		return;

	threadInfo.m_nPriority=::GetThreadPriority(hThread);
	if( threadInfo.m_nPriority==THREAD_PRIORITY_ERROR_RETURN )
	{
		::CloseHandle(hThread);
		return;
	}
	
	::CloseHandle(hThread);

	switch(threadInfo.m_nPriority)
	{
		case THREAD_PRIORITY_NORMAL:
			lstrcpy(szTemp,_T("正常"));
			break;
		case THREAD_PRIORITY_ABOVE_NORMAL:
			lstrcpy(szTemp,_T("高于正常"));
			break;
		case THREAD_PRIORITY_HIGHEST:
			lstrcpy(szTemp,_T("高"));
			break;
		case THREAD_PRIORITY_TIME_CRITICAL:
			lstrcpy(szTemp,_T("很高"));
			break;
		case THREAD_PRIORITY_BELOW_NORMAL:
			lstrcpy(szTemp,_T("低于正常"));
			break;
		case THREAD_PRIORITY_LOWEST:
			lstrcpy(szTemp,_T("低"));
			break;
		case THREAD_PRIORITY_IDLE:
			lstrcpy(szTemp,_T("空闲"));
			break;

		default:
			lstrcpy(szTemp,_T("未知"));
			break;
	}
	
	threadInfo.m_strPriority=szTemp;
}

// 获取线程起始地址和模块信息
void EnumThread::GetThreadStartAddress(ThreadInfo &threadInfo,DWORD dwThreadID)
{
	LPVOID	lpStartAddr			=0;
	LONG	lStatus				=0;
	HANDLE hThread=NULL,hProcess=NULL;
	THREAD_BASIC_INFORMATION tbi={0};
	TCHAR szTemp[MAX_PATH]={0};

	hThread=::OpenThread(THREAD_ALL_ACCESS,FALSE,dwThreadID);
	if( hThread==NULL )
		return;
	
	lStatus=ZwQueryInformationThread(hThread,ThreadQuerySetWin32StartAddress,&lpStartAddr,sizeof(lpStartAddr),NULL);
	if( lStatus<0 )
	{
		// 访问出错
		::CloseHandle(hThread);
		return;
	}

	_stprintf_s(szTemp,MAX_PATH,_T("0x%p"),lpStartAddr);

	lStatus=ZwQueryInformationThread(hThread,ThreadBasicInformation,&tbi,sizeof(tbi),NULL);

	hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,(DWORD)tbi.ClientId.UniqueProcess);
	if( hProcess==NULL )
	{
		::CloseHandle(hThread);
		return;
	}
	

	if( ::GetMappedFileName(hProcess,&lpStartAddr,szTemp,MAX_PATH) ) // 成功
	{
		//lstrcpy(szTemp,_tcsrchr(szTemp,_T('\\'))+1);
		//_stprintf(szTemp,_T("")
		
	}
	else
	{
		int nError=::GetLastError();
		int nLastError=0;
	}


	::CloseHandle(hThread);
	::CloseHandle(hProcess);

	threadInfo.m_strStartAddr=szTemp;
}