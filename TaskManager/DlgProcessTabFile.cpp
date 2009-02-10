// DlgProcessTabFile.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessTabFile.h"


// CDlgProcessTabFile �Ի���
BEGIN_EASYSIZE_MAP(CDlgProcessTabFile)
	EASYSIZE(IDC_LIST_PROCESS_TAB_FILE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgProcessTabFile, CDialog)


CString CDlgProcessTabFile::m_strItemName[]=
{
	_T("�����ļ�"),_T("����ʱ��"),_T("����ʱ��"),_T("�޸�ʱ��"),
	_T("�ļ�����"),_T("����ע��"),_T("��˾����"),_T("�ļ�����"),
	_T("�ļ��汾"),_T("�ڲ�����"),_T("��Ȩ����"),_T("ԭʼ����"),
	_T("������Ʒ"),_T("��Ʒ�汾")
};

// �����ļ���Ϣ�߳�
DWORD WINAPI CDlgProcessTabFile::FileInfoThread(LPVOID pVoid)
{
	CDlgProcessTabFile *pThis=(CDlgProcessTabFile *)pVoid;

	// ������Ϣ
	if( pThis->m_strFilePath.IsEmpty() )
		return -1;

	CFileVersion version;
	if( !version.Open(pThis->m_strFilePath) )	// û��ȡ�ɹ����˳�
		return -1;

	//pThis->m_vecItemInfo.reserve(16);
	
	pThis->m_vecItemInfo.push_back(pThis->m_strFilePath);
	pThis->m_vecItemInfo.push_back(version.GetFileConsturctTime());
	pThis->m_vecItemInfo.push_back(version.GetFileAccessTime());
	pThis->m_vecItemInfo.push_back(version.GetFileModifiedTime());
	pThis->m_vecItemInfo.push_back(version.GetFileAttributes());
	pThis->m_vecItemInfo.push_back(version.GetComments());
	pThis->m_vecItemInfo.push_back(version.GetCompanyName());
	pThis->m_vecItemInfo.push_back(version.GetFileDescription());
	pThis->m_vecItemInfo.push_back(version.GetProductVersion());
	pThis->m_vecItemInfo.push_back(version.GetInternalName());
	pThis->m_vecItemInfo.push_back(version.GetLegalCopyright());
	pThis->m_vecItemInfo.push_back(version.GetOriginalFilename());
	pThis->m_vecItemInfo.push_back(version.GetFixedFileVersion());
	pThis->m_vecItemInfo.push_back(version.GetFileVersion());
	/*pThis->m_vecItemInfo.push_back(version.GetComments());
	pThis->m_vecItemInfo.push_back(version.GetLegalCopyright());
	pThis->m_vecItemInfo.push_back(version.GetPrivateBuild());
	pThis->m_vecItemInfo.push_back(version.GetSpecialBuild());*/

	// ������Ϣ����UI
	pThis->PostMessage(WM_UPDATE_FILE_INFO,0,0);

#ifdef _DEBUG
	::OutputDebugString(_T("Exsit FileInfo Thread\n"));
#endif
	return 0;
}


CDlgProcessTabFile::CDlgProcessTabFile(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessTabFile::IDD, pParent)
	, m_pFileThread(NULL)
	, m_strFilePath(_T(""))
{

}

CDlgProcessTabFile::~CDlgProcessTabFile()
{
}

void CDlgProcessTabFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS_TAB_FILE, m_wndListProcessTabFile);
}


BEGIN_MESSAGE_MAP(CDlgProcessTabFile, CDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROCESS_TAB_FILE, &CDlgProcessTabFile::OnNMClickListProcessTabFile)
	ON_MESSAGE(WM_FLUSH_FILE,OnFlushFileInfo)
	ON_MESSAGE(WM_UPDATE_FILE_INFO,OnLoadFileInfo)
END_MESSAGE_MAP()


// CDlgProcessTabFile ��Ϣ�������

BOOL CDlgProcessTabFile::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;

	
	InitList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgProcessTabFile::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;
	CDialog::OnSize(nType, cx, cy);
}


void CDlgProcessTabFile::InitList()
{
	// ��ʼ��Colnum
	m_wndListProcessTabFile.InsertColumn(0,_T("��Ŀ"),LVCFMT_LEFT,150);
	m_wndListProcessTabFile.InsertColumn(1,_T("ֵ"),LVCFMT_LEFT,445);

	// ���÷��
	m_wndListProcessTabFile.ModifyStyle(0,LVS_REPORT);
	m_wndListProcessTabFile.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EDITLABELS); 

	
	m_font.CreatePointFont(80,_T("΢���ź�"));
	m_wndListProcessTabFile.SetFont(&m_font);


	InsertItem();
}

void CDlgProcessTabFile::InsertItem()
{
	m_wndListProcessTabFile.DeleteAllItems();

	// ����ļ�·��Ϊ�գ����˳�
	if( m_strFilePath.IsEmpty() )
		return;

	// ��������
	for(int i=0; i<sizeof(m_strItemName)/sizeof(m_strItemName[0]); ++i)
	{
		m_wndListProcessTabFile.InsertItem(i,m_strItemName[i]);
		m_wndListProcessTabFile.SetItemText(i,1,m_vecItemInfo.at(i));
	}

}

void CDlgProcessTabFile::OnDestroy()
{
	// �������
	if( m_pFileThread!=NULL )
	{
		::WaitForSingleObject(m_pFileThread->m_hThread,INFINITE);
		delete m_pFileThread;
		m_pFileThread=NULL;
	}

	m_font.DeleteObject();

	CDialog::OnDestroy();
}

// �����ļ���Ϣ
LRESULT CDlgProcessTabFile::OnFlushFileInfo(WPARAM wParam,LPARAM lParam)
{
	// �ȴ������
	if( m_pFileThread!=NULL )
	{
		::WaitForSingleObject(m_pFileThread->m_hThread,INFINITE);
		delete m_pFileThread;
		m_pFileThread=NULL;
	}

	// �����ļ�·��
	if( wParam==0 )
		return -1;
	
	m_strFilePath=(LPTSTR)wParam;
	// ���
	m_vecItemInfo.clear();

	// �����߳̽��м����ļ���Ϣ
	m_pFileThread=AfxBeginThread((AFX_THREADPROC)FileInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pFileThread->m_bAutoDelete=FALSE;
	m_pFileThread->ResumeThread();

	return 0;
}

// ����UI
LRESULT CDlgProcessTabFile::OnLoadFileInfo(WPARAM wParam,LPARAM lParam)
{
	CString strPath((LPTSTR)wParam);

	// ��ֹ��˸
	m_wndListProcessTabFile.SetRedraw(FALSE);

	// �����Ϣ
	InsertItem();

	m_wndListProcessTabFile.SetRedraw(TRUE);
	m_wndListProcessTabFile.Invalidate();
	m_wndListProcessTabFile.UpdateWindow();

	return 0;
}


void CDlgProcessTabFile::OnNMClickListProcessTabFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	int nItem=pListView->iItem;
	if( nItem!=-1 )
	{
		CRect rc;   
		m_wndListProcessTabFile.GetSubItemRect(nItem,1,LVIR_BOUNDS,rc);   
		
		CEdit edit;
		if( edit.Create(ES_CENTER,rc,&m_wndListProcessTabFile,1000))   
		{   
			edit.MoveWindow(rc.left,rc.top,rc.Width(),rc.Height(),TRUE);   
			edit.ShowWindow(TRUE);  
		}
	}

	*pResult = 0;
}
