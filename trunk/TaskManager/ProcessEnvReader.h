/**
* @ ProcessEnvReader.cpp - Implementation File for CProcessEnvReader class
* @ Author - Sarath C
**/

#pragma once

#include <vector>
#include <tlhelp32.h>
#include <Winternl.h>


namespace enumEnvironment
{
	/*
	typedef struct _PEB 
	{
		BOOL InheritedAddressSpace;
		BOOL ReadImageFileExecOptions;
		BOOL BeingDebugged;
		BOOL Spare;

		HANDLE Mutant;
		PVOID ImageBaseAddress;
		PPEB_LDR_DATA LoaderData;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters; 
		PVOID SubSystemData;

		PVOID ProcessHeap;
		PVOID FastPebLock;

		PPEBLOCKROUTINE FastPebLockRoutine;
		PPEBLOCKROUTINE FastPebUnlockRoutine;

		ULONG EnvironmentUpdateCount;
		PPVOID KernelCallbackTable;
		PVOID EventLogSection;
		PVOID EventLog;

		PPEB_FREE_BLOCK FreeList;
		ULONG TlsExpansionCounter;

		PVOID TlsBitmap;
		ULONG TlsBitmapBits[0x2];

		PVOID ReadOnlySharedMemoryBase;
		PVOID ReadOnlySharedMemoryHeap;

		PPVOID ReadOnlyStaticServerData;
		PVOID AnsiCodePageData;
		PVOID OemCodePageData;
		PVOID UnicodeCaseTableData;

		ULONG NumberOfProcessors;
		ULONG NtGlobalFlag;

		BYTE Spare2[0x4];
		LARGE_INTEGER CriticalSectionTimeout;

		ULONG HeapSegmentReserve;
		ULONG HeapSegmentCommit;

		ULONG HeapDeCommitTotalFreeThreshold;
		ULONG HeapDeCommitFreeBlockThreshold;

		ULONG NumberOfHeaps;
		ULONG MaximumNumberOfHeaps;

		PPVOID *ProcessHeaps;
		PVOID GdiSharedHandleTable;
		PVOID ProcessStarterHelper;
		PVOID GdiDCAttributeList;
		PVOID LoaderLock;

		ULONG OSMajorVersion;
		ULONG OSMinorVersion;

		ULONG OSBuildNumber;
		ULONG OSPlatformId;

		ULONG ImageSubSystem;
		ULONG ImageSubSystemMajorVersion;
		ULONG ImageSubSystemMinorVersion;

		ULONG GdiHandleBuffer[0x22];
		ULONG PostProcessInitRoutine;

		ULONG TlsExpansionBitmap;
		BYTE TlsExpansionBitmapBits[0x80];

		ULONG SessionId;
	}PEB, *PPEB;
	*/


	// macro do safe cleanp wtih specified function with object
	inline void SAFE_ICON_CLEANUP( HICON hIcon  )
	{
		if( hIcon )
			DestroyIcon( hIcon );
	}

	// macro do safe cleanp wtih specified function with object
	inline void SAFE_ARRAY_CLEANUP( void* pData  )
	{
		if( pData )
			free( pData );
	}

	/**
	* Structure to hold the environment string block and it's size
	**/
	struct _ENVSTRING_t
	{
		LPCWSTR pData;
		int nSize;

		_ENVSTRING_t() // Ctor of structure
		{
			pData = 0; nSize = 0;
		}

		~_ENVSTRING_t() // Ctor of structure
		{ Clear(); }
		void Clear() // dtor of structure
		{
			if( pData )
			{
				delete[] pData;
				pData = 0;
			}
			nSize = 0;
		}
	};

	// enumeration to specify the destinaton of copying while export data
	enum COPY_DEST_e { DEST_CLIPBRD, DEST_FILE };

	// arrays and other definitions used to process strings
	typedef std::vector<PROCESSENTRY32> ProcessInfoArray;
	typedef std::pair<CString,CString> EnvVariableValuePair;
	typedef std::vector<EnvVariableValuePair> EnvVarValArray;

	/**
	* Class provides interfaces to read process information and other misc services
	**/
	class CProcessEnvReader
	{
	public:
		// Wrapper to call NtQueryInformationProcess using Run-time dynamic linking
		static NTSTATUS QueryInformationProcesss( IN HANDLE ProcessHandle,
			IN PROCESSINFOCLASS ProcessInformationClass,
			OUT PVOID ProcessInformation,
			IN ULONG ProcessInformationLength,
			OUT PULONG ReturnLength OPTIONAL );

		/* Process Helper functions */
		static BOOL ReadEnvironmentBlock( HANDLE hProcess, _ENVSTRING_t& stEnvData );
		static HANDLE OpenProcessToRead( DWORD dwPID );
		static void ReleaseHandle( HANDLE hHandle );
		static BOOL HasReadAccess( HANDLE hProcess, void* pAddress, int& nSize );
		static void EnumProcessInfo( ProcessInfoArray& arrProcessInfo );
		static CString GetProcessNameFromHandle( HANDLE hProcess );
		static CString GetProcessNameFromID( DWORD dwPID );

		/* GDI Helper functions */
		static void LoadIconFromProcess( HANDLE hProcess, HICON& hIconSmall, HICON& hIconLarge );

		/* String helper functions */
		static void ConvertUnicodeToMBCS( LPCWSTR lpStringToConvert, int nLen, CString& csMBCSStr );
		static void ParseEnvironmentStrings( LPCWSTR lpStringToConvert, int nLen, CStringArray& EnvStrArr );
		static void SeparateVariablesAndValues( const CStringArray& EnvStrArray, EnvVarValArray& varValArr );

		/* Export functions */
		static void ExportEnvStrings( const CStringArray& csArrEnvStr, COPY_DEST_e Dest, HWND hClipboardOwner );
		static void UpdateProcessMiscInfo( HANDLE hProcess, int nItemSelected );
	};


}; // end of namespace