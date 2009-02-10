#ifndef __GET_CPU_USAGE_H
#define __GET_CPU_USAGE_H

#include <stdio.h>
#include <windows.h>
#include <tchar.h>

namespace CPUUsage
{

#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004)
	typedef LONG NTSTATUS;
#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))



	//self def struct.
	typedef struct CpuData 
	{
		DWORD dwPID;
		UINT cpuusage;
		__int64  lastidle;
		__int64  lastsys;
		LARGE_INTEGER  cputime;
		LARGE_INTEGER  KernelTime;
		LARGE_INTEGER  UserTime;
	} *pCpuData;

	typedef struct
	{
		LARGE_INTEGER IdleProcessTime;
		LARGE_INTEGER IoReadTransferCount;
		LARGE_INTEGER IoWriteTransferCount;
		LARGE_INTEGER IoOtherTransferCount;
		ULONG IoReadOperationCount;
		ULONG IoWriteOperationCount;
		ULONG IoOtherOperationCount;
		ULONG AvailablePages;
		ULONG CommittedPages;
		ULONG CommitLimit;
		ULONG PeakCommitment;
		ULONG PageFaultCount;
		ULONG CopyOnWriteCount;
		ULONG TransitionCount;
		ULONG CacheTransitionCount;
		ULONG DemandZeroCount;
		ULONG PageReadCount;
		ULONG PageReadIoCount;
		ULONG CacheReadCount;
		ULONG CacheIoCount;
		ULONG DirtyPagesWriteCount;
		ULONG DirtyWriteIoCount;
		ULONG MappedPagesWriteCount;
		ULONG MappedWriteIoCount;
		ULONG PagedPoolPages;
		ULONG NonPagedPoolPages;
		ULONG PagedPoolAllocs;
		ULONG PagedPoolFrees;
		ULONG NonPagedPoolAllocs;
		ULONG NonPagedPoolFrees;
		ULONG FreeSystemPtes;
		ULONG ResidentSystemCodePage;
		ULONG TotalSystemDriverPages;
		ULONG TotalSystemCodePages;
		ULONG NonPagedPoolLookasideHits;
		ULONG PagedPoolLookasideHits;
		ULONG Spare3Count;
		ULONG ResidentSystemCachePage;
		ULONG ResidentPagedPoolPage;
		ULONG ResidentSystemDriverPage;
		ULONG CcFastReadNoWait;
		ULONG CcFastReadWait;
		ULONG CcFastReadResourceMiss;
		ULONG CcFastReadNotPossible;
		ULONG CcFastMdlReadNoWait;
		ULONG CcFastMdlReadWait;
		ULONG CcFastMdlReadResourceMiss;
		ULONG CcFastMdlReadNotPossible;
		ULONG CcMapDataNoWait;
		ULONG CcMapDataWait;
		ULONG CcMapDataNoWaitMiss;
		ULONG CcMapDataWaitMiss;
		ULONG CcPinMappedDataCount;
		ULONG CcPinReadNoWait;
		ULONG CcPinReadWait;
		ULONG CcPinReadNoWaitMiss;
		ULONG CcPinReadWaitMiss;
		ULONG CcCopyReadNoWait;
		ULONG CcCopyReadWait;
		ULONG CcCopyReadNoWaitMiss;
		ULONG CcCopyReadWaitMiss;
		ULONG CcMdlReadNoWait;
		ULONG CcMdlReadWait;
		ULONG CcMdlReadNoWaitMiss;
		ULONG CcMdlReadWaitMiss;
		ULONG CcReadAheadIos;
		ULONG CcLazyWriteIos;
		ULONG CcLazyWritePages;
		ULONG CcDataFlushes;
		ULONG CcDataPages;
		ULONG ContextSwitches;
		ULONG FirstLevelTbFills;
		ULONG SecondLevelTbFills;
		ULONG SystemCalls;
	} SYSTEM_PERFORMANCE_INFORMATION;

	typedef struct _SYSTEM_TIMEOFDAY_INFORMATION
	{
		LARGE_INTEGER BootTime;
		LARGE_INTEGER CurrentTime;
		LARGE_INTEGER TimeZoneBias;
		ULONG TimeZoneId;
		ULONG Reserved;
	} SYSTEM_TIMEOFDAY_INFORMATION, *PSYSTEM_TIMEOFDAY_INFORMATION;

