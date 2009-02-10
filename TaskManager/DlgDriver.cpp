// DlgDriver.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgDriver.h"
#include "GetHardSoftInfo.h"
#include "CMemDC.h"
#include "cpdh.h"


// CDlgDriver 对话框

BEGIN_EASYSIZE_MAP(CDlgDriver)
	EASYSIZE(IDC_STATIC_DISK_GROUP1,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_DISK_GROUP2,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_BORDER,0)

	EASYSIZE(IDC_STATIC_SERIAL,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_REVISION,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_BUFFER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_DRIVER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_SIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_TYPE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)

	EASYSIZE(IDC_STATIC_DRIVE_MODEL_NUM,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_SERIAL_NUM,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_RIVISION_NUM,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_DIRVE_SIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_CONTROL,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_BUFFER_SIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_STATIC_DIRVE_TYPE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)

	EASYSIZE(IDC_TREE_DRIVE,ES_BORDER,ES_BORDER,ES_KEEPSIZE,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_CHART,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgDriver, CGDIPlusDialog)


DWORD WINAPI CDlgDriver::ScanDiskThread(LPVOID pVoid)
{
	ASSERT(pVoid);
	CDlgDriver *pThis=(CDlgDriver *)pVoid;

	while(!pThis->m_bExit)
	{
		// 是否暂停
		if( !pThis->m_bPause )
		{
			// 获取硬盘信息
			pThis->m_DiskVolume.Enum();
			pThis->m_vecVolume=pThis->m_DiskVolume.GetDiskVolumeInfo();

			// 获取Disk Performance Counter
			vector<CString> Counters;
			vector<CString> Instances;
			try
			{
				CPdh::EnumObjectItems(NULL, _T("PhysicalDisk"), Counters, Instances, PERF_DETAIL_WIZARD);
			}
			catch (CPdhException* pEx)
			{
				// catch errors from PDH
				CString sError;
				pEx->GetErrorMessage(sError);
				TRACE(_T("a CPdhException occurred, Error:%s\n"), sError);

				//Also display a message box displaying the error to the end user
				CString sMsg;
				sMsg.Format(_T("a CPdhException occurred, Error:%s"), sError);
				//AfxMessageBox(sMsg);
				pEx->Delete();
			}

			for(int i=0; i<Counters.size(); i++)
			{
				pThis->m_vecDiskPerf.push_back(Counters.at(i));
			}
	
			pThis->PostMessage(WM_UPDATE_DISK_INFO);

			// 退出
			pThis->m_bExit=TRUE;
		}
		else
		{
			//::OutputDebugString(_T("ScanDiskThread Paused\n"));
		}

		Sleep(1000);
	}

#ifdef _DEBUG
	::OutputDebugString(_T("Leveal ScanDiskThread\n"));
#endif
	return 0;
}


CDlgDriver::CDlgDriver(CWnd* pParent /*=NULL*/)
	: CGDIPlusDialog(CDlgDriver::IDD, pParent)
	, m_strDriveModel(_T(""))
	, m_strDriveSerial(_T(""))
	, m_strRevision(_T(""))
	, m_strDriveSize(_T(""))
	, m_strDriveContraller(_T(""))
	, m_strDriveBufferSize(_T(""))
	, m_strDriveType(_T(""))
	, m_strDriveVolumeType(_T("分区格式: --"))
	, m_strDriveUsage(_T("使用空间: --"))
	, m_strDriveFree(_T("空闲空间: --"))
	, m_pImageBack(NULL)
	, m_pImageDiskMain(NULL)
	, m_pImageDiskRemove(NULL)
	, m_pImageDiskNetwork(NULL)
	, m_pImageDiskDVD(NULL)
	, m_pScanDiskThread(NULL)
	, m_bExit(FALSE)
	, m_bPause(TRUE)
	, n_nPerfCountItem(0)
{

}

CDlgDriver::~CDlgDriver()
{
}

