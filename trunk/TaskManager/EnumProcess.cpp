#include "stdafx.h"
#include "EnumProcess.h"
#include "ToolHelp.h"
#include "ProcessCPUUsage.h"
#include "NtSystemInfo.h"
#include "EnumWTS.h"
#include "GetCPUUsage.h"
#include <iostream>
#include <sstream>

#ifdef _UNICODE
#define ostringstream wostringstream
#define memset	wmemset
#endif


using namespace enumProcess;

EnumProcess::EnumProcess()
{
	CPUUsage::PerfDataInitialize();	
}

EnumProcess::EnumProcess(const EnumProcess &src)
{
	*this=src;
}

EnumProcess::~EnumProcess()
{
	CPUUsage::PerfDataUninitialize();

	Clear();
}


void EnumProcess::Clear()
{
	// 销毁icon句柄
	for(TvecProInfo::iterator iter=m_vecProInfo.begin(); iter!=m_vecProInfo.end(); ++iter)
	{
		if( iter->m_hIcon!=NULL )
			DestroyIcon(iter->m_hIcon);
	}

	m_vecProInfo.clear();
	m_mapProcessTree.clear();

	TvecProInfo veTemp;
	veTemp.swap( m_vecProInfo );
}

void EnumProcess::Init()
{
	using namespace NT;

	SystemProcessInformation sysProcess;

	map<DWORD,NT::SYSTEM_PROCESS_INFORMATION *>::const_iterator iter=
		sysProcess.m_mapProcessInfo.begin();

	ProcessInfo proInfo;
	proInfo.m_bSuspend			=FALSE;
	while(iter!=sysProcess.m_mapProcessInfo.end() )
	{
		// 进程信息
		//proInfo.m_strProcessName=iter->second->ProcessName.Buffer;
		GetProcessID(proInfo,iter->second->ProcessId);
		proInfo.m_dwID				=iter->second->ProcessId;	
		proInfo.m_dwParentID		=iter->second->InheritedFromProcessId;


		++iter;
		m_vecProInfo.push_back(proInfo);
	}
	
}

void EnumProcess::Enum()
{
	// 清空
	Clear();
	// 初始化
	//Init();



	// 遍历进程信息
	PROCESSENTRY32 pe={sizeof(pe)};
	ToolHelp theProcess(TH32CS_SNAPPROCESS);

	ProcessInfo proInfo;
	proInfo.m_bSuspend			=FALSE;							// Process State

	CPUUsage::GetAllProcCPUUsage();								// CPU Usage

	for(BOOL bProcessOk=theProcess.ProcessFirst(&pe); bProcessOk; bProcessOk=theProcess.ProcessNext(&pe))
	{
		m_mapProcessTree.insert(make_pair(pe.th32ParentProcessID,pe.th32ProcessID));	// 子进程容器

		// 进程信息
		proInfo.m_strProcessName=pe.szExeFile;						// Process Name

		GetProcessID(proInfo,pe.th32ProcessID);						// Process ID
		proInfo.m_dwID				=pe.th32ProcessID;	
		proInfo.m_dwParentID		=pe.th32ParentProcessID;		// Process Parent ID
		

		GetCPUUsage(proInfo,proInfo.m_dwID);						// CPU Usage
		GetCPUTime(proInfo,proInfo.m_dwID);						// CPU Time
		
		GetMemInfo(proInfo,proInfo.m_dwID);						// Memory Info
		GetResourceObject(proInfo,proInfo.m_dwID);				// Resource Info
		GetProcessSessionID(proInfo,proInfo.m_dwID);				// Session ID
		GetProcessHandleCount(proInfo,proInfo.m_dwID);			// Handle count

		GetProcessIOInfo(proInfo,proInfo.m_dwID);					// IO Information

		GetProcessPriority(proInfo,proInfo.m_dwID);				// Process Priority

		GetProcessPath(proInfo,proInfo.m_dwID);					// Process Path
		GetThreadCount(proInfo,pe.cntThreads);						// Process Thread Count
		GetProcessUserName(proInfo,proInfo.m_dwID);			// Process User Name
		GetProcessIcon(proInfo,proInfo.m_strProcessPath.c_str());	// Process Icon

		m_vecProInfo.push_back(proInfo);
	}
	
}

void EnumProcess::GetProcessID(ProcessInfo &proInfo,DWORD dwID)
{
	string strID;
	
	ostringstream os;
	os << dwID << endl;
	strID=os.str();

	proInfo.m_strProcessID=strID;
}

