// DlgProcessHandle.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessHandle.h"

BEGIN_EASYSIZE_MAP(CDlgProcessHandle)
	EASYSIZE(IDC_LIST_PROCESS_TAB_HANDLE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP
// CDlgProcessHandle 对话框

IMPLEMENT_DYNAMIC(CDlgProcessHandle, CDialog)

const HandleColInfo CDlgProcessHandle::m_HandleColInfo[]=
{
	_T("名称"),		400,
	_T("类型"),		50,
	_T("标识"),		100,
	_T("地址"),		100
};


// 更新模块信息线程
DWORD WINAPI CDlgProcessHandle::HandleInfoThread(LPVOID pVoid)
{
	CDlgProcessHandle *pThis=(CDlgProcessHandle *)pVoid;

	// 获取Handle信息
	SystemHandleInformation sysHandleInfo(pThis->m_nProcessID);
	pThis->m_vecHandleInfo=sysHandleInfo.GetHandleInfo();

	
	// 发送消息更新UI
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


// CDlgProcessHandle 消息处理程序

BOOL CDlgProcessHandle::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgProcessHandle::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CDialog::OnSize(nType, cx, cy);
}

void CDlgProcessHandle::OnDestroy()
{
	// 等待并清除
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
	// 初始化Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_HandleColInfo)/sizeof(m_HandleColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_HandleColInfo[i].strColName,m_HandleColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListHandle.SetHeadings(strCol);

	// 设置风格
	m_wndListHandle.ModifyStyle(0,LVS_REPORT);
	m_wndListHandle.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 
	
	// 设置字体
	m_font.CreatePointFont(80,_T("微软雅黑"));
	m_wndListHandle.SetFont(&m_font);

	
	// 插入数据
	InsertItem();
}

void CDlgProcessHandle::InsertItem()
{
	// 清空数据
	m_wndListHandle.DeleteAllItems();

	// 插入数据
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


// 开启线程读取Module信息
LRESULT CDlgProcessHandle::OnFlushHandleInfo(WPARAM wParam,LPARAM lParam)
{
	// 等待并清除
	if( m_pHandleThread!=NULL )
	{
		::WaitForSingleObject(m_pHandleThread->m_hThread,INFINITE);
		delete m_pHandleThread;
		m_pHandleThread=NULL;
	}

	// 设置进程ID
	if( wParam==0 )
		return -1;
	
	m_nProcessID=(int)wParam;

	// 清空
	m_vecHandleInfo.clear();
	

	// 开启线程进行加载文件信息
	m_pHandleThread=AfxBeginThread((AFX_THREADPROC)HandleInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pHandleThread->m_bAutoDelete=FALSE;
	m_pHandleThread->ResumeThread();

	return 0;
}

// 更新UI
LRESULT CDlgProcessHandle::OnLoadHandleInfo(WPARAM wParam,LPARAM lParam)
{
	// 防止闪烁
	m_wndListHandle.SetRedraw(FALSE);

	// 添加信息
	InsertItem();

	m_wndListHandle.SetRedraw(TRUE);
	m_wndListHandle.Invalidate();
	m_wndListHandle.UpdateWindow();

	return 0;
}