// DlgModule.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgModule.h"
#include "EnumModule.h"

// CDlgModule �Ի���
BEGIN_EASYSIZE_MAP(CDlgModule)
	EASYSIZE(IDC_LIST_MODULE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgModule, CDialog)


const ModuleListColInfo CDlgModule::m_ModuleListColInfo[]=
{
	_T("ģ������"),		100,
	_T("ģ�����ַ"),	100,
	_T("ģ���С"),		100,
	_T("ģ���ļ�"),		250,
	_T("������ϵͳ"),	100,
	_T("��������"),		50,
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


// CDlgModule ��Ϣ�������

DWORD CDlgModule::ScanModuleThread(LPVOID pVoid)
{
	CDlgModule *pThis=(CDlgModule *)pVoid;

	while( !pThis->m_bExit )
	{
		if( !pThis->m_bPause )
		{
			// ������Ϣ
			//enumModule::EnumModule modInfo;
			//modInfo.Enum();

			//pThis->m_mapModuleInfo=modInfo.GetSystemModuleInfo();

			// ������Ϣ����UI
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

	// �����̱߳���������Ϣ
	m_pModuleThread=AfxBeginThread((AFX_THREADPROC)ScanModuleThread,this,0,0,CREATE_SUSPENDED,0);
	m_pModuleThread->m_bAutoDelete=FALSE;
	m_pModuleThread->ResumeThread();


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

BOOL CDlgModule::SetPause(BOOL bPause)
{
	m_bPause=bPause;	

	return m_bPause;
}

void CDlgModule::OnDestroy()
{
	// �������
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
	// ��ʼ��Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_ModuleListColInfo)/sizeof(m_ModuleListColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_ModuleListColInfo[i].strColName,m_ModuleListColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListModule.SetHeadings(strCol);

	// ���÷��
	m_wndListModule.ModifyStyle(0,LVS_REPORT);
	m_wndListModule.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 

	// ����ͼ��
	m_iImageSmall.Create(16,16,ILC_MASK | ILC_COLOR32,1,1);
	// ���ͼ��
	m_wndListModule.SetImageList(&m_iImageSmall, LVSIL_SMALL);

	// ��������
	InsertItem();
}

void CDlgModule::InsertItem()
{
	m_wndListModule.DeleteAllItems();

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

		// ���ͼ��
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
	// ��ֹ��˸
	m_wndListModule.SetRedraw(FALSE);
	
	// �����Ϣ
	InsertItem();

	m_wndListModule.SetRedraw(TRUE);
	m_wndListModule.Invalidate();
	m_wndListModule.UpdateWindow();

	m_bExit=TRUE;
	return 0;
}

// ���˫�������Կ�
void CDlgModule::OnNMDblclkListModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}

// �Ҽ�������Dumpbin
void CDlgModule::OnNMRclickListModule(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}
