// DlgProcess.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcess.h"
#include "DlgCPUProcessor.h"
#include "DlgProcessInfo.h"

#include <Tlhelp32.h>
#include <functional>
#include <algorithm>


CString g_strColName[MAX_PROCESS_COLUM]=
{
	_T("进程名称"),		_T("进程 ID"),		_T("CPU 使用"),		_T("CPU 时间"),
	_T("内存使用"),		_T("内存使用增量"),	_T("内存使用峰值"),	_T("页面错误"),		
	_T("错误页面增量"), _T("页面池"),		_T("非页面缓冲池"),	_T("用户对象"),		
	_T("GDI 对象"),		_T("会话 ID"),		_T("用户名称"),		_T("基本优先级"),	
	_T("句柄数量"),		_T("线程数量"),		_T("I/O 读取"),		_T("I/O 读取字节数"),
	_T("I/O 写入"),		_T("I/O 写入字节"),	_T("I/O 其他"),		_T("I/O 其他字节"),	
	_T("内存使用峰值"),	_T("虚拟内存大小"),	_T("映像路径")    
};

UINT g_uColWidth[MAX_PROCESS_COLUM]=
{
	100,60,60,60,
	60,60,60,60,
	60,60,60,60,
	60,60,60,60,
	60,60,60,60,
	60,60,60,60,
	60,60,200
};

UINT g_uColMFT[MAX_PROCESS_COLUM]=
{
	LVCFMT_LEFT,	LVCFMT_LEFT,	LVCFMT_RIGHT,	LVCFMT_LEFT,
	LVCFMT_RIGHT,	LVCFMT_RIGHT,	LVCFMT_RIGHT,	LVCFMT_RIGHT,
	LVCFMT_RIGHT,	LVCFMT_RIGHT,	LVCFMT_RIGHT,	LVCFMT_LEFT,
	LVCFMT_LEFT,	LVCFMT_LEFT,	LVCFMT_LEFT,	LVCFMT_LEFT,
	LVCFMT_LEFT,	LVCFMT_LEFT,	LVCFMT_RIGHT,	LVCFMT_RIGHT,
	LVCFMT_RIGHT,	LVCFMT_RIGHT,	LVCFMT_RIGHT,	LVCFMT_RIGHT,
	LVCFMT_RIGHT,	LVCFMT_RIGHT,	LVCFMT_LEFT,
};

// CDlgProcess 对话框
BEGIN_EASYSIZE_MAP(CDlgProcess)
	EASYSIZE(IDC_LIST_PROCESS,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_TAB_PROCESS,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_SPLITTERY,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


IMPLEMENT_DYNAMIC(CDlgProcess, CDialog)

CDlgProcess::CDlgProcess(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcess::IDD, pParent)
	, m_bExit(FALSE)
	, m_bPause(TRUE)
	, m_pThreadProcess(NULL)
	, m_pUpdateEvent(NULL)
	, m_nCurItem(0)
	, m_nCurSelCol(0)
{

}

CDlgProcess::~CDlgProcess()
{
}

void CDlgProcess::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, m_wndListProcess);
	DDX_Control(pDX, IDC_TAB_PROCESS, m_wndTabProcess);
}


BEGIN_MESSAGE_MAP(CDlgProcess, CDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_INITMENUPOPUP()

	ON_MESSAGE(WM_UPDATE_PROCESS_LIST,OnUpdateProcessList)
	ON_MESSAGE(WM_SET_PAUSE,OnSetPause)
	ON_MESSAGE(WM_SPLITTER_MOVED,OnSetSize)

	ON_NOTIFY(NM_CLICK, IDC_LIST_PROCESS, &CDlgProcess::OnNMClickListProcess)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_PROCESS, &CDlgProcess::OnLvnGetdispinfoListProcess)
	ON_NOTIFY(LVN_ODFINDITEM, IDC_LIST_PROCESS, &CDlgProcess::OnLvnOdfinditemListProcess)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_PROCESS, &CDlgProcess::OnLvnColumnclickListProcess)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROCESS, &CDlgProcess::OnNMRclickListProcess)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESS, &CDlgProcess::OnNMDblclkListProcess)

	ON_COMMAND(ID_PROCESS_END, &CDlgProcess::OnProcessEnd)
	ON_COMMAND(ID_PROCESS_END_TREE, &CDlgProcess::OnProcessEndTree)
	ON_COMMAND(ID_PROCESS_DEBUG, &CDlgProcess::OnProcessDebug)
	ON_COMMAND(ID_PROCESS_CPU, &CDlgProcess::OnProcessCpu)
	ON_COMMAND(ID_PROCESS_PROPERTY, &CDlgProcess::OnProcessProperty)
	ON_COMMAND(ID_PROCESS_OPEN, &CDlgProcess::OnProcessOpen)

	ON_COMMAND_RANGE(ID_PROCESS_REALTIME,ID_PROCESS_LOW,OnMenu)
	ON_COMMAND(ID_PROCESS_SUSPEND, &CDlgProcess::OnProcessSuspend)
	
	ON_COMMAND(ID_PROCESS_VIEW, &CDlgProcess::OnProcessView)
