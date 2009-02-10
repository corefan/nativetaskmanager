/**
	作者:陈煜
	日期:2008/4/26
**/
#include "stdafx.h"
#include "ToolHelp.h"


ToolHelp::ToolHelp(DWORD dwFlag, DWORD dwProcessID)
{
	m_hSnapshot=INVALID_HANDLE_VALUE;
	CreateSnapshot(dwFlag,dwProcessID);
}

ToolHelp::~ToolHelp()
{
	if( m_hSnapshot != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hSnapshot);
	}
}


BOOL ToolHelp::CreateSnapshot(DWORD dwFlag, DWORD dwProcessID)
{
	if( m_hSnapshot != INVALID_HANDLE_VALUE )
	{
		CloseHandle(m_hSnapshot);
	}

	if( dwFlag==0 )
	{
		m_hSnapshot=INVALID_HANDLE_VALUE;
	}
	else
	{
		m_hSnapshot=CreateToolhelp32Snapshot(dwFlag,dwProcessID);
	}

	return m_hSnapshot!=INVALID_HANDLE_VALUE;
}


BOOL ToolHelp::ProcessFirst(PROCESSENTRY32W *pPro) const
{
	BOOL bOk=Process32First(m_hSnapshot,pPro);


	if( bOk && (pPro->th32ProcessID==0) )
	{
		bOk=ProcessNext(pPro);	//移出[System Process](PID=0)
	}


	return bOk;
}


BOOL ToolHelp::ProcessNext(PROCESSENTRY32W *pPro) const
{
	BOOL bOk=Process32Next(m_hSnapshot,pPro);

	if( bOk && (pPro->th32ProcessID==0) )
	{
		bOk=ProcessNext(pPro);	//移出[System Process](PID=0)
	}


	return bOk;
}


BOOL ToolHelp::ProcessFind(DWORD dwProcessID, PROCESSENTRY32W *pPro) const
{
	BOOL bFind=FALSE;
	
	for(BOOL bOk=ProcessFirst(pPro); bOk; bOk=ProcessNext(pPro) )
	{
		bFind=( pPro->th32ProcessID == dwProcessID );
		if( bFind )
		{
			break;
		}
	}

	return bFind;
}

///////////////////////////////////////////////////////////

BOOL ToolHelp::ModuleFirst(MODULEENTRY32 *pMod) const
{
	return Module32First(m_hSnapshot,pMod);
}

BOOL ToolHelp::ModuleNext(MODULEENTRY32 *pMod) const
{
	return Module32Next(m_hSnapshot,pMod);
}

BOOL ToolHelp::ModuleFind(PVOID pBaseAddr,MODULEENTRY32 *pMod) const
{
	BOOL bFind=FALSE;
	for(BOOL bOk=ModuleFirst(pMod); bOk; bOk=ModuleNext(pMod))
	{
		bFind= (pMod->modBaseAddr==pBaseAddr);
		if( bFind )
		{
			break;
		}
	}

	return bFind;
}

BOOL ToolHelp::ModuleFind(LPTSTR pszModName,MODULEENTRY32 *pMod) const
{
	BOOL bFind=FALSE;
	for(BOOL bOk=ModuleFirst(pMod); bOk; ModuleNext(pMod))
	{
		bFind=(lstrcmp(pMod->szModule,pszModName)==0 ||
			lstrcmp(pMod->szExePath,pszModName)==0);

		if( bFind )
		{
			break;
		}
	}

	return bFind;
}


////////////////////////////////////////////////////////////////////
BOOL ToolHelp::ThreadFirst(THREADENTRY32 *pThread) const
{
	return Thread32First(m_hSnapshot,pThread);
}

BOOL ToolHelp::ThreadNext(THREADENTRY32 *pThread) const
{
	return Thread32Next(m_hSnapshot,pThread);
}

/////////////////////////////////////////////////////////////////

BOOL ToolHelp::HeapListFirst(HEAPLIST32 *pHeap) const
{
	return (Heap32ListFirst(m_hSnapshot,pHeap));
}

BOOL ToolHelp::HeapListNext(HEAPLIST32 *pHeap) const
{
	return (Heap32ListNext(m_hSnapshot,pHeap));
}

BOOL ToolHelp::HeapFirst(HEAPENTRY32 *pHeap,DWORD dwProcessID,UINT dwHeapID) const
{
	return Heap32First(pHeap,dwProcessID,dwHeapID);
}

BOOL ToolHelp::HeapNext(HEAPENTRY32 *pHeap) const
{
	return Heap32Next(pHeap);
}


int ToolHelp::HowManyHeaps() const
{
	int nCount=0;
	HEAPLIST32 hl={sizeof(hl)};

	BOOL bOk=HeapListFirst(&hl);
	for(; bOk; bOk=HeapListNext(&hl) )
	{
		nCount++;
	}

	return nCount;
}

////////////////////////////////////////////////////////////////////////////////////
BOOL ToolHelp::ReadProcessMemory(DWORD dwProcessID, LPCVOID pBaseAddr, PVOID pBuffer, 
								 DWORD dwRead, PDWORD pdwNumOfByteRead)
{
	return Toolhelp32ReadProcessMemory(dwProcessID,pBaseAddr,pBuffer,dwRead,pdwNumOfByteRead);
}
/////////////////////////////////////////////////////////////////////////////

BOOL ToolHelp::EnableDebugPrivilege(BOOL bEnable)
{
	BOOL bOk=FALSE;
	HANDLE hToken;
	
	if( OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) ) 
	{
		
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;

		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

		bOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}

	return(bOk);
}