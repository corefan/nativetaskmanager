/**
	作者:陈煜
	日期:2008/4/26
**/

#ifndef TOOLHELP_H
#define TOOLHELP_H

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <psapi.h>




class ToolHelp
{
	private:
		HANDLE m_hSnapshot;

	public:
		ToolHelp(DWORD dwFlag=0, DWORD dwProcessID=0);
		~ToolHelp();

		BOOL CreateSnapshot(DWORD dwFlags, DWORD dwProcessID=0);


		BOOL ProcessFirst(PROCESSENTRY32 *pPro) const;
		BOOL ProcessNext(PROCESSENTRY32 *pPro) const;
		BOOL ProcessFind(DWORD dwProcessID, PROCESSENTRY32 *pPro) const;


		BOOL ModuleFirst(MODULEENTRY32 *pModule) const;
		BOOL ModuleNext(MODULEENTRY32 *pModule) const;
		BOOL ModuleFind(PVOID pBaseAddr, MODULEENTRY32 *pMod) const;
		BOOL ModuleFind(LPTSTR pszModName,MODULEENTRY32 *pMod) const;


		BOOL ThreadFirst(THREADENTRY32 *pThread) const;
		BOOL ThreadNext(THREADENTRY32 *pThread) const;


		BOOL HeapListFirst(HEAPLIST32 *pHeap) const;
		BOOL HeapListNext(HEAPLIST32 *pHeap) const;

		BOOL HeapFirst(HEAPENTRY32 *pHeap,DWORD dwProcessID,UINT dwHeapID) const;
		BOOL HeapNext(HEAPENTRY32 *pHeap) const;

		int HowManyHeaps() const;

	public:
		static BOOL ReadProcessMemory(DWORD dwProcessID,LPCVOID pBaseAddr,PVOID pBuffer,DWORD dwRead,
			PDWORD pdwNumOfByteRead=NULL);
		
		static BOOL EnableDebugPrivilege(BOOL bEnable);

};


#endif