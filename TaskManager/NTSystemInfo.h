// 作者：陈煜 Email:chenyu2202863@yahoo.com.cn
// .
//
// 
//
//////////////////////////////////////////////////////////////////////////////////////
//
// SystemInfo.h, v2.1

#ifndef SYSTEMINFO_H_INCLUDED
#define SYSTEMINFO_H_INCLUDED



#pragma warning( disable : 4786 )
#pragma warning( disable : 4200 )
#pragma warning( disable : 4996 )

#include <string>
#include <map>
#include <vector>

using namespace std;

#ifdef _UNICODE
#define string	wstring
#endif

namespace NT
{
	//////////////////////////////////////////////////////////////////////////////////////
	//
	// Typedefs
	//
	//////////////////////////////////////////////////////////////////////////////////////
	// =================================================================
	// SYSTEM INFO CLASSES
	// =================================================================

	typedef enum _SYSTEMINFOCLASS
	{
		NTSystemBasicInformation=0,           // 0x002C
		NTSystemProcessorInformation,         // 0x000C
		NTSystemPerformanceInformation,       // 0x0138
		NTSystemTimeInformation,              // 0x0020
		NTSystemPathInformation,              // not implemented
		NTSystemProcessInformation,           // 0x00C8+ per process
		NTSystemCallInformation,              // 0x0018 + (n * 0x0004)
		NTSystemConfigurationInformation,     // 0x0018
		NTSystemProcessorCounters,            // 0x0030 per cpu
		NTSystemGlobalFlag,                   // 0x0004
		NTSystemInfo10,                       // not implemented
		NTSystemModuleInformation,            // 0x0004 + (n * 0x011C)
		NTSystemLockInformation,              // 0x0004 + (n * 0x0024)
		NTSystemInfo13,                       // not implemented
		NTSystemPagedPoolInformation,         // checked build only
		NTSystemNonPagedPoolInformation,      // checked build only
		NTSystemHandleInformation,            // 0x0004  + (n * 0x0010)
		NTSystemObjectInformation,            // 0x0038+ + (n * 0x0030+)
		NTSystemPageFileInformation,          // 0x0018+ per page file
		NTSystemInstemulInformation,          // 0x0088
		NTSystemInfo20,                       // invalid info class
		NTSystemCacheInformation,             // 0x0024
		NTSystemPoolTagInformation,           // 0x0004 + (n * 0x001C)
		NTSystemInfo23,                       // 0x0000, or 0x0018 per cpu
		NTSystemDpcInformation,               // 0x0014
		NTSystemInfo25,                       // checked build only
		NTSystemLoadDriver,                   // 0x0018, set mode only
		NTSystemUnloadDriver,                 // 0x0004, set mode only
		NTSystemTimeAdjustmentInformation,    // 0x000C, 0x0008 writeable
		NTSystemInfo29,                       // checked build only
		NTSystemInfo30,                       // checked build only
		NTSystemInfo31,                       // checked build only
		NTSystemCrashDumpInformation,         // 0x0004
		NTSystemInfo33,                       // 0x0010
		NTSystemCrashDumpStateInformation,    // 0x0004
		NTSystemDebuggerInformation,          // 0x0002
		NTSystemThreadSwitchInformation,      // 0x0030
		NTSystemRegistryQuotaInformation,     // 0x000C
		NTSystemAddDriver,                    // 0x0008, set mode only
		NTSystemPrioritySeparationInformation,// 0x0004, set mode only
		NTSystemInfo40,                       // not implemented
		NTSystemInfo41,                       // not implemented
		NTSystemInfo42,                       // invalid info class
		NTSystemInfo43,                       // invalid info class
		NTSystemTimeZoneInformation,          // 0x00AC
		NTSystemLookasideInformation,         // n * 0x0020
		NTMaxSystemInfoClass
	} SYSTEMINFOCLASS, *PSYSTEMINFOCLASS, **PPSYSTEMINFOCLASS;

	typedef LONG			KPRIORITY;
	typedef LARGE_INTEGER   QWORD;