void CDlgDriver::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_DRIVE_MODEL_NUM,	m_strDriveModel);
	DDX_Text(pDX, IDC_STATIC_SERIAL_NUM,		m_strDriveSerial);
	DDX_Text(pDX, IDC_STATIC_RIVISION_NUM,		m_strRevision);
	DDX_Text(pDX, IDC_STATIC_DIRVE_SIZE,		m_strDriveSize);
	DDX_Text(pDX, IDC_STATIC_CONTROL,			m_strDriveContraller);
	DDX_Text(pDX, IDC_STATIC_BUFFER_SIZE,		m_strDriveBufferSize);
	DDX_Text(pDX, IDC_STATIC_DIRVE_TYPE,		m_strDriveType);
	DDX_Text(pDX, IDC_STATIC_FILE_SYSTEM,		m_strDriveVolumeType);
	DDX_Text(pDX, IDC_STATIC_DISK_USAGE,		m_strDriveUsage);
	DDX_Text(pDX, IDC_STATIC_DISK_FREE,			m_strDriveFree);

	DDX_Control(pDX, IDC_STATIC_DRIVE_MODEL_NUM, m_wndStatic1);
	DDX_Control(pDX, IDC_STATIC_SERIAL_NUM, m_wndStatic2);
	DDX_Control(pDX, IDC_STATIC_RIVISION_NUM, m_wndStatic3);
	DDX_Control(pDX, IDC_STATIC_DIRVE_SIZE, m_wndStatic4);
	DDX_Control(pDX, IDC_STATIC_CONTROL, m_wndStatic5);
	DDX_Control(pDX, IDC_STATIC_BUFFER_SIZE, m_wndStatic6);
	DDX_Control(pDX, IDC_STATIC_DIRVE_TYPE, m_wndStatic7);
	DDX_Control(pDX, IDC_STATIC_FILE_SYSTEM, m_wndStatic8);
	DDX_Control(pDX, IDC_STATIC_DISK_USAGE, m_wndStatic9);
	DDX_Control(pDX, IDC_STATIC_DISK_FREE, m_wndStatic10);
	DDX_Control(pDX, IDC_STATIC_SERIAL, m_wndStatic11);
	DDX_Control(pDX, IDC_STATIC_REVISION, m_wndStatic12);
	DDX_Control(pDX, IDC_STATIC_BUFFER, m_wndStatic13);
	DDX_Control(pDX, IDC_STATIC_DRIVER, m_wndStatic14);
	DDX_Control(pDX, IDC_STATIC_SIZE, m_wndStatic15);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_wndStatic16);
	DDX_Control(pDX, IDC_STATIC_DISK_PERFORMANCE, m_wndStatic17);
	//DDX_Control(pDX, IDC_STATIC_DISK_FREE, m_wndStatic18);
	//DDX_Control(pDX, IDC_STATIC_FILE_SYSTEM, m_wndStatic19);

	DDX_Control(pDX, IDC_TREE_DRIVE, m_wndTree);
	DDX_Control(pDX, IDC_COMBO_SELECT_DISK_PERFORMANCE, m_wndComboDisk);
}


BEGIN_MESSAGE_MAP(CDlgDriver, CGDIPlusDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()

	ON_MESSAGE(WM_UPDATE_DISK_INFO,Update)
	ON_MESSAGE(WM_SET_PAUSE,OnSetPause)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DRIVE, &CDlgDriver::OnTvnSelchangedTreeDrive)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_DISK_VIEW, &CDlgDriver::OnBnClickedButtonDiskView)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgDriver 消息处理程序
