#ifndef __VOLUME_INFO_H
#define __VOLUME_INFO_H

#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>


using namespace std;

#ifdef _UNICODE
#define string wstring
#endif


namespace Volume
{
	typedef struct _tagDiskInfo
	{
		string	strName;			// 分区卷标名
		string	strType;			// 分区类型
		string	strFileSystem;		// 分区格式
		//string	strSerialNum;		// 分区序列号
		//string	strTotalByte;		// 分区的总容量
		//string	strFreeByte;		// 分区的可用容量

		DWORD	dwType;
		DWORD	dwSerialNum;
		__int64 iTotalByte;
		__int64 iFreeByte;

		//BOOL	bPreserved;			// 保持名字支持
		//BOOL	bCaseSensitive;		// 快速检索支持
		//BOOL	bCompressed;		// 压缩支持
		//BOOL	bNameStream;		// 名字流支持
		//BOOL	bPersistentACL;		// ACL支持
		//BOOL	bReadOnly;			// 只读支持
		//BOOL	bSequencialWrite;	// 顺序写支持
		//BOOL	bEncryption;		// 加密支持
		//BOOL	bObjectIDS;			// 对象标识支持
		//BOOL	bReparsePoint;		// 重新扫描点支持
		//BOOL	bSparseFile;		// 稀疏文件支持
		//BOOL	bTranscation;		// 事务支持
		//BOOL	bUnicodeFileName;	// 文件名Unicode支持
		//BOOL	bQuotas;			// 配额支持
		//BOOL	bLongFileName;		// 长文件名支持
		//BOOL	bHasCompressedFile;	// 是否有压缩文件
	}DiskInfo;


	class DiskVolume
	{
	public:
		DiskVolume(void);
		virtual ~DiskVolume(void);

	public:
		void	Enum();
		void	Clear();

		vector<DiskInfo> &GetDiskVolumeInfo() {return m_vecVolumeInfo;}

		BOOL	SetVolumeName(LPCTSTR lpszVolumeRootName,LPCTSTR lpszLable);
		BOOL	HasLongFilenameSupport(DWORD dwMaxLen);
		BOOL	TestSupport(DWORD uFlag);
	private:
		DWORD	GetVolumeType(LPTSTR lpszVolumeRootName);
		void	GetVolumeType(LPTSTR lpszVolumeRootName,DiskInfo &diskInfo);

		void	GetVolumeInformation(LPTSTR lpszVolumeRootName,DiskInfo &diskInfo);
		void	GetDiskByte(LPCTSTR lpszVolumeRootName,DiskInfo &diskInfo);
	
	private:
		vector<DiskInfo> m_vecVolumeInfo;
	};// end of class
}; // end of namespace 


#endif