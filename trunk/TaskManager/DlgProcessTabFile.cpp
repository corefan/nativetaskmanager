// DlgProcessTabFile.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessTabFile.h"


// CDlgProcessTabFile 对话框
BEGIN_EASYSIZE_MAP(CDlgProcessTabFile)
	EASYSIZE(IDC_LIST_PROCESS_TAB_FILE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgProcessTabFile, CDialog)


CString CDlgProcessTabFile::m_strItemName[]=
{
	_T("程序文件"),_T("创建时间"),_T("访问时间"),_T("修改时间"),
	_T("文件属性"),_T("常规注解"),_T("公司名称"),_T("文件描述"),
	_T("文件版本"),_T("内部名称"),_T("版权声明"),_T("原始名称"),
	_T("所属产品"),_T("产品版本")
};

// 更新文件信息线程
DWORD WINAPI CDlgProcessTabFile::FileInfoThread(LPVOID pVoid)
{
	CDlgProcessTabFile *pThis=(CDlgProcessTabFile *)pVoid;

	// 加载信息
	if( pThis->m_strFilePath.IsEmpty() )
		return -1;

	CFileVersion version;
	if( !version.Open(pThis->m_strFilePath) )	// 没获取成功则退出
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

	// 发送消息更新UI
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


// CDlgProcessTabFile 消息处理程序

BOOL CDlgProcessTabFile::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;

	
	InitList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CDlgProcessTabFile::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;
	CDialog::OnSize(nType, cx, cy);
}


void CDlgProcessTabFile::InitList()
{
	// 初始化Colnum
	m_wndListProcessTabFile.InsertColumn(0,_T("项目"),LVCFMT_LEFT,150);
	m_wndListProcessTabFile.InsertColumn(1,_T("值"),LVCFMT_LEFT,445);

	// 设置风格
	m_wndListProcessTabFile.ModifyStyle(0,LVS_REPORT);
	m_wndListProcessTabFile.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EDITLABELS); 

	
	m_font.CreatePointFont(80,_T("微软雅黑"));
	m_wndListProcessTabFile.SetFont(&m_font);


	InsertItem();
}

void CDlgProcessTabFile::InsertItem()
{
	m_wndListProcessTabFile.DeleteAllItems();

	// 如果文件路径为空，则退出
	if( m_strFilePath.IsEmpty() )
		return;

	// 插入数据
	for(int i=0; i<sizeof(m_strItemName)/sizeof(m_strItemName[0]); ++i)
	{
		m_wndListProcessTabFile.InsertItem(i,m_strItemName[i]);
		m_wndListProcessTabFile.SetItemText(i,1,m_vecItemInfo.at(i));
	}

}

void CDlgProcessTabFile::OnDestroy()
{
	// 清除数据
	if( m_pFileThread!=NULL )
	{
		::WaitForSingleObject(m_pFileThread->m_hThread,INFINITE);
		delete m_pFileThread;
		m_pFileThread=NULL;
	}

	m_font.DeleteObject();

	CDialog::OnDestroy();
}

// 加载文件信息
LRESULT CDlgProcessTabFile::OnFlushFileInfo(WPARAM wParam,LPARAM lParam)
{
	// 等待并清除
	if( m_pFileThread!=NULL )
	{
		::WaitForSingleObject(m_pFileThread->m_hThread,INFINITE);
		delete m_pFileThread;
		m_pFileThread=NULL;
	}

	// 设置文件路径
	if( wParam==0 )
		return -1;
	
	m_strFilePath=(LPTSTR)wParam;
	// 清空
	m_vecItemInfo.clear();

	// 开启线程进行加载文件信息
	m_pFileThread=AfxBeginThread((AFX_THREADPROC)FileInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pFileThread->m_bAutoDelete=FALSE;
	m_pFileThread->ResumeThread();

	return 0;
}

// 更新UI
LRESULT CDlgProcessTabFile::OnLoadFileInfo(WPARAM wParam,LPARAM lParam)
{
	CString strPath((LPTSTR)wParam);

	// 防止闪烁
	m_wndListProcessTabFile.SetRedraw(FALSE);

	// 添加信息
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
