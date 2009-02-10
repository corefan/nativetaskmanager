// DlgModule.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgModule.h"
#include "EnumModule.h"

// CDlgModule 对话框
BEGIN_EASYSIZE_MAP(CDlgModule)
	EASYSIZE(IDC_LIST_MODULE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgModule, CDialog)


const ModuleListColInfo CDlgModule::m_ModuleListColInfo[]=
{
	_T("模块名称"),		100,
	_T("模块基地址"),	100,
	_T("模块大小"),		100,
	_T("模块文件"),		250,
	_T("所属子系统"),	100,
	_T("所属进程"),		50,
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

CDlgModule::CDlgModule(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgModule::IDD, pParent)
	, m_pModuleThread(NULL)
	, m_bExit(FALSE)
	, m_bPause(TRUE)
{

}

CDlgModule::~CDlgModule()
{
}

void CDlgModule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODULE, m_wndListModule);
}


BEGIN_MESSAGE_MAP(CDlgModule, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_MESSAGE(WM_UPDATE_MODULE_LIST,OnUpdateModuleList)
	ON_MESSAGE(WM_SET_PAUSE,OnSetPause)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MODULE, &CDlgModule::OnNMDblclkListModule)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_MODULE, &CDlgModule::OnNMRclickListModule)
END_MESSAGE_MAP()


// CDlgModule 消息处理程序

DWORD CDlgModule::ScanModuleThread(LPVOID pVoid)
{
	CDlgModule *pThis=(CDlgModule *)pVoid;

	while( !pThis->m_bExit )
	{
		if( !pThis->m_bPause )
		{
			// 加载信息
			//enumModule::EnumModule modInfo;
			//modInfo.Enum();

			//pThis->m_mapModuleInfo=modInfo.GetSystemModuleInfo();

			// 发送消息更新UI
			pThis->PostMessage(WM_UPDATE_MODULE_LIST,0,0);
		}

	
		Sleep(1000);
	}

#ifdef _DEBUG
	::OutputDebugString(_T("Leveal ScanModuleThread\n"));
#endif

	return 0;
}

BOOL CDlgModule::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	InitList();

	// 开启线程遍历进程信息
	m_pModuleThread=AfxBeginThread((AFX_THREADPROC)ScanModuleThread,this,0,0,CREATE_SUSPENDED,0);
	m_pModuleThread->m_bAutoDelete=FALSE;
	m_pModuleThread->ResumeThread();


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgModule::SetPause(BOOL bPause)
{
	m_bPause=bPause;	

	return m_bPause;
}

void CDlgModule::OnDestroy()
{
	// 清除数据
	if( m_pModuleThread!=NULL )
	{
		m_bPause=FALSE;
		m_bExit=TRUE;

		::WaitForSingleObject(m_pModuleThread->m_hThread,INFINITE);
		delete m_pModuleThread;
		m_pModuleThread=NULL;
	}

	CDialog::OnDestroy();
}

void CDlgModule::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CDialog::OnSize(nType, cx, cy);
}


void CDlgModule::InitList()
{
	// 初始化Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_ModuleListColInfo)/sizeof(m_ModuleListColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_ModuleListColInfo[i].strColName,m_ModuleListColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListModule.SetHeadings(strCol);

	// 设置风格
	m_wndListModule.ModifyStyle(0,LVS_REPORT);
	m_wndListModule.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 

	// 设置图标
	m_iImageSmall.Create(16,16,ILC_MASK | ILC_COLOR32,1,1);
	// 添加图标
	m_wndListModule.SetImageList(&m_iImageSmall, LVSIL_SMALL);

	// 插入数据
	InsertItem();
}

void CDlgModule::InsertItem()
{
	m_wndListModule.DeleteAllItems();

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

	for(map<string,ModuleInfo>::iterator iter= m_mapModuleInfo.begin(); iter!= m_mapModuleInfo.end();iter++) 
	{
		m_wndListModule.AddItem(iter->second.m_strModuleName.c_str(),
			iter->second.m_strModuleBaseAddr.c_str(),
			iter->second.m_strModuleSize.c_str(),
			iter->second.m_strModulePath.c_str(),
			iter->second.m_strSubSystem.c_str(),
			iter->second.m_strProcessID.c_str(),
			iter->second.m_strLinker.c_str(),
			iter->second.m_strDateTime.c_str(),
			iter->second.m_strCompanyName.c_str(),
			iter->second.m_strDescription.c_str(),
			iter->second.m_strVersion.c_str(),
			iter->second.m_strInternalName.c_str(),
			iter->second.m_strCopyright.c_str(),
			iter->second.m_strOrignalName.c_str(),
			iter->second.m_strProductName.c_str(),
			iter->second.m_strProductVersion.c_str(),
			iter->second.m_strComments.c_str(),
			iter->second.m_strLegalTrademarks.c_str(),
			iter->second.m_strPrivateBuild.c_str(),
			iter->second.m_strSpecialBuild.c_str()
			);

		// 添加图标
		nIndex=m_iImageSmall.Add(iter->second.m_hIcon);
		
		if( nIndex!=-1 )
		{
			lView.iItem	=nItem++;
			lView.iImage=nIndex;
			m_wndListModule.SetItem(&lView);
		}
	}
}

LRESULT CDlgModule::OnSetPause(WPARAM wParam,LPARAM lParam)
{
	SetPause((BOOL)lParam);

	return 0;
}


LRESULT CDlgModule::OnUpdateModuleList(WPARAM wParam,LPARAM lParam)
{
	// 防止闪烁
	m_wndListModule.SetRedraw(FALSE);
	
	// 添加信息
	InsertItem();

	m_wndListModule.SetRedraw(TRUE);
	m_wndListModule.Invalidate();
	m_wndListModule.UpdateWindow();

	m_bExit=TRUE;
	return 0;
}

// 左键双击打开属性框
void CDlgModule::OnNMDblclkListModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}

// 右键单击打开Dumpbin
void CDlgModule::OnNMRclickListModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}
