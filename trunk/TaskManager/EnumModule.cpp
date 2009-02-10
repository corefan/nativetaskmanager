#include "stdafx.h"
#include "EnumModule.h"
#include "ToolHelp.h"
#include "ModulVer.h"

using namespace enumModule;

#ifdef _UNICODE
#define wmemset	memset
#endif


EnumModule::EnumModule(DWORD dwProcessID/*dwProcessID=0*/)
	: m_dwProcessID(dwProcessID)
{

}

EnumModule::EnumModule(const EnumModule &src)
{
	*this=src;
}

EnumModule::~EnumModule()
{
	Clear(m_vecModuleInfo);
	TRACE("~EnumModule()\n");
}


TvecModuleInfo &EnumModule::GetModuleInfo()
{
	return m_vecModuleInfo;
}

map<string,ModuleInfo> &EnumModule::GetSystemModuleInfo()
{
	return m_mapModuleInfo;
}

template<typename T>
void EnumModule::Clear(vector<T>& vt )
{
	// 销毁icon句柄
	for(vector<T>::iterator iter=vt.begin(); iter!=vt.end(); ++iter)
	{
		if( iter->m_hIcon!=NULL )
			DestroyIcon(iter->m_hIcon);
	}
	/*
	for(map<string,ModuleInfo>::iterator iter=m_mapModuleInfo.begin(); iter!=m_mapModuleInfo.end(); ++iter)
	{
		if( iter->second.m_hIcon!=NULL )
			::DestroyIcon(iter->second.m_hIcon);
	}*/

	vt.clear();
	m_mapModuleInfo.clear();
}

// 遍历模块
void EnumModule::Enum()
{
	// 清空
	Clear(m_vecModuleInfo);

	
	// 系统模块
	if( m_dwProcessID==0 )
	{
		ToolHelp thePro(TH32CS_SNAPPROCESS);
		PROCESSENTRY32 pe={sizeof(pe)};
		BOOL bOk=thePro.ProcessFirst(&pe);

		ModuleInfo modInfo;
		for(; bOk; bOk=thePro.ProcessNext(&pe))
		{
			ToolHelp theModule(TH32CS_SNAPMODULE,pe.th32ProcessID);
			MODULEENTRY32 Mod={sizeof(Mod)};
			BOOL bSuc=theModule.ModuleFirst(&Mod);

			for(; bSuc; bSuc=theModule.ModuleNext(&Mod))
			{
				modInfo.m_strModuleName		=Mod.szModule;			// 获取模块名称
				modInfo.m_strModulePath		=Mod.szExePath;			// 获取模块路径

				GetModuleProcessID(modInfo,Mod.th32ProcessID);		// 获取模块所属进程ID
				GetModuleBaseAddr(modInfo,Mod.modBaseAddr);			// 获取模块基地址
				GetModuleBaseSize(modInfo,Mod.modBaseSize);			// 获取模块大小

				GetModuleIcon(modInfo,Mod.szExePath);				// 获取模块图标

				GetModuleSpecialInfo(modInfo,Mod.szModule);			// 获取模块详细信息
				GetModuleMoreInfo(modInfo);							// 获取模块特殊信息

				m_mapModuleInfo[modInfo.m_strModuleName]=modInfo;			// 添加数据
			}
		}
	}
	else
	{
		// 遍历进程模块信息
		MODULEENTRY32 me32={sizeof(me32)};
		ToolHelp theModule(TH32CS_SNAPMODULE,m_dwProcessID);

		
		for(BOOL bModuleOk=theModule.ModuleFirst(&me32); bModuleOk; bModuleOk=theModule.ModuleNext(&me32))
		{
			ModuleInfo modInfo;
			modInfo.m_strModuleName		=me32.szModule;			// 获取模块名称
			modInfo.m_strModulePath		=me32.szExePath;		// 获取模块路径

			GetModuleProcessID(modInfo,me32.th32ProcessID);		// 获取模块所属进程ID
			GetModuleBaseAddr(modInfo,me32.modBaseAddr);		// 获取模块基地址
			GetModuleBaseSize(modInfo,me32.modBaseSize);		// 获取模块大小
			
			GetModuleIcon(modInfo,me32.szExePath);				// 获取模块图标

			GetModuleSpecialInfo(modInfo,me32.szModule);		// 获取模块详细信息
			GetModuleMoreInfo(modInfo);							// 获取模块特殊信息

			m_vecModuleInfo.push_back(modInfo);					// 添加数据
		}
	}
}

