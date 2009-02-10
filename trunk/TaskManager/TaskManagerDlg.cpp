// TaskManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "TaskManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTaskManagerDlg �Ի���


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



// CTaskManagerDlg ��Ϣ�������

BOOL CTaskManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//ModifyStyle(0,WS_CLIPCHILDREN);

	// ��ʼ��Tab
	InitTab();

	INIT_EASYSIZE;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


void CTaskManagerDlg::InitTab()
{
	TabInfo	tabInfo;

	// ����ÿ������ҳ
	m_wndDlgApp.Create(CDlgApplication::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgApp;
	tabInfo.strName		=_T("Ӧ�ó���");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgProcess.Create(CDlgProcess::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgProcess;
	tabInfo.strName		=_T("����");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgModule.Create(CDlgModule::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgModule;
	tabInfo.strName		=_T("ģ��");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgService.Create(CDlgService::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgService;
	tabInfo.strName		=_T("����");
	m_vecTabInfo.push_back(tabInfo);

	m_wndDlgDrive.Create(CDlgDriver::IDD,&m_wndTabMain);
	tabInfo.pDlg		=&m_wndDlgDrive;
	tabInfo.strName		=_T("Ӳ��");
	m_vecTabInfo.push_back(tabInfo);


	// ����ѡ��
	for(vector<TabInfo>::iterator iter=m_vecTabInfo.begin();
		iter!=m_vecTabInfo.end();
		++iter)
	{
		m_wndTabMain.AddTab(iter->pDlg,iter->strName.GetBuffer(0));
		iter->strName.ReleaseBuffer();
	}

	
	// �ı�����ҳ�Ĵ�Сλ��
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTaskManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
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
	// ����λ��
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