	//include sys header
	typedef struct _THREAD_INFO
	{
		LARGE_INTEGER CreateTime;
		DWORD dwUnknown1;
		DWORD dwStartAddress;
		DWORD StartEIP;
		DWORD dwOwnerPID;
		DWORD dwThreadId;
		DWORD dwCurrentPriority;
		DWORD dwBasePriority;
		DWORD dwContextSwitches;
		DWORD Unknown;
		DWORD WaitReason;

	}THREADINFO, *PTHREADINFO;

	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaxLength;
		PWSTR Buffer;
	} UNICODE_STRING;

	typedef struct _SYSTEM_PROCESS_INFORMATION
	{
		ULONG NextEntryOffset;
		ULONG NumberOfThreads;
		LARGE_INTEGER SpareLi1;
		LARGE_INTEGER SpareLi2;
		LARGE_INTEGER SpareLi3;
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ImageName;
		DWORD BasePriority;
		DWORD UniqueProcessId;
		DWORD InheritedFromUniqueProcessId;
		ULONG HandleCount;
		ULONG SessionId;
		ULONG PageDirectoryFrame;

		//
		// This part corresponds to VM_COUNTERS_EX.
		// NOTE: *NOT* THE SAME AS VM_COUNTERS!
		//
		ULONG PeakVirtualSize;
		ULONG VirtualSize;
		ULONG PageFaultCount;
		ULONG PeakWorkingSetSize;
		ULONG WorkingSetSize;
		ULONG QuotaPeakPagedPoolUsage;
		ULONG QuotaPagedPoolUsage;
		ULONG QuotaPeakNonPagedPoolUsage;
		ULONG QuotaNonPagedPoolUsage;
		ULONG PagefileUsage;
		ULONG PeakPagefileUsage;
		ULONG PrivateUsage;

		//
		// This part corresponds to IO_COUNTERS
		//
		LARGE_INTEGER ReadOperationCount;
		LARGE_INTEGER WriteOperationCount;
		LARGE_INTEGER OtherOperationCount;
		LARGE_INTEGER ReadTransferCount;
		LARGE_INTEGER WriteTransferCount;
		LARGE_INTEGER OtherTransferCount;

		THREADINFO TH[1];
	} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;


	typedef struct _SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION {
		LARGE_INTEGER IdleTime;
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER DpcTime;
		LARGE_INTEGER InterruptTime;
		ULONG InterruptCount;
	} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION, *PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;


	typedef struct _SYSTEM_BASIC_INFORMATION
	{
		ULONG Reserved;
		ULONG TimerResolution;
		ULONG PageSize;
		ULONG NumberOfPhysicalPages;
		ULONG LowestPhysicalPageNumber;
		ULONG HighestPhysicalPageNumber;
		ULONG AllocationGranularity;
		ULONG MinimumUserModeAddress;
		ULONG MaximumUserModeAddress;
		ULONG ActiveProcessorsAffinityMask;
		CCHAR NumberOfProcessors;
	} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;


	/****************************************************************
	//        获取单个进程CPU使用率
	//        Author: lwf
	****************************************************************/

	CRITICAL_SECTION    PerfDataCriticalSection;
	CpuData             *pPerfDataOld = NULL; /* Older perf data (saved to establish delta values) */
	CpuData             *pPerfData = NULL;    /* Most recent copy of perf data */
	HMODULE				hModule=NULL;
	ULONG               ProcessCountOld = 0;
	ULONG               ProcessCount = 0;
	SYSTEM_BASIC_INFORMATION        SystemBasicInfo;
	SYSTEM_PERFORMANCE_INFORMATION    SystemPerfInfo;
	PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION SystemProcessorTimeInfo = NULL;
	LARGE_INTEGER                    liOldIdleTime = {{0,0}};
	double                            dbIdleTime;
	double                            dbKernelTime;
	double                            dbSystemTime;
	double                            OldKernelTime = 0;
	LARGE_INTEGER                    liOldSystemTime = {{0,0}};
	long ( __stdcall *NtQuerySystemInformation )( DWORD, PVOID, DWORD, DWORD* );


	BOOL PerfDataInitialize(void)
	{
		SID_IDENTIFIER_AUTHORITY NtSidAuthority = {SECURITY_NT_AUTHORITY};
		NTSTATUS    status;

		InitializeCriticalSection(&PerfDataCriticalSection);
		
		hModule=GetModuleHandle( _T("ntdll.dll") );
		NtQuerySystemInformation = (long(__stdcall*)(DWORD,PVOID,DWORD,DWORD*))
			GetProcAddress(hModule,"NtQuerySystemInformation");

		/*
		* Get number of processors in the system
		*/
		status = NtQuerySystemInformation(0, &SystemBasicInfo, sizeof(SystemBasicInfo), NULL);
		if (status != NO_ERROR)
			return FALSE;

		/*
		* Create the SYSTEM Sid
		*/
		return TRUE;
	}

	void PerfDataUninitialize(void)
	{
		FreeLibrary(hModule);
		DeleteCriticalSection(&PerfDataCriticalSection);
	}


	void GetAllProcCPUUsage()
	{
		ULONG                             ulSize;
		LONG                          status;
		LPBYTE                            pBuffer;
		ULONG                             BufferSize;
		PSYSTEM_PROCESS_INFORMATION             pSPI;
		pCpuData                          pPDOld;
		ULONG                             Idx, Idx2;
		HANDLE                            hProcess;
		HANDLE                            hProcessToken;
		double                            CurrentKernelTime;
		SYSTEM_PERFORMANCE_INFORMATION    SysPerfInfo;
		SYSTEM_TIMEOFDAY_INFORMATION      SysTimeInfo;
		PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION SysProcessorTimeInfo;
		ULONG                             Buffer[64]; /* must be 4 bytes aligned! */



		/* Get new system time */
		status = NtQuerySystemInformation(3, &SysTimeInfo, sizeof(SysTimeInfo), 0);
		if (status != NO_ERROR)
			return;

		/* Get new CPU's idle time */
		status = NtQuerySystemInformation(2, &SysPerfInfo, sizeof(SysPerfInfo), NULL);
		if (status != NO_ERROR)
			return;

		/* Get processor time information */
		SysProcessorTimeInfo = (PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)HeapAlloc(GetProcessHeap(), 0, sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * SystemBasicInfo.NumberOfProcessors);
		status = NtQuerySystemInformation(8, SysProcessorTimeInfo, sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * SystemBasicInfo.NumberOfProcessors, &ulSize);
		if (status != NO_ERROR)
			return;

		/* Get process information
		* We don't know how much data there is so just keep
		* increasing the buffer size until the call succeeds
		*/
		BufferSize = 0;
		do
		{
			BufferSize += 0x10000;
			pBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, BufferSize);

			status = NtQuerySystemInformation(5, pBuffer, BufferSize, &ulSize);

			if (status == STATUS_INFO_LENGTH_MISMATCH) {
				HeapFree(GetProcessHeap(), 0, pBuffer);
			}

		} while (status == STATUS_INFO_LENGTH_MISMATCH);

		EnterCriticalSection(&PerfDataCriticalSection);

		/*
		* Save system performance info
		*/
		memcpy(&SystemPerfInfo, &SysPerfInfo, sizeof(SYSTEM_PERFORMANCE_INFORMATION));

		/*
		* Save system processor time info
		*/
		if (SystemProcessorTimeInfo)
		{
			HeapFree(GetProcessHeap(), 0, SystemProcessorTimeInfo);
		}
		SystemProcessorTimeInfo = SysProcessorTimeInfo;

		/*
		* Save system handle info
		*/

		for (CurrentKernelTime=0, Idx=0; Idx<(ULONG)SystemBasicInfo.NumberOfProcessors; Idx++) {
			CurrentKernelTime += Li2Double(SystemProcessorTimeInfo[Idx].KernelTime);
			CurrentKernelTime += Li2Double(SystemProcessorTimeInfo[Idx].DpcTime);
			CurrentKernelTime += Li2Double(SystemProcessorTimeInfo[Idx].InterruptTime);
		}

		/* If it's a first call - skip idle time calcs */
		if (liOldIdleTime.QuadPart != 0) {
			/*  CurrentValue = NewValue - OldValue */
			dbIdleTime = Li2Double(SysPerfInfo.IdleProcessTime) - Li2Double(liOldIdleTime);
			dbKernelTime = CurrentKernelTime - OldKernelTime;
			dbSystemTime = Li2Double(SysTimeInfo.CurrentTime) - Li2Double(liOldSystemTime);

			/*  CurrentCpuIdle = IdleTime / SystemTime */
			dbIdleTime = dbIdleTime / dbSystemTime;
			dbKernelTime = dbKernelTime / dbSystemTime;

			/*  CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors */
			dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SystemBasicInfo.NumberOfProcessors; /* + 0.5; */
			dbKernelTime = 100.0 - dbKernelTime * 100.0 / (double)SystemBasicInfo.NumberOfProcessors; /* + 0.5; */
		}

		/* Store new CPU's idle and system time */
		liOldIdleTime = SysPerfInfo.IdleProcessTime;
		liOldSystemTime = SysTimeInfo.CurrentTime;
		OldKernelTime = CurrentKernelTime;

		/* Determine the process count
		* We loop through the data we got from NtQuerySystemInformation
		* and count how many structures there are (until RelativeOffset is 0)
		*/
		ProcessCountOld = ProcessCount;
		ProcessCount = 0;
		pSPI = (PSYSTEM_PROCESS_INFORMATION)pBuffer;
		while (pSPI) 
		{
			ProcessCount++;
			if (pSPI->NextEntryOffset == 0)
				break;
			pSPI = (PSYSTEM_PROCESS_INFORMATION)((LPBYTE)pSPI + pSPI->NextEntryOffset);
		}

		/* Now alloc a new PERFDATA array and fill in the data */
		if (pPerfDataOld) {
			HeapFree(GetProcessHeap(), 0, pPerfDataOld);
		}
		pPerfDataOld = pPerfData;
		pPerfData = (pCpuData)HeapAlloc(GetProcessHeap(), 0, sizeof(CpuData) * ProcessCount);
		pSPI = (PSYSTEM_PROCESS_INFORMATION)pBuffer;
		for (Idx=0; Idx<ProcessCount; Idx++) 
		{
			/* Get the old perf data for this process (if any) */
			/* so that we can establish delta values */
			pPDOld = NULL;
			for (Idx2=0; Idx2<ProcessCountOld; Idx2++) 
			{
				if (pPerfDataOld[Idx2].dwPID == pSPI->UniqueProcessId) 
				{
					pPDOld = &pPerfDataOld[Idx2];
					break;
				}
			}

			/* Clear out process perf data structure */
			memset(&pPerfData[Idx], 0, sizeof(CpuData));

			pPerfData[Idx].dwPID = pSPI->UniqueProcessId;

			if (pPDOld)   
			{
				double    CurTime = Li2Double(pSPI->KernelTime) + Li2Double(pSPI->UserTime);
				double    OldTime = Li2Double(pPDOld->KernelTime) + Li2Double(pPDOld->UserTime);
				double    CpuTime = (CurTime - OldTime) / dbSystemTime;
				CpuTime = CpuTime * 100.0 / (double)SystemBasicInfo.NumberOfProcessors; /* + 0.5; */
				pPerfData[Idx].cpuusage = (ULONG)CpuTime;
			}
			pPerfData[Idx].cputime.QuadPart = pSPI->UserTime.QuadPart + pSPI->KernelTime.QuadPart;

			if (pSPI->UniqueProcessId != NULL)
			{
				hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | READ_CONTROL, FALSE, PtrToUlong(pSPI->UniqueProcessId));
				if (hProcess)
				{
					/* don't query the information of the system process. It's possible but
					returns Administrators as the owner of the process instead of SYSTEM */
					if (pSPI->UniqueProcessId != 0x4)
					{
						if (OpenProcessToken(hProcess, TOKEN_QUERY, &hProcessToken))
						{
							DWORD RetLen = 0;
							BOOL Ret;

							Ret = GetTokenInformation(hProcessToken, TokenUser, (LPVOID)Buffer, sizeof(Buffer), &RetLen);
							CloseHandle(hProcessToken);
						}

					}

					CloseHandle(hProcess);
				}
			}
			pPerfData[Idx].UserTime.QuadPart = pSPI->UserTime.QuadPart;
			pPerfData[Idx].KernelTime.QuadPart = pSPI->KernelTime.QuadPart;
			pSPI = (PSYSTEM_PROCESS_INFORMATION)((LPBYTE)pSPI + pSPI->NextEntryOffset);
		}
		HeapFree(GetProcessHeap(), 0, pBuffer);
		LeaveCriticalSection(&PerfDataCriticalSection);
	}

	int PerfGetIndexByProcessId(DWORD dwProcessId)
	{
		int Index, FoundIndex = -1;

		EnterCriticalSection(&PerfDataCriticalSection);

		for (Index = 0; Index < (int)ProcessCount; Index++)
		{
			if ((DWORD)pPerfData[Index].dwPID == dwProcessId)
			{
				FoundIndex = Index;
				break;
			}
		}

		LeaveCriticalSection(&PerfDataCriticalSection);

		return FoundIndex;
	}



	ULONG PerfDataGetCPUUsage(DWORD dwProcessId)
	{
		ULONG    CpuUsage;
		int Index, FoundIndex = -1;

		EnterCriticalSection(&PerfDataCriticalSection);


		for (Index = 0; Index < (int)ProcessCount; Index++)
		{
			if ((DWORD)pPerfData[Index].dwPID == dwProcessId)
			{
				FoundIndex = Index;
				break;
			}
		}

		if (Index < (int)ProcessCount)
			CpuUsage = pPerfData[Index].cpuusage;
		else
			CpuUsage = 0;

		LeaveCriticalSection(&PerfDataCriticalSection);

		return CpuUsage;
	}

}

#endif