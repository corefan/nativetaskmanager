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
		string	strName;			// ���������
		string	strType;			// ��������
		string	strFileSystem;		// ������ʽ
		//string	strSerialNum;		// �������к�
		//string	strTotalByte;		// ������������
		//string	strFreeByte;		// �����Ŀ�������

		DWORD	dwType;
		DWORD	dwSerialNum;
		__int64 iTotalByte;
		__int64 iFreeByte;

		//BOOL	bPreserved;			// ��������֧��
		//BOOL	bCaseSensitive;		// ���ټ���֧��
		//BOOL	bCompressed;		// ѹ��֧��
		//BOOL	bNameStream;		// ������֧��
		//BOOL	bPersistentACL;		// ACL֧��
		//BOOL	bReadOnly;			// ֻ��֧��
		//BOOL	bSequencialWrite;	// ˳��д֧��
		//BOOL	bEncryption;		// ����֧��
		//BOOL	bObjectIDS;			// �����ʶ֧��
		//BOOL	bReparsePoint;		// ����ɨ���֧��
		//BOOL	bSparseFile;		// ϡ���ļ�֧��
		//BOOL	bTranscation;		// ����֧��
		//BOOL	bUnicodeFileName;	// �ļ���Unicode֧��
		//BOOL	bQuotas;			// ���֧��
		//BOOL	bLongFileName;		// ���ļ���֧��
		//BOOL	bHasCompressedFile;	// �Ƿ���ѹ���ļ�
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