void EnumProcess::GetCPUUsage(ProcessInfo &proInfo,DWORD dwID)
{
	int nUsage=CPUUsage::PerfDataGetCPUUsage(dwID);
	static TCHAR szTemp[32]={0};
	wsprintf(szTemp,_T("%02d"),nUsage);
	proInfo.m_strCPUUsage=szTemp;

}

void EnumProcess::GetCPUTime(ProcessInfo &proInfo,DWORD dwID)
{
	HANDLE hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,dwID);
	if( hProcess==NULL )
		return;

	static FILETIME tCreate,tExit,tKernel,tUser,tCPU;
	static SYSTEMTIME sysCPU;

	::GetProcessTimes(hProcess,&tCreate,&tExit,&tKernel,&tUser);
	::CloseHandle(hProcess);

	tCPU.dwHighDateTime	=tKernel.dwHighDateTime	+ tUser.dwHighDateTime;
	tCPU.dwLowDateTime	=tKernel.dwLowDateTime	+ tUser.dwLowDateTime;

	::FileTimeToSystemTime(&tCPU,&sysCPU);
	
	static TCHAR szCPUTime[32]={0};
	wsprintf(szCPUTime,_T("%02d:%02d:%02d"),sysCPU.wHour,sysCPU.wMinute,sysCPU.wSecond);

	proInfo.m_strCPUTime=szCPUTime;
}

// 获取进程中内存详细信息
void EnumProcess::GetMemInfo(ProcessInfo &proInfo,DWORD dwID)
{
	HANDLE hProcess=NULL;
	PROCESS_MEMORY_COUNTERS pmc={0};

	hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, dwID );
    if( NULL == hProcess )
        return;

	static TCHAR szMemoryInfo[32]={0};


	if( ::GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) )
    {	
		FormatNumber((DWORD)pmc.WorkingSetSize>>10,szMemoryInfo,_T(','),3);
		//wsprintf(szMemoryInfo,_T("%d K"),pmc.WorkingSetSize>>10);		// 占用物理内存
		proInfo.m_strMemUsage=szMemoryInfo;
		proInfo.m_strMemUsage+=_T(" K");

		//memset(szMemoryInfo,0,32);
		//FormatNumber(pmc.WorkingSetSize>>10,szMemoryInfo,_T(","),3);
		//wsprintf(szMemoryInfo,_T("%d K"),0);							// 占用物理内存增量（需要计算）
		//proInfo.m_strMemDelta=szMemoryInfo;
		//proInfo.m_strMemUsage+=_T(" K");

		memset(szMemoryInfo,0,32);
		FormatNumber((DWORD)pmc.PeakWorkingSetSize>>10,szMemoryInfo,_T(','),3);
		//wsprintf(szMemoryInfo,_T("%d K"),pmc.PeakWorkingSetSize>>10);	// 占用物理内存最大值
		proInfo.m_strMemPeakUsage=szMemoryInfo;
		proInfo.m_strMemPeakUsage+=_T(" K");

		memset(szMemoryInfo,0,32);
		FormatNumber((DWORD)pmc.PagefileUsage>>10,szMemoryInfo,_T(','),3);
		//wsprintf(szMemoryInfo,_T("%d K"),pmc.PagefileUsage>>10);		// 占用的虚拟内存
		proInfo.m_strPageSize=szMemoryInfo;
		proInfo.m_strPageSize+=_T(" K");

		//memset(szMemoryInfo,0,32);
		//FormatNumber(pmc.PeakPagefileUsage>>10,szMemoryInfo,_T(","),3);
		////wsprintf(szMemoryInfo,_T("%d K"),pmc.PeakPagefileUsage>>10);	// 占用的虚拟内存最大值
		//proInfo.m_strVir=szMemoryInfo;
		//proInfo.PeakPagefileUsage+=_T(" K");

		memset(szMemoryInfo,0,32);
		FormatNumber(pmc.PageFaultCount>>10,szMemoryInfo,_T(','),3);
		//wsprintf(szMemoryInfo,_T("%d K"),pmc.PageFaultCount>>10);		// 错误页面数
		proInfo.m_strPageFaults=szMemoryInfo;
		proInfo.m_strPageFaults+=_T(" K");

		memset(szMemoryInfo,0,32);
		FormatNumber((DWORD)pmc.QuotaPagedPoolUsage>>10,szMemoryInfo,_T(','),3);
		//wsprintf(szMemoryInfo,_T("%d K"),pmc.QuotaPagedPoolUsage>>10);	// 占用的页面池
		proInfo.m_strPagePoolUsage=szMemoryInfo;
		proInfo.m_strPagePoolUsage+=_T(" K");

		//memset(szMemoryInfo,0,32);
		//FormatNumber(pmc.QuotaPeakPagedPoolUsage>>10,szMemoryInfo,_T(","),3);
		////wsprintf(szMemoryInfo,_T("%d K"),pmc.QuotaPeakPagedPoolUsage>>10);	// 占用的页面池最大值
		//proInfo.m_strPagePoolUsage=szMemoryInfo;
		//proInfo.QuotaPeakPagedPoolUsage+=_T(" K");

		memset(szMemoryInfo,0,32);
		FormatNumber((DWORD)pmc.QuotaNonPagedPoolUsage>>10,szMemoryInfo,_T(','),3);
		//wsprintf(szMemoryInfo,_T("%d K"),pmc.QuotaNonPagedPoolUsage>>10);	// 占用的非页面池
		proInfo.m_strPageNonPollUsage=szMemoryInfo;
		proInfo.m_strPageNonPollUsage+=_T(" K");

		//memset(szMemoryInfo,0,32);
		//FormatNumber(pmc.QuotaPeakNonPagedPoolUsage>>10,szMemoryInfo,_T(","),3);
		////wsprintf(szMemoryInfo,_T("%d K"),pmc.QuotaPeakNonPagedPoolUsage>>10);// 占用的非页面池最大值
		//proInfo.QuotaPeakNonPagedPoolUsage=szMemoryInfo;
		//proInfo.QuotaPeakNonPagedPoolUsage+=_T(" K");
	}

	::CloseHandle(hProcess);
}

