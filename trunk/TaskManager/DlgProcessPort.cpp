// DlgProcessPort.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessPort.h"


// CDlgProcessPort 对话框
BEGIN_EASYSIZE_MAP(CDlgProcessPort)
	EASYSIZE(IDC_LIST_PROCESS_TAB_PORT,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgProcessPort, CDialog)

const PortColInfo CDlgProcessPort::m_PortColInfo[]=
{
	_T("端口类型"),	100,
	_T("端口状态"),	100,
	_T("本地IP"),	100,
	_T("本地端口号"),100,
	_T("远程IP"),	100,
	_T("远程端口号"),100,
};

// 更新模块信息线程
DWORD WINAPI CDlgProcessPort::PortInfoThread(LPVOID pVoid)
{
	CDlgProcessPort *pThis=(CDlgProcessPort *)pVoid;

	pThis->m_cs.Lock();
	// 遍历进程中使用端口信息
	EnumPort enumPort(pThis->m_nProcessID);
	enumPort.Enum();

	pThis->m_vecPortInfo=enumPort.GetPortInfo();
	pThis->m_cs.Unlock();

	// 发送消息更新UI
	pThis->PostMessage(WM_UPDATE_PORT_INFO,0,0);

#ifdef _DEBUG
	::OutputDebugString(_T("Exsit PortInfo Thread\n"));
#endif
	return 0;
}


CDlgProcessPort::CDlgProcessPort(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessPort::IDD, pParent)
	, m_nProcessID(0)
	, m_pPortThread(NULL)
{

}

CDlgProcessPort::~CDlgProcessPort()
{
}

void CDlgProcessPort::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS_TAB_PORT, m_wndListPort);
}


BEGIN_MESSAGE_MAP(CDlgProcessPort, CDialog)
	ON_WM_SIZE()
	ON_WM_DESTROY()

	ON_MESSAGE(WM_FLUSH_PORT,OnFlushPortInfo)
	ON_MESSAGE(WM_UPDATE_PORT_INFO,OnLoadPortInfo)
END_MESSAGE_MAP()


// CDlgProcessPort 消息处理程序
void CDlgProcessPort::PreSubclassWindow()
{
	

	CDialog::PreSubclassWindow();
}

BOOL CDlgProcessPort::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;
	
	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgProcessPort::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CDialog::OnSize(nType, cx, cy);

}

void CDlgProcessPort::OnDestroy()
{
	// 等待并清除
	if( m_pPortThread!=NULL )
	{
		::WaitForSingleObject(m_pPortThread->m_hThread,INFINITE);
		delete m_pPortThread;
		m_pPortThread=NULL;
	}

	m_font.DeleteObject();

	CDialog::OnDestroy();
}


void CDlgProcessPort::InitList()
{
	// 设置风格
	m_wndListPort.ModifyStyle(0,LVS_REPORT);
	m_wndListPort.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 

	// 初始化Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_PortColInfo)/sizeof(m_PortColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_PortColInfo[i].strColName,m_PortColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListPort.SetHeadings(strCol);

	// 设置字体
	m_font.CreatePointFont(80,_T("微软雅黑"));
	m_wndListPort.SetFont(&m_font);

	// 插入数据
	InsertItem();
}

void CDlgProcessPort::InsertItem()
{
	m_wndListPort.DeleteAllItems();


	// 插入数据
	int nIndex=0,nItem=0;
	LVITEM lView={0};
	lView.mask	=LVIF_IMAGE;

	m_cs.Lock();
	for(vector<PortInfo>::iterator iter= m_vecPortInfo.begin(); iter!= m_vecPortInfo.end();iter++) 
	{
		m_wndListPort.AddItem(iter->m_strType.c_str(),
			iter->m_strStatus.c_str(),
			iter->m_strLocalIP.c_str(),
			iter->m_strLocalPort.c_str(),
			iter->m_strRemoteIP.c_str(),
			iter->m_strRemotePort.c_str()
			);
	}
	m_cs.Unlock();
	
}

// 开启线程读取Port信息
LRESULT CDlgProcessPort::OnFlushPortInfo(WPARAM wParam,LPARAM lParam)
{
	// 等待并清除
	if( m_pPortThread!=NULL )
	{
		::WaitForSingleObject(m_pPortThread->m_hThread,INFINITE);
		delete m_pPortThread;
		m_pPortThread=NULL;
	}

	// 设置进程ID
	if( wParam==0 )
		return -1;
	
	m_nProcessID=(int)wParam;

	// 清空
	m_vecPortInfo.clear();

	// 开启线程进行加载文件信息
	m_pPortThread=AfxBeginThread((AFX_THREADPROC)PortInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pPortThread->m_bAutoDelete=FALSE;
	m_pPortThread->ResumeThread();

	return 0;
}

// 更新UI
LRESULT CDlgProcessPort::OnLoadPortInfo(WPARAM wParam,LPARAM lParam)
{
	// 防止闪烁
	m_wndListPort.SetRedraw(FALSE);

	// 添加信息
	InsertItem();

	m_wndListPort.SetRedraw(TRUE);
	m_wndListPort.Invalidate();
	m_wndListPort.UpdateWindow();

	return 0;
}