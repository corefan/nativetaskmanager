// DlgProcessEnvironment.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessEnvironment.h"


BEGIN_EASYSIZE_MAP(CDlgProcessEnvironment)
	EASYSIZE(IDC_LIST_PROCESS_TAB_ENV,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


// CDlgProcessEnvironment 对话框

IMPLEMENT_DYNAMIC(CDlgProcessEnvironment, CDialog)

const EnvColInfo CDlgProcessEnvironment::m_EnvColInfo[]=
{
	_T("变量"),		100,
	_T("值"),		500,
};


// 更新模块信息线程
DWORD WINAPI CDlgProcessEnvironment::EnvInfoThread(LPVOID pVoid)
{
	CDlgProcessEnvironment *pThis=(CDlgProcessEnvironment *)pVoid;

	// 加载信息
	HANDLE hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,pThis->m_nProcessID);
	
	if( hProcess )
	{
		_ENVSTRING_t stEnvData;
		CStringArray strArray;
		// Read the process environment block
		if( CProcessEnvReader::ReadEnvironmentBlock( hProcess, stEnvData ))
		{
			// Parse the retrieved data
			CProcessEnvReader::ParseEnvironmentStrings( stEnvData.pData,
				stEnvData.nSize/2,
				strArray );

			// Seperate values and variables
			CProcessEnvReader::SeparateVariablesAndValues(strArray, pThis->m_vecEnv );
		}

		::CloseHandle(hProcess);
	}
	
	// 发送消息更新UI
	pThis->PostMessage(WM_UPDATE_ENV_INFO,0,0);

#ifdef _DEBUG
	::OutputDebugString(_T("Exsit EnvironmentInfo Thread\n"));
#endif

	return 0;
}



CDlgProcessEnvironment::CDlgProcessEnvironment(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessEnvironment::IDD, pParent)
	, m_pEnvThread(NULL)
	, m_nProcessID(0)
{

}

CDlgProcessEnvironment::~CDlgProcessEnvironment()
{
}

void CDlgProcessEnvironment::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS_TAB_ENV, m_wndListEnv);
}


BEGIN_MESSAGE_MAP(CDlgProcessEnvironment, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_MESSAGE(WM_FLUSH_ENV,OnFlushEnvInfo)
	ON_MESSAGE(WM_UPDATE_ENV_INFO,OnLoadEnvInfo)
END_MESSAGE_MAP()


// CDlgProcessEnvironment 消息处理程序

BOOL CDlgProcessEnvironment::OnInitDialog()
{
	CDialog::OnInitDialog();

	INIT_EASYSIZE;

	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgProcessEnvironment::OnDestroy()
{
	// 等待并清除
	if( m_pEnvThread!=NULL )
	{
		::WaitForSingleObject(m_pEnvThread->m_hThread,INFINITE);
		delete m_pEnvThread;
		m_pEnvThread=NULL;
	}

	m_font.DeleteObject();

	CDialog::OnDestroy();
}

void CDlgProcessEnvironment::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CDialog::OnSize(nType, cx, cy);
}



void CDlgProcessEnvironment::InitList()
{
	// 初始化Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_EnvColInfo)/sizeof(m_EnvColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_EnvColInfo[i].strColName,m_EnvColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListEnv.SetHeadings(strCol);

	// 设置风格
	m_wndListEnv.ModifyStyle(0,LVS_REPORT);
	m_wndListEnv.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 
	
	// 设置字体
	m_font.CreatePointFont(80,_T("微软雅黑"));
	m_wndListEnv.SetFont(&m_font);

	
	// 插入数据
	InsertItem();
}

void CDlgProcessEnvironment::InsertItem()
{
	// 清空数据
	m_wndListEnv.DeleteAllItems();

	// 插入数据
	int nIndex=0,nItem=0;
	LVITEM lView={0};
	lView.mask	=LVIF_IMAGE;

	for(EnvVarValArray::iterator iter=m_vecEnv.begin(); iter!=m_vecEnv.end(); ++iter)
	{
		m_wndListEnv.AddItem(iter->first,
			iter->second);
	}
}


// 开启线程读取Module信息
LRESULT CDlgProcessEnvironment::OnFlushEnvInfo(WPARAM wParam,LPARAM lParam)
{
	// 等待并清除
	if( m_pEnvThread!=NULL )
	{
		::WaitForSingleObject(m_pEnvThread->m_hThread,INFINITE);
		delete m_pEnvThread;
		m_pEnvThread=NULL;
	}

	// 设置进程ID
	if( wParam==0 )
		return -1;
	
	m_nProcessID=(int)wParam;

	// 清空
	m_vecEnv.clear();

	// 开启线程进行加载文件信息
	m_pEnvThread=AfxBeginThread((AFX_THREADPROC)EnvInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pEnvThread->m_bAutoDelete=FALSE;
	m_pEnvThread->ResumeThread();

	return 0;
}

// 更新UI
LRESULT CDlgProcessEnvironment::OnLoadEnvInfo(WPARAM wParam,LPARAM lParam)
{
	// 防止闪烁
	m_wndListEnv.SetRedraw(FALSE);

	// 添加信息
	InsertItem();

	m_wndListEnv.SetRedraw(TRUE);
	m_wndListEnv.Invalidate();
	m_wndListEnv.UpdateWindow();

	return 0;
}