END_MESSAGE_MAP()


DWORD CDlgProcess::ScanProcessThread(LPVOID pVoid)
{
	CDlgProcess *pThis=(CDlgProcess *)pVoid;

	int nCount=0;

	while( !pThis->m_bExit )
	{
		// 是否暂停
		if( !pThis->m_bPause )
		{
			// 加锁
			pThis->m_cs.Lock();

			// 遍历进程
			pThis->m_enumProcess.Enum();
			pThis->m_vecProcessInfo=pThis->m_enumProcess.GetProcessInfo();

			// 解锁
			pThis->m_cs.Unlock();

			// 发送消息更新UI
			if( nCount!=pThis->m_vecProcessInfo.size() )
				pThis->PostMessage(WM_UPDATE_PROCESS_LIST,TRUE);
			else
				pThis->PostMessage(WM_UPDATE_PROCESS_LIST,FALSE);

			// 判断是否更新图标
			nCount=(int)pThis->m_vecProcessInfo.size();
		}
		else
		{
			// 清空内存
			if( !pThis->m_vecProcessInfo.empty() )
			{
				pThis->m_enumProcess.Clear();
			}

			//::OutputDebugString(_T("ScanProcessThread Paused\n"));
		}

		Sleep(1000);
	}

#ifdef _DEBUG
	::OutputDebugString(_T("Leveal ScanProcessThread\n"));
#endif

	return 0;
}
// CDlgProcess 消息处理程序
BOOL CDlgProcess::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	m_wndSplitterY.BindWithControl(this,IDC_STATIC_SPLITTERY);
	m_wndSplitterY.SetMinHeight(100,100);

	m_wndSplitterY.AttachAsAbovePane(IDC_LIST_PROCESS);
	m_wndSplitterY.AttachAsBelowPane(IDC_TAB_PROCESS);

	m_wndSplitterY.RecalcLayout();

	InitListProcess();
	InitTab();

	m_pUpdateEvent=new CEvent(FALSE,TRUE);
	// 开启线程遍历进程信息
	m_pThreadProcess=AfxBeginThread((AFX_THREADPROC)ScanProcessThread,this,0,0,CREATE_SUSPENDED,0);
	m_pThreadProcess->m_bAutoDelete=FALSE;
	m_pThreadProcess->ResumeThread();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgProcess::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	// 更新位置
	if( m_wndTabProcess.GetSafeHwnd()!=NULL ) 
	{
		CRect rectTab;
		m_wndTabProcess.GetClientRect(&rectTab);
		rectTab.top		+= 20;
		rectTab.bottom	-= 3;
		rectTab.left	+= 2;
		rectTab.right	-= 3;

		for(vector<TabInfo>::iterator iter=m_vecTabInfo.begin(); iter!=m_vecTabInfo.end(); ++iter)
		{
			iter->pDlg->MoveWindow(rectTab);
		}
	}


	CDialog::OnSize(nType, cx, cy);
}

void CDlgProcess::OnDestroy()
{
	CDialog::OnDestroy();

	// 清除数据
	if( m_pThreadProcess!=NULL )
	{
		m_bPause=FALSE;
		m_bExit=TRUE;

		::WaitForSingleObject(m_pThreadProcess->m_hThread,INFINITE);
		delete m_pThreadProcess;
		m_pThreadProcess=NULL;
	}

	if( m_pUpdateEvent!=NULL )
	{
		delete m_pUpdateEvent;
		m_pUpdateEvent=NULL;
	}
}

BOOL CDlgProcess::SetPause(BOOL bPause)
{
	m_bPause=bPause;	

	return m_bPause;
}

LRESULT CDlgProcess::OnSetPause(WPARAM wParam,LPARAM lParam)
{
	SetPause((BOOL)lParam);

	return 0;
}

LRESULT CDlgProcess::OnSetSize(WPARAM wParam,LPARAM lParam)
{
	for(vector<TabInfo>::iterator iter=m_vecTabInfo.begin(); iter!=m_vecTabInfo.end(); ++iter)
	{
		iter->pDlg->SendMessage(WM_SIZE);
	}

	return 0;
}


