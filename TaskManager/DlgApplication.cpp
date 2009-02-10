// DlgApplication.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgApplication.h"


#include <algorithm>


// CDlgApplication 对话框

BEGIN_EASYSIZE_MAP(CDlgApplication)
	EASYSIZE(IDC_LIST_APPLICATION,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_CHANGE_TASK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_TERMINATE_TASK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_NEW_TASK,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP




IMPLEMENT_DYNAMIC(CDlgApplication, CDialog)

CDlgApplication::CDlgApplication(CWnd* pParent /*=NULL*/)
: CDialog(CDlgApplication::IDD,pParent)
	, m_nMenuIndex(1)
	, m_nCurSelItem(-1)
	, m_pScanTaskThread(NULL)
	, m_bExit(FALSE)
	, m_bPause(FALSE)
	, m_pSelectedItem(NULL)
	, m_nSelectedItemCount(0)
{

}

CDlgApplication::~CDlgApplication()
{
}

void CDlgApplication::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_APPLICATION, m_wndListApp);
}


BEGIN_MESSAGE_MAP(CDlgApplication, CDialog)
	ON_WM_SIZE()

	ON_COMMAND(ID_APPLICATION_NEW_TASK, &CDlgApplication::OnApplicationNewTask)
	ON_COMMAND(ID_APPLICATION_LARGE, &CDlgApplication::OnApplicationLarge)
	ON_COMMAND(ID_APPLICATION_SMALL, &CDlgApplication::OnApplicationSmall)
	ON_COMMAND(ID_APPLICATION_DETAIL, &CDlgApplication::OnApplicationDetail)
	ON_COMMAND(ID_APPLICATION_CHANGE, &CDlgApplication::OnApplicationChange)
	ON_COMMAND(ID_APPLICATION_TOP, &CDlgApplication::OnApplicationTop)
	ON_COMMAND(ID_APPLICATION_MIN, &CDlgApplication::OnApplicationMin)
	ON_COMMAND(ID_APPLICATION_MAX, &CDlgApplication::OnApplicationMax)
	ON_COMMAND(ID_APPLICATION_CASCADE, &CDlgApplication::OnApplicationCascade)
	ON_COMMAND(ID_APPLICATION_LANDSCAPE, &CDlgApplication::OnApplicationLandscape)
	ON_COMMAND(ID_APPLICATION_PORTRAIT, &CDlgApplication::OnApplicationPortrait)
	ON_COMMAND(ID_APPLICATION_TERMINATE, &CDlgApplication::OnApplicationTerminate)
	ON_COMMAND(ID_APPLICATION_TURN_PROCESS, &CDlgApplication::OnApplication)

	ON_UPDATE_COMMAND_UI(ID_APPLICATION_LARGE, &CDlgApplication::OnUpdateApplicationLarge)
	ON_UPDATE_COMMAND_UI(ID_APPLICATION_SMALL, &CDlgApplication::OnUpdateApplicationSmall)
	ON_UPDATE_COMMAND_UI(ID_APPLICATION_DETAIL, &CDlgApplication::OnUpdateApplicationDetail)

	ON_NOTIFY(NM_RCLICK, IDC_LIST_APPLICATION, &CDlgApplication::OnNMRclickListApplication)
	ON_WM_INITMENUPOPUP()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_TASK, &CDlgApplication::OnBnClickedButtonChangeTask)
	ON_BN_CLICKED(IDC_BUTTON_TERMINATE_TASK, &CDlgApplication::OnBnClickedButtonTerminateTask)
	ON_BN_CLICKED(IDC_BUTTON_NEW_TASK, &CDlgApplication::OnBnClickedButtonNewTask)
	ON_NOTIFY(NM_CLICK, IDC_LIST_APPLICATION, &CDlgApplication::OnNMClickListApplication)

	ON_MESSAGE(WM_DELETETASK,DeleteTask)
	ON_MESSAGE(WM_ADDTASK,AddTask)
	ON_MESSAGE(WM_UPDATE_APP_LIST,UpdateList)
	ON_MESSAGE(WM_SET_PAUSE,OnSetPause)
END_MESSAGE_MAP()


