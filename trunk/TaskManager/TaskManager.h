// TaskManager.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "ExitWindows.h"

// CTaskManagerApp:
// 有关此类的实现，请参阅 TaskManager.cpp
//

class CTaskManagerApp : public CWinApp
{
public:
	CTaskManagerApp();

// 重写
	public:
	virtual BOOL InitInstance();

private:
	CExitWindows	m_exitWindow;
// 实现

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