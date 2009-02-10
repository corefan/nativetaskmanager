// ProcessThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessThread.h"

const ThreadColInfo CDlgProcessThread::m_ThreadCol[]=
{
	_T("�߳�ID"),		LVCFMT_LEFT,	50,
	_T("����ʱ��"),		LVCFMT_LEFT,	150,
	_T("����̬ʱ��"),	LVCFMT_LEFT,	100,
	_T("�û�̬ʱ��"),	LVCFMT_LEFT,	100,
	_T("���ȼ�"),		LVCFMT_LEFT,	50,
	_T("״̬"),			LVCFMT_LEFT,	100,
	_T("�������л�����"),LVCFMT_LEFT,	50,
	_T("��ʼ��ַ"),		LVCFMT_LEFT,	100
};

// �����߳���Ϣ�߳�
DWORD WINAPI CDlgProcessThread::ThreadInfoThread(LPVOID pVoid)
{
	CDlgProcessThread *pThis=(CDlgProcessThread *)pVoid;

	// ������Ϣ
	enumThread::EnumThread threadInfo(pThis->m_nProcessID);
	threadInfo.Enum();

	pThis->m_vecThreadInfo=threadInfo.GetThreadInfo();
	
	// ������Ϣ����UI
	pThis->PostMessage(WM_UPDATE_THREAD_INFO,0,0);

#ifdef _DEBUG
	::OutputDebugString(_T("Exsit ThreadInfo Thread\n"));
#endif
	return 0;
}

// CProcessThread �Ի���
BEGIN_EASYSIZE_MAP(CDlgProcessThread)
	EASYSIZE(IDC_LIST_PROCESS_TAB_THREAD,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgProcessThread, CDialog)

CDlgProcessThread::CDlgProcessThread(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessThread::IDD, pParent)
	, m_pThreadThread(NULL)
	, m_nProcessID(0)
	, m_nCurItem(0)
{

}

CDlgProcessThread::~CDlgProcessThread()
{
}

void CDlgProcessThread::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS_TAB_THREAD, m_wndListThread);
}


BEGIN_MESSAGE_MAP(CDlgProcessThread, CDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_INITMENUPOPUP()

	ON_MESSAGE(WM_FLUSH_THREAD,OnFlushThreadInfo)
	ON_MESSAGE(WM_UPDATE_THREAD_INFO,OnLoadThreadInfo)

	ON_COMMAND(ID_THREAD_SUSPEND, &CDlgProcessThread::OnThreadSuspend)
	ON_COMMAND(ID_THREAD_RESUME, &CDlgProcessThread::OnThreadResume)
	ON_COMMAND(ID_THREAD_TERMINATE, &CDlgProcessThread::OnThreadTerminate)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS_TAB_THREAD, &CDlgProcessThread::OnNMRclickListProcessTabThread)

	ON_COMMAND_RANGE(ID_THREAD_REALTIME,ID_THREAD_IDLE,OnMenu)
END_MESSAGE_MAP()


// CProcessThread ��Ϣ�������

void CDlgProcessThread::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CDialog::OnSize(nType, cx, cy);
}