// 获取进程中资源对象数
void EnumProcess::GetResourceObject(ProcessInfo &proInfo,DWORD dwID)
{
	HANDLE hProcess=NULL;
	static TCHAR szTemp[16]={0};

	hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,dwID);
	if( hProcess==NULL )
		return;

	int nUserObject=0,nGDIObject=0;
	nUserObject	=::GetGuiResources(hProcess,GR_USEROBJECTS);	// 获取User Object	
	nGDIObject	=::GetGuiResources(hProcess,GR_GDIOBJECTS);		// 获取GDI Object

	::CloseHandle(hProcess);

	wsprintf(szTemp,_T("%d"),nUserObject);
	proInfo.m_strUserObject	=szTemp;

	memset(szTemp,0,16);
	wsprintf(szTemp,_T("%d"),nGDIObject);
	proInfo.m_strGDIObject	=szTemp;
}

void EnumProcess::GetProcessSessionID(ProcessInfo &proInfo, DWORD dwID)
{
	HANDLE hProcess=NULL;
	static TCHAR szTemp[16]={0};

	hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,dwID);
	if( hProcess==NULL )
		return;

	DWORD dwSessionID=0;
	//static WTS::EnumWTS wts;

	//wts.WTSEnumerateProcesses();

	//if( !wts.ProcessIdToSessionId(dwID,&dwSessionID) )
	//	dwSessionID=-1;

	if( !::ProcessIdToSessionId(dwID,&dwSessionID) )
		dwSessionID=-1;

	::CloseHandle(hProcess);
	
	wsprintf(szTemp,_T("%ld"),dwSessionID);
	proInfo.m_strSessionID=szTemp;
}

void EnumProcess::GetProcessHandleCount(ProcessInfo &proInfo, DWORD dwID)
{
	HANDLE hProcess=NULL;
	static TCHAR szTemp[16]={0};

	hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,dwID);
	if( hProcess==NULL )
		return;

	DWORD dwHandleCount=0;
	::GetProcessHandleCount(hProcess,&dwHandleCount);
	::CloseHandle(hProcess);
	
	wsprintf(szTemp,_T("%ld"),dwHandleCount);
	proInfo.m_strHandleCount=szTemp;
}