void EnumModule::GetModuleProcessID(ModuleInfo &moduleInfo,int nPID)
{
	TCHAR szTemp[16]={0};
	wsprintf(szTemp,_T("%d"),nPID);

	moduleInfo.m_strProcessID=szTemp;
}

void EnumModule::GetModuleBaseAddr(ModuleInfo &moduleInfo,BYTE *pByteAddr)
{
	TCHAR szBaseAddr[32]={0};
	wsprintf(szBaseAddr,_T("%p"),pByteAddr);

	moduleInfo.m_strModuleBaseAddr=szBaseAddr;
}

void EnumModule::GetModuleBaseSize(ModuleInfo &moduleInfo,DWORD dwBaseSize)
{
	TCHAR szBaseSize[32]={0};

	dwBaseSize/=1024;
	wsprintf(szBaseSize,_T("%d KB"),dwBaseSize);

	moduleInfo.m_strModuleSize=szBaseSize;
}

void EnumModule::GetModuleIcon(ModuleInfo &moduleInfo,LPCTSTR lpszPath)
{
	SHFILEINFO shfi={0};

	__try
	{
		// 获取图标
		int nRet=(int)SHGetFileInfo(lpszPath, 0,&shfi, sizeof(shfi), SHGFI_ICON);
	}
	__finally
	{
		
	}

	moduleInfo.m_hIcon=shfi.hIcon;
}

void EnumModule::GetModuleSpecialInfo(ModuleInfo &moduleInfo,LPCTSTR lpszDllName)
{
	CModuleVersion ver;
	//DLLVERSIONINFO dvi;
	
	//static TCHAR szTemp[64]={0};

	if( ver.GetFileVersionInfo(lpszDllName) )
	{
		/*wsprintf(szTemp,_T("%d.%d.%d.%d"),HIWORD(ver.dwFileVersionMS), LOWORD(ver.dwFileVersionMS),
			HIWORD(ver.dwFileVersionLS), LOWORD(ver.dwFileVersionLS));

		moduleInfo.m_strVersion			=szTemp;*/
		moduleInfo.m_strCompanyName		=ver.GetValue(_T("CompanyName"));
		moduleInfo.m_strDescription		=ver.GetValue(_T("FileDescription"));
		moduleInfo.m_strVersion			=ver.GetValue(_T("FileVersion"));
		moduleInfo.m_strInternalName	=ver.GetValue(_T("InternalName"));
		moduleInfo.m_strCopyright		=ver.GetValue(_T("LegalCopyright"));
		moduleInfo.m_strOrignalName		=ver.GetValue(_T("OriginalFilename"));
		moduleInfo.m_strProductName		=ver.GetValue(_T("ProductName"));
		moduleInfo.m_strProductVersion	=ver.GetValue(_T("ProductVersion"));
		moduleInfo.m_strComments		=ver.GetValue(_T("Comments"));
		moduleInfo.m_strLegalTrademarks	=ver.GetValue(_T("LegalTrademarks"));
		moduleInfo.m_strPrivateBuild	=ver.GetValue(_T("PrivateBuild"));
		moduleInfo.m_strSpecialBuild	=ver.GetValue(_T("SpecialBuild"));
	}

	// 2nd get version using DllGetVersion API
	//

	/*DLLVERSIONINFO dvi;
	if (ver.DllGetVersion(m_strDllName, dvi)) 
	{
		strText.Format(_T("DLL Version = %d.%02d"), 
			dvi.dwMajorVersion, 
			dvi.dwMinorVersion);


		strText.Format(_T("Build# = %d\n"),dvi.dwBuildNumber);


		strText=_T("Platform is ");
		if (dvi.dwPlatformID == DLLVER_PLATFORM_WINDOWS)
			strText +=_T("Windows");
		else if (dvi.dwPlatformID == DLLVER_PLATFORM_NT)
			strText +=_T("Windows NT");
		else 
			strText += _T("unrecognized");

	}*/
}

