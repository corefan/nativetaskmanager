// DlgProcessHandle.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessHandle.h"

BEGIN_EASYSIZE_MAP(CDlgProcessHandle)
	EASYSIZE(IDC_LIST_PROCESS_TAB_HANDLE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP
// CDlgProcessHandle �Ի���

IMPLEMENT_DYNAMIC(CDlgProcessHandle, CDialog)

const HandleColInfo CDlgProcessHandle::m_HandleColInfo[]=
{
	_T("����"),		400,
	_T("����"),		50,
	_T("��ʶ"),		100,
	_T("��ַ"),		100
};


// ����ģ����Ϣ�߳�
DWORD WINAPI CDlgProcessHandle::HandleInfoThread(LPVOID pVoid)
{
	CDlgProcessHandle *pThis=(CDlgProcessHandle *)pVoid;

	// ��ȡHandle��Ϣ
	SystemHandleInformation sysHandleInfo(pThis->m_nProcessID);
	pThis->m_vecHandleInfo=sysHandleInfo.GetHandleInfo();

	
	// ������Ϣ����UI
	pThis->PostMessage(WM_UPDATE_HANDLE_INFO,0,0);

#ifdef _DEBUG
	::OutputDebugString(_T("Exsit HandleInfo Thread\n"));
#endif

	return 0;
}

CDlgProcessHandle::CDlgProcessHandle(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessHandle::IDD, pParent)
	, m_pHandleThread(NULL)
	, m_nProcessID(0)
{

}

CDlgProcessHandle::~CDlgProcessHandle()
{
}

void CDlgProcessHandle::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS_TAB_HANDLE, m_wndListHandle);
}


BEGIN_MESSAGE_MAP(CDlgProcessHandle, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_MESSAGE(WM_FLUSH_HANDLE,OnFlushHandleInfo)
	ON_MESSAGE(WM_UPDATE_HANDLE_INFO,OnLoadHandleInfo)
END_MESSAGE_MAP()


// CDlgProcessHandle ��Ϣ�������

BOOL CDlgProcessHandle::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgProcessHandle::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CDialog::OnSize(nType, cx, cy);
}

void CDlgProcessHandle::OnDestroy()
{
	// �ȴ������
	if( m_pHandleThread!=NULL )
	{
		::WaitForSingleObject(m_pHandleThread->m_hThread,INFINITE);
		delete m_pHandleThread;
		m_pHandleThread=NULL;
	}

	m_font.DeleteObject();

	CDialog::OnDestroy();
}


void CDlgProcessHandle::InitList()
{
	// ��ʼ��Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_HandleColInfo)/sizeof(m_HandleColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_HandleColInfo[i].strColName,m_HandleColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListHandle.SetHeadings(strCol);

	// ���÷��
	m_wndListHandle.ModifyStyle(0,LVS_REPORT);
	m_wndListHandle.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 
	
	// ��������
	m_font.CreatePointFont(80,_T("΢���ź�"));
	m_wndListHandle.SetFont(&m_font);

	
	// ��������
	InsertItem();
}

void CDlgProcessHandle::InsertItem()
{
	// �������
	m_wndListHandle.DeleteAllItems();

	// ��������
	int nIndex=0,nItem=0;
	LVITEM lView={0};
	lView.mask	=LVIF_IMAGE;

	for(vector<SystemHandleInformation::HANDLE_INFORMATION>::const_iterator iter=m_vecHandleInfo.begin();
		iter!=m_vecHandleInfo.end(); ++iter)
	{
		m_wndListHandle.AddItem(iter->strPath.c_str(),
			iter->strType.c_str(),
			iter->strFlag.c_str(),
			iter->strAddr.c_str());
	}
	
}


// �����̶߳�ȡModule��Ϣ
LRESULT CDlgProcessHandle::OnFlushHandleInfo(WPARAM wParam,LPARAM lParam)
{
	// �ȴ������
	if( m_pHandleThread!=NULL )
	{
		::WaitForSingleObject(m_pHandleThread->m_hThread,INFINITE);
		delete m_pHandleThread;
		m_pHandleThread=NULL;
	}

	// ���ý���ID
	if( wParam==0 )
		return -1;
	
	m_nProcessID=(int)wParam;

	// ���
	m_vecHandleInfo.clear();
	

	// �����߳̽��м����ļ���Ϣ
	m_pHandleThread=AfxBeginThread((AFX_THREADPROC)HandleInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pHandleThread->m_bAutoDelete=FALSE;
	m_pHandleThread->ResumeThread();

	return 0;
}

// ����UI
LRESULT CDlgProcessHandle::OnLoadHandleInfo(WPARAM wParam,LPARAM lParam)
{
	// ��ֹ��˸
	m_wndListHandle.SetRedraw(FALSE);

	// �����Ϣ
	InsertItem();

	m_wndListHandle.SetRedraw(TRUE);
	m_wndListHandle.Invalidate();
	m_wndListHandle.UpdateWindow();

	return 0;
}