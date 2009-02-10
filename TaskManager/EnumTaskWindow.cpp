#include "StdAfx.h"
#include "EnumTaskWindow.h"
#include <Psapi.h>

#pragma comment(lib,"psapi")


CEnumTaskWindow::CEnumTaskWindow(void)
{
}

CEnumTaskWindow::~CEnumTaskWindow(void)
{
	// 销毁窗口图标
	for(vector<TaskWndInfo>::iterator iter=m_vecTask.begin(); iter!=m_vecTask.end(); ++iter)
	{
		if( iter->m_hIcon!=NULL )
			::DestroyIcon(iter->m_hIcon);
	}
}

void CEnumTaskWindow::Clear()
{
	// 清除
	for(vector<TaskWndInfo>::iterator iter=m_vecTask.begin(); iter!=m_vecTask.end(); ++iter)
	{
		if( iter->m_hIcon!=NULL )
			::DestroyIcon(iter->m_hIcon);
	}

	vector<TaskWndInfo> veTemp;
	veTemp.swap( m_vecTask );

	m_vecTask.clear();
}

void CEnumTaskWindow::EnumTaskWindow(HWND hWnd)
{
	Clear();

	// 枚举窗口
	::EnumWindows(EnumTaskWindowProc,(LPARAM)(this));
}


BOOL CEnumTaskWindow::EnumTaskWindowProc(HWND hWnd,LPARAM lParam)
{
	CEnumTaskWindow *pThis=(CEnumTaskWindow *)(lParam);

	TCHAR szWndText[128]={0};
    int nRetVal;

	// 检测是否有自己的窗口句柄
	if( 0 != ::GetWindow(hWnd, GW_OWNER) )
		return TRUE;

	// 检测窗口是否可见
	if( FALSE == IsWindowVisible(hWnd) )
		return TRUE;

	// 获取窗口标题
	nRetVal = ::InternalGetWindowText(hWnd, szWndText, 127);
	if( nRetVal == 0 || szWndText[0] == 0 )
		return TRUE;

	// 排除Program Manager
	if( _tcscmp(_T("Program Manager"), szWndText) == 0 )
		return TRUE;
	//if( _tcscmp(_T("Task Management"),szWndText)==0 )
	//	return TRUE;

	// 添加信息
	TaskWndInfo taskInfo;
	taskInfo.m_hWnd		=hWnd;										// 存储句柄
	::GetWindowThreadProcessId(hWnd,&taskInfo.m_dwID);				// 存储Process ID
	taskInfo.m_hIcon	=pThis->GetWindowIcon(hWnd/*taskInfo.m_dwID*/);		// 存储窗口图标句柄
	taskInfo.m_strText	=szWndText;									// 存储名称

	pThis->m_vecTask.push_back(taskInfo);


	return TRUE;
}


HICON CEnumTaskWindow::GetWindowIcon(HWND hWnd/*DWORD dwID*/)
{
	/*
	HANDLE hProcess=NULL;
	TCHAR szImagePath[MAX_PATH]={0};

	SHFILEINFO shfi;
	memset(&shfi,0,sizeof(shfi));

	__try
	{
		// 打开进程句柄
		hProcess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwID);
		if( hProcess==NULL )
			__leave;

		// 获取进程模块路径
		//::GetProcessImageFileName(hProcess,szImagePath,MAX_PATH);
		::GetModuleFileNameEx(hProcess,NULL,szImagePath,MAX_PATH);
		if( szImagePath[0]==NULL )
			__leave;

		// 获取图标
		int nRet=(int)SHGetFileInfo(szImagePath, 0,&shfi, sizeof(shfi), SHGFI_ICON);
		if( nRet>0 )
			return shfi.hIcon;
	}
	__finally
	{
		if( hProcess!=NULL )
			::CloseHandle(hProcess);
	}

	return NULL;
	*/
	
	HICON hIcon = NULL;
	hIcon = (HICON)::GetClassLong(hWnd,GCL_HICON);
	if(hIcon == NULL)
	{
		hIcon = (HICON)::GetClassLong(hWnd,GCL_HICONSM);
		//return hIcon;
	}

	if(hIcon == NULL)
	{
		hIcon = (HICON)::SendMessage(hWnd, WM_GETICON, ICON_BIG, 0);
		//return hIcon;
	}

	if(hIcon == NULL)
	{
		hIcon = (HICON)::SendMessage(hWnd, WM_GETICON, ICON_SMALL, 0);
		//return hIcon;
	}

	return hIcon;
}

vector<TaskWndInfo> &CEnumTaskWindow::GetTaskWndInfo()
{
	return m_vecTask;
}