BOOL CDlgDriver::OnInitDialog()
{
	CGDIPlusDialog::OnInitDialog();

	INIT_EASYSIZE;

	// 启动线程检测
	m_pScanDiskThread=AfxBeginThread((AFX_THREADPROC)ScanDiskThread,this,0,0,CREATE_SUSPENDED,NULL);
	m_pScanDiskThread->m_bAutoDelete=FALSE;
	m_pScanDiskThread->ResumeThread();

	InitTree();
	InitChart();
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CDlgDriver::SetPause(BOOL bPause)
{
	m_bPause=bPause;	

	return m_bPause;
}


void CDlgDriver::InitDiskInfo()
{
	using namespace DISK_INFO;
	CGetMachineInfo m_diskInfo;

	m_strDriveModel			=m_diskInfo.str_DN_Modol;
	m_strDriveSerial		=m_diskInfo.str_DN_Serial;
	m_strRevision 			=m_diskInfo.str_DN_ControllerRevision;
	m_strDriveSize 			=m_diskInfo.str_HardDeskSize+_T("  G");
	m_strDriveContraller 	=m_diskInfo.str_Controller;
	m_strDriveBufferSize  	=m_diskInfo.str_HardDeskBufferSize+_T("  KB");
	m_strDriveType			=m_diskInfo.str_HardDeskType;	

	UpdateData(FALSE);
}

void CDlgDriver::InitTree()
{
	// 创建图标及关联
	m_wndImage.Create(32,32,ILC_COLOR32 | ILC_MASK ,0,0);
	m_wndTree.SetImageList(&m_wndImage,TVSIL_NORMAL);

	// 状态图片
	LoadBitmap();
}

void CDlgDriver::InitChart()
{
	// 初始化Chart Graph
	m_wndChart.SubclassDlgItem(IDC_STATIC_CHART, this); 
	// Set the chart title text
	m_wndChart.SetTitle(_T("硬盘逻辑分区信息"));

	// Reset the chart and delete all pieces
	m_wndChart.Reset();
}

void CDlgDriver::InitBarChart()
{
	// 获取PieChart位置 
	CRect rcClient;
	GetDlgItem(IDC_STATIC_CHART)->GetWindowRect(&rcClient);
	ScreenToClient(&rcClient);

	if ( !m_wndBarChart.Create(rcClient, this, IDC_STATIC_PERFORMANCE_CHART ) )
	{
		if (!m_wndBarChart.GetSafeHwnd())
		{
			AfxMessageBox(_T("不能创建磁盘性能图!"));
			return;
		}

		m_wndBarChart.Reset();
		m_wndBarChart.SetAutoScale(FALSE);

		m_wndBarChart.SetTitle(_T("磁盘性能图"));

		m_wndBarChart.SetBKColor(RGB(255, 255, 240));
		m_wndBarChart.ShowTooltip(TRUE);
		m_wndBarChart.ShowBarText(TRUE,TRUE);
	}

	// 初始化性能计数器
	InitCounter();
}

void CDlgDriver::InitPerformaceDisk()
{
	// 初始化性能选择框
	int nIndex=0;
	for(vector<CString>::iterator iter=m_vecDiskPerf.begin(); iter!=m_vecDiskPerf.end(); ++iter)
	{
		m_wndComboDisk.AddString(*iter);
		m_wndComboDisk.SetCheck(nIndex,FALSE);
		
		nIndex++;
	}
}

void CDlgDriver::InitCounter()
{
	// 初始化性能计数器
	if( !m_PerfMon.Initialize() )
	{
		AfxMessageBox(_T("初始化性能计数器失败!"));
		return;
	}
}


void CDlgDriver::LoadBitmap()
{
	// 加载图片
	m_pImageBack		=new Bitmap(_T("Image\\Diak_Main.png"));
	m_pImageDiskMain	=new Bitmap(_T("Image\\Disk_Hard.png"));
	m_pImageDiskRemove	=new Bitmap(_T("Image\\Disk_Remove.png"));
	m_pImageDiskNetwork =new Bitmap(_T("Image\\Disk_Network.png"));
	m_pImageDiskDVD		=new Bitmap(_T("Image\\Disk_DVD.png"));
}

void CDlgDriver::DestroyBitmap()
{	
	// 释放图片内存
	if( m_pImageBack!=NULL )
		delete m_pImageBack;
	if( m_pImageDiskMain!=NULL )
		delete m_pImageDiskMain;
	if( m_pImageDiskRemove!=NULL )
		delete m_pImageDiskRemove;
	if( m_pImageDiskDVD!=NULL )
		delete m_pImageDiskDVD;
	if( m_pImageDiskNetwork!=NULL )
		delete m_pImageDiskNetwork;

}

void CDlgDriver::AddItem()
{
	int nImage=0;
	for(vector<DiskInfo>::iterator iter=m_vecVolume.begin(); iter!=m_vecVolume.end(); ++iter)
	{
		HBITMAP hBitmap=NULL;
		CBitmap* pbm=NULL;

		// 根据硬盘类型选择加载图片
		switch(iter->dwType)
		{
		case DRIVE_REMOVABLE:
			m_pImageDiskRemove->GetHBITMAP(Color(0,0,0),&hBitmap);
			break;
		case DRIVE_FIXED:
			m_pImageDiskMain->GetHBITMAP(Color(0,0,0),&hBitmap);
			break;
		case DRIVE_CDROM:
			m_pImageDiskDVD->GetHBITMAP(Color(0,0,0),&hBitmap);
			break;
		case DRIVE_REMOTE:
			m_pImageDiskNetwork->GetHBITMAP(Color(0,0,0),&hBitmap);
			break;
		default:
			m_pImageDiskMain->GetHBITMAP(Color(0,0,0),&hBitmap);
			break;
		}
		
		// GDI+和GDI图片句柄转换
		pbm = CBitmap::FromHandle(hBitmap);
		m_wndImage.Add(pbm,RGB(0,0,0));

		// 对应添加图片
		HTREEITEM hItem=m_wndTree.InsertItem(iter->strName.c_str(),nImage,nImage);
		m_wndTree.SetItemData(hItem,nImage);

		nImage++;
		pbm->DeleteObject();
	}
}

void CDlgDriver::DisplayDiskChart(int nItem)
{
	m_wndChart.Reset();

	// 更新Volume信息
	if( m_vecVolume.at(nItem).dwType==DRIVE_CDROM )
	{
		// 如果是光驱则另行处理
		m_strDriveVolumeType=_T("分区格式: --");
		m_strDriveFree		=_T("空闲空间: --");
		m_strDriveUsage		=_T("使用空间: --");
	}
	else
	{
		m_strDriveVolumeType=(_T("分区格式: ")+m_vecVolume.at(nItem).strFileSystem).c_str();
		__int64 i64Free		=m_vecVolume.at(nItem).iFreeByte;
		__int64 i64Usage	=m_vecVolume.at(nItem).iTotalByte-i64Free;

		double nFreeGB	=i64Free/(double)1024/1024/1024;
		double nUsageGB=i64Usage/(double)1024/1024/1024;

		
		m_strDriveFree.Format(_T("空闲空间: %0.2fG"),nFreeGB);
		m_strDriveUsage.Format(_T("使用空间: %0.2fG"),nUsageGB);

		
		int nPercent=(360*(1-(double)m_vecVolume.at(nItem).iFreeByte/m_vecVolume.at(nItem).iTotalByte));

		TRACE1("%d\n",nPercent);
		m_wndChart.AddPiece(RGB(0,0,255),RGB(255,0,0),0,_T("空闲"));
		m_wndChart.AddPiece(RGB(0,200,200),RGB(255,0,0),nPercent,m_vecVolume.at(nItem).strName.c_str());
	}
	
	UpdateData(FALSE);
}

LRESULT CDlgDriver::OnSetPause(WPARAM wParam,LPARAM lParam)
{
	SetPause((BOOL)lParam);

	return 0;
}

LRESULT CDlgDriver::Update(WPARAM wParam,LPARAM lParam)
{
	InitDiskInfo();

	AddItem();

	InitPerformaceDisk();

	return 0;
}


void CDlgDriver::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;

	CGDIPlusDialog::OnSize(nType, cx, cy);
}

void CDlgDriver::OnDestroy()
{
	CGDIPlusDialog::OnDestroy();

	DestroyBitmap();

	// 释放线程资源
	if( m_pScanDiskThread!=NULL )
	{
		m_bPause=FALSE;
		m_bExit=TRUE;
		
		::WaitForSingleObject(m_pScanDiskThread->m_hThread,INFINITE);
		delete m_pScanDiskThread;
		m_pScanDiskThread=NULL;
	}

	// 释放性能计数器
	KillTimer(1);
	m_PerfMon.Uninitialize();
}

void CDlgDriver::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CMemDC memDC(&dc);

	CRect rcClient;
	GetClientRect(&rcClient);

	Graphics graphics(memDC.GetSafeHdc());
	memDC->FillSolidRect(rcClient, dc.GetBkColor());

	int nOffsetX=0,nOffsetY=0;
	nOffsetX=rcClient.CenterPoint().x-m_pImageBack->GetWidth()/2;
	nOffsetY=rcClient.CenterPoint().y-m_pImageBack->GetHeight()/2;

	// 绘制背景图
	graphics.DrawImage(m_pImageBack,nOffsetX,nOffsetY/2);

}

BOOL CDlgDriver::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return CGDIPlusDialog::OnEraseBkgnd(pDC);
}

void CDlgDriver::OnTvnSelchangedTreeDrive(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	HTREEITEM hItem=pNMTreeView->itemNew.hItem;
	int i=(int)m_wndTree.GetItemData(hItem);
	
	if( m_wndBarChart.m_hWnd!=NULL )
		m_wndBarChart.ShowWindow(SW_HIDE);
	if( m_wndChart.m_hWnd!=NULL )
		m_wndChart.ShowWindow(SW_SHOW);
	DisplayDiskChart(i);

	*pResult = 0;
}

HBRUSH CDlgDriver::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CGDIPlusDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*if( nCtlColor==CTLCOLOR_STATIC || nCtlColor==CTLCOLOR_EDIT )   
	{   
		pDC->SetBkMode(TRANSPARENT);   
		return   (HBRUSH)::GetStockObject(NULL_BRUSH);   
	} */

	return hbr;
}

void CDlgDriver::OnBnClickedButtonDiskView()
{
	// 隐藏Pie Chart
	m_wndChart.ShowWindow(SW_HIDE);

	if( m_wndBarChart.m_hWnd==NULL )
	{
		InitBarChart();

		SetTimer(5,1000,NULL);
	}
		
	// 移出所有
	m_wndBarChart.RemoveAll();
	m_PerfMon.RemoveAllCounter();
	n_nPerfCountItem=0;

	// 添加新
	int nCount=m_wndComboDisk.GetCount();
	for(int i=0; i<nCount; ++i)
	{
		if( m_wndComboDisk.GetCheck(i) )
		{
			CString strText=m_vecDiskPerf.at(i);
			m_wndBarChart.AddBar(100,strText,RGB(rand()%255,rand()%255,rand()%255));

			CString strCounter=_T("\\PhysicalDisk(_Total)\\")+strText;
			n_nPerfCountItem=m_PerfMon.AddCounter(strCounter);
		}
	}
	
	m_wndBarChart.Refresh();
	m_wndBarChart.ShowWindow(SW_SHOW);
}

void CDlgDriver::OnTimer(UINT_PTR nIDEvent)
{
	// collect the data
	if( !m_PerfMon.CollectQueryData() )
	{
		//AfxMessageBox(_T("访问性能计数器出错!"));
		return;
	}

	// update counters
	for(int i=0; i<n_nPerfCountItem; ++i)
	{
		long lVal = m_PerfMon.GetCounterValue(i);
		if( lVal<0 )
			lVal=0;

		m_wndBarChart.SetItemData(i,lVal);
	}
	
	m_wndBarChart.Refresh();

	CGDIPlusDialog::OnTimer(nIDEvent);
}
