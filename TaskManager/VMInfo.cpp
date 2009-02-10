#include "StdAfx.h"
#include "VMInfo.h"


using namespace VM;


VMInfo::VMInfo(HANDLE hProcess)
	: m_hProcess(hProcess)
	, m_dwAllocGran(0)
	, m_bExpandRegions(FALSE)
{
}

VMInfo::~VMInfo(void)
{
}

void VMInfo::SetExpand(BOOL bExpand)
{
	m_bExpandRegions=bExpand;
}

vector<VMQUERY> &VMInfo::GetVMInfo()
{
	return m_vecVMInfo;
}

//void VMInfo::GetMemState()
//{
//	
//}

// Iterates through a region's blocks and returns findings in VMQUERY_HELP
BOOL VMInfo::QueryHelp(LPCVOID pvAddress,VMQUERY_HELP &vmQueryHelp) 
{
	// Each element contains a page protection
	// (i.e.: 0=reserved, PAGE_NOACCESS, PAGE_READWRITE, etc.)
	DWORD dwProtectBlock[4] = { 0 }; 
	ZeroMemory(&vmQueryHelp, sizeof(vmQueryHelp));

	// Get address of region containing passed memory address.
	MEMORY_BASIC_INFORMATION mbi;
	BOOL fOk = (::VirtualQueryEx(m_hProcess, pvAddress, &mbi, sizeof(mbi)) == sizeof(mbi));

	if( !fOk )
		return(fOk);   // Bad memory address, return failure

	// Walk starting at the region's base address (which never changes)
	PVOID pvRgnBaseAddress	= mbi.AllocationBase;

	// Walk starting at the first block in the region (changes in the loop)
	PVOID pvAddressBlk		= pvRgnBaseAddress;

	// Save the memory type of the physical storage block.
	vmQueryHelp.dwRgnStorage = mbi.Type;

	for (;;) 
	{
		// Get info about the current block.
		fOk = (::VirtualQueryEx(m_hProcess, pvAddressBlk, &mbi, sizeof(mbi)) 
			== sizeof(mbi));
		if (!fOk)
			break;   // Couldn't get the information, end loop.

		// Is this block in the same region?
		if( mbi.AllocationBase != pvRgnBaseAddress )
			break;   // Found a block in the next region; end loop.

		// We have a block contained in the region.

		// The following if statement is for detecting stacks in Windows 98.
		// A Windows 98 stack region's last 4 blocks look like this:
		//    reserved block, no access block, read-write block, reserved block
		if( vmQueryHelp.dwRgnBlocks < 4 ) 
		{
			// 0th through 3rd block, remember the block's protection
			dwProtectBlock[vmQueryHelp.dwRgnBlocks] = 
				(mbi.State == MEM_RESERVE) ? 0 : mbi.Protect;
		}
		else 
		{
			// We've seen 4 blocks in this region.
			// Shift the protection values down in the array.
			::MoveMemory(&dwProtectBlock[0], &dwProtectBlock[1],sizeof(dwProtectBlock) - sizeof(DWORD));

			// Add the new protection value to the end of the array.
			dwProtectBlock[3] = (mbi.State == MEM_RESERVE) ? 0 : mbi.Protect;
		}

		vmQueryHelp.dwRgnBlocks++;             // Add another block to the region
		vmQueryHelp.RgnSize += mbi.RegionSize; // Add block's size to region size

		// If block has PAGE_GUARD attribute, add 1 to this counter
		if( (mbi.Protect & PAGE_GUARD) == PAGE_GUARD ) 
			vmQueryHelp.dwRgnGuardBlks++;

		// Take a best guess as to the type of physical storage committed to the
		// block. This is a guess because some blocks can convert from MEM_IMAGE
		// to MEM_PRIVATE or from MEM_MAPPED to MEM_PRIVATE; MEM_PRIVATE can
		// always be overridden by MEM_IMAGE or MEM_MAPPED.
		if( vmQueryHelp.dwRgnStorage == MEM_PRIVATE )
			vmQueryHelp.dwRgnStorage = mbi.Type;

		// Get the address of the next block.
		pvAddressBlk = (PVOID) ((PBYTE) pvAddressBlk + mbi.RegionSize);
	}

	// After examining the region, check to see whether it is a thread stack
	// Windows 2000: Assume stack if region has at least 1 PAGE_GUARD block
	// Windows 9x:   Assume stack if region has at least 4 blocks with
	//               3rd block from end: reserved
	//               2nd block from end: PAGE_NOACCESS
	//               1st block from end: PAGE_READWRITE
	//               block at end: another reserved block.
	vmQueryHelp.fRgnIsAStack =
		(vmQueryHelp.dwRgnGuardBlks > 0)         ||
		((vmQueryHelp.dwRgnBlocks >= 4)          &&
		(dwProtectBlock[0] == 0)              && 
		(dwProtectBlock[1] == PAGE_NOACCESS)  &&
		(dwProtectBlock[2] == PAGE_READWRITE) &&
		(dwProtectBlock[3] == 0));

	return(TRUE);
}

BOOL VMInfo::Query(LPCVOID pvAddress,VMQUERY &vmq) 
{
	if( m_dwAllocGran == 0 ) 
	{
		// Set allocation granularity if this is the first call
		SYSTEM_INFO sinf;
		::GetSystemInfo(&sinf);
		m_dwAllocGran = sinf.dwAllocationGranularity;
	}

	ZeroMemory(&vmq, sizeof(vmq));

	// Get the MEMORY_BASIC_INFORMATION for the passed address.
	MEMORY_BASIC_INFORMATION mbi={0};
	BOOL fOk = (::VirtualQueryEx(m_hProcess, pvAddress, &mbi, sizeof(mbi)) 
		== sizeof(mbi));

	if( !fOk )
		return(fOk);   // Bad memory address, return failure

	// The MEMORY_BASIC_INFORMATION structure contains valid information.
	// Time to start setting the members of our own VMQUERY structure.

	// First, fill in the block members. We'll fill the region members later.
	switch (mbi.State)
	{
	case MEM_FREE:       // Free block (not reserved)
		vmq.pvBlkBaseAddress		= NULL;
		vmq.BlkSize					= 0;
		vmq.dwBlkProtection			= 0;
		vmq.dwBlkStorage			= MEM_FREE;
		break;

	case MEM_RESERVE:    // Reserved block without committed storage in it.
		vmq.pvBlkBaseAddress		= mbi.BaseAddress;
		vmq.BlkSize					= mbi.RegionSize;

		// For an uncommitted block, mbi.Protect is invalid. So we will 
		// show that the reserved block inherits the protection attribute 
		// of the region in which it is contained.
		vmq.dwBlkProtection			= mbi.AllocationProtect;  
		vmq.dwBlkStorage			= MEM_RESERVE;
		break;

	case MEM_COMMIT:     // Reserved block with committed storage in it.
		vmq.pvBlkBaseAddress		= mbi.BaseAddress;
		vmq.BlkSize					= mbi.RegionSize;
		vmq.dwBlkProtection			= mbi.Protect;   
		vmq.dwBlkStorage			= mbi.Type;
		break;

	default:
		DebugBreak();
		break;
	}

	// Now fill in the region data members.
	VMQUERY_HELP VMQHelp;
	switch (mbi.State) 
	{
	  case MEM_FREE:       // Free block (not reserved)
		  vmq.pvRgnBaseAddress = mbi.BaseAddress;
		  vmq.dwRgnProtection  = mbi.AllocationProtect;
		  vmq.RgnSize          = mbi.RegionSize;
		  vmq.dwRgnStorage     = MEM_FREE;
		  vmq.dwRgnBlocks      = 0;
		  vmq.dwRgnGuardBlks   = 0;
		  vmq.fRgnIsAStack	   = FALSE;
		  break;

	  case MEM_RESERVE:    // Reserved block without committed storage in it.
		  vmq.pvRgnBaseAddress = mbi.AllocationBase;
		  vmq.dwRgnProtection  = mbi.AllocationProtect;

		  // Iterate through all blocks to get complete region information.         
		  QueryHelp(pvAddress, VMQHelp);

		  vmq.RgnSize          = VMQHelp.RgnSize;
		  vmq.dwRgnStorage     = VMQHelp.dwRgnStorage;
		  vmq.dwRgnBlocks      = VMQHelp.dwRgnBlocks;
		  vmq.dwRgnGuardBlks   = VMQHelp.dwRgnGuardBlks;
		  vmq.fRgnIsAStack     = VMQHelp.fRgnIsAStack;
		  break;

	  case MEM_COMMIT:     // Reserved block with committed storage in it.
		  vmq.pvRgnBaseAddress = mbi.AllocationBase;
		  vmq.dwRgnProtection  = mbi.AllocationProtect;

		  // Iterate through all blocks to get complete region information.         
		  QueryHelp(pvAddress, VMQHelp);

		  vmq.RgnSize          = VMQHelp.RgnSize;
		  vmq.dwRgnStorage     = VMQHelp.dwRgnStorage;
		  vmq.dwRgnBlocks      = VMQHelp.dwRgnBlocks;
		  vmq.dwRgnGuardBlks   = VMQHelp.dwRgnGuardBlks;
		  vmq.fRgnIsAStack     = VMQHelp.fRgnIsAStack;
		  break;

	  default:
		  DebugBreak();
		  break;
	}

	return(fOk);
}


void VMInfo::Enum(HANDLE hProcess)
{
	// Çå¿Õ
	m_vecVMInfo.clear();

	if( hProcess==NULL )
	{
		if( m_hProcess == NULL ) 
		{
			return;
		}
	}
	else
	{
		m_hProcess=hProcess;
	}

	// Walk the virtual address space, adding entries to the list box.
	BOOL fOk = TRUE;
	PVOID pvAddress = NULL;

	while(fOk) 
	{
		VMQUERY vmq;
		fOk = Query(pvAddress, vmq);

		if (fOk) 
		{
			if( m_bExpandRegions ) 
			{
				for(DWORD dwBlock = 0; fOk && (dwBlock < vmq.dwRgnBlocks); dwBlock++) 
				{
					// Get the address of the next region to test.
					pvAddress = ((PBYTE) pvAddress + vmq.BlkSize);
					if (dwBlock < vmq.dwRgnBlocks - 1) 
					{
						// Don't query the memory info after the last block.
						fOk = Query(pvAddress, vmq);
					}
				}
			}

			// Get the address of the next region to test.
			pvAddress = ((PBYTE) vmq.pvRgnBaseAddress + vmq.RgnSize);
		}

		m_vecVMInfo.push_back(vmq);
	}
}