// CDlgApplication 消息处理程序
DWORD WINAPI CDlgApplication::ScanTaskThread(LPVOID pVoid)
{
	ASSERT(pVoid);
	CDlgApplication *pThis=(CDlgApplication *)pVoid;

	// 临时
	vector<TaskWndInfo> vecTemp;
	CEnumTaskWindow taskWindow;
	typedef vector<TaskWndInfo>::iterator iterator;
	
	while(!pThis->m_bExit)
	{
		// 是否暂停
		if( !pThis->m_bPause )
		{
			// 检测窗口
			taskWindow.EnumTaskWindow(pThis->GetSafeHwnd());
			vecTemp=taskWindow.GetTaskWndInfo();

			// 加锁
			pThis->m_cs.Lock();
			for(iterator iterNew=vecTemp.begin(),iterOld=pThis->m_vecTask.begin();
				iterNew!=vecTemp.end() && iterOld!=pThis->m_vecTask.end();
				++iterNew,++iterOld)
			{
				if( iterNew->m_hWnd!=iterOld->m_hWnd )
				{
					// 重新赋值
					pThis->m_vecTask=vecTemp;
					::PostMessage(pThis->GetSafeHwnd(),WM_UPDATE_APP_LIST,0,0);
					break;
				}
			}
			// 解锁
			pThis->m_cs.Unlock();
		}
		else
		{
			// 清空内存
			if( !pThis->m_vecTask.empty() )
			{
				vector<TaskWndInfo> veTemp;
				veTemp.swap( pThis->m_vecTask );

				pThis->m_vecTask.clear();

				taskWindow.Clear();
			}

			//::OutputDebugString(_T("ScanTaskThread Paused\n"));
		}

		Sleep(1000);
	}

#ifdef _DEBUG
	::OutputDebugString(_T("Leveal ScanTaskThread\n"));
#endif
	return 0;
}




BOOL CDlgApplication::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 初始化ListCtrl并添加元素
	InitListApp();

	// 启动线程检测
	m_pScanTaskThread=AfxBeginThread((AFX_THREADPROC)ScanTaskThread,this,0,0,CREATE_SUSPENDED,NULL);
	m_pScanTaskThread->m_bAutoDelete=FALSE;
	m_pScanTaskThread->ResumeThread();

	INIT_EASYSIZE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgApplication::SetPause(BOOL bPause)
{
	m_bPause=bPause;	

	return m_bPause;
}

void CDlgApplication::InitListApp()
{
	// 设置风格
	m_wndListApp.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER );

	// 插入信息项
	m_wndListApp.InsertColumn(0,_T("任务"),LVCFMT_LEFT,250);
	m_wndListApp.InsertColumn(1,_T("状态"),LVCFMT_LEFT,100);

	
	m_iImageLarge.Create(32,32,ILC_MASK | ILC_COLOR32,0,1);
	m_iImageSmall.Create(16,16,ILC_MASK | ILC_COLOR32,0,1);

	// 加锁
	m_cs.Lock();
	// 添加Task Window项
	CEnumTaskWindow taskWindow;
	taskWindow.EnumTaskWindow(m_hWnd);
	m_vecTask=taskWindow.GetTaskWndInfo();
	// 解锁
	m_cs.Unlock();

	// 添加项
	InsertAppItem();

	// 添加图标
	m_wndListApp.SetImageList(&m_iImageLarge, LVSIL_NORMAL);
	m_wndListApp.SetImageList(&m_iImageSmall, LVSIL_SMALL);

	//SetTimer(IDT_APP_TIMER,1000,NULL);
}

void CDlgApplication::InsertAppItem()
{
	// 清空
	m_wndListApp.DeleteAllItems();
	// 删除ImageList的Image
	int nCount=m_iImageLarge.GetImageCount();
	for (int i=0;i < nCount;i++)
	{
		m_iImageLarge.Remove(0);
		m_iImageSmall.Remove(0);
	}

	int nIndex=0;
	BOOL bSuc=FALSE;

	// 加锁
	m_cs.Lock();
	for(vector<TaskWndInfo>::iterator iter=m_vecTask.begin(); iter!=m_vecTask.end(); ++iter)
	{
		// 添加图标
		int nImage=m_iImageLarge.Add(iter->m_hIcon);
		m_iImageSmall.Add(iter->m_hIcon);

		// 插入项
		m_wndListApp.InsertItem(nIndex,iter->m_strText.c_str(),nImage);

		// 关联句柄
		m_wndListApp.SetItemData(nIndex,(DWORD_PTR)iter->m_hWnd);
		
		// 测试是否为正常响应窗口
		bSuc=TestStates(iter->m_hWnd);
		if( bSuc )
		{
			m_wndListApp.SetItemText(nIndex,1,_T("Running"));
		}
		else
		{
			m_wndListApp.SetItemText(nIndex,1,_T("Timeout"));
		}

		nIndex++;
	}
	// 解锁
	m_cs.Unlock();
}

void CDlgApplication::DeleteSelItem()
{
	// 删除控件上所选项
	m_wndListApp.DeleteItem(m_nCurSelItem);
	m_iImageLarge.Remove(m_nCurSelItem);
	m_iImageSmall.Remove(m_nCurSelItem);


	// 加锁
	m_cs.Lock();
	// 删除容器里所选项
	int nIndex=0;
	for(vector<TaskWndInfo>::iterator iter=m_vecTask.begin(); iter!=m_vecTask.end(); ++iter)
	{
		if( nIndex==m_nCurSelItem ) 
		{
			m_vecTask.erase(iter);
			return;
		}
	}
	// 解锁
	m_cs.Unlock();

	//m_wndListApp.RedrawWindow();
}

BOOL CDlgApplication::TestStates(HWND hWnd)
{
	/*DWORD dwRes;
	if( ::SendMessageTimeout(hWnd,WM_PAINT,0,0,SMTO_NORMAL,1000,&dwRes) )
	{
		return TRUE;
	}

	return FALSE;*/

	return !::IsHungAppWindow(hWnd);

}

LRESULT CDlgApplication::OnSetPause(WPARAM wParam,LPARAM lParam)
{
	SetPause((BOOL)lParam);

	return 0;
}


LRESULT CDlgApplication::UpdateList(WPARAM wParam,LPARAM lParam)
{
	// 防止闪烁措施
	m_wndListApp.SetRedraw(FALSE);

	// 更新数据
	InsertAppItem();

	// 恢复并更新
	m_wndListApp.SetRedraw(TRUE);
    m_wndListApp.Invalidate();
    m_wndListApp.UpdateWindow();

	return 0;
}


void CDlgApplication::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CDialog::OnSize(nType, cx, cy);

	
}

void CDlgApplication::OnApplicationNewTask()
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

void CDlgApplication::OnApplicationLarge()
{
	// 改变风格
	m_wndListApp.ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON ,LVS_ICON);
	m_wndListApp.SetRedraw(TRUE);

	// 更新Menu
	m_nMenuIndex=1;
}

void CDlgApplication::OnApplicationSmall()
{
	// 改变风格
	m_wndListApp.ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON ,LVS_SMALLICON);
	m_wndListApp.SetRedraw(TRUE);

	// 更新Menu
	m_nMenuIndex=2;
}

void CDlgApplication::OnApplicationDetail()
{
	// 改变风格
	m_wndListApp.ModifyStyle(LVS_ICON | LVS_LIST | LVS_REPORT | LVS_SMALLICON ,LVS_REPORT);
	m_wndListApp.SetRedraw(TRUE);

	// 更新Menu
	m_nMenuIndex=3;
}

void CDlgApplication::OnApplicationChange()
{
	// 切换到选择窗口
	HWND hWnd=(HWND)m_wndListApp.GetItemData(m_nCurSelItem);

	// 如果是最小化,则恢复
	if( ::IsIconic(hWnd) )
		::ShowWindow(hWnd,SW_RESTORE);
	::SetWindowPos(hWnd,HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	
}

void CDlgApplication::OnApplicationTop()
{
	// 前置所选择窗口
	HWND hWnd=(HWND)m_wndListApp.GetItemData(m_nCurSelItem);

	// 如果是最小化,则恢复
	if( ::IsIconic(hWnd) )
		::ShowWindow(hWnd,SW_RESTORE);
	::SetForegroundWindow(hWnd);
}

void CDlgApplication::OnApplicationMin()
{
	// 最小化所选择窗口
	HWND hWnd=(HWND)m_wndListApp.GetItemData(m_nCurSelItem);

	// 在另外线程中最小化所选择窗口
	ShowWindowAsync(hWnd,SW_MINIMIZE);
}

void CDlgApplication::OnApplicationMax()
{
	// 最大化所选择窗口
	HWND hWnd=(HWND)m_wndListApp.GetItemData(m_nCurSelItem);

	// 在另外线程中最大化所选择窗口
	ShowWindowAsync(hWnd,SW_MAXIMIZE);
}

void CDlgApplication::OnApplicationCascade()
{
	// 层叠窗口
	//HWND hWnd=(HWND)m_wndListApp.GetItemData(m_nCurSelItem);

	// 忽略非活动窗口
	::CascadeWindows(::GetDesktopWindow(),MDITILE_SKIPDISABLED,NULL,
		m_nSelectedItemCount,m_pSelectedItem);
}

void CDlgApplication::OnApplicationLandscape()
{
	// 横向平铺窗口
	::TileWindows(::GetDesktopWindow(),MDITILE_HORIZONTAL,NULL,
		m_nSelectedItemCount,m_pSelectedItem);
}

void CDlgApplication::OnApplicationPortrait()
{
	// 纵向平铺窗口
	::TileWindows(::GetDesktopWindow(),MDITILE_VERTICAL,NULL,
		m_nSelectedItemCount,m_pSelectedItem);
}

void CDlgApplication::OnApplicationTerminate()
{
	// 关闭程序
	HWND hWnd=(HWND)m_wndListApp.GetItemData(m_nCurSelItem);

	if( !::IsHungAppWindow(hWnd) )
	{
		::PostMessage(hWnd,WM_CLOSE,0,0);

		// 删除所选项
		//DeleteSelItem();
	}
	else
	{
		// 从窗口句柄得到进程ＩＤ
		DWORD dwPID=0;
		::GetWindowThreadProcessId(hWnd,&dwPID);

		// 获取进程终止权限
		HANDLE hProcess=::OpenProcess(PROCESS_TERMINATE,FALSE,dwPID);
		if( hProcess!=NULL )
		{
			// 终止进程
			::TerminateProcess(hProcess,-1);
			::CloseHandle(hProcess);
		}
		
	}
}

void CDlgApplication::OnApplication()
{
	// 转到进程
}

void CDlgApplication::OnUpdateApplicationLarge(CCmdUI *pCmdUI)
{
	if( m_nMenuIndex==1 )
	{
		pCmdUI->SetRadio(TRUE);
	}
	else
	{
		pCmdUI->SetRadio(FALSE);
	}
}

void CDlgApplication::OnUpdateApplicationSmall(CCmdUI *pCmdUI)
{
	if( m_nMenuIndex==2 )
	{
		pCmdUI->SetRadio(TRUE);
	}
	else
	{
		pCmdUI->SetRadio(FALSE);
	}
}

void CDlgApplication::OnUpdateApplicationDetail(CCmdUI *pCmdUI)
{
	if( m_nMenuIndex==3 )
	{
		pCmdUI->SetRadio(TRUE);
	}
	else
	{
		pCmdUI->SetRadio(FALSE);
	}
}

void CDlgApplication::OnBnClickedButtonChangeTask()
{
	if( m_nCurSelItem!=-1 )
	{
		OnApplicationChange();
	}
}

void CDlgApplication::OnBnClickedButtonTerminateTask()
{
	if( m_nCurSelItem!=-1 )
	{
		OnApplicationTerminate();
	}
}

void CDlgApplication::OnBnClickedButtonNewTask()
{
	OnApplicationNewTask();
}

void CDlgApplication::OnNMRclickListApplication(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nItem=pListView->iItem;

	//if( nItem==-1 )					// 没有选中项
	//{
	//	PopupMenu(pListView,IDR_MENU_APPLICATION_TWO);
	//}
	//else							// 选中 判断是否多选
	//{
	//	m_nCurSelItem=nItem;
	//	PopupMenu(pListView,IDR_MENU_APPLICATION_ONE);
	//}

	POSITION pos=m_wndListApp.GetFirstSelectedItemPosition();
	if( pos == NULL )
	{
		PopupMenu(pListView,IDR_MENU_APPLICATION_TWO);
		//TRACE0("No items were selected!\n");
	}
	else
	{
		// 释放先前选择的窗口
		if( m_pSelectedItem!=NULL )
		{
			delete []m_pSelectedItem;
			m_pSelectedItem=NULL;
			m_nSelectedItemCount=0;
		}

		// 申请内存保存当权选择的窗口句柄
		m_pSelectedItem=new HWND[(sizeof(HWND)*m_vecTask.size())];
		for(int i=0; i<(int)m_vecTask.size(); ++i)
			*(m_pSelectedItem+i)=NULL;

		// 保存窗口句柄
		int nIndex=0;
		while(pos)
		{
			int nPos=m_wndListApp.GetNextSelectedItem(pos);

			*(m_pSelectedItem+nIndex)=m_vecTask.at(nPos).m_hWnd;
			nIndex++;
		}

		// 弹出菜单
		PopupMenu(pListView,IDR_MENU_APPLICATION_ONE);
	}

	*pResult = 0;
}

void CDlgApplication::OnNMClickListApplication(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nItem=pListView->iItem;
	if( nItem!=-1 )
		m_nCurSelItem=nItem;

	*pResult = 0;
}


void CDlgApplication::OnTimer(UINT_PTR nIDEvent)
{
	InsertAppItem();

	CDialog::OnTimer(nIDEvent);
}

void CDlgApplication::OnDestroy()
{
	// 释放选择的窗口
	if( m_pSelectedItem!=NULL )
	{
		delete []m_pSelectedItem;
		m_pSelectedItem=NULL;
	}

	// 释放线程资源
	if( m_pScanTaskThread!=NULL )
	{
		m_bPause=FALSE;
		m_bExit=TRUE;
		

		::WaitForSingleObject(m_pScanTaskThread->m_hThread,INFINITE);
		delete m_pScanTaskThread;
		m_pScanTaskThread=NULL;
	}

	KillTimer(IDT_APP_TIMER);

	CDialog::OnDestroy();
}


void CDlgApplication::PopupMenu(LPNMLISTVIEW pListView,UINT uID)
{
	// 加载菜单
	CMenu TrackMenuTemp,*pTrackMenu;
	TrackMenuTemp.LoadMenu(uID);

	//显示
	pTrackMenu=TrackMenuTemp.GetSubMenu(0);

	// 判断需要忽略的菜单项
	if( uID==IDR_MENU_APPLICATION_ONE )
	{
		m_nSelectedItemCount=0;//=sizeof(m_pSelectedItem)/sizeof(*m_pSelectedItem);
		while(*(m_pSelectedItem+m_nSelectedItemCount)!=NULL)
		{
			m_nSelectedItemCount++;
		}

		if( m_nSelectedItemCount<=1 )
		{
			pTrackMenu->EnableMenuItem(ID_APPLICATION_CASCADE,		MF_BYCOMMAND | MF_DISABLED	| MF_GRAYED);
			pTrackMenu->EnableMenuItem(ID_APPLICATION_LANDSCAPE,	MF_BYCOMMAND | MF_DISABLED	| MF_GRAYED);
			pTrackMenu->EnableMenuItem(ID_APPLICATION_PORTRAIT,		MF_BYCOMMAND | MF_DISABLED	| MF_GRAYED);
		}
		else
		{
			pTrackMenu->EnableMenuItem(ID_APPLICATION_MAX,			MF_BYCOMMAND | MF_DISABLED	| MF_GRAYED);
			pTrackMenu->EnableMenuItem(ID_APPLICATION_MIN,			MF_BYCOMMAND | MF_DISABLED	| MF_GRAYED);
		}
	}


	//BOOL bSuc=pTrackMenu->SetDefaultItem(0,TRUE);
	//CPoint point(pListView->ptAction.x,pListView->ptAction.y);
		
	// 获取当前鼠标位置
	CPoint point;
	GetCursorPos(&point);
	//ClientToScreen(&point);

	// 弹出菜单
	pTrackMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
}

void CDlgApplication::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)   
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



BOOL CDlgApplication::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;//CInitPopupMenuDlg::OnEraseBkgnd(pDC);
}

void CDlgApplication::PostNcDestroy()
{
	
	CDialog::PostNcDestroy();
}


LRESULT CDlgApplication::DeleteTask(WPARAM wParam,LPARAM lParam)
{
	TRACE("Delete\n");
	TaskWndInfo taskInfo=*(TaskWndInfo *)wParam;

	BOOL bExsit=FALSE;
	for(int i=0; i<m_wndListApp.GetItemCount(); ++i)
	{
		if( (HWND)m_wndListApp.GetItemData(i)==taskInfo.m_hWnd )
		{
			m_wndListApp.DeleteItem(i);
			m_iImageLarge.Remove(i);
			m_iImageSmall.Remove(i);

			bExsit=TRUE;
			break;
		}
	}

	return 0;
}

LRESULT CDlgApplication::AddTask(WPARAM wParam,LPARAM lParam)
{
	TRACE("Add\n");

	TaskWndInfo taskInfo=*(TaskWndInfo *)wParam;

	int nIndex=m_wndListApp.GetItemCount();
	// 添加图标
	int nIndexIcon=m_iImageLarge.Add(taskInfo.m_hIcon);
	m_iImageSmall.Add(taskInfo.m_hIcon);

	// 插入项
	m_wndListApp.InsertItem(nIndex,taskInfo.m_strText.c_str(),nIndexIcon);

	// 关联句柄
	m_wndListApp.SetItemData(nIndex,(DWORD_PTR)taskInfo.m_hWnd);

	// 测试是否为正常响应窗口
	BOOL bSuc=TestStates(taskInfo.m_hWnd);
	if( bSuc )
	{
		m_wndListApp.SetItemText(nIndex,1,_T("Running"));
	}
	else
	{
		m_wndListApp.SetItemText(nIndex,1,_T("Timeout"));
	}
	return 0;
}