void CDlgProcess::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)   
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


void CDlgProcess::InitListProcess()
{
	// 加载信息
	for(int i=0; i<MAX_PROCESS_COLUM; ++i)
	{
		/*BOOL bSelect=FALSE;
		if( i==COL_IMAGENAME	|| i==COL_PID			|| 
			i==COL_CPU			|| i==COL_CPUTIME		||
			i==COL_MEMUSAGE		|| i==COL_MEMDELTA		||
			i==COL_MEMPEAKSAGE	|| i==COL_PAGEFAULTS	||
			i==COL_PAGEPOOL		|| i==COL_NONPAGEPOOL	||
			i==COL_USEROBJECTS	|| i==COL_GDIOBJECTS	||
			i==COL_SESSIONID	|| i==COL_USERNAME		||
			i==COL_HANDLECOUNT	|| 
			i==COL_THREADCOUNT	|| 
			i==COL_PEEKMEMORYUSAGE || i==COL_VIRTUALMEMORYSIZE ||
			i==COL_IMAGEPATH 
			)
			bSelect=TRUE;*/

		m_proCol[i].bSelected	=TRUE;//bSelect;
		m_proCol[i].strColName	=g_strColName[i];
		m_proCol[i].uID			=COL_IMAGENAME+i;
		m_proCol[i].uWidth		=g_uColWidth[i];				// Column Width
		m_proCol[i].uMFT		=g_uColMFT[i];					// Column Format
	}

	// 设置风格
	m_wndListProcess.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER );

	// 设置图标
	m_iImageSmall.Create(16,16,ILC_MASK | ILC_COLOR32,0,1);
	// 添加图标
	m_wndListProcess.SetImageList(&m_iImageSmall, LVSIL_SMALL);
	

	// 设置默认列
	CString strCol,strTemp;

	for(int i=0; i<MAX_PROCESS_COLUM; ++i)
	{
		if( m_proCol[i].bSelected==TRUE )
		{
			/*strTemp.Format(_T("%s,%d;"),m_proCol[i].strColName,m_proCol[i].uWidth);
			strCol+=strTemp;*/
			static int nCol=0;
			m_wndListProcess.InsertColumn(nCol++,m_proCol[i].strColName,m_proCol[i].uMFT,m_proCol[i].uWidth);
		}
	}

	// 设置列
	//m_wndListProcess.SetHeadings(strCol);

	//m_wndListProcess.SetItemCount(100);

	// 遍历进程
	m_enumProcess.Enum();
	m_vecProcessInfo=m_enumProcess.GetProcessInfo();

	// 插入数据
	InsertItem();
}


void CDlgProcess::InsertItem()
{
	// 清空
	//m_wndListProcess.DeleteAllItems();
	//m_vecProcessInfo.clear();

	// 删除ImageList的Image
	int nCount=m_iImageSmall.GetImageCount();
	for (int i=0;i < nCount;i++)
	{
		m_iImageSmall.Remove(0);
	}

	// 等待更新UI事件
	/*::WaitForSingleObject(m_pUpdateEvent->m_hObject,INFINITE);
	m_pUpdateEvent->ResetEvent();*/

	// 插入数据
	int nIndex=0,nItem=0;
	LVITEM lView={0};
	lView.mask	=LVIF_IMAGE;

	m_cs.Lock();

	for(TvecProInfo::iterator iter= m_vecProcessInfo.begin(); iter!= m_vecProcessInfo.end();iter++) 
	{
		/*m_wndListProcess.AddItem(iter->m_strProcessName.c_str(),
			iter->m_strProcessID.c_str(),
			iter->m_strUserName.c_str(),
			iter->m_strThreadCount.c_str(),
			iter->m_strProcessPath.c_str());*/

		// 关联项
		//m_wndListProcess.SetItemData(nItem,);

		// 添加图标
		nIndex=m_iImageSmall.Add(iter->m_hIcon);
		lView.iItem	=nItem++;
		if( nIndex!=-1 )
		{
			lView.iImage=nIndex;
		}

		/*m_wndListProcess.SetItem(&lView);*/
	}

	m_wndListProcess.SetItemCount((int)m_vecProcessInfo.size());

	m_cs.Unlock();
}