BOOL CDlgProcessThread::OnInitDialog()
{
	INIT_EASYSIZE;

	CDialog::OnInitDialog();



	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgProcessThread::OnDestroy()
{
	// �ȴ������
	if( m_pThreadThread!=NULL )
	{
		::WaitForSingleObject(m_pThreadThread->m_hThread,INFINITE);
		delete m_pThreadThread;
		m_pThreadThread=NULL;
	}

	m_font.DeleteObject();

	CDialog::OnDestroy();
}


void CDlgProcessThread::InitList()
{
	// ���÷��
	m_wndListThread.ModifyStyle(0,LVS_REPORT);
	m_wndListThread.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

	// ��������
	m_font.CreatePointFont(80,_T("΢���ź�"));
	m_wndListThread.SetFont(&m_font);

	// ��ʼ��Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_ThreadCol)/sizeof(m_ThreadCol[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_ThreadCol[i].strColName,m_ThreadCol[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListThread.SetHeadings(strCol);
}

void CDlgProcessThread::InsertItem()
{
	// ��������
	int nIndex=0;
	for(vector<ThreadInfo>::iterator iter=m_vecThreadInfo.begin(); iter!=m_vecThreadInfo.end(); ++iter)
	{
		m_wndListThread.AddItem(iter->m_strThreadID.c_str(),
			iter->m_strCreateTime.c_str(),
			iter->m_strKernelTime.c_str(),
			iter->m_strUserTime.c_str(),
			iter->m_strPriority.c_str(),
			iter->m_strStatus.c_str(),
			iter->m_strSwitchCount.c_str(),
			iter->m_strStartAddr.c_str());

		nIndex++;
	}
}

// �����̶߳�ȡThread��Ϣ
LRESULT CDlgProcessThread::OnFlushThreadInfo(WPARAM wParam,LPARAM lParam)
{
	// �ȴ������
	if( m_pThreadThread!=NULL )
	{
		::WaitForSingleObject(m_pThreadThread->m_hThread,INFINITE);
		delete m_pThreadThread;
		m_pThreadThread=NULL;
	}

	// ���ý���ID
	if( wParam==0 )
		return -1;
	
	m_nProcessID=(int)wParam;

	// ���
	m_vecThreadInfo.clear();

	// �����߳̽��м����ļ���Ϣ
	m_pThreadThread=AfxBeginThread((AFX_THREADPROC)ThreadInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pThreadThread->m_bAutoDelete=FALSE;
	m_pThreadThread->ResumeThread();

	return 0;
}

// ����UI
LRESULT CDlgProcessThread::OnLoadThreadInfo(WPARAM wParam,LPARAM lParam)
{
	m_wndListThread.DeleteAllItems();

	// ��ֹ��˸
	m_wndListThread.SetRedraw(FALSE);

	// �����Ϣ
	InsertItem();

	m_wndListThread.SetRedraw(TRUE);
	m_wndListThread.Invalidate();
	m_wndListThread.UpdateWindow();

	return 0;
}


void CDlgProcessThread::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)   
{   
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}  
void CDlgProcessThread::OnThreadSuspend()
{
	// �����߳�
	DWORD dwThreadID=m_vecThreadInfo.at(m_nCurItem).m_nThreadID;
	HANDLE hThread=::OpenThread(THREAD_SUSPEND_RESUME ,FALSE,dwThreadID);
	if( hThread!=NULL )
	{
		::SuspendThread(hThread);
		::CloseHandle(hThread);
	}

	PostMessage(WM_FLUSH_THREAD,m_nProcessID);
}

void CDlgProcessThread::OnThreadResume()
{
	// �ָ��߳�
	DWORD dwThreadID=m_vecThreadInfo.at(m_nCurItem).m_nThreadID;
	HANDLE hThread=::OpenThread(THREAD_SUSPEND_RESUME ,FALSE,dwThreadID);
	if( hThread!=NULL )
	{
		::ResumeThread(hThread);
		::CloseHandle(hThread);
	}

	PostMessage(WM_FLUSH_THREAD,m_nProcessID);
}

void CDlgProcessThread::OnThreadTerminate()
{
	// ǿ�ƹر��߳�
	DWORD dwThreadID=m_vecThreadInfo.at(m_nCurItem).m_nThreadID;
	HANDLE hThread=::OpenThread(THREAD_TERMINATE ,FALSE,dwThreadID);
	if( hThread!=NULL )
	{
		::TerminateThread(hThread,0);
		int n=GetLastError();
		::CloseHandle(hThread);
	}

	PostMessage(WM_FLUSH_THREAD,m_nProcessID);
}

void CDlgProcessThread::OnMenu(UINT uID)
{
	DWORD dwPriority=0;
	CString strPriority;

	switch(uID)
	{
	case ID_THREAD_REALTIME:
		dwPriority=THREAD_PRIORITY_TIME_CRITICAL;
		strPriority=_T("ʵʱ");
		break;
	case ID_THREAD_HIGHT:
		dwPriority=THREAD_PRIORITY_HIGHEST;
		strPriority=_T("��");
		break;
	case ID_THREAD_ABOVE_NORMAL:
		dwPriority=THREAD_PRIORITY_ABOVE_NORMAL;
		strPriority=_T("�ϸ�");
		break;
	case ID_THREAD_NORMAL:
		dwPriority=THREAD_PRIORITY_NORMAL;
		strPriority=_T("����");
		break;
	case ID_THREAD_BELOW_NORMAL:
		dwPriority=THREAD_PRIORITY_BELOW_NORMAL;
		strPriority=_T("�ϵ�");
		break;
	case ID_THREAD_LOW:
		dwPriority=THREAD_PRIORITY_LOWEST;
		strPriority=_T("��");
		break;
	case ID_THREAD_IDLE:
		dwPriority=THREAD_PRIORITY_IDLE;
		strPriority=_T("����");
		break;

	default:
		break;
	}

	//m_cs.Lock();

	// �޸Ľ�����Ϣ
	m_vecThreadInfo.at(m_nCurItem).m_nPriority=dwPriority;
	m_vecThreadInfo.at(m_nCurItem).m_strPriority=strPriority;

	// ���ý������ȼ�
	HANDLE hThread=::OpenThread(THREAD_SET_INFORMATION ,FALSE,m_vecThreadInfo.at(m_nCurItem).m_nThreadID);

	//m_cs.Unlock();

	if( hThread!=NULL )
	{
		::SetThreadPriority(hThread,dwPriority);
		int n=GetLastError();
		::CloseHandle(hThread);
	}

	PostMessage(WM_FLUSH_THREAD,m_nProcessID);
}

void CDlgProcessThread::OnNMRclickListProcessTabThread(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nItem=pListView->iItem;
	if( nItem==-1 )					// û��ѡ����
	{
		//
	}
	else							// ѡ��
	{
		m_nCurItem=nItem;
		PopupMenu();
	}

	*pResult = 0;
}


void CDlgProcessThread::PopupMenu()
{
	// ���ز˵�
	CMenu TrackMenuTemp,*pTrackMenu;
	TrackMenuTemp.LoadMenu(IDR_MENU_PROCESS_THREAD);

	//��ʾ
	pTrackMenu=TrackMenuTemp.GetSubMenu(0);
	
	// ���ò˵�״̬
	SetMenuState(pTrackMenu);
		
	// ��ȡ��ǰ���λ��
	CPoint point;
	GetCursorPos(&point);

	// �����˵�
	pTrackMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
}

void CDlgProcessThread::SetMenuState(CMenu *pTrackMenu)
{
	// �����߳����ȼ�״̬
	int uMenuID=0;

	switch(m_vecThreadInfo.at(m_nCurItem).m_nPriority)
	{
	case THREAD_PRIORITY_NORMAL:
		uMenuID=ID_THREAD_NORMAL;
		break;
	case THREAD_PRIORITY_ABOVE_NORMAL:
		uMenuID=ID_THREAD_ABOVE_NORMAL;
		break;
	case THREAD_PRIORITY_HIGHEST:
		uMenuID=ID_THREAD_HIGHT;
		break;
	case THREAD_PRIORITY_TIME_CRITICAL:
		uMenuID=ID_THREAD_REALTIME;
		break;
	case THREAD_PRIORITY_BELOW_NORMAL:
		uMenuID=ID_THREAD_BELOW_NORMAL;
		break;
	case THREAD_PRIORITY_LOWEST:
		uMenuID=ID_THREAD_LOW;
		break;
	case THREAD_PRIORITY_IDLE:
		uMenuID=ID_THREAD_IDLE;
		break;
	default:
		uMenuID=ID_THREAD_IDLE;
		break;
	}
	pTrackMenu->CheckMenuItem(uMenuID,MF_BYCOMMAND | MF_CHECKED);


	// ���õ����˵����߳�״̬
	if( m_vecThreadInfo.at(m_nCurItem).m_nStatus==5 ) // Suspend
	{
		pTrackMenu->EnableMenuItem(ID_THREAD_SUSPEND,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	else
	{
		pTrackMenu->EnableMenuItem(ID_THREAD_RESUME,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
}