// 获取IO运行信息
void EnumProcess::GetProcessIOInfo(ProcessInfo &proInfo, DWORD dwID)
{
	HANDLE hProcess=NULL;
	static TCHAR szTemp[16]={0};

	hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,dwID);
	if( hProcess==NULL )
		return;

	IO_COUNTERS ioInfo={0};
	::GetProcessIoCounters(hProcess,&ioInfo);
	::CloseHandle(hProcess);

	wsprintf(szTemp,_T("%ld "),ioInfo.ReadOperationCount);
	proInfo.m_strIORead=szTemp;

	memset(szTemp,0,16);
	wsprintf(szTemp,_T("%ld K"),ioInfo.ReadTransferCount>>10);
	proInfo.m_strIOReadByte=szTemp;

	memset(szTemp,0,16);
	wsprintf(szTemp,_T("%ld "),ioInfo.WriteOperationCount);
	proInfo.m_strIOWrite=szTemp;

	memset(szTemp,0,16);
	wsprintf(szTemp,_T("%ld K"),ioInfo.WriteTransferCount>>10);
	proInfo.m_strIOWriteByte=szTemp;

	memset(szTemp,0,16);
	wsprintf(szTemp,_T("%ld "),ioInfo.OtherOperationCount);
	proInfo.m_strIOOther=szTemp;

	memset(szTemp,0,16);
	wsprintf(szTemp,_T("%ld K"),ioInfo.OtherTransferCount>>10);
	proInfo.m_strIOOtherByte=szTemp;
}

// 获取进程路径
void EnumProcess::GetProcessPath(ProcessInfo &proInfo,DWORD dwID)
{
	HANDLE hProcess=NULL;
	static TCHAR szPath[MAX_PATH]={0};
	static TCHAR szWin32Path[MAX_PATH]={0};
	
	hProcess=::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | TOKEN_ADJUST_PRIVILEGES,
		FALSE, dwID );

	::GetModuleFileNameEx(hProcess,NULL,szPath,MAX_PATH-1);		// 获取进程路径
	
	if( hProcess!=NULL )
		::CloseHandle(hProcess);

	/*
	// 排除以外路径
	if( szPath[0]==_T('\\') )
	{
		CString strPath(szPath);
		strPath=strPath.Mid(strPath.ReverseFind(_T('\\'))+1);

		strPath=_T("C:\\Windows\\system32\\")+strPath;

		proInfo.m_strProcessPath=strPath;
	}
	else
	{
		proInfo.m_strProcessPath=szPath;
	}
	*/
	
	// 判断路径并转换
	TranslateFilename(szPath,szWin32Path);
	proInfo.m_strProcessPath=szWin32Path;

	
}

// 获取进程优先级
void EnumProcess::GetProcessPriority(ProcessInfo &proInfo,DWORD dwID)
{
	HANDLE hProcess=NULL;
	
	hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION,FALSE, dwID );
	if( hProcess==NULL )
		return;

	DWORD dwPriority=::GetPriorityClass(hProcess);
	::CloseHandle(hProcess);

	string strTemp;
	switch(dwPriority)
	{
	case REALTIME_PRIORITY_CLASS:
		strTemp=_T("实时");
		break;
	case HIGH_PRIORITY_CLASS:
		strTemp=_T("高");
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		strTemp=_T("较高");
		break;
	case NORMAL_PRIORITY_CLASS:
		strTemp=_T("正常");
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		strTemp=_T("较低");
		break;
	case IDLE_PRIORITY_CLASS:
		strTemp=_T("低");
		break;
	default:
		strTemp=_T("错误");
		break;
	}

	proInfo.m_strPriority	=strTemp;
	proInfo.m_dwPriority	=dwPriority;
}

// 获取进程线程数
void EnumProcess::GetThreadCount(ProcessInfo &proInfo,DWORD dwCount)
{
	string strThreadCount;
	
	ostringstream os;
	os << dwCount << endl;
	strThreadCount=os.str();

	proInfo.m_strThreadCount=strThreadCount;				// 获取进程包含线程数量
}

