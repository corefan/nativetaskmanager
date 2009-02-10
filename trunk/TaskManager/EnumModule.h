#ifndef __ENUM_MODULE_H__
#define __ENUM_MODULE_H__

#include <string>
#include <vector>
#include <map>
#include "PEEXE.h"

using namespace std;

namespace enumModule
{
#ifdef _UNICODE
#define string wstring
#endif


// ģ����ϸ��Ϣ
class ModuleInfo
{
	public:
		HICON	m_hIcon;					// ģ��ͼ��
		string	m_strModuleName;			// ģ������
		string	m_strModuleBaseAddr;		// ģ�����ַ
		string  m_strModuleSize;			// ģ���С
		string	m_strModulePath;			// ģ��·��
		string  m_strCompanyName;			// ģ�鹫˾����
		string	m_strDescription;			// ģ�����
		string	m_strVersion;				// ģ��汾
		string	m_strInternalName;			// ģ���ڲ�����
		string	m_strCopyright;				// ģ���Ȩ
		string	m_strOrignalName;			// ģ��ԭʼ����
		string	m_strProductName;			// ģ�鴴������
		string	m_strProductVersion;		// ģ�鴴����Ϣ
		string	m_strComments;				// ģ��ע��
		string	m_strLegalTrademarks;		// ģ�鷨�ɽ���
		string	m_strPrivateBuild;			// ģ���ڲ�������
		string	m_strSpecialBuild;			// ģ���ر𴴽���
		string	m_strSubSystem;				// ģ������ϵͳ
		string	m_strProcessID;				// ģ����������ID
		string	m_strLinker;				// ģ������
		string	m_strDateTime;				// ģ�鴴������ʱ��

};


typedef vector<ModuleInfo> TvecModuleInfo;


class EnumModule
{
	public:
		EnumModule(DWORD dwProcessID=0);
		EnumModule(const EnumModule &src);
		//EnumProcess &operator=(const EnumProcess &src);
		virtual ~EnumModule();

		TvecModuleInfo &GetModuleInfo();
		map<string,ModuleInfo> &GetSystemModuleInfo();

		template<typename T>
		void Clear(vector<T>& vt );
		//void	Clear();		// ���
		void	Enum();			// ����ģ��

	private:
		void	GetModuleName(ModuleInfo &moduleInfo);
		void	GetModuleProcessID(ModuleInfo &moduleInfo,int nPID);
		void 	GetModuleBaseAddr(ModuleInfo &moduleInfo,BYTE *pByteAddr);
		void	GetModuleBaseSize(ModuleInfo &moduleInfo,DWORD dwBaseSize);

		void	GetModuleIcon(ModuleInfo &moduleInfo,LPCTSTR lpszPath);

		void	GetModuleSpecialInfo(ModuleInfo &moduleInfo,LPCTSTR lpszDllName);
		void	GetModuleMoreInfo(ModuleInfo &moduleInfo);

	private:
		// ʱ��ת��
		BOOL	TimeDateStampToFileTime(DWORD timeDateStamp, LPFILETIME pFileTime);
		BOOL	GetFileDateAsString(LPFILETIME pFt, LPTSTR pszDate, unsigned cbIn);
		BOOL	GetFileTimeAsString(LPFILETIME pFt, LPTSTR pszTime, unsigned cbIn, BOOL fSeconds);

	private:
		TvecModuleInfo			m_vecModuleInfo;				// ����ģ��
		map<string,ModuleInfo>	m_mapModuleInfo;				// ϵͳģ��

		DWORD			m_dwProcessID;
};

};

#endif