	typedef struct _UNICODE_STRING				// Unicode字符串
	{
		WORD  Length;
		WORD  MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING;


	typedef struct _CLIENT_ID
	{
		HANDLE UniqueProcess;					// 线程的父进程句柄
		HANDLE UniqueThread;					// 线程句柄
	}CLIENT_ID;

	typedef enum _THREAD_STATE
	{
		StateInitialized,						// 初始化
		StateReady,								// 准备
		StateRunning,							// 运行
		StateStandby,							// 挂起
		StateTerminated,						// 关闭
		StateWait,								// 等待
		StateTransition,						// 转变
		StateUnknown							// 未知
	}THREAD_STATE;


	typedef enum _KWAIT_REASON					// 线程等待原因
	{
		Executive,
		FreePage,
		PageIn,
		PoolAllocation,
		DelayExecution,
		Suspended,
		UserRequest,
		WrExecutive,
		WrFreePage,
		WrPageIn,
		WrPoolAllocation,
		WrDelayExecution,
		WrSuspended,
		WrUserRequest,
		WrEventPair,
		WrQueue,
		WrLpcReceive,
		WrLpcReply,
		WrVertualMemory,
		WrPageOut,
		WrRendezvous,
		Spare2,
		Spare3,
		Spare4,
		Spare5,
		Spare6,
		WrKernel
	}KWAIT_REASON;



	typedef struct _SYSTEM_THREAD
	{
		LARGE_INTEGER KernelTime;               //CPU内核模式使用时间；
		LARGE_INTEGER UserTime;                 //CPU用户模式使用时间；
		LARGE_INTEGER CreateTime;               //线程创建时间；
		ULONG         WaitTime;                 //等待时间；
		PVOID         StartAddress;             //线程开始的虚拟地址；
		CLIENT_ID     ClientId;                 //线程标识符；
		KPRIORITY     Priority;                 //线程优先级；
		KPRIORITY     BasePriority;             //基本优先级；
		ULONG         ContextSwitchCount;       //环境切换数目；
		THREAD_STATE  State;                    //当前状态；
		KWAIT_REASON  WaitReason;               //等待原因；
	} SYSTEM_THREAD;




	typedef struct _PROCESS_BASIC_INFORMATION 
	{
		DWORD ExitStatus;						// 返回码
		PVOID PebBaseAddress;					// PEB地址
		DWORD AffinityMask;						// 使用CPU
		DWORD BasePriority;						// 基本优先级
		DWORD UniqueProcessId;					// 进程ID
		DWORD InheritedFromUniqueProcessId;		// 父进程ID
	} PROCESS_BASIC_INFORMATION;

	typedef struct _VM_COUNTERS
	{
		ULONG PeakVirtualSize;                  // 虚拟存储峰值大小；
		ULONG VirtualSize;                      // 虚拟存储大小；
		ULONG PageFaultCount;                   // 页故障数目；
		ULONG PeakWorkingSetSize;               // 工作集峰值大小；
		ULONG WorkingSetSize;                   // 工作集大小；
		ULONG QuotaPeakPagedPoolUsage;          // 分页池使用配额峰值；
		ULONG QuotaPagedPoolUsage;              // 分页池使用配额；
		ULONG QuotaPeakNonPagedPoolUsage;       // 非分页池使用配额峰值；
		ULONG QuotaNonPagedPoolUsage;           // 非分页池使用配额；
		ULONG PagefileUsage;                    // 页文件使用情况；
		ULONG PeakPagefileUsage;                // 页文件使用峰值；
	} VM_COUNTERS;


	typedef struct _IO_COUNTERS
	{
		LARGE_INTEGER ReadOperationCount;       // I/O读操作数目；
		LARGE_INTEGER WriteOperationCount;      // I/O写操作数目；
		LARGE_INTEGER OtherOperationCount;      // I/O其他操作数目；
		LARGE_INTEGER ReadTransferCount;        // I/O读数据数目；
		LARGE_INTEGER WriteTransferCount;       // I/O写数据数目；
		LARGE_INTEGER OtherTransferCount;       // I/O其他操作数据数目；
	}IO_COUNTERS,*PIO_COUNTERS;




	typedef struct _SYSTEM_PROCESS_INFORMATION
	{
		ULONG          NextEntryDelta;          // 构成结构序列的偏移量；
		ULONG          ThreadCount;             // 线程数目；
		ULONG          Reserved1[6];			// 保留
		LARGE_INTEGER  CreateTime;              // 创建时间；
		LARGE_INTEGER  UserTime;                // 用户模式(Ring 3)的CPU时间；
		LARGE_INTEGER  KernelTime;              // 内核模式(Ring 0)的CPU时间；
		UNICODE_STRING ProcessName;             // 进程名称；
		KPRIORITY      BasePriority;            // 进程优先权；
		ULONG          ProcessId;               // 进程标识符；
		ULONG          InheritedFromProcessId;  // 父进程的标识符；
		ULONG          HandleCount;             // 句柄数目；
		ULONG          Reserved2[2];			// 保留
		VM_COUNTERS    VmCounters;              // 虚拟存储器的结构；
		IO_COUNTERS    IoCounters;              // IO计数结构；
		SYSTEM_THREAD  Threads[1];              // 进程相关线程的结构数组；
	} SYSTEM_PROCESS_INFORMATION;




	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemInfoUtils
	//
	//////////////////////////////////////////////////////////////////////////////////////

	// Helper functions

	class SystemInfoUtils
	{
	public:

		//////////////////////////////////////////////////////////////////////////////////
		// String conversion functions

		// From wide char string to CString
		static void LPCWSTR2CString( LPCWSTR strW, string& str );
		// From unicode string to CString
		static void Unicode2CString( UNICODE_STRING* strU, string& str );

		//////////////////////////////////////////////////////////////////////////////////
		// File name conversion functions

		static BOOL GetDeviceFileName( LPCTSTR, string& );
		static BOOL GetFsFileName( LPCTSTR, string& );

		//////////////////////////////////////////////////////////////////////////////////
		// Information functions

		static DWORD GetNTMajorVersion();
	};



	//////////////////////////////////////////////////////////////////////////////////////
	//
	// INtDll
	//
	//////////////////////////////////////////////////////////////////////////////////////

	class INtDll
	{
	public:
		typedef DWORD (WINAPI *PNtQueryObject)( HANDLE, DWORD, VOID*, DWORD, VOID* );
		typedef DWORD (WINAPI *PNtQuerySystemInformation)( DWORD, VOID*, DWORD, ULONG* );
		typedef DWORD (WINAPI *PNtQueryInformationThread)(HANDLE, ULONG, PVOID,	DWORD, DWORD* );
		typedef DWORD (WINAPI *PNtQueryInformationFile)(HANDLE, PVOID,	PVOID, DWORD, DWORD );
		typedef DWORD (WINAPI *PNtQueryInformationProcess)(HANDLE, DWORD, PVOID, DWORD, PVOID );

	public:
		static PNtQuerySystemInformation	NtQuerySystemInformation;
		static PNtQueryObject				NtQueryObject;
		static PNtQueryInformationThread	NtQueryInformationThread;
		static PNtQueryInformationFile		NtQueryInformationFile;
		static PNtQueryInformationProcess	NtQueryInformationProcess;

		static BOOL							NtDllStatus;

		static DWORD						dwNTMajorVersion;

	protected:
		static BOOL Init();
	};


	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemProcessInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////

	class SystemProcessInformation : public INtDll
	{
	public:
		enum { BufferSize = 0x10000 };

	public:
		SystemProcessInformation();
		virtual ~SystemProcessInformation();

		BOOL Refresh();
		SYSTEM_PROCESS_INFORMATION* GetSpecialProcessInfo(DWORD dwID);


	public:
		map<DWORD,SYSTEM_PROCESS_INFORMATION *> m_mapProcessInfo;

		SYSTEM_PROCESS_INFORMATION* m_pCurrentProcessInfo;

	protected:
		UCHAR*						m_pBuffer;
	};


	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemThreadInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////

	class SystemThreadInformation : public INtDll
	{
	public:
		SystemThreadInformation( DWORD pID);
		virtual ~SystemThreadInformation();

		BOOL Refresh();
		vector<SYSTEM_THREAD> &GetThreadInfo();

	public:

		vector<SYSTEM_THREAD> m_vecThreadInfo;
		DWORD m_processId;
	};


	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemHandleInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////

	class SystemHandleInformation : public INtDll
	{
	public:
		enum 
		{
			OB_TYPE_UNKNOWN			= 0,
			OB_TYPE_TYPE			= 1,
			OB_TYPE_DIRECTORY,
			OB_TYPE_SYMBOLIC_LINK,
			OB_TYPE_TOKEN,
			OB_TYPE_PROCESS,
			OB_TYPE_THREAD,
			OB_TYPE_UNKNOWN_7,
			OB_TYPE_EVENT,
			OB_TYPE_EVENT_PAIR,
			OB_TYPE_MUTANT,
			OB_TYPE_UNKNOWN_11,
			OB_TYPE_SEMAPHORE,
			OB_TYPE_TIMER,
			OB_TYPE_PROFILE,
			OB_TYPE_WINDOW_STATION,
			OB_TYPE_DESKTOP,
			OB_TYPE_SECTION,
			OB_TYPE_KEY,
			OB_TYPE_PORT,
			OB_TYPE_WAITABLE_PORT,
			OB_TYPE_UNKNOWN_21,
			OB_TYPE_UNKNOWN_22,
			OB_TYPE_UNKNOWN_23,
			OB_TYPE_UNKNOWN_24,
			//OB_TYPE_CONTROLLER,
			//OB_TYPE_DEVICE,
			//OB_TYPE_DRIVER,
			OB_TYPE_IO_COMPLETION,
			OB_TYPE_FILE                        
		} SystemHandleType;

		typedef struct _SYSTEM_HANDLE
		{
			DWORD	ProcessID;
			WORD	HandleType;
			WORD	HandleNumber;
			DWORD	KernelAddress;
			DWORD	Flags;
		} SYSTEM_HANDLE;

		typedef struct _SYSTEM_HANDLE_INFORMATION
		{
			DWORD			Count;
			SYSTEM_HANDLE	Handles[1];
		} SYSTEM_HANDLE_INFORMATION;

		typedef struct tagSYSTEM_HANDLE_INFORMATION
		{
			string	strPath;
			string	strType;
			string	strAddr;
			string	strFlag;
		}HANDLE_INFORMATION;

	public:
		SystemHandleInformation( DWORD pID = (DWORD)-1);
		virtual ~SystemHandleInformation();


		BOOL Refresh();
		BOOL IsSupportedHandle( SYSTEM_HANDLE& handle );
		vector<HANDLE_INFORMATION> &GetHandleInfo();

		void GetHandleName(SYSTEM_HANDLE &sysHandle,HANDLE_INFORMATION &handleInfo);
		void GetHandleType(SYSTEM_HANDLE &sysHandle,HANDLE_INFORMATION &handleInfo);
		void GetHandleFlag(DWORD dwHandleFlag,HANDLE_INFORMATION &handleInfo);
		void GetHandleAddress(DWORD dwHandleAddr,HANDLE_INFORMATION &handleInfo);
		

		//Information functions
		static BOOL GetType( HANDLE, WORD&, DWORD processId = GetCurrentProcessId() );
		static BOOL GetTypeToken( HANDLE, string&, DWORD processId = GetCurrentProcessId() );
		static BOOL GetTypeFromTypeToken( LPCTSTR typeToken, WORD& type );
		static BOOL GetNameByType( HANDLE, WORD, string& str, DWORD processId = GetCurrentProcessId());
		static BOOL GetName( HANDLE, string&, DWORD processId = GetCurrentProcessId() );

		//Thread related functions
		//static BOOL GetThreadId( HANDLE, DWORD&, DWORD processId = GetCurrentProcessId() );

		//Process related functions
		//static BOOL GetProcessId( HANDLE, DWORD&, DWORD processId = GetCurrentProcessId() );
		//static BOOL GetProcessPath( HANDLE h, string& strPath, DWORD processId = GetCurrentProcessId());

		//File related functions
		//static BOOL GetFileName( HANDLE, string&, DWORD processId = GetCurrentProcessId() );

	public:
		//For remote handle support
		static HANDLE OpenProcess( DWORD processId );
		static HANDLE DuplicateHandle( HANDLE hProcess, HANDLE hRemote );

	public:
		vector<HANDLE_INFORMATION> m_vecHandleInfo;
		DWORD	m_processId;
	};


	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemPerformanceInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////
	class SystemPerformanceInformation : public INtDll
	{
	public:
		typedef struct _SYSTEM_PERFORMANCE_INFORMATION
		{
			LARGE_INTEGER  IdleTime;				// 系统非工作时间
			LARGE_INTEGER  ReadTransferCount;		// The number of bytes read by all calls to ZwReadFile.
			LARGE_INTEGER  WriteTransferCount;		// The number of bytes written by all calls to ZwWriteFile.	
			LARGE_INTEGER  OtherTransferCount;		// The number of bytes transferred to satisfy all other I/O operations, such as ZwDeviceIoControlFile.
			ULONG          ReadOperationCount;		// The number of calls to ZwReadFile.
			ULONG          WriteOperationCount;		// The number of calls to ZwWriteFile.
			ULONG          OtherOperationCount;		// The number of calls to all other I/O system services such as ZwDeviceIoControlFile.
			ULONG          AvailablePages;			// The number of pages of physical memory available to processes running on the system.
			ULONG          TotalCommittedPages;		// The number of pages of committed virtual memory.
			ULONG          TotalCommitLimit;		// The number of pages of virtual memory that could be committed without extending the system’s pagefiles.
			ULONG          PeakCommitment;			// The peak number of pages of committed virtual memory.
			ULONG          PageFaults;				// The number of page faults (both soft and hard).
			ULONG          WriteCopyFaults;			// The number of page faults arising from attempts to write to copy-on-write pages.
			ULONG          TransitionFaults;		// The number of soft page faults (excluding demand zero faults).
			ULONG          Reserved1;				// 
			ULONG          DemandZeroFaults;		// The number of demand zero faults.
			ULONG          PagesRead;				// The number of pages read from disk to resolve page faults.
			ULONG          PageReadIos;				// The number of read operations initiated to resolve page faults.
			ULONG          Reserved2[2];			//
			ULONG          PagefilePagesWritten;	// The number of pages written to the system’s pagefiles.
			ULONG          PagefilePageWriteIos;	// The number of write operations performed on the system’s pagefiles.
			ULONG          MappedFilePagesWritten;	// The number of pages written to mapped files.
			ULONG          MappedFileWriteIos;		// The number of write operations performed on mapped files.
			ULONG          PagedPoolUsage;			// The number of pages of virtual memory used by the paged pool.
			ULONG          NonPagedPoolUsage;		// The number of pages of virtual memory used by the nonpaged pool.
			ULONG          PagedPoolAllocs;			// The number of allocations made from the paged pool.
			ULONG          PagedPoolFrees;			// The number of allocations returned to the paged pool.
			ULONG          NonPagedPoolAllocs;		// The number of allocations made from the nonpaged pool.
			ULONG          NonPagedPoolFress;		// The number of allocations returned to the nonpaged pool.
			ULONG          TotalFreeSystemPtes;		// The number of available System Page Table Entries.
			ULONG          SystemCodePage;			// The number of pages of pageable operating system code and static data in physical memory.The meaning of “operating system code and static data” is defined by address range (lowest system address to start of system cache) and includes a contribution from win32k.sys.
			ULONG          TotalSystemDriverPages;	// The number of pages of pageable device driver code and static data.
			ULONG          TotalSystemCodePages;	// The number of pages of pageable operating system code and static data.The meaning of “operating system code and static data” is defined by load time (SERVICE_BOOT_START driver or earlier) and does not include a contribution from win32k.sys.
			ULONG          SmallNonPagedLookasideListAllocateHits;	// The number of times an allocation could be satisfied by one of the small nonpaged lookaside lists.
			ULONG          SmallPagedLookasideListAllocateHits;		// The number of times an allocation could be satisfied by one of the small-paged lookaside lists.
			ULONG          Reserved3;				
			ULONG          MmSystemCachePage;		// The number of pages of the system cache in physical memory.
			ULONG          PagedPoolPage;			// The number of pages of paged pool in physical memory.
			ULONG          SystemDriverPage;		// The number of pages of pageable device driver code and static data in physical memory.
			ULONG          FastReadNoWait;			// The number of asynchronous fast read operations.
			ULONG          FastReadWait;			// The number of synchronous fast read operations.
			ULONG          FastReadResourceMiss;	// The number of fast read operations not possible because of resource conflicts.
			ULONG          FastReadNotPossible;		// The number of fast read operations not possible because file system intervention required.
			ULONG          FastMdlReadNoWait;		// The number of asynchronous fast read operations requesting a Memory Descriptor List (MDL) for the data.
			ULONG          FastMdlReadWait;			// The number of synchronous fast read operations requesting an MDL for the data.
			ULONG          FastMdlReadResourceMiss;	// The number of synchronous fast read operations requesting an MDL for the data not possible because of resource conflicts.
			ULONG          FastMdlReadNotPossible;	// The number of synchronous fast read operations requesting an MDL for the data not possible because file system intervention required.
			ULONG          MapDataNoWait;			// The number of asynchronous data map operations.
			ULONG          MapDataWait;				// The number of synchronous data map operations.
			ULONG          MapDataNoWaitMiss;		// The number of asynchronous data map operations that incurred page faults.
			ULONG          MapDataWaitMiss;			// The number of synchronous data map operations that incurred page faults.
			ULONG          PinMappedDataCount;		// The number of requests to pin mapped data.
			ULONG          PinReadNoWait;			// The number of asynchronous requests to pin mapped data.
			ULONG          PinReadWait;				// The number of synchronous requests to pin mapped data.
			ULONG          PinReadNoWaitMiss;		// The number of asynchronous requests to pin mapped data that incurred page faults when pinning the data.
			ULONG          PinReadWaitMiss;			// The number of synchronous requests to pin mapped data that incurred page faults when pinning the data.
			ULONG          CopyReadNoWait;			// The number of asynchronous copy read operations.
			ULONG          CopyReadWait;			// The number of synchronous copy read operations.
			ULONG          CopyReadNoWaitMiss;		// The number of asynchronous copy read operations that incurred page faults when reading from the cache.
			ULONG          CopyReadWaitMiss;		// The number of synchronous copy read operations that incurred page faults when reading from the cache.
			ULONG          MdlReadNoWait;			// The number of synchronous read operations requesting an MDL for the cached data.
			ULONG          MdlReadWait;				// The number of synchronous read operations requesting an MDL for the cached data.
			ULONG          MdlReadNoWaitMiss;		// The number of synchronous read operations requesting an MDL for the cached data that incurred page faults.
			ULONG          MdlReadWaitMiss;			// The number of synchronous read operations requesting an MDL for the cached data that incurred page faults.
			ULONG          ReadAheadIos;			// The number of read ahead operations performed in anticipation of sequential access.
			ULONG          LazyWriteIos;			// The number of write operations initiated by the Lazy Writer.
			ULONG          LazyWritePages;			// The number of pages written by the Lazy Writer.
			ULONG          DataFlushes;				// The number of cache flushes in response to flush requests.
			ULONG          DataPages;				// The number of cache pages flushed in response to flush requests.
			ULONG          ContextSwitches;			// The number of context switches.
			ULONG          FirstLevelTbFills;		// The number of first level translation buffer fills.
			ULONG          SecondLevelTbFills;		// The number of second level translation buffer fills.
			ULONG          SystemCall;				// The number of system calls executed.
		}SYSTEM_PERFORMANCE_INFORMATION,*PSYSTEM_PERFORMANCE_INFORMATION;

	public:
		SystemPerformanceInformation();
		virtual ~SystemPerformanceInformation();


		BOOL Refresh();

	public:
		SYSTEM_PERFORMANCE_INFORMATION m_sysPerformanceInfo;
	};


	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemTimeInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////
	class SystemTimeInformation : public INtDll
	{
	public:
		typedef struct __SYSTEM_PROCESSOR_TIMES
		{
			LARGE_INTEGER IdleTime;				// The idle time, measured in units of 100-nanoseconds, of the processor.
			LARGE_INTEGER KernelTime;			// The time the processor spent executing in kernel mode, measured in units of 100-nanoseconds.
			LARGE_INTEGER UserTime;				// The time the processor spent executing in user mode, measured in units of 100-nanoseconds.
			LARGE_INTEGER DpcTime;				// The time the processor spent executing deferred procedure calls, measured in units of 100-nanoseconds.
			LARGE_INTEGER InterruptTime;		// The time the processor spent executing interrupt routines, measured in units of 100-nanoseconds.
			ULONG         InterruptCount;		// The number of interrupts serviced by the processor.
		}SYSTEM_PROCESSOR_TIMES,*PSYSTEM_PROCESSOR_TIMES;

	public:
		SystemTimeInformation();
		virtual ~SystemTimeInformation();


		BOOL Refresh();

	public:
		SYSTEM_PROCESSOR_TIMES m_sysTime;
	};


	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemPageFileInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////
	class SystemPageFileInformation : public INtDll
	{
	public:
		typedef struct _SYSTEM_PAGEFILE_INFORMATION
		{
			ULONG NetxEntryOffset;			// The offset from the start of the SystemInformation buffer to the next entry.
			ULONG CurrentSize;				// The current size in pages of the page file.
			ULONG TotalUsed;				// The number of pages in the page file that are in use.
			ULONG PeakUsed;					// The peak number of pages in the page file that have been in use.
			UNICODE_STRING FileName;		// The filepath of the page file.
		}SYSTEM_PAGEFILE_INFORMATION,*PSYSTEM_PAGEFILE_INFORMATION;

	public:
		SystemPageFileInformation();
		virtual ~SystemPageFileInformation();


		BOOL Refresh();

	public:
		SYSTEM_PAGEFILE_INFORMATION m_pageInfo;
	};


	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemCacheInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////
	class SystemCacheInformation : public INtDll
	{
	public:
		typedef struct _SYSTEM_CACHE_INFORMATION
		{
			ULONG SystemCacheWsSize;			// The size in bytes of the system working set.
			ULONG SystemCacheWsPeakSize;		// The peak size in bytes of the system working set.
			ULONG SystemCacheWsFaults;			// The number of page faults incurred by the system working set.
			ULONG SystemCacheWsMinimum;			// The minimum desirable size in pages of the system working set.
			ULONG SystemCacheWsMaximum;			// The maximum desirable size in pages of the system working set.
			ULONG TransitionSharedPages;		// The sum of the number of pages in the system working set and the number of shared pages on the Standby list.This value is only valid in Windows 2000.
			ULONG TransitionSharedPagesPeak;	// The peak number of shared pages on the Standby list.This value is only valid in Windows 2000.
			ULONG Reserved[2];					// 
		}SYSTEM_CACHE_INFORMATION,*PSYSTEM_CACHE_INFORMATION;

	public:
		SystemCacheInformation();
		virtual ~SystemCacheInformation();


		BOOL Refresh();

	public:
		SYSTEM_CACHE_INFORMATION m_cacheInfo;
	};



	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemModuleInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////

	class SystemModuleInformation
	{
	public:
		typedef struct _MODULE_INFO
		{
			DWORD	ProcessId;
			TCHAR	FullPath[_MAX_PATH];
			HMODULE Handle;
		} MODULE_INFO;

	public:
		typedef DWORD (WINAPI *PEnumProcessModules)(
			HANDLE hProcess,      // handle to process
			HMODULE *lphModule,   // array of module handles
			DWORD cb,             // size of array
			LPDWORD lpcbNeeded    // number of bytes required
			);

		typedef DWORD (WINAPI *PGetModuleFileNameEx)(
			HANDLE hProcess,    // handle to process
			HMODULE hModule,    // handle to module
			LPTSTR lpFilename,  // path buffer
			DWORD nSize         // maximum characters to retrieve
			);

	public:
		SystemModuleInformation( DWORD pID = (DWORD)-1, BOOL bRefresh = FALSE );

		BOOL Refresh();

	protected:
		void GetModuleListForProcess( DWORD processID );

	public:
		DWORD m_processId;
		CList< MODULE_INFO, MODULE_INFO& > m_ModuleInfos;

	protected:
		PEnumProcessModules		m_EnumProcessModules;
		PGetModuleFileNameEx	m_GetModuleFileNameEx;
	};

	//////////////////////////////////////////////////////////////////////////////////////
	//
	// SystemWindowInformation
	//
	//////////////////////////////////////////////////////////////////////////////////////

	class SystemWindowInformation
	{
	public:
		enum { MaxCaptionSize = 1024 };

		typedef struct _WINDOW_INFO
		{
			DWORD	ProcessId;
			TCHAR	Caption[MaxCaptionSize];
			HWND	hWnd;
		} WINDOW_INFO;

	public:
		SystemWindowInformation( DWORD pID = (DWORD)-1, BOOL bRefresh = FALSE );

		BOOL Refresh();

	protected:
		static BOOL CALLBACK EnumerateWindows( HWND hwnd, LPARAM lParam );

	public:
		DWORD m_processId;
		CList< WINDOW_INFO, WINDOW_INFO& > m_WindowInfos;
	};


}
#endif

