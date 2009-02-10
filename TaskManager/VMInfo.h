#ifndef __VM_INFO_H
#define __VM_INFO_H

#include <vector>
#include <string>

#ifdef _UNICODE
#define wstring string
#endif

using namespace std;


namespace VM
{
	typedef struct 
	{
		// Region information
		PVOID  pvRgnBaseAddress;
		DWORD  dwRgnProtection;  // PAGE_*
		SIZE_T RgnSize;
		DWORD  dwRgnStorage;     // MEM_*: Free, Image, Mapped, Private
		DWORD  dwRgnBlocks;
		DWORD  dwRgnGuardBlks;   // If > 0, region contains thread stack
		BOOL   fRgnIsAStack;     // TRUE if region contains thread stack

		// Block information
		PVOID  pvBlkBaseAddress;
		DWORD  dwBlkProtection;  // PAGE_*
		SIZE_T BlkSize;
		DWORD  dwBlkStorage;     // MEM_*: Free, Reserve, Image, Mapped, Private
	} VMQUERY, *PVMQUERY;

	// Helper structure
	typedef struct 
	{
		SIZE_T RgnSize;
		DWORD  dwRgnStorage;     // MEM_*: Free, Image, Mapped, Private
		DWORD  dwRgnBlocks;
		DWORD  dwRgnGuardBlks;   // If > 0, region contains thread stack
		BOOL   fRgnIsAStack;     // TRUE if region contains thread stack
	} VMQUERY_HELP;


	class VMInfo
	{
	public:
		VMInfo(HANDLE hProcess=NULL);
		virtual ~VMInfo(void);

		BOOL QueryHelp(LPCVOID pvAddress,VMQUERY_HELP &vmQuery);
		BOOL Query(LPCVOID pvAddress,VMQUERY &vmq);		// 收集进程中内存分布信息

		void Enum(HANDLE hProcess=NULL);		// 枚举内存分布
		void SetExpand(BOOL bExpand);			// 设置扩展风格

		vector<VMQUERY> &GetVMInfo();			// 获取内存分布信息

	//private:
	//	void GetMemState();						// 过去内存状态

	private:
		HANDLE			m_hProcess;				// 进程句柄
		int				m_dwAllocGran;	
		BOOL			m_bExpandRegions;		// 扩展标志
		VMQUERY_HELP	m_vmQueryHelp;
		VMQUERY			m_vmQuery;				// 内存分布信息

		vector<VMQUERY> m_vecVMInfo;			// 内存分布信息容器
	};	// end of class

};	// end of namespace

#endif