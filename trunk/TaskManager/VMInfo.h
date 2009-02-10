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
		BOOL Query(LPCVOID pvAddress,VMQUERY &vmq);		// �ռ��������ڴ�ֲ���Ϣ

		void Enum(HANDLE hProcess=NULL);		// ö���ڴ�ֲ�
		void SetExpand(BOOL bExpand);			// ������չ���

		vector<VMQUERY> &GetVMInfo();			// ��ȡ�ڴ�ֲ���Ϣ

	//private:
	//	void GetMemState();						// ��ȥ�ڴ�״̬

	private:
		HANDLE			m_hProcess;				// ���̾��
		int				m_dwAllocGran;	
		BOOL			m_bExpandRegions;		// ��չ��־
		VMQUERY_HELP	m_vmQueryHelp;
		VMQUERY			m_vmQuery;				// �ڴ�ֲ���Ϣ

		vector<VMQUERY> m_vecVMInfo;			// �ڴ�ֲ���Ϣ����
	};	// end of class

};	// end of namespace

#endif