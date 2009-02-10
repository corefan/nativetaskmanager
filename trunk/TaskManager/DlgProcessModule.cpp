// DlgProcessModule.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessModule.h"
#include "ToolHelp.h"
#include "DlgDump.h"


// CDlgProcessModule �Ի���
BEGIN_EASYSIZE_MAP(CDlgProcessModule)
	EASYSIZE(IDC_LIST_PROCESS_TAB_MODULE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgProcessModule, CDialog)


const ModuleColInfo CDlgProcessModule::m_ModuleColInfo[]=
{
	_T("ģ������"),		100,
	_T("ģ�����ַ"),	100,
	_T("ģ���С"),		100,
	_T("ģ���ļ�"),		250,
	_T("������ϵͳ"),	100,
	_T("���Ӱ汾"),		100,
	_T("��������ʱ��"),	150,
	_T("��˾����"),		200,
	_T("����"),			250,
	_T("�汾"),			100,
	_T("�ڲ�����"),		100,
	_T("��Ȩ"),			150,
	_T("ԭʼ����"),		100,
	_T("��������"),		100,
	_T("�����汾"),		100,
	_T("ע��"),			150,
	_T("���ɽ���"),		100,
	_T("�ڲ�������"),	100,
	_T("�ر𴴽���"),	100
};

// ����ģ����Ϣ�߳�
DWORD WINAPI CDlgProcessModule::ModuleInfoThread(LPVOID pVoid)
{
	CDlgProcessModule *pThis=(CDlgProcessModule *)pVoid;

	// ������Ϣ
	enumModule::EnumModule modInfo(pThis->m_nProcessID);
	modInfo.Enum();

	pThis->m_vecModuleInfo=modInfo.GetModuleInfo();
	
	// ������Ϣ����UI
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



// CDlgProcessModule ��Ϣ�������
BOOL CDlgProcessModule::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;

	InitList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgProcessModule::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;
	CDialog::OnSize(nType, cx, cy);
}

void CDlgProcessModule::OnDestroy()
{
	// �ȴ������
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
	// ��ʼ��Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_ModuleColInfo)/sizeof(m_ModuleColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_ModuleColInfo[i].strColName,m_ModuleColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListProcessTabModule.SetHeadings(strCol);

	// ���÷��
	m_wndListProcessTabModule.ModifyStyle(0,LVS_REPORT);
	m_wndListProcessTabModule.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 

	// ����ͼ��
	m_iImageSmall.Create(16,16,ILC_MASK | ILC_COLOR32,1,1);
	// ���ͼ��
	m_wndListProcessTabModule.SetImageList(&m_iImageSmall, LVSIL_SMALL);

	// ��������
	m_font.CreatePointFont(80,_T("΢���ź�"));
	m_wndListProcessTabModule.SetFont(&m_font);

	
	

	// ��������
	InsertItem();
}

void CDlgProcessModule::InsertItem()
{
	m_wndListProcessTabModule.DeleteAllItems();

	// ɾ��ImageList��Image
	int nCount=m_iImageSmall.GetImageCount();
	for (int i=0;i < nCount;i++)
	{
		m_iImageSmall.Remove(0);
	}

	// ��������
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

		// ���ͼ��
		nIndex=m_iImageSmall.Add(iter->m_hIcon);
		
		if( nIndex!=-1 )
		{
			lView.iItem	=nItem++;
			lView.iImage=nIndex;
			m_wndListProcessTabModule.SetItem(&lView);
		}
	}
}


// �����̶߳�ȡModule��Ϣ
LRESULT CDlgProcessModule::OnFlushModuleInfo(WPARAM wParam,LPARAM lParam)
{
	// �ȴ������
	if( m_pModuleThread!=NULL )
	{
		::WaitForSingleObject(m_pModuleThread->m_hThread,INFINITE);
		delete m_pModuleThread;
		m_pModuleThread=NULL;
	}

	// ���ý���ID
	if( wParam==0 )
		return -1;
	
	m_nProcessID=(int)wParam;

	// ���
	for(TvecModuleInfo::iterator iter=m_vecModuleInfo.begin(); iter!=m_vecModuleInfo.end(); ++iter)
	{
		if( iter->m_hIcon!=NULL )
			DestroyIcon(iter->m_hIcon);
	}
	m_vecModuleInfo.clear();

	// �����߳̽��м����ļ���Ϣ
	m_pModuleThread=AfxBeginThread((AFX_THREADPROC)ModuleInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pModuleThread->m_bAutoDelete=FALSE;
	m_pModuleThread->ResumeThread();

	return 0;
}

// ����UI
LRESULT CDlgProcessModule::OnLoadModuleInfo(WPARAM wParam,LPARAM lParam)
{
	// ��ֹ��˸
	m_wndListProcessTabModule.SetRedraw(FALSE);

	// �����Ϣ
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


// �Ҽ�������Dumpbin
void CDlgProcessModule::OnNMRclickListProcessTabModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	LPCTSTR szModulePath=NULL;
	if( pListView->iItem!=-1 )
	{
		// ѡ��һ���ȡ��·��
		szModulePath=m_vecModuleInfo.at(pListView->iItem).m_strModulePath.c_str();
	}
	else
		return;

	// ��Dumpbin����
	CDlgDump dlg(szModulePath,m_vecModuleInfo.at(pListView->iItem).m_strModuleName.c_str());
	dlg.DoModal();



	*pResult = 0;
}


// ���˫�������Կ�
void CDlgProcessModule::OnNMDblclkListProcessTabModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	LPCTSTR szModulePath=NULL;
	if( pListView->iItem!=-1 )
	{
		// ѡ��һ���ȡ��·��
		szModulePath=m_vecModuleInfo.at(pListView->iItem).m_strModulePath.c_str();
	}

	// �����Կ�
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
