/*#############################################################################
# ENUMNTSRV.H
#
# SCA Software International S.A.
# http://www.scasoftware.com
# scaadmin@scasoftware.com
#
# Copyright (c) 1999 SCA Software International S.A.
#
# Date: 05.12.1999.
# Author: Zoran M.Todorovic
#
# This software is provided "AS IS", without a warranty of any kind.
# You are free to use/modify this code but leave this header intact.
#
#############################################################################*/

#ifndef __ENUMNTSRV_H__
#define __ENUMNTSRV_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <deque>
#include <string>
#include <Winsvc.h>

using namespace std;


namespace enumService
{
#ifdef UNICODE
#define string wstring
#endif


typedef struct tagServiceInfo
{
	string		m_strServiceName;
	string		m_strDisplayName;
	CString		m_strBinaryPath;
	string		m_strDescription;
	string		m_strServiceType;
	string		m_strCurrentState;
	string		m_strStartType;
	//CString m_strServiceType;
	//CString m_strStartType;

	HICON		m_hIcon;
	DWORD		m_dwServiceType;
	DWORD		m_dwStartType;
	DWORD		m_dwErrorControl;
	DWORD		m_dwCurrentState;
}ServiceInfo;

typedef deque<ServiceInfo> ServiceInfoList;

//=============================================================================
// class TNtServiceInfo
//
//=============================================================================

class EnumServiceInfo 
{
public:
	EnumServiceInfo();
	EnumServiceInfo(const EnumServiceInfo& source);
	//EnumServiceInfo& operator=(const EnumServiceInfo& source);
	virtual ~EnumServiceInfo();

	ServiceInfoList &GetServiceInfo();

	string GetServiceType(DWORD dwServiceType);
	LPCTSTR GetStartType(DWORD dwStartType);
	LPCTSTR GetErrorControl(DWORD dwErrorControl);
	LPCTSTR GetCurrentState(DWORD dwCurretnState);
	HICON	GetServiceIcon(LPCTSTR lpszPath);
	
	void Clear();
	void EnumServices(DWORD dwType, DWORD dwState);

	static void MyTranslator(unsigned code, EXCEPTION_POINTERS *pInfo)
	{
		throw pInfo;
	}

private:
	ServiceInfoList	m_vecServiceInfo;
	HICON			m_hIcon;
};	// end of class

};	// end of namespace
#endif

/*#############################################################################
# End of file ENUMNTSRV.H
#############################################################################*/
