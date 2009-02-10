#include "stdafx.h"
#include "DiskInfo.h"


using namespace Volume;


DiskVolume::DiskVolume(void)
{
}

DiskVolume::~DiskVolume(void)
{
	Clear();
}


void DiskVolume::Enum()
{
	// 清空
	Clear();

	// 获取当前系统的分区总数
	TCHAR szVolume[64]={0};
	::GetLogicalDriveStrings(63,szVolume);

	TCHAR *pszTemp=szVolume;
	DiskInfo diskInfo;

	while(*pszTemp)
	{
		// 获取每个分区的详细信息
		diskInfo.strName=pszTemp;
		GetVolumeType(pszTemp,diskInfo);
		GetVolumeInformation(pszTemp,diskInfo);
		
		
		m_vecVolumeInfo.push_back(diskInfo);

		// Go to the next NULL character.
		while (*pszTemp++);
	}
}

void DiskVolume::Clear()
{
	m_vecVolumeInfo.clear();
}

void DiskVolume::GetVolumeType(LPTSTR lpszVolumeRootName,DiskInfo &diskInfo)
{
	DWORD dwType=GetVolumeType(lpszVolumeRootName);
	
	string strVolumeType;
	switch(dwType)
	{
	case DRIVE_UNKNOWN:			// The drive type cannot be determined.
		strVolumeType=_T("未知");
		break;
	case DRIVE_NO_ROOT_DIR:		// The root path is invalid; for example, there is no volume is mounted at the path.
		strVolumeType=_T("根目录错误");
		break;
	case DRIVE_REMOVABLE:		// The drive has removable media; for example, a floppy drive or flash card reader.
		strVolumeType=_T("移动设备");
		break;
	case DRIVE_FIXED:			// The drive has fixed media; for example, a hard drive, flash drive, or thumb drive.
		strVolumeType=_T("硬盘");
		break;
	case DRIVE_REMOTE:			// The drive is a remote (network) drive.
		strVolumeType=_T("远程设备");
		break;
	case DRIVE_CDROM:			// The drive is a CD-ROM drive.
		strVolumeType=_T("CD-ROM");
		break;
	case DRIVE_RAMDISK:			// The drive is a RAM disk.
		strVolumeType=_T("内存盘");
		break;
	default:
		{
			TCHAR szError[32]={0};
			wsprintf(szError,_T("未知错误:%d"),GetLastError());
			strVolumeType=szError;
		}
		break;
	}

	diskInfo.dwType	= dwType;
	diskInfo.strType= strVolumeType;
}

DWORD DiskVolume::GetVolumeType(LPTSTR lpszVolumeRootName)
{
	return ::GetDriveType(lpszVolumeRootName);
}

void DiskVolume::GetVolumeInformation(LPTSTR lpszVolumeRootName, DiskInfo &diskInfo)
{
	TCHAR szVolumeName[128]			={0};
	TCHAR szFileSystemNameBuf[128]	={0};
	DWORD dwVolumeSerialNum			=0;
	DWORD dwMaxComponentLen			=0;
	DWORD dwFileSystemFlag			=0;
	

	if( ::GetVolumeInformation(lpszVolumeRootName,szVolumeName,128,&dwVolumeSerialNum,
		&dwMaxComponentLen,&dwFileSystemFlag,szFileSystemNameBuf,128) )
	{
		diskInfo.strName		+=szVolumeName;
		diskInfo.dwSerialNum	=dwVolumeSerialNum;
		diskInfo.strFileSystem	=szFileSystemNameBuf;

		/*TCHAR szSerialNum[64]={0};
		wsprintf(szSerialNum,_T("%ld"),dwVolumeSerialNum);
		diskInfo.strSerialNum=szSerialNum;
		*/

		GetDiskByte(lpszVolumeRootName,diskInfo);
		//diskInfo.strSerialNum.
		//diskInfo.
	}
}