// 获取进程用户名
void EnumProcess::GetProcessUserName(ProcessInfo &proInfo,DWORD dwID)
{
	/*
	HANDLE hProcess	=NULL;
	HANDLE hToken	=NULL;
	BOOL bResult	=FALSE;
	DWORD dwSize	=0;

	static TCHAR szUserName[MAX_PATH]	={0};
	static TCHAR szDomain[MAX_PATH]		={0};
	DWORD dwDomainSize					=MAX_PATH;
	DWORD dwNameSize					=MAX_PATH;

	SID_NAME_USE    SNU;
	PTOKEN_USER pTokenUser=NULL;
	__try
	{ 
		hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | TOKEN_ADJUST_PRIVILEGES,
			FALSE,dwID);

		if( hProcess==NULL )
			__leave;

		if( !::OpenProcessToken(hProcess,TOKEN_QUERY,&hToken) )
		{
			int nErr=::GetLastError();
			bResult = FALSE;
			__leave;
		}

		if( !::GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize) )
		{
			if( ::GetLastError() != ERROR_INSUFFICIENT_BUFFER )
			{
				bResult = FALSE; 
				__leave;
			}
		}

		pTokenUser = NULL;
		pTokenUser = (PTOKEN_USER)malloc(dwSize);
		if( pTokenUser == NULL )
		{
			bResult = FALSE;
			__leave;
		}

		if( !::GetTokenInformation(hToken,TokenUser,pTokenUser,dwSize,&dwSize) )
		{
			bResult = FALSE;
			__leave;
		}

		if( ::LookupAccountSid(NULL,pTokenUser->User.Sid,szUserName,&dwNameSize,szDomain,&dwDomainSize,&SNU) != 0 )
		{
			proInfo.m_strUserName+=_T("\\\\");
			proInfo.m_strUserName+=szDomain;
			proInfo.m_strUserName+=_T("\\");
			proInfo.m_strUserName+=szUserName;
		}
	}
	__finally
	{
		if( hProcess!=NULL )
			::CloseHandle(hProcess);

		if( hToken!=NULL )
			::CloseHandle(hToken);

		if( pTokenUser!=NULL )
			free(pTokenUser);
	}
	*/
	
	static WTS::EnumWTS wts;
	static BOOL bFirst=TRUE;

	if( bFirst )
	{
		wts.WTSEnumerateProcesses();
		bFirst=FALSE;
	}

	proInfo.m_strUserName=wts.GetProcessUserName(dwID);
	
}

void EnumProcess::GetProcessIcon(ProcessInfo &proInfo,LPCTSTR lpszPath)
{
	SHFILEINFO shfi={0};

	__try
	{
		// 获取图标
		int nRet=(int)SHGetFileInfo(lpszPath, 0,&shfi, sizeof(shfi), SHGFI_ICON );
		if( nRet>0 )
			__leave;
	}
	__finally
	{
		
	}

	proInfo.m_hIcon=shfi.hIcon;
}

// 内部转换路径函数
void EnumProcess::TranslateFilename(LPCTSTR szFilename, LPTSTR szWin32Name)
{
// sanity checks
   if (szFilename == NULL)
   {
      szWin32Name = _T("");
      return;
   }
   if (szFilename[0] == _T('\0'))
   {
      szWin32Name = _T("");
      return;
   }

// check for "strange" filenames
   LPCTSTR pszInfo = _tcsstr(szFilename, _T("\\SystemRoot\\"));
   if (pszInfo == szFilename)
   {
   //       \SystemRoot\System32\smss.exe
   // -->   c:\winnt\System32\smss.exe  using GetWindowsDirectory()
      UINT Len = ::GetWindowsDirectory(szWin32Name, MAX_PATH);
      if (Len != 0)
      {
         lstrcat(szWin32Name, _T("\\"));
         lstrcat(szWin32Name, &szFilename[lstrlen(_T("\\SystemRoot\\"))]);

         return;
      }
   }
   else
   {
   //       \??\C:\WINNT\system32\winlogon.exe
   // -->   C:\WINNT\system32\winlogon.exe  
      pszInfo = _tcsstr(szFilename, _T("\\??\\"));
      if (pszInfo == szFilename)
      {
         lstrcpy(szWin32Name, &szFilename[_tcslen(_T("\\??\\"))]);
      }
      else
         lstrcpy(szWin32Name, szFilename);

      return;
   }

// standard name
   lstrcpy(szWin32Name, szFilename);
}


// 内部数据格式转换函数
void EnumProcess::FormatNumber(DWORD dwNumber, TCHAR *szNumber, TCHAR cSeparator, DWORD GroupLength)
{
   LPTSTR   pFirst      = szNumber;
   LPTSTR   pChar       = pFirst;
   DWORD    DigitCount  = 0;
   DWORD    wNumChars   = 0;

   while (dwNumber > 0)
   {
   // extract the last digit
      wsprintf(pChar, _T("%u"), dwNumber % 10);

   // next character in the buffer
      pChar++;

   // next digit in the current group
      DigitCount++;

   // eat the last digit
      dwNumber = dwNumber / 10;

   // add the separator if needed
      if ((DigitCount == GroupLength) && (dwNumber > 0))
      {
         *pChar = cSeparator;

      // begin a new digit group after the separator
         DigitCount = 0;
         pChar++;
      }
   };

// close the string
   *pChar = _T('\0');

// reverse it
   _tcsrev(pFirst);
}