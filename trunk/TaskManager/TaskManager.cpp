// TaskManager.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "TaskManager.h"
#include "TaskManagerDlg.h"
#include "AdjustPrivilege.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTaskManagerApp

BEGIN_MESSAGE_MAP(CTaskManagerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_COMMAND(ID_FILE_NEW, &CTaskManagerApp::OnFileNew)
	ON_COMMAND(ID_FILE_EXIT, &CTaskManagerApp::OnFileExit)
	ON_COMMAND(ID_OPTION_TOP, &CTaskManagerApp::OnOptionTop)
	ON_COMMAND(ID_OPTION_MIN, &CTaskManagerApp::OnOptionMin)
	ON_COMMAND(ID_OPTION_HIDE, &CTaskManagerApp::OnOptionHide)
	ON_COMMAND(ID_VIEW_FLUSH, &CTaskManagerApp::OnViewFlush)
	ON_COMMAND(ID_VIEW_COL, &CTaskManagerApp::OnViewCol)
	ON_COMMAND(ID_HELP_ABOUT, &CTaskManagerApp::OnHelpAbout)
	ON_COMMAND(ID_SYSTEM_STANDBY, &CTaskManagerApp::OnSystemStandby)
	ON_COMMAND(ID_SYSTEM_HIBERANTE, &CTaskManagerApp::OnSystemHiberante)
	ON_COMMAND(ID_SYSTEM_TURNOFF, &CTaskManagerApp::OnSystemTurnoff)
	ON_COMMAND(ID_SYSTEM_RESTART, &CTaskManagerApp::OnSystemRestart)
	ON_COMMAND(ID_SYSTEM_LOGOFF, &CTaskManagerApp::OnSystemLogoff)
	ON_COMMAND(ID_SYSTEM_SWITCH, &CTaskManagerApp::OnSystemSwitch)
END_MESSAGE_MAP()


// CTaskManagerApp 构造

CTaskManagerApp::CTaskManagerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CTaskManagerApp 对象

CTaskManagerApp theApp;


// CTaskManagerApp 初始化

BOOL CTaskManagerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	// 调整权限
	CPrivilegeEnabler::Instance();


	CTaskManagerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此处放置处理何时用“确定”来关闭
		//  对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用“取消”来关闭
		//  对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

void CTaskManagerApp::OnFileNew()
{
	//　打开运行窗口
	CoInitialize(NULL);

	IShellDispatch *shell = NULL;
	HRESULT hr = CoCreateInstance(CLSID_Shell
		, NULL
		, CLSCTX_SERVER
		, IID_IDispatch,
		(void**)&shell);
	if (FAILED(hr) || !shell)
	{
		return;
	}

	shell->FileRun();
	shell->Release();

	CoUninitialize();
}

void CTaskManagerApp::OnFileExit()
{
	// 退出程序
	PostQuitMessage(0);
}

void CTaskManagerApp::OnOptionTop()
{
	// 窗口置顶
	::SetWindowPos(GetMainWnd()->GetSafeHwnd(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
}

void CTaskManagerApp::OnOptionMin()
{
	// 隐藏窗口
}

void CTaskManagerApp::OnOptionHide()
{
	
}

void CTaskManagerApp::OnViewFlush()
{
	// 刷新
}

void CTaskManagerApp::OnViewCol()
{
	// 弹出进程选择例
}

void CTaskManagerApp::OnHelpAbout()
{
	// 关于对话框
}

void CTaskManagerApp::OnSystemStandby()
{
	// 待机
	m_exitWindow.StandBy();
}

void CTaskManagerApp::OnSystemHiberante()
{
	// 休眠
	m_exitWindow.Hibernate();
}

void CTaskManagerApp::OnSystemTurnoff()
{
	// 关机
	m_exitWindow.ForceShutDown();
}

void CTaskManagerApp::OnSystemRestart()
{
	// 重启
	m_exitWindow.ForceReStart();
}

void CTaskManagerApp::OnSystemLogoff()
{
	// 注销
	m_exitWindow.LogOff();
}

void CTaskManagerApp::OnSystemSwitch()
{
	// 切换用户
	m_exitWindow.LockWindow();
}