void CDlgProcess::InitTab()
{
	TabInfo	tabInfo;

	// 创建每个属性页
	m_wndDlgProcessFile.Create(CDlgProcessTabFile::IDD,&m_wndTabProcess);
	tabInfo.pDlg		=&m_wndDlgProcessFile;
	tabInfo.strName		=_T("文件信息");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgProcessModule.Create(CDlgProcessModule::IDD,&m_wndTabProcess);
	tabInfo.pDlg		=&m_wndDlgProcessModule;
	tabInfo.strName		=_T("模块信息");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgProcessThread.Create(CDlgProcessThread::IDD,&m_wndTabProcess);
	tabInfo.pDlg		=&m_wndDlgProcessThread;
	tabInfo.strName		=_T("线程信息");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgProcessMemory.Create(CDlgProcessMemory::IDD,&m_wndTabProcess);
	tabInfo.pDlg		=&m_wndDlgProcessMemory;
	tabInfo.strName		=_T("内存信息");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgProcessPort.Create(CDlgProcessPort::IDD,&m_wndTabProcess);
	tabInfo.pDlg		=&m_wndDlgProcessPort;
	tabInfo.strName		=_T("端口信息");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgProcessEnv.Create(CDlgProcessEnvironment::IDD,&m_wndTabProcess);
	tabInfo.pDlg		=&m_wndDlgProcessEnv;
	tabInfo.strName		=_T("环境变量信息");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgProcessHandle.Create(CDlgProcessHandle::IDD,&m_wndTabProcess);
	tabInfo.pDlg		=&m_wndDlgProcessHandle;
	tabInfo.strName		=_T("句柄信息");
	m_vecTabInfo.push_back(tabInfo);

	// 创建选项
	for(vector<TabInfo>::iterator iter=m_vecTabInfo.begin();
		iter!=m_vecTabInfo.end();
		++iter)
	{
		m_wndTabProcess.AddTab(iter->pDlg,iter->strName.GetBuffer(0));
		iter->strName.ReleaseBuffer();
	}

	
	// 改变属性页的大小位置
	CRect rc;
	m_wndTabProcess.GetClientRect(rc);
	rc.top		+= 20;
	rc.bottom	-= 3;
	rc.left		+= 2;
	rc.right	-= 3;
	m_vecTabInfo.at(0).pDlg->MoveWindow(&rc);
}

// 更新图标
void CDlgProcess::UpdateImageList()
{
	// 删除ImageList的Image
	int nCount=m_iImageSmall.GetImageCount();
	for (int i=0;i < nCount;i++)
	{
		m_iImageSmall.Remove(0);
	}
	nCount=m_iImageSmall.GetImageCount();
	

	m_cs.Lock();

	int nIndex=0;
	for(TvecProInfo::iterator iter= m_vecProcessInfo.begin(); iter!= m_vecProcessInfo.end();iter++) 
	{
		// 添加图标
		nIndex=m_iImageSmall.Add(iter->m_hIcon);
	}

	m_cs.Unlock();
}

// 挂起进程
void CDlgProcess::SuspendProcess(DWORD dwProcessID, BOOL fSuspend)
{
	//Get the list of threads in the system.
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(
		TH32CS_SNAPTHREAD, dwProcessID);

   if(hSnapshot != INVALID_HANDLE_VALUE) 
   {
      //Walk the list of threads.
      THREADENTRY32 te = { sizeof(te) };
	  BOOL fOk = ::Thread32First(hSnapshot, &te);

	  for(; fOk; fOk = ::Thread32Next(hSnapshot, &te)) 
      {
         //Is this thread in the desired process?
         if(te.th32OwnerProcessID == dwProcessID) 
         {
            //Attempt to convert the thread ID into a handle.
			 HANDLE hThread = ::OpenThread(THREAD_SUSPEND_RESUME,
               FALSE, te.th32ThreadID);

            if(hThread != NULL) 
            {
               //Suspend or resume the thread.
               if( !fSuspend )
				   ::SuspendThread(hThread);
               else
				   ::ResumeThread(hThread);
            }
			::CloseHandle(hThread);
         }
      }
	  ::CloseHandle(hSnapshot);
   }
}



void CDlgProcess::OnNMClickListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 传递消息
	if( pListView->iItem!=-1 )
	{
		m_nCurItem=pListView->iItem;
	}


	*pResult = 0;
}

void CDlgProcess::OnNMDblclkListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	// 打开进程信息窗口
	if( pListView->iItem!=-1 )
	{
		m_cs.Lock();

		int nID=m_vecProcessInfo.at(pListView->iItem).m_dwID;
		LPCTSTR pszName=m_vecProcessInfo.at(pListView->iItem).m_strProcessName.c_str();

		m_cs.Unlock();

		// 以非模式对话框打开
		CDlgProcessInfo *pDlg=new CDlgProcessInfo(pszName,nID);
		pDlg->Create(CDlgProcessInfo::IDD,this);
		pDlg->ShowWindow(SW_SHOW);
	}

	*pResult = 0;
}


LRESULT CDlgProcess::OnUpdateProcessList(WPARAM wParam,LPARAM lParam)
{
	// 如果数量变化则更新图标
	if( wParam==FALSE )
		UpdateImageList();
	

	// 更新Virtual List并且不滚动和只更新当前显示部分区域
	m_wndListProcess.SetItemCountEx((int)m_vecProcessInfo.size(),LVSICF_NOSCROLL | LVSICF_NOINVALIDATEALL);
	m_wndListProcess.Invalidate();
	return 0;
}

void CDlgProcess::OnLvnGetdispinfoListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	
	//创建指针指向当前item
	LV_ITEM* pItem= &(pDispInfo)->item;

	//Which item number?
	int itemid = pItem->iItem;
	

	//Do the list need text information?
	if( pItem->mask & LVIF_TEXT )
	{
		CString text;

		//Which column?

		// 获取子项的栏目名
		static TCHAR szBuffer[32]	= {0};
		static LVCOLUMN col			= {0};
		col.mask			= LVCF_TEXT;
		col.pszText			= szBuffer;
		col.cchTextMax		= 80;

		if( itemid>=(int)m_vecProcessInfo.size() )
			return;


		// 加锁
		m_cs.Lock();

		if( m_wndListProcess.GetColumn(pItem->iSubItem,&col) )
		{
			for(int i=0; i<MAX_PROCESS_COLUM; ++i)
			{
				if( lstrcmp(m_proCol[i].strColName,szBuffer)==0 )
				{
					pItem->iSubItem=i;
					break;
				}
			}
		}

		if( m_proCol[pItem->iSubItem].bSelected )	// 如果需要显示
		{
			switch( pItem->iSubItem )
			{
			case COL_IMAGENAME:
				text =m_vecProcessInfo.at(itemid).m_strProcessName.c_str();
				break;
			case COL_PID:
				text =m_vecProcessInfo.at(itemid).m_strProcessID.c_str();
				break;
			case COL_CPU:
				text =m_vecProcessInfo.at(itemid).m_strCPUUsage.c_str();
				break;
			case COL_CPUTIME:
				text =m_vecProcessInfo.at(itemid).m_strCPUTime.c_str();
				break;
			case COL_MEMUSAGE:
				text =m_vecProcessInfo.at(itemid).m_strMemUsage.c_str();
				break;
			case COL_MEMDELTA:
				text =m_vecProcessInfo.at(itemid).m_strMemDelta.c_str();
				break;
			case COL_MEMPEAKSAGE:
				text =m_vecProcessInfo.at(itemid).m_strMemPeakUsage.c_str();
				break;
			case COL_PAGEFAULTS:
				text =m_vecProcessInfo.at(itemid).m_strPageFaults.c_str();
				break;
			case COL_PAGEFAULTSDELTA:
				break;
			case COL_PAGEPOOL:
				text =m_vecProcessInfo.at(itemid).m_strPagePoolUsage.c_str();
				break;
			case COL_NONPAGEPOOL:
				text =m_vecProcessInfo.at(itemid).m_strPageNonPollUsage.c_str();
				break;
			case COL_USEROBJECTS:
				text =m_vecProcessInfo.at(itemid).m_strUserObject.c_str();
				break;
			case COL_GDIOBJECTS:
				text =m_vecProcessInfo.at(itemid).m_strGDIObject.c_str();
				break;
			case COL_SESSIONID:
				text =m_vecProcessInfo.at(itemid).m_strSessionID.c_str();
				break;
			case COL_USERNAME:
				text =m_vecProcessInfo.at(itemid).m_strUserName.c_str();
				break;
			case COL_BASEPRIORITY:
				text =m_vecProcessInfo.at(itemid).m_strPriority.c_str();
				break;
			case COL_HANDLECOUNT:
				text =m_vecProcessInfo.at(itemid).m_strHandleCount.c_str();
				break;
			case COL_THREADCOUNT:
				text =m_vecProcessInfo.at(itemid).m_strThreadCount.c_str();
				break;
			case COL_IOREADS:
				text =m_vecProcessInfo.at(itemid).m_strIORead.c_str();
				break;
			case COL_IOREADBYTES:
				text =m_vecProcessInfo.at(itemid).m_strIOReadByte.c_str();
				break;
			case COL_IOWRITES:
				text =m_vecProcessInfo.at(itemid).m_strIOWrite.c_str();
				break;
			case COL_IOWRITEBYTES:
				text =m_vecProcessInfo.at(itemid).m_strIOWriteByte.c_str();
				break;
			case COL_IOOTHER:
				text =m_vecProcessInfo.at(itemid).m_strIOOther.c_str();
				break;
			case COL_IOOTHERBYTES:
				text =m_vecProcessInfo.at(itemid).m_strIOOtherByte.c_str();
				break;
			case COL_PEEKMEMORYUSAGE:
				text =m_vecProcessInfo.at(itemid).m_strMemPeakUsage.c_str();
				break;
			case COL_VIRTUALMEMORYSIZE:
				text =m_vecProcessInfo.at(itemid).m_strPageSize.c_str();
				break;
			case COL_IMAGEPATH:
				text =m_vecProcessInfo.at(itemid).m_strProcessPath.c_str();
				break;
			default:
				text=_T("你中奖了!什么都没有!");
				break;
			}
		}
		// 解锁
		m_cs.Unlock();

		//Copy the text to the LV_ITEM structure
		//Maximum number of characters is in pItem->cchTextMax
		lstrcpyn(pItem->pszText, text, pItem->cchTextMax);

		
	}

	//Do the list need image information?
	if( pItem->mask & LVIF_IMAGE) 
	{
		//Set which image to use
		pItem->iImage=itemid;//m_vecProcessInfo.at(itemid).m_hIcon;
	}


	*pResult = 0;
}