void EnumModule::GetModuleMoreInfo(ModuleInfo &moduleInfo)
{
	PE_EXE exe(moduleInfo.m_strModulePath.c_str());
	if (!exe.IsValid())
	{
		return;
	}

	// 获取所属子系统
	WORD SubSystem=exe.GetSubsystem();
	string szSubSystem;

	switch(SubSystem)
	{
	case IMAGE_SUBSYSTEM_NATIVE:
		szSubSystem = _T("Native");
		break;

	case IMAGE_SUBSYSTEM_WINDOWS_GUI:
		szSubSystem = _T("GUI");
		break;

	case IMAGE_SUBSYSTEM_WINDOWS_CUI:
		szSubSystem = _T("Console");
		break;

	case IMAGE_SUBSYSTEM_OS2_CUI:
		szSubSystem = _T("OS/2");
		break;

	case IMAGE_SUBSYSTEM_POSIX_CUI:
		szSubSystem = _T("Posix");
		break;

	case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:
		szSubSystem = _T("Win9x driver");
		break;

	case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:
		szSubSystem = _T("CE");
		break;

	case IMAGE_SUBSYSTEM_UNKNOWN:
	default:
		szSubSystem = _T("Unknown");
		break;
	}

	moduleInfo.m_strSubSystem=szSubSystem;

	// 获取创建时间
	FILETIME fTime;
	TCHAR szFileDate[64]={0},szFileTime[64]={0};

	TimeDateStampToFileTime(exe.GetTimeDateStamp(), &fTime);
	GetFileDateAsString(&fTime, szFileDate, sizeof(szFileDate));
	GetFileTimeAsString(&fTime, szFileTime, sizeof(szFileTime), TRUE);

	moduleInfo.m_strDateTime+=szFileDate;
	moduleInfo.m_strDateTime+=_T(" ");
	moduleInfo.m_strDateTime+=szFileTime;

	// 获取Linker版本
	WORD version=exe.GetLinkerVersion();
	TCHAR szLinkerVersion[32]={0};
	wsprintf(szLinkerVersion,_T("%d"),version);
	moduleInfo.m_strLinker=szLinkerVersion;
}

// Convert a TimeDateStamp (i.e., # of seconds since 1/1/1970) into a FILETIME
BOOL EnumModule::TimeDateStampToFileTime(DWORD timeDateStamp, LPFILETIME pFileTime)
{
   __int64 t1970 = 0x019DB1DED53E8000; // Magic... GMT...  Don't ask....

   __int64 timeStampIn100nsIncr = (__int64)timeDateStamp * 10000000;

   __int64 finalValue = t1970 + timeStampIn100nsIncr;

   memcpy( pFileTime, &finalValue, sizeof( finalValue ) );

   return(TRUE);
}

BOOL EnumModule::GetFileDateAsString(LPFILETIME pFt, LPTSTR pszDate, unsigned cbIn)
{
	FILETIME ftLocal;
	SYSTEMTIME st;

	if( !::FileTimeToLocalFileTime(pFt, &ftLocal))
		return(FALSE);

	if( !::FileTimeToSystemTime(&ftLocal, &st))
		return(FALSE);

	TCHAR szTemp[32]={0};

	wsprintf(
		szTemp, 
		_T("%04u/%02u/%02u"),
		st.wYear,st.wMonth, st.wDay
		);
	lstrcpyn(pszDate, szTemp, cbIn);

	return(TRUE);
}


BOOL EnumModule::GetFileTimeAsString(LPFILETIME pFt, LPTSTR pszTime, unsigned cbIn, BOOL fSeconds)
{
	FILETIME ftLocal;
	SYSTEMTIME st;

	if (!FileTimeToLocalFileTime(pFt, &ftLocal))
		return(FALSE);

	if (!FileTimeToSystemTime(&ftLocal, &st))
		return(FALSE);

	TCHAR szTemp[32]={0};

	if (fSeconds)  // Want seconds???
	{
		wsprintf(szTemp, _T("%02u:%02u:%02u"), st.wHour, st.wMinute, st.wSecond);
	}
	else           // No thanks..  Just hours and minutes
	{
		wsprintf(szTemp, _T("%02u:%02u"), st.wHour, st.wMinute);
	}

	lstrcpyn(pszTime, szTemp, cbIn);

	return(TRUE);
}