// TaskManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "TaskManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTaskManagerDlg 对话框


BEGIN_EASYSIZE_MAP(CTaskManagerDlg)
	EASYSIZE(IDC_TAB,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


BEGIN_MESSAGE_MAP(CTaskManagerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()

	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, OnTcnSelchangeTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, OnTcnSelchangingTab)

	ON_MESSAGE(WM_TAB_SEL_CHANGED,OnTabSelChange)
END_MESSAGE_MAP()


CTaskManagerDlg::CTaskManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void CTaskManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_wndTabMain);
}



// CTaskManagerDlg 消息处理程序

BOOL CTaskManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ModifyStyle(0,WS_CLIPCHILDREN);

	// 初始化Tab
	InitTab();

	INIT_EASYSIZE;
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CTaskManagerDlg::InitTab()
{
	TabInfo	tabInfo;

	// 创建每个属性页
	m_wndDlgApp.Create(CDlgApplication::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgApp;
	tabInfo.strName		=_T("应用程序");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgProcess.Create(CDlgProcess::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgProcess;
	tabInfo.strName		=_T("进程");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgModule.Create(CDlgModule::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgModule;
	tabInfo.strName		=_T("模块");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgService.Create(CDlgService::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgService;
	tabInfo.strName		=_T("服务");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgDrive.Create(CDlgDriver::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgDrive;
	tabInfo.strName		=_T("硬盘");
	m_vecTabInfo.push_back(tabInfo);


	// 创建选项
	for(vector<TabInfo>::iterator iter=m_vecTabInfo.begin();
		iter!=m_vecTabInfo.end();
		++iter)
	{
		m_wndTabMain.AddTab(iter->pDlg,iter->strName.GetBuffer(0));
		iter->strName.ReleaseBuffer();
	}

	
	// 改变属性页的大小位置
	CRect rc;
	m_wndTabMain.GetClientRect(rc);
	rc.top		+= 20;
	rc.bottom	-= 3;
	rc.left		+= 2;
	rc.right	-= 3;
	m_vecTabInfo.at(0).pDlg->MoveWindow(&rc);
	
	
	//m_vecTabInfo.at(0).pDlg->ShowWindow(SW_SHOW);
}

LRESULT CTaskManagerDlg::OnTabSelChange(WPARAM wParam,LPARAM lParam)
{
	static DWORD dwLastItem=0;
	DWORD dwCurItem=(DWORD)lParam;
	
	m_vecTabInfo.at(dwLastItem).pDlg->PostMessage(WM_SET_PAUSE,0,(LPARAM)TRUE);
	m_vecTabInfo.at(dwCurItem).pDlg->PostMessage(WM_SET_PAUSE,0,(LPARAM)FALSE);

	dwLastItem=dwCurItem;
	return 0;
}

void CTaskManagerDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}

void CTaskManagerDlg::OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	*pResult = 0;
}


void CTaskManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTaskManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CTaskManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTaskManagerDlg::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;
	
	/*for(vector<TabInfo>::iterator iter=m_vecTabInfo.begin();
		iter!=m_vecTabInfo.end();
		++iter)
	{
		iter->pDlg->MoveWindow(5,20,cx-15,cy-25);
	}*/
	// 更新位置
	if( GetSafeHwnd()!=NULL ) 
	{
		CRect rectTab;
		m_wndTabMain.GetClientRect(&rectTab);
		rectTab.top		+= 20;
		rectTab.bottom	-= 3;
		rectTab.left	+= 2;
		rectTab.right	-= 3;

		for(vector<TabInfo>::iterator iter=m_vecTabInfo.begin(); iter!=m_vecTabInfo.end(); ++iter)
		{
			iter->pDlg->MoveWindow(rectTab);
		}
	}
	

	CDialog::OnSize(nType, cx, cy);
}

BOOL CTaskManagerDlg::OnEraseBkgnd(CDC* pDC)
{

	return CDialog::OnEraseBkgnd(pDC);
}