void CDlgProcess::OnLvnOdfinditemListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVFINDITEM pFindInfo = reinterpret_cast<LPNMLVFINDITEM>(pNMHDR);
	// pNMHDR has information about the item we should find
	// In pResult we should save which item that should be selected

	/* pFindInfo->iStart is from which item we should search.
	   We search to bottom, and then restart at top and will stop
	   at pFindInfo->iStart, unless we find an item that match
	 */
	*pResult=-1;
	// Set the default return value to -1
	// That means we didn't find any match.

	//Is search NOT based on string?
	if( (pFindInfo->lvfi.flags & LVFI_STRING) == 0 )
	{
		//This will probably never happend...
		return;
	}

	/*
		Let's look on a sample list;
		
		  Name
		  Anders
		* Anna
		  Annika
		  Bob
		  Emma
		  Emmanuel

		Anna is selected. 
		If "A" is written, Annika should be selected.
		If "AND" is written, Anders should be selected. 
		If "ANNK" is written, the selection should stay on Anna.
		If "E" is written, Emma should be selected.

    */

	//This is the string we search for
	CString searchstr = pFindInfo->lvfi.psz;

//	TRACE(_T("Find: %s\n"), searchstr);
	
	int startPos = pFindInfo->iStart;
	//Is startPos outside the list (happens if last item is selected)
	if(startPos >= m_wndListProcess.GetItemCount())
		startPos = 0;

	int currentPos=startPos;
	
	m_cs.Lock();
	//Let's search...
	do
	{		
		//Do this word begins with all characters in searchstr?
		if( _tcsnicmp(m_vecProcessInfo.at(currentPos).m_strProcessName.c_str(), searchstr, searchstr.GetLength()) == 0)
		{
			//Select this item and stop search.
			*pResult = currentPos;
			break;
		}

		//Go to next item
		currentPos++;

		//Need to restart at top?
		if(currentPos >= m_wndListProcess.GetItemCount())
			currentPos = 0;

	//Stop if back to start
	}while(currentPos != startPos);	

	m_cs.Unlock();
}

// 排序
bool CDlgProcess::Compare(const ProcessInfo &lv,const ProcessInfo &rv)
{
	return lv.m_strProcessName<rv.m_strProcessName;
}

void CDlgProcess::OnLvnColumnclickListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	LVCOLUMN Col;
	CString ColName;

	// Set text only.
	Col.mask=LVCF_TEXT;

	m_cs.Lock();

	// Reset the header columns (to indicate the sort order).
	for(int i = 0; i < MAX_PROCESS_COLUM; i++)
	{
		if( m_proCol[i].bSelected==FALSE )
			continue;

		if( i == pNMLV->iSubItem )
		{
			ColName=m_proCol[i].strColName+_T("  ^");
			
			// 排序
			m_nCurSelCol=i;
			sort(m_vecProcessInfo.begin(),m_vecProcessInfo.end(),Compare);
		}
		else
		{
			ColName=m_proCol[i].strColName;
		}	

		Col.pszText=ColName.GetBuffer(0);
		Col.cchTextMax=ColName.GetLength()+1;
		m_wndListProcess.SetColumn(i, &Col);
		ColName.ReleaseBuffer();
	}

	m_cs.Unlock();

	// Redraw the list.
	m_wndListProcess.Invalidate();

	*pResult = 0;
}

void CDlgProcess::OnNMRclickListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nItem=pListView->iItem;
	if( nItem==-1 )					// 没有选中项
	{
		//
	}
	else							// 选中
	{
		m_nCurItem=nItem;
		PopupMenu();
	}

	*pResult = 0;
}

void CDlgProcess::PopupMenu()
{
	// 加载菜单
	CMenu TrackMenuTemp,*pTrackMenu;
	TrackMenuTemp.LoadMenu(IDR_MENU_PROCESS);

	//显示
	pTrackMenu=TrackMenuTemp.GetSubMenu(0);
	
	// 设置菜单状态
	SetMenuState(pTrackMenu);
		
	// 获取当前鼠标位置
	CPoint point;
	GetCursorPos(&point);

	// 弹出菜单
	pTrackMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
}

void CDlgProcess::SetMenuState(CMenu *pTrackMenu)
{
	// 设置进程优先级状态
	int uMenuID=0;

	m_cs.Lock();

	switch(m_vecProcessInfo.at(m_nCurItem).m_dwPriority)
	{
	case REALTIME_PRIORITY_CLASS:
		uMenuID=ID_PROCESS_REALTIME;
		break;
	case HIGH_PRIORITY_CLASS:
		uMenuID=ID_PROCESS_HIGH;
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		uMenuID=ID_PROCESS_ABOVE_HIGH;
		break;
	case NORMAL_PRIORITY_CLASS:
		uMenuID=ID_PROCESS_NORMAL;
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		uMenuID=ID_PROCES_BELOW_LOW;
		break;
	case IDLE_PRIORITY_CLASS:
		uMenuID=ID_PROCESS_LOW;
		break;
	default:
		break;
	}

	m_cs.Unlock();


	pTrackMenu->CheckMenuItem(uMenuID,MF_BYCOMMAND | MF_CHECKED);
}


void CDlgProcess::OnProcessEnd()
{
	m_cs.Lock();

	// 获取当前进程号
	DWORD dwPID=m_vecProcessInfo.at(m_nCurItem).m_dwID;

	// 获取进程终止权限
	HANDLE hProcess=::OpenProcess(PROCESS_TERMINATE,FALSE,dwPID);
	if( hProcess!=NULL )
	{
		// 终止进程
		::TerminateProcess(hProcess,-1);
		::CloseHandle(hProcess);
	}

	m_cs.Unlock();
}

void CDlgProcess::OnProcessEndTree()
{
	m_cs.Lock();

	// 结束进程树即时结束该进程的所有子进程和父进程(explorer后)
	DWORD dwPID=m_vecProcessInfo.at(m_nCurItem).m_dwID;

	typedef multimap<DWORD,DWORD> ProTree;
	ProTree processTree=m_enumProcess.GetProcessTree();

	typedef ProTree::size_type sz_type;
	sz_type szCount=processTree.count(dwPID);

	ProTree::iterator iter=processTree.find(dwPID);

	for(sz_type i=0; i!=szCount; ++i,++iter)
	{
		// 获取进程终止权限
		HANDLE hProcess=::OpenProcess(PROCESS_TERMINATE,FALSE,iter->second);
		if( hProcess!=NULL )
		{
			// 终止进程
			::TerminateProcess(hProcess,-1);
			::CloseHandle(hProcess);
		}
	}

	m_cs.Unlock();

	// 获取选择的父进程终止权限
	HANDLE hProcess=::OpenProcess(PROCESS_TERMINATE,FALSE,dwPID);
	if( hProcess!=NULL )
	{
		// 终止进程
		::TerminateProcess(hProcess,-1);
		::CloseHandle(hProcess);
	}

}

void CDlgProcess::OnProcessSuspend()
{
	m_cs.Lock();

	// 获取进程ID
	DWORD dwPID=m_vecProcessInfo.at(m_nCurItem).m_dwID;
	BOOL bSuspend=m_vecProcessInfo.at(m_nCurItem).m_bSuspend;
	m_vecProcessInfo.at(m_nCurItem).m_bSuspend=!bSuspend;
	

	m_cs.Unlock();
	
	// 挂起或恢复进程
	SuspendProcess(dwPID,bSuspend);
}

void CDlgProcess::OnProcessDebug()
{
	// TODO: 在此添加命令处理程序代码
}

void CDlgProcess::OnMenu(UINT uID)
{
	DWORD dwPriority=0;
	CString strPriority;

	switch(uID)
	{
	case ID_PROCESS_REALTIME:
		dwPriority=REALTIME_PRIORITY_CLASS;
		strPriority=_T("实时");
		break;
	case ID_PROCESS_HIGH:
		dwPriority=HIGH_PRIORITY_CLASS;
		strPriority=_T("高");
		break;
	case ID_PROCESS_ABOVE_HIGH:
		dwPriority=ABOVE_NORMAL_PRIORITY_CLASS;
		strPriority=_T("较高");
		break;
	case ID_PROCESS_NORMAL:
		dwPriority=NORMAL_PRIORITY_CLASS;
		strPriority=_T("正常");
		break;
	case ID_PROCES_BELOW_LOW:
		dwPriority=BELOW_NORMAL_PRIORITY_CLASS;
		strPriority=_T("较低");
		break;
	case ID_PROCESS_LOW:
		dwPriority=IDLE_PRIORITY_CLASS;
		strPriority=_T("低");
		break;
	default:
		break;
	}

	m_cs.Lock();

	// 修改进程信息
	m_vecProcessInfo.at(m_nCurItem).m_dwPriority=dwPriority;
	m_vecProcessInfo.at(m_nCurItem).m_strPriority=strPriority;

	// 设置进程优先级
	HANDLE hProcess=::OpenProcess(PROCESS_SET_INFORMATION ,FALSE,m_vecProcessInfo.at(m_nCurItem).m_dwID);

	m_cs.Unlock();

	if( hProcess!=NULL )
	{
		::SetPriorityClass(hProcess,dwPriority);
		::CloseHandle(hProcess);
	}
}


void CDlgProcess::OnProcessProperty()
{
	// 打开文件或文件夹属性   
	m_cs.Lock();

	LPCTSTR pszModuleFilePath=m_vecProcessInfo.at(m_nCurItem).m_strProcessPath.c_str();

	m_cs.Unlock();

	SHELLEXECUTEINFO   sei;   

	sei.hwnd			=GetSafeHwnd();   
	sei.lpVerb			=_T("properties");		
	sei.lpFile			=pszModuleFilePath;   
	sei.lpDirectory		=NULL;   
	sei.lpParameters	=NULL;   
	sei.nShow			=SW_SHOWNORMAL;   
	sei.fMask			=SEE_MASK_INVOKEIDLIST;   
	sei.lpIDList		=NULL;   
	sei.cbSize			=sizeof(SHELLEXECUTEINFO);   
	::ShellExecuteEx(&sei);
}

void CDlgProcess::OnProcessOpen()
{
	// 打开进程所在路径
	CString strPath=m_vecProcessInfo.at(m_nCurItem).m_strProcessPath.c_str();
	if( strPath.IsEmpty() )
		return;

	strPath=strPath.Left(strPath.ReverseFind(_T('\\')));

	// 打开
	::ShellExecute(GetSafeHwnd(),_T("explore"),strPath,0,0,SW_SHOWNORMAL);
}

void CDlgProcess::OnProcessView()
{
	m_cs.Lock();

	int nID=m_vecProcessInfo.at(m_nCurItem).m_dwID;
	LPCTSTR pszPath=m_vecProcessInfo.at(m_nCurItem).m_strProcessPath.c_str();

	m_cs.Unlock();

	// 刷新FileInfo 
	m_wndDlgProcessFile.SendMessage(WM_FLUSH_FILE,
		(WPARAM)pszPath);

	// 刷新Modulenfo 

	m_wndDlgProcessModule.PostMessage(WM_FLUSH_MODULE,
		(WPARAM)nID);

	// 刷新Thread Info
	m_wndDlgProcessThread.PostMessage(WM_FLUSH_THREAD,
		(WPARAM)nID);

	// 刷新Memory Info
	m_wndDlgProcessMemory.PostMessage(WM_FLUSH_MEMORY,
		(WPARAM)nID);

	// 刷新Port Info
	m_wndDlgProcessPort.PostMessage(WM_FLUSH_PORT,
		(WPARAM)nID);

	// 刷新Environment Info
	m_wndDlgProcessEnv.PostMessage(WM_FLUSH_ENV,
		(WPARAM)nID);

	// 刷新Handle Info
	m_wndDlgProcessHandle.PostMessage(WM_FLUSH_HANDLE,
		(WPARAM)nID);
}


void CDlgProcess::OnProcessCpu()
{
	m_cs.Lock();

	// 设置进程绑定CPU
	HANDLE hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_INFORMATION ,FALSE,
		m_vecProcessInfo.at(m_nCurItem).m_dwID);

	m_cs.Unlock();

	if( hProcess!=NULL )
	{
		CDlgCPUProcessor dlgCPU(hProcess,this);
		dlgCPU.DoModal();

		::CloseHandle(hProcess);
	}
}








