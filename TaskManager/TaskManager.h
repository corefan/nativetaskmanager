// TaskManager.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "ExitWindows.h"

// CTaskManagerApp:
// �йش����ʵ�֣������ TaskManager.cpp
//

class CTaskManagerApp : public CWinApp
{
public:
	CTaskManagerApp();

// ��д
	public:
	virtual BOOL InitInstance();

private:
	CExitWindows	m_exitWindow;
// ʵ��

	DECLARE_MESSAGE_MAP()

	afx_msg void OnFileNew();
	afx_msg void OnFileExit();

	afx_msg void OnOptionTop();
	afx_msg void OnOptionMin();
	afx_msg void OnOptionHide();

	afx_msg void OnViewFlush();
	afx_msg void OnViewCol();

	afx_msg void OnHelpAbout();

	afx_msg void OnSystemStandby();
	afx_msg void OnSystemHiberante();
	afx_msg void OnSystemTurnoff();
	afx_msg void OnSystemRestart();
	afx_msg void OnSystemLogoff();
	afx_msg void OnSystemSwitch();
};

extern CTaskManagerApp theApp;