void DiskVolume::GetDiskByte(LPCTSTR lpszVolumeRootName,DiskInfo &diskInfo)
{
	// 如果是光驱则退出
	if( diskInfo.dwType==DRIVE_CDROM )
	{
		diskInfo.iTotalByte=0;
		diskInfo.iFreeByte=0;

		return;
	}
		
	ULARGE_INTEGER FreeBytesAvailableToCaller;
	ULARGE_INTEGER TotalNumberOfBytes;
	ULARGE_INTEGER TotalNumberOfFreeBytes;

	OSVERSIONINFO os;
	ULARGE_INTEGER empty = {0,0};
	BOOL ok = FALSE;

	// Check to see what OS version we are...
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &os );
	if( os.dwPlatformId==VER_PLATFORM_WIN32_NT ) 
		ok = TRUE;
	if( (os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) && (LOWORD(os.dwBuildNumber)>1000) ) 
		ok = TRUE;

	if ( ok ) 
	{
		// Prepare funciton call
		FreeBytesAvailableToCaller	= empty;
		TotalNumberOfBytes			= empty; 
		TotalNumberOfFreeBytes		= empty; 
		// Call the function through the pointer.
		BOOL success = GetDiskFreeSpaceEx( lpszVolumeRootName,
			&FreeBytesAvailableToCaller, 
			&TotalNumberOfBytes, 
			&TotalNumberOfFreeBytes );
		if( !success ) 
			return;
		
		diskInfo.iTotalByte=TotalNumberOfBytes.QuadPart;
		diskInfo.iFreeByte=FreeBytesAvailableToCaller.QuadPart;
	}
	else 
	{
		// Not a GetDiskFreeSpaceEx(...) kind of OS...
		// So we'll just have to do with GetDiskFreeSpace(...)
		DWORD SectorsPerCluster;
		DWORD BytesPerSector;
		DWORD NumberOfFreeClusters;
		DWORD TotalNumberOfClusters;

		if( ::GetDiskFreeSpace( lpszVolumeRootName, &SectorsPerCluster, &BytesPerSector, 
			&NumberOfFreeClusters, &TotalNumberOfClusters ) == FALSE ) 
			return; //Error

		diskInfo.iTotalByte=SectorsPerCluster * BytesPerSector * TotalNumberOfClusters;
		diskInfo.iFreeByte=SectorsPerCluster * BytesPerSector * NumberOfFreeClusters;
	}

	/*TCHAR szTemp[64]={0};
	wsprintf(szTemp,_T("%I64d"),diskInfo.iTotalByte);
	diskInfo.strTotalByte=szTemp;

	wsprintf(szTemp,_T("%I64d"),diskInfo.iFreeByte);
	diskInfo.strFreeByte=szTemp;*/
}


BOOL DiskVolume::HasLongFilenameSupport(DWORD dwMaxLen)
{
	if( dwMaxLen>=255 )
		return TRUE;
	else
		return FALSE;
}

BOOL DiskVolume::TestSupport(DWORD uFlag)
{
	switch(uFlag)
	{
	case FILE_CASE_PRESERVED_NAMES:
		if( (uFlag&FILE_CASE_PRESERVED_NAMES)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_CASE_SENSITIVE_SEARCH:
		if( (uFlag&FILE_CASE_SENSITIVE_SEARCH)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_FILE_COMPRESSION:
		if( (uFlag&FILE_FILE_COMPRESSION)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_NAMED_STREAMS:
		if( (uFlag&FILE_NAMED_STREAMS)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_PERSISTENT_ACLS:
		if( (uFlag&FILE_PERSISTENT_ACLS)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_READ_ONLY_VOLUME:
		if( (uFlag&FILE_READ_ONLY_VOLUME)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	/*case FILE_SEQUENTIAL_WRITE_ONCE:
		if( (uFlag&FILE_SEQUENTIAL_WRITE_ONCE)>0 )
		{
			diskInfo.bSequencialWrite=TRUE;
		}
		else
		{
			diskInfo.bSequencialWrite=FALSE;
		}
		break;*/
	case FILE_SUPPORTS_ENCRYPTION:
		if( (uFlag&FILE_SUPPORTS_ENCRYPTION)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_SUPPORTS_OBJECT_IDS:
		if( (uFlag&FILE_SUPPORTS_OBJECT_IDS)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_SUPPORTS_REPARSE_POINTS:
		if( (uFlag&FILE_SUPPORTS_REPARSE_POINTS)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_SUPPORTS_SPARSE_FILES:
		if( (uFlag&FILE_SUPPORTS_SPARSE_FILES)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	/*case FILE_SUPPORTS_TRANSACTIONS:
		if( (uFlag&FILE_SUPPORTS_TRANSACTIONS)>0 )
		{
			diskInfo.bTranscation=TRUE;
		}
		else
		{
			diskInfo.bTranscation=FALSE;
		}
		break;*/
	case FILE_UNICODE_ON_DISK:
		if( (uFlag&FILE_UNICODE_ON_DISK)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_VOLUME_IS_COMPRESSED:
		if( (uFlag&FILE_VOLUME_IS_COMPRESSED)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	case FILE_VOLUME_QUOTAS:
		if( (uFlag&FILE_VOLUME_QUOTAS)>0 )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		break;
	default:
		return FALSE;
		break;
	}
}

BOOL DiskVolume::SetVolumeName(LPCTSTR lpszVolumeRootName,LPCTSTR lpszLable)
{
	return ::SetVolumeLabel(lpszVolumeRootName,lpszLable);
}