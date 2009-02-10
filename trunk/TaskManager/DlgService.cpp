// DlgService.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgService.h"
#include "ServiceHelper.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgService �Ի���

BEGIN_EASYSIZE_MAP(CDlgService)
	EASYSIZE(IDC_LIST_SERVER,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BUTTON_SERVER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_COMBO_SERVICE_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_SERVICE_TYPE,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_COMBO_SERVICE_STATUS,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_SERVICE_STATUS,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,0)
END_EASYSIZE_MAP



IMPLEMENT_DYNAMIC(CDlgService, CDialog)


DWORD WINAPI CDlgService::ScanServiceThread(LPVOID pVoid)
{
	ASSERT(pVoid);
	CDlgService *pThis=(CDlgService *)pVoid;

	while(!pThis->m_bExit)
	{
		// �Ƿ���ͣ
		if( !pThis->m_bPause )
		{
			// ��ȡ������ϸ��Ϣ
			pThis->m_enumService.EnumServices(pThis->m_nServiceType, pThis->m_nServiceStatus);
			pThis->m_vecServiceInfo=pThis->m_enumService.GetServiceInfo();

			// ����
			pThis->PostMessage(WM_UPDATE_SERVICE_IFNO);
			
		}
		else
		{
			//::OutputDebugString(_T("ScanServiceThread Paused\n"));
			if( !pThis->m_vecServiceInfo.empty() )
			{
				pThis->m_enumService.Clear();
			}
		}

		pThis->m_bPause=TRUE;
		Sleep(1000);
	}

#ifdef _DEBUG
	::OutputDebugString(_T("Leveal ScanServiceThread\n"));
#endif
	return 0;
}


CDlgService::CDlgService(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgService::IDD, pParent)
	, m_nCurSelItem(0)
	, m_pScanServiceThread(NULL)
	, m_bExit(FALSE)
	, m_bPause(TRUE)
{

}

CDlgService::~CDlgService()
{
}

void CDlgService::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER, m_wndListServer);
	DDX_Control(pDX, IDC_COMBO_SERVICE_TYPE, m_wndComboServiceType);
	DDX_Control(pDX, IDC_COMBO_SERVICE_STATUS, m_wndComboServiceStatus);
}


BEGIN_MESSAGE_MAP(CDlgService, CDialog)
	ON_WM_SIZE()
	ON_WM_INITMENUPOPUP()

	ON_COMMAND(ID_SERVICE_OPEN, &CDlgService::OnServiceOpen)
	ON_COMMAND(ID_SERVICE_SATRT, &CDlgService::OnServiceSatrt)
	ON_COMMAND(ID_SERVICE_STOP, &CDlgService::OnServiceStop)
	ON_COMMAND(ID_SERVICE_PAUSE, &CDlgService::OnServicePause)
	ON_COMMAND(ID_SERVICE_RESUME, &CDlgService::OnServiceResume)
	ON_COMMAND(ID_SERVICE_DELETE, &CDlgService::OnServiceDelete)
	ON_COMMAND(ID_SERVICE_AUTO_START, &CDlgService::OnServiceAutoStart)
	ON_COMMAND(ID_SERVICE_MANNUAL_START, &CDlgService::OnServiceManualStart)
	ON_COMMAND(ID_SERVICE_DISABLED, &CDlgService::OnServiceDisabled)

	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER, &CDlgService::OnNMClickListServer)
	ON_BN_CLICKED(IDC_BUTTON_SERVER, &CDlgService::OnBnClickedButtonServer)

	ON_MESSAGE(WM_UPDATE_SERVICE_IFNO,Update)
	ON_MESSAGE(WM_SET_PAUSE,OnSetPause)

	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_SERVICE_TYPE, &CDlgService::OnCbnSelchangeComboServiceType)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVICE_STATUS, &CDlgService::OnCbnSelchangeComboServiceStatus)
END_MESSAGE_MAP()


// CDlgService ��Ϣ�������

BOOL CDlgService::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	InitCombo();


	// �����̼߳��
	m_pScanServiceThread=AfxBeginThread((AFX_THREADPROC)ScanServiceThread,this,0,0,CREATE_SUSPENDED,NULL);
	m_pScanServiceThread->m_bAutoDelete=FALSE;
	m_pScanServiceThread->ResumeThread();

	InitList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgService::OnDestroy()
{
	CDialog::OnDestroy();

	// �ͷ��߳���Դ
	if( m_pScanServiceThread!=NULL )
	{
		m_bPause=FALSE;
		m_bExit=TRUE;
		
		::WaitForSingleObject(m_pScanServiceThread->m_hThread,INFINITE);
		delete m_pScanServiceThread;
		m_pScanServiceThread=NULL;
	}
}


void CDlgService::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CDialog::OnSize(nType, cx, cy);
}

BOOL CDlgService::SetPause(BOOL bPause)
{
	m_bPause=bPause;

	return m_bPause;
}

void CDlgService::InitList()
{
	// ���÷��
	m_wndListServer.ModifyStyle(0,LVS_REPORT);
	m_wndListServer.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_DOUBLEBUFFER);

	// ������
	/*m_wndListServer.InsertColumn(0,_T("��ʾ����"),LVCFMT_LEFT,80);
	m_wndListServer.InsertColumn(1,_T("��������"),LVCFMT_LEFT,80);
	m_wndListServer.InsertColumn(2,_T("����"),LVCFMT_LEFT,150);
	m_wndListServer.InsertColumn(3,_T("��ǰ״̬"),LVCFMT_LEFT,80);
	m_wndListServer.InsertColumn(4,_T("��������"),LVCFMT_LEFT,80);
	m_wndListServer.InsertColumn(5,_T("����·��"),LVCFMT_LEFT,150);*/
	m_wndListServer.SetHeadings(_T("��ʾ����,100;\
		��������,80;\
		����,150;\
		��ǰ״̬,80;\
		��������,80;\
		����·��,250"));

	//m_wndListServer.LoadColumnInfo();

	// ����ͼ��
	m_iImageSmall.Create(16,16,ILC_MASK | ILC_COLOR32,0,1);
	// ���ͼ��
	m_wndListServer.SetImageList(&m_iImageSmall, LVSIL_SMALL);
}

void CDlgService::InitCombo()
{
	// ���ѡ��
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_KERNEL_DRIVER,_T("�ں���������")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_FILE_SYSTEM_DRIVER,_T("�ļ�ϵͳ��������")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_RECOGNIZER_DRIVER,_T("Recognizer��������")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_ADAPTER,_T("��������������")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_DRIVER,_T("--������������")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_WIN32_OWN_PROCESS,_T("���̷���")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_WIN32_SHARE_PROCESS,_T("�������")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_WIN32,_T("--����ϵͳ����")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_INTERACTIVE_PROCESS,_T("Integrate Process����")));
	m_mapServiceType.insert(pair<UINT,CString>(SERVICE_TYPE_ALL,_T("--ȫ������")));

	int nIndex=0;
	for(map<UINT,CString>::iterator iter=m_mapServiceType.begin(); 
		iter!=m_mapServiceType.end(); ++iter)
	{
		m_wndComboServiceType.AddString((*iter).second);
		m_wndComboServiceType.SetItemData(nIndex++,(*iter).first);
	}
	
	// ����Ĭ����
	m_wndComboServiceType.SetCurSel(7);	// ����ϵͳ����
	m_nServiceType=SERVICE_WIN32;



	// �����
	m_mapServiceStatus.insert(pair<UINT,CString>(SERVICE_ACTIVE,_T("�����")));
	m_mapServiceStatus.insert(pair<UINT,CString>(SERVICE_INACTIVE,_T("�ǻ����")));
	m_mapServiceStatus.insert(pair<UINT,CString>(SERVICE_STATE_ALL,_T("���ж�����")));

	nIndex=0;
	for(map<UINT,CString>::iterator iter=m_mapServiceStatus.begin(); 
		iter!=m_mapServiceStatus.end(); ++iter)
	{
		m_wndComboServiceStatus.AddString((*iter).second);
		m_wndComboServiceStatus.SetItemData(nIndex++,(*iter).first);
	}

	// ����Ĭ����
	m_wndComboServiceStatus.SetCurSel(2);	// ����ϵͳ����
	m_nServiceStatus=SERVICE_STATE_ALL;
}

void CDlgService::InsertItem()
{
	m_wndListServer.DeleteAllItems();
	
	// ������
	int nIndex=0,nItem=0;
	LVITEM lView={0};
	lView.mask	=LVIF_IMAGE;

	for(ServiceInfoList::iterator iter =m_vecServiceInfo.begin(); iter != m_vecServiceInfo.end(); ++iter) 
	{
		m_wndListServer.AddItem(iter->m_strDisplayName.c_str(),
			iter->m_strServiceName.c_str(),
			iter->m_strDescription.c_str(),
			iter->m_strCurrentState.c_str(),
			iter->m_strStartType.c_str(),
			iter->m_strBinaryPath);

		//m_wndListServer.InsertItem(nIndex,it->m_strDisplayName);
		/*m_wndListServer.SetItemText(nIndex,1,it->m_strServiceName);
		m_wndListServer.SetItemText(nIndex,2,it->m_strDescription.c_str());

		m_wndListServer.SetItemText(nIndex,3,it->GetCurrentState());
		m_wndListServer.SetItemText(nIndex,4,it->GetStartType());
		m_wndListServer.SetItemText(nIndex,5,it->m_strBinaryPath);*/
		
		// ���ͼ��
		nIndex=m_iImageSmall.Add(iter->m_hIcon);
		lView.iItem	=nItem++;
		if( nIndex!=-1 )
		{
			lView.iImage=nIndex;
		}
	}
}

LRESULT CDlgService::OnSetPause(WPARAM wParam,LPARAM lParam)
{
	SetPause((BOOL)lParam);

	return 0;
}

LRESULT CDlgService::Update(WPARAM wParam,LPARAM lParam)
{
	// ��ֹ��˸
	m_wndListServer.SetRedraw(FALSE);

	// �����
	InsertItem();

	m_wndListServer.SetRedraw(TRUE);
	m_wndListServer.Invalidate();
	m_wndListServer.UpdateWindow();

	return 0;
}

void CDlgService::SetItemInfo(int nItem,int nSubItem,LPCTSTR lpszText)
{
	m_wndListServer.SetItemText(nItem,nSubItem,lpszText);
}


void CDlgService::OnServiceOpen()
{
	// �򿪷�������·��
	CString strPath=m_vecServiceInfo.at(m_nCurSelItem).m_strBinaryPath;
	if( strPath.IsEmpty() )
		return;

	// �滻�����ŵ�·��
	strPath.Replace(_T("\""),_T(""));

	strPath=strPath.Left(strPath.ReverseFind(_T('\\')));

	// ��
	ShellExecute(GetSafeHwnd(),_T("explore"),strPath,0,0,SW_SHOWNORMAL);

}

void CDlgService::OnServiceSatrt()
{
	// ������ѡ�����
	CServiceHelper srvHelp;
	srvHelp.SetServiceName(m_vecServiceInfo.at(m_nCurSelItem).m_strServiceName.c_str());

	if( srvHelp.Start() )
	{
		SetItemInfo(m_nCurSelItem,3,_T("����"));
		m_vecServiceInfo.at(m_nCurSelItem).m_dwCurrentState=4;
		//InsertItem();
	}
}

void CDlgService::OnServiceStop()
{
	// ֹͣ��ѡ�����
	CServiceHelper srvHelp;
	srvHelp.SetServiceName(m_vecServiceInfo.at(m_nCurSelItem).m_strServiceName.c_str());

	if( srvHelp.Stop() )
	{
		SetItemInfo(m_nCurSelItem,3,_T("ֹͣ"));
		m_vecServiceInfo.at(m_nCurSelItem).m_dwCurrentState=1;
		//InsertItem();
	}
}

void CDlgService::OnServicePause()
{
	// ��ͣ��ѡ�����
	CServiceHelper srvHelp;
	srvHelp.SetServiceName(m_vecServiceInfo.at(m_nCurSelItem).m_strServiceName.c_str());

	if( srvHelp.Pause() )
	{
		SetItemInfo(m_nCurSelItem,3,_T("��ͣ"));
		m_vecServiceInfo.at(m_nCurSelItem).m_dwCurrentState=7;
		//InsertItem();
	}
}

void CDlgService::OnServiceResume()
{
	// �ָ���ѡ�����
	CServiceHelper srvHelp;
	srvHelp.SetServiceName(m_vecServiceInfo.at(m_nCurSelItem).m_strServiceName.c_str());

	if( srvHelp.Continue() )
	{
		SetItemInfo(m_nCurSelItem,3,_T("����"));
		m_vecServiceInfo.at(m_nCurSelItem).m_dwCurrentState=4;
		//InsertItem();
	}
}

void CDlgService::OnServiceDelete()
{
	// ɾ����ѡ�����
	CServiceHelper srvHelp;
	srvHelp.SetServiceName(m_vecServiceInfo.at(m_nCurSelItem).m_strServiceName.c_str());

	if( srvHelp.Delete() )
	{
		m_wndListServer.DeleteItem(m_nCurSelItem);
		
		int nIndex=0;
		for(ServiceInfoList::iterator iter = m_vecServiceInfo.begin(); iter != m_vecServiceInfo.end(); ++iter) 
		{
			if( nIndex == m_nCurSelItem )
			{
				m_vecServiceInfo.erase(iter);
				break;
			}

			nIndex++;
		}
	}
}

void CDlgService::OnServiceAutoStart()
{
	CServiceHelper srvHelp;
	srvHelp.SetServiceName(m_vecServiceInfo.at(m_nCurSelItem).m_strServiceName.c_str());

	if( srvHelp.ChangeStartType(SERVICE_AUTO_START) )
	{
		SetItemInfo(m_nCurSelItem,4,_T("�Զ�����"));
		m_vecServiceInfo.at(m_nCurSelItem).m_dwStartType=2;
		//InsertItem();
	}
}

void CDlgService::OnServiceManualStart()
{
	CServiceHelper srvHelp;
	srvHelp.SetServiceName(m_vecServiceInfo.at(m_nCurSelItem).m_strServiceName.c_str());

	if( srvHelp.ChangeStartType(SERVICE_DEMAND_START) )
	{
		SetItemInfo(m_nCurSelItem,4,_T("�ֶ�����"));
		m_vecServiceInfo.at(m_nCurSelItem).m_dwStartType=3;
		//InsertItem();
	}
}

void CDlgService::OnServiceDisabled()
{
	CServiceHelper srvHelp;
	srvHelp.SetServiceName(m_vecServiceInfo.at(m_nCurSelItem).m_strServiceName.c_str());

	if( srvHelp.ChangeStartType(SERVICE_DISABLED) )
	{
		SetItemInfo(m_nCurSelItem,4,_T("�ر�"));
		m_vecServiceInfo.at(m_nCurSelItem).m_dwStartType=4;
		//InsertItem();
	}
}

void CDlgService::OnBnClickedButtonServer()
{
	ShellExecute(GetSafeHwnd(),_T("open"),_T("C:\\WINDOWS\\system32\\services.msc"),0,0,SW_SHOWNORMAL);
}


void CDlgService::OnCbnSelchangeComboServiceType()
{
	// ��ȡ��ǰѡ������
	int nIndex=m_wndComboServiceType.GetCurSel();
	if( nIndex==CB_ERR )
		return;

	// �����б�
	m_nServiceType=(UINT)m_wndComboServiceType.GetItemData(nIndex);
	m_bPause=FALSE;
}

void CDlgService::OnCbnSelchangeComboServiceStatus()
{
	// ��ȡ��ǰѡ������
	int nIndex=m_wndComboServiceStatus.GetCurSel();
	if( nIndex==CB_ERR )
		return;

	// �����б�
	m_nServiceStatus=(UINT)m_wndComboServiceStatus.GetItemData(nIndex);
	m_bPause=FALSE;
}


void CDlgService::OnNMClickListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nItem=pListView->iItem;

	if( nItem!=-1 )					// û��ѡ����
	{
		m_nCurSelItem=nItem;
		// �����˵�
		PopupMenu(pListView,IDR_MENU_SERVICE);
	}


	*pResult = 0;
}


void CDlgService::PopupMenu(LPNMLISTVIEW pListView,UINT uID)			// ��ʾ�˵�
{
	// ���ز˵�
	CMenu TrackMenuTemp,*pTrackMenu;
	TrackMenuTemp.LoadMenu(uID);

	//��ʾ
	pTrackMenu=TrackMenuTemp.GetSubMenu(0);
	//BOOL bSuc=pTrackMenu->SetDefaultItem(0,TRUE);

	SetMenuState(pTrackMenu);
	//CPoint point(pListView->ptAction.x,pListView->ptAction.y);
	//ClientToScreen(&point);
	CPoint point;
	::GetCursorPos(&point);

	pTrackMenu->TrackPopupMenu(TPM_LEFTALIGN,point.x,point.y,this);
}


void CDlgService::SetMenuState(CMenu *pTrackMenu)
{
	DWORD dwStartType=0;
	DWORD dwCurState=0;

	// ����
	if( m_vecServiceInfo.at(m_nCurSelItem).m_dwCurrentState==4 )  // RUNNING
	{
		pTrackMenu->EnableMenuItem(ID_SERVICE_SATRT,MF_GRAYED | MF_BYCOMMAND);

		pTrackMenu->EnableMenuItem(ID_SERVICE_RESUME,MF_GRAYED | MF_BYCOMMAND);
	}

	// ֹͣ
	if( m_vecServiceInfo.at(m_nCurSelItem).m_dwCurrentState==1 )  // STOPPED
	{
		pTrackMenu->EnableMenuItem(ID_SERVICE_STOP,MF_GRAYED | MF_BYCOMMAND);
		pTrackMenu->EnableMenuItem(ID_SERVICE_PAUSE,MF_GRAYED | MF_BYCOMMAND);
		pTrackMenu->EnableMenuItem(ID_SERVICE_RESUME,MF_GRAYED | MF_BYCOMMAND);
	}

	// ��ͣ
	if( m_vecServiceInfo.at(m_nCurSelItem).m_dwCurrentState==7 )  // PAUSED
	{
		pTrackMenu->EnableMenuItem(ID_SERVICE_SATRT,MF_GRAYED | MF_BYCOMMAND);
		pTrackMenu->EnableMenuItem(ID_SERVICE_STOP,MF_GRAYED | MF_BYCOMMAND);
		pTrackMenu->EnableMenuItem(ID_SERVICE_PAUSE,MF_GRAYED | MF_BYCOMMAND);
	}

	// ��������
	CMenu *pSubMenu=pTrackMenu->GetSubMenu(8);
	switch(m_vecServiceInfo.at(m_nCurSelItem).m_dwStartType)		
	{	
	case 0:		// BOOT_START
	case 1:		// SYSTEM_START
	case 2:		// AUTO_START 
		{
			if( pSubMenu!=NULL )
			{
				pSubMenu->CheckMenuItem(ID_SERVICE_AUTO_START,MF_BYCOMMAND | MF_CHECKED);
			}
		}
		break;

	case 3:		// MANNUALSTART
		{
			if( pSubMenu!=NULL )
			{
				pSubMenu->CheckMenuItem(ID_SERVICE_MANNUAL_START,MF_BYCOMMAND | MF_CHECKED);
			}
		}
		break;

	case 4:		// DISABLE
		{
			if( pSubMenu!=NULL )
			{
				pSubMenu->CheckMenuItem(ID_SERVICE_DISABLED,MF_BYCOMMAND | MF_CHECKED);
			}
		}
		break;
	}

}


void CDlgService::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)   
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





