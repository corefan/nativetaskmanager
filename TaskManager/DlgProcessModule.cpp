// DlgProcessModule.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessModule.h"
#include "ToolHelp.h"
#include "DlgDump.h"


// CDlgProcessModule 对话框
BEGIN_EASYSIZE_MAP(CDlgProcessModule)
	EASYSIZE(IDC_LIST_PROCESS_TAB_MODULE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgProcessModule, CDialog)


const ModuleColInfo CDlgProcessModule::m_ModuleColInfo[]=
{
	_T("模块名称"),		100,
	_T("模块基地址"),	100,
	_T("模块大小"),		100,
	_T("模块文件"),		250,
	_T("所属子系统"),	100,
	_T("链接版本"),		100,
	_T("创建日期时间"),	150,
	_T("公司名称"),		200,
	_T("介绍"),			250,
	_T("版本"),			100,
	_T("内部名称"),		100,
	_T("版权"),			150,
	_T("原始名称"),		100,
	_T("创建名称"),		100,
	_T("创建版本"),		100,
	_T("注释"),			150,
	_T("法律交易"),		100,
	_T("内部创建号"),	100,
	_T("特别创建号"),	100
};

// 更新模块信息线程
DWORD WINAPI CDlgProcessModule::ModuleInfoThread(LPVOID pVoid)
{
	CDlgProcessModule *pThis=(CDlgProcessModule *)pVoid;

	// 加载信息
	enumModule::EnumModule modInfo(pThis->m_nProcessID);
	modInfo.Enum();

	pThis->m_vecModuleInfo=modInfo.GetModuleInfo();
	
	// 发送消息更新UI
	pThis->PostMessage(WM_UPDATE_MODULE_INFO,0,0);

#ifdef _DEBUG
	::OutputDebugString(_T("Exsit ModuleInfo Thread\n"));
#endif
	return 0;
}


CDlgProcessModule::CDlgProcessModule(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessModule::IDD, pParent)
	, m_nProcessID(0)
	, m_pModuleThread(NULL)
{

}

CDlgProcessModule::~CDlgProcessModule()
{
}

void CDlgProcessModule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS_TAB_MODULE, m_wndListProcessTabModule);
}


BEGIN_MESSAGE_MAP(CDlgProcessModule, CDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()

	ON_MESSAGE(WM_FLUSH_MODULE,OnFlushModuleInfo)
	ON_MESSAGE(WM_UPDATE_MODULE_INFO,OnLoadModuleInfo)

	ON_WM_INITMENUPOPUP()
	
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS_TAB_MODULE, &CDlgProcessModule::OnNMRclickListProcessTabModule)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESS_TAB_MODULE, &CDlgProcessModule::OnNMDblclkListProcessTabModule)
END_MESSAGE_MAP()



// CDlgProcessModule 消息处理程序
BOOL CDlgProcessModule::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;

	InitList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgProcessModule::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;
	CDialog::OnSize(nType, cx, cy);
}

void CDlgProcessModule::OnDestroy()
{
	// 等待并清除
	if( m_pModuleThread!=NULL )
	{
		::WaitForSingleObject(m_pModuleThread->m_hThread,INFINITE);
		delete m_pModuleThread;
		m_pModuleThread=NULL;
	}

	m_font.DeleteObject();

	CDialog::OnDestroy();
}

void CDlgProcessModule::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)   
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


void CDlgProcessModule::InitList()
{
	// 初始化Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_ModuleColInfo)/sizeof(m_ModuleColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_ModuleColInfo[i].strColName,m_ModuleColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListProcessTabModule.SetHeadings(strCol);

	// 设置风格
	m_wndListProcessTabModule.ModifyStyle(0,LVS_REPORT);
	m_wndListProcessTabModule.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 

	// 设置图标
	m_iImageSmall.Create(16,16,ILC_MASK | ILC_COLOR32,1,1);
	// 添加图标
	m_wndListProcessTabModule.SetImageList(&m_iImageSmall, LVSIL_SMALL);

	// 设置字体
	m_font.CreatePointFont(80,_T("微软雅黑"));
	m_wndListProcessTabModule.SetFont(&m_font);

	
	

	// 插入数据
	InsertItem();
}

