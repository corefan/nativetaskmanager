#include "StdAfx.h"
#include "EnumTaskWindow.h"
#include <Psapi.h>

#pragma comment(lib,"psapi")


CEnumTaskWindow::CEnumTaskWindow(void)
{
}

CEnumTaskWindow::~CEnumTaskWindow(void)
{
	// ���ٴ���ͼ��
	for(vector<TaskWndInfo>::iterator iter=m_vecTask.begin(); iter!=m_vecTask.end(); ++iter)
	{
		if( iter->m_hIcon!=NULL )
			::DestroyIcon(iter->m_hIcon);
	}
}

void CEnumTaskWindow::Clear()
{
	// ���
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

	// ö�ٴ���
	::EnumWindows(EnumTaskWindowProc,(LPARAM)(this));
}


BOOL CEnumTaskWindow::EnumTaskWindowProc(HWND hWnd,LPARAM lParam)
{
	CEnumTaskWindow *pThis=(CEnumTaskWindow *)(lParam);

	TCHAR szWndText[128]={0};
    int nRetVal;

	// ����Ƿ����Լ��Ĵ��ھ��
	if( 0 != ::GetWindow(hWnd, GW_OWNER) )
		return TRUE;

	// ��ⴰ���Ƿ�ɼ�
	if( FALSE == IsWindowVisible(hWnd) )
		return TRUE;

	// ��ȡ���ڱ���
	nRetVal = ::InternalGetWindowText(hWnd, szWndText, 127);
	if( nRetVal == 0 || szWndText[0] == 0 )
		return TRUE;

	// �ų�Program Manager
	if( _tcscmp(_T("Program Manager"), szWndText) == 0 )
		return TRUE;
	//if( _tcscmp(_T("Task Management"),szWndText)==0 )
	//	return TRUE;

	// �����Ϣ
	TaskWndInfo taskInfo;
	taskInfo.m_hWnd		=hWnd;										// �洢���
	::GetWindowThreadProcessId(hWnd,&taskInfo.m_dwID);				// �洢Process ID
	taskInfo.m_hIcon	=pThis->GetWindowIcon(hWnd/*taskInfo.m_dwID*/);		// �洢����ͼ����
	taskInfo.m_strText	=szWndText;									// �洢����

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
		// �򿪽��̾��
		hProcess=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwID);
		if( hProcess==NULL )
			__leave;

		// ��ȡ����ģ��·��
		//::GetProcessImageFileName(hProcess,szImagePath,MAX_PATH);
		::GetModuleFileNameEx(hProcess,NULL,szImagePath,MAX_PATH);
		if( szImagePath[0]==NULL )
			__leave;

		// ��ȡͼ��
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