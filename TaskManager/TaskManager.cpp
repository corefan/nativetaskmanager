// TaskManager.cpp : ����Ӧ�ó��������Ϊ��
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


// CTaskManagerApp ����

CTaskManagerApp::CTaskManagerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CTaskManagerApp ����

CTaskManagerApp theApp;


// CTaskManagerApp ��ʼ��

BOOL CTaskManagerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));


	// ����Ȩ��
	CPrivilegeEnabler::Instance();


	CTaskManagerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˴����ô����ʱ�á�ȷ�������ر�
		//  �Ի���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ�á�ȡ�������ر�
		//  �Ի���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

void CTaskManagerApp::OnFileNew()
{
	//�������д���
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
	// �˳�����
	PostQuitMessage(0);
}

void CTaskManagerApp::OnOptionTop()
{
	// �����ö�
	::SetWindowPos(GetMainWnd()->GetSafeHwnd(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
}

void CTaskManagerApp::OnOptionMin()
{
	// ���ش���
}

void CTaskManagerApp::OnOptionHide()
{
	
}

void CTaskManagerApp::OnViewFlush()
{
	// ˢ��
}

void CTaskManagerApp::OnViewCol()
{
	// ��������ѡ����
}

void CTaskManagerApp::OnHelpAbout()
{
	// ���ڶԻ���
}

void CTaskManagerApp::OnSystemStandby()
{
	// ����
	m_exitWindow.StandBy();
}

void CTaskManagerApp::OnSystemHiberante()
{
	// ����
	m_exitWindow.Hibernate();
}

void CTaskManagerApp::OnSystemTurnoff()
{
	// �ػ�
	m_exitWindow.ForceShutDown();
}

void CTaskManagerApp::OnSystemRestart()
{
	// ����
	m_exitWindow.ForceReStart();
}

void CTaskManagerApp::OnSystemLogoff()
{
	// ע��
	m_exitWindow.LogOff();
}

void CTaskManagerApp::OnSystemSwitch()
{
	// �л��û�
	m_exitWindow.LockWindow();
}