void CDlgProcessModule::InsertItem()
{
	m_wndListProcessTabModule.DeleteAllItems();

	// 删除ImageList的Image
	int nCount=m_iImageSmall.GetImageCount();
	for (int i=0;i < nCount;i++)
	{
		m_iImageSmall.Remove(0);
	}

	// 插入数据
	int nIndex=0,nItem=0;
	LVITEM lView={0};
	lView.mask	=LVIF_IMAGE;

	for(TvecModuleInfo::iterator iter= m_vecModuleInfo.begin(); iter!= m_vecModuleInfo.end();iter++) 
	{
		m_wndListProcessTabModule.AddItem(iter->m_strModuleName.c_str(),
			iter->m_strModuleBaseAddr.c_str(),
			iter->m_strModuleSize.c_str(),
			iter->m_strModulePath.c_str(),
			iter->m_strSubSystem.c_str(),
			iter->m_strLinker.c_str(),
			iter->m_strDateTime.c_str(),
			iter->m_strCompanyName.c_str(),
			iter->m_strDescription.c_str(),
			iter->m_strVersion.c_str(),
			iter->m_strInternalName.c_str(),
			iter->m_strCopyright.c_str(),
			iter->m_strOrignalName.c_str(),
			iter->m_strProductName.c_str(),
			iter->m_strProductVersion.c_str(),
			iter->m_strComments.c_str(),
			iter->m_strLegalTrademarks.c_str(),
			iter->m_strPrivateBuild.c_str(),
			iter->m_strSpecialBuild.c_str()
			);

		// 添加图标
		nIndex=m_iImageSmall.Add(iter->m_hIcon);
		
		if( nIndex!=-1 )
		{
			lView.iItem	=nItem++;
			lView.iImage=nIndex;
			m_wndListProcessTabModule.SetItem(&lView);
		}
	}
}


// 开启线程读取Module信息
LRESULT CDlgProcessModule::OnFlushModuleInfo(WPARAM wParam,LPARAM lParam)
{
	// 等待并清除
	if( m_pModuleThread!=NULL )
	{
		::WaitForSingleObject(m_pModuleThread->m_hThread,INFINITE);
		delete m_pModuleThread;
		m_pModuleThread=NULL;
	}

	// 设置进程ID
	if( wParam==0 )
		return -1;
	
	m_nProcessID=(int)wParam;

	// 清空
	for(TvecModuleInfo::iterator iter=m_vecModuleInfo.begin(); iter!=m_vecModuleInfo.end(); ++iter)
	{
		if( iter->m_hIcon!=NULL )
			DestroyIcon(iter->m_hIcon);
	}
	m_vecModuleInfo.clear();

	// 开启线程进行加载文件信息
	m_pModuleThread=AfxBeginThread((AFX_THREADPROC)ModuleInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pModuleThread->m_bAutoDelete=FALSE;
	m_pModuleThread->ResumeThread();

	return 0;
}

// 更新UI
LRESULT CDlgProcessModule::OnLoadModuleInfo(WPARAM wParam,LPARAM lParam)
{
	// 防止闪烁
	m_wndListProcessTabModule.SetRedraw(FALSE);

	// 添加信息
	InsertItem();

	m_wndListProcessTabModule.SetRedraw(TRUE);
	m_wndListProcessTabModule.Invalidate();
	m_wndListProcessTabModule.UpdateWindow();

	return 0;
}


void CDlgProcessModule::OnLButtonDown(UINT nFlags, CPoint point)
{
	

	CDialog::OnLButtonDown(nFlags, point);
}


// 右键单击打开Dumpbin
void CDlgProcessModule::OnNMRclickListProcessTabModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	LPCTSTR szModulePath=NULL;
	if( pListView->iItem!=-1 )
	{
		// 选中一项获取其路径
		szModulePath=m_vecModuleInfo.at(pListView->iItem).m_strModulePath.c_str();
	}
	else
		return;

	// 打开Dumpbin窗口
	CDlgDump dlg(szModulePath,m_vecModuleInfo.at(pListView->iItem).m_strModuleName.c_str());
	dlg.DoModal();



	*pResult = 0;
}


// 左键双击打开属性框
void CDlgProcessModule::OnNMDblclkListProcessTabModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	LPCTSTR szModulePath=NULL;
	if( pListView->iItem!=-1 )
	{
		// 选中一项获取其路径
		szModulePath=m_vecModuleInfo.at(pListView->iItem).m_strModulePath.c_str();
	}

	// 打开属性框
	SHELLEXECUTEINFO   sei;   

	sei.hwnd			=GetSafeHwnd();   
	sei.lpVerb			=_T("properties");		
	sei.lpFile			=szModulePath;   
	sei.lpDirectory		=NULL;   
	sei.lpParameters	=NULL;   
	sei.nShow			=SW_SHOWNORMAL;   
	sei.fMask			=SEE_MASK_INVOKEIDLIST;   
	sei.lpIDList		=NULL;   
	sei.cbSize			=sizeof(SHELLEXECUTEINFO);   
	::ShellExecuteEx(&sei);

	*pResult = 0;
}
