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


// 模块详细信息
class ModuleInfo
{
	public:
		HICON	m_hIcon;					// 模块图标
		string	m_strModuleName;			// 模块名字
		string	m_strModuleBaseAddr;		// 模块基地址
		string  m_strModuleSize;			// 模块大小
		string	m_strModulePath;			// 模块路径
		string  m_strCompanyName;			// 模块公司名称
		string	m_strDescription;			// 模块介绍
		string	m_strVersion;				// 模块版本
		string	m_strInternalName;			// 模块内部名称
		string	m_strCopyright;				// 模块版权
		string	m_strOrignalName;			// 模块原始名称
		string	m_strProductName;			// 模块创建名称
		string	m_strProductVersion;		// 模块创建信息
		string	m_strComments;				// 模块注释
		string	m_strLegalTrademarks;		// 模块法律交易
		string	m_strPrivateBuild;			// 模块内部创建号
		string	m_strSpecialBuild;			// 模块特别创建号
		string	m_strSubSystem;				// 模块所属系统
		string	m_strProcessID;				// 模块所属进程ID
		string	m_strLinker;				// 模块链接
		string	m_strDateTime;				// 模块创建日期时间

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
		//void	Clear();		// 清空
		void	Enum();			// 遍历模块

	private:
		void	GetModuleName(ModuleInfo &moduleInfo);
		void	GetModuleProcessID(ModuleInfo &moduleInfo,int nPID);
		void 	GetModuleBaseAddr(ModuleInfo &moduleInfo,BYTE *pByteAddr);
		void	GetModuleBaseSize(ModuleInfo &moduleInfo,DWORD dwBaseSize);

		void	GetModuleIcon(ModuleInfo &moduleInfo,LPCTSTR lpszPath);

		void	GetModuleSpecialInfo(ModuleInfo &moduleInfo,LPCTSTR lpszDllName);
		void	GetModuleMoreInfo(ModuleInfo &moduleInfo);

	private:
		// 时间转换
		BOOL	TimeDateStampToFileTime(DWORD timeDateStamp, LPFILETIME pFileTime);
		BOOL	GetFileDateAsString(LPFILETIME pFt, LPTSTR pszDate, unsigned cbIn);
		BOOL	GetFileTimeAsString(LPFILETIME pFt, LPTSTR pszTime, unsigned cbIn, BOOL fSeconds);

	private:
		TvecModuleInfo			m_vecModuleInfo;				// 进程模块
		map<string,ModuleInfo>	m_mapModuleInfo;				// 系统模块

		DWORD			m_dwProcessID;
};

};

#endif