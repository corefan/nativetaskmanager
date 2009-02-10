// DlgProcessInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessInfo.h"


// CDlgProcessInfo 对话框

IMPLEMENT_DYNAMIC(CDlgProcessInfo, CDialog)

CDlgProcessInfo::CDlgProcessInfo(CString strName,int nPID,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessInfo::IDD, pParent)
	, m_strProcessName(strName)
	, m_nPID(nPID)
	, m_hProcess(NULL)
	, m_nCPUUsage(0)
	, m_nMemoryUsage(0)
	, m_nIOUsage(0)
	, m_nCPUUsageKernel(0)
	, m_nIOMaxRange(100)
	, m_nMemoryMaxRange(50)
	, m_strCPUMin(_T(""))
	, m_strCPUAvg(_T(""))
	, m_strCPUMax(_T(""))
	, m_strCPUVal(_T(""))
	, m_strCPUMinKernel(_T(""))
	, m_strCPUAvgKernel(_T(""))
	, m_strCPUMaxKernel(_T(""))
	, m_strCPUValKernel(_T(""))
{

}

CDlgProcessInfo::~CDlgProcessInfo()
{
	if( m_hProcess!=NULL )
		::CloseHandle(m_hProcess);

	TRACE("~CDlgProcessInfo\n");
}

void CDlgProcessInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_PROCESS_CPU, m_wndCPUUsage);
	DDX_Control(pDX, IDC_PROGRESS_PROCESS_CPU_KERNEL, m_wndCPUUsageKernel);
	DDX_Text(pDX, IDC_STATIC_PROCESS_CPU_MIN, m_strCPUMin);
	DDX_Text(pDX, IDC_STATIC_PROCESS_CPU_AVG, m_strCPUAvg);
	DDX_Text(pDX, IDC_STATIC_PROCESS_CPU_MAX, m_strCPUMax);
	DDX_Text(pDX, IDC_STATIC_PROCESS_CPU_VAL, m_strCPUVal);
	DDX_Text(pDX, IDC_STATIC_PROCESS_CPU_MIN_KERNEL, m_strCPUMinKernel);
	DDX_Text(pDX, IDC_STATIC_PROCESS_CPU_AVG_KERNEL, m_strCPUAvgKernel);
	DDX_Text(pDX, IDC_STATIC_PROCESS_CPU_MAX_KERNEL, m_strCPUMaxKernel);
	DDX_Text(pDX, IDC_STATIC_PROCESS_CPU_VAL_KERNEL, m_strCPUValKernel);
}


BEGIN_MESSAGE_MAP(CDlgProcessInfo, CDialog)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgProcessInfo 消息处理程序

BOOL CDlgProcessInfo::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_strProcessName+_T(" 信息"));

	InitCounter();
	InitCtrl();

	// init timer
	SetTimer(1, 1000, NULL); // 刷新

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgProcessInfo::InitCounter()
{
	if( !m_PerfMon.Initialize() )
	{
		AfxMessageBox(_T("初始化性能计数器失败!"));
		return;
	}
	
	// 添加指定进程CPU使用率
	CString	strCPUUsage;
	CString strName=m_strProcessName.Left(m_strProcessName.GetLength()-4);

	strCPUUsage=_T("\\Process(")+strName+_T(")\\% Processor Time");
	m_nCPUUsage=m_PerfMon.AddCounter(strCPUUsage);

	CString	strCPUUsageKernel;
	strName=m_strProcessName.Left(m_strProcessName.GetLength()-4);

	strCPUUsageKernel=_T("\\Process(")+strName+_T(")\\% Privileged Time");
	m_nCPUUsageKernel=m_PerfMon.AddCounter(strCPUUsageKernel);

	
	// 添加指定进程私有字节使用率
	CString	strMemoryUsage;

	strMemoryUsage=_T("\\Process(")+strName+_T(")\\Private Bytes");
	m_nMemoryUsage=m_PerfMon.AddCounter(strMemoryUsage);

	// 添加制定进程IO字节使用率
	CString	strIOUsage;

	strIOUsage=_T("\\Process(")+strName+_T(")\\IO Data Operations/sec");
	m_nIOUsage=m_PerfMon.AddCounter(strIOUsage);

	m_nIOReadUsage	=m_nIOUsage+1;
	m_nIOWriteUsage	=m_nIOUsage+2;
	m_nIOOtherUsage	=m_nIOUsage+3;


	/*strIOUsage=_T("\\Process(")+strName+_T(")\\IO Read Operations/sec");
	m_nIOReadUsage=m_PerfMon.AddCounter(strIOUsage);

	strIOUsage=_T("\\Process(")+strName+_T(")\\IO Write Operations/sec");
	m_nIOWriteUsage=m_PerfMon.AddCounter(strIOUsage);

	strIOUsage=_T("\\Process(")+strName+_T(")\\IO Other Operations/sec");
	m_nIOOtherUsage=m_PerfMon.AddCounter(strIOUsage);*/
	
	m_hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,m_nPID);
}

void CDlgProcessInfo::InitCtrl()
{
	// 设置CPU Progress
	m_wndCPUUsage.SetRange32(0,100);
	m_wndCPUUsage.SetPos(0);
	m_wndCPUUsageKernel.SetRange32(0,100);
	m_wndCPUUsageKernel.SetPos(0);

	// 设置CPU graph
	CRect rcCPU;
	GetDlgItem(IDC_STATIC_CPU)->GetWindowRect(&rcCPU);
	ScreenToClient(&rcCPU);
	rcCPU.top+=15;
	rcCPU.bottom-=5;
	rcCPU.right-=5;
	rcCPU.left+=5;

	if( !m_wndCPUCtrl.Create(WS_CHILD | WS_VISIBLE,rcCPU, this, 10000) )
	{
		TRACE0("Create CPU Usage Graph Failed!");
	}

	m_wndCPUCtrl.SetGridCount(100);
	m_wndCPUCtrl.SetStringExtend(_T("%"));


	// 设置私有字节 Graph
	CRect rcMemory;
	GetDlgItem(IDC_STATIC_MEMORY)->GetWindowRect(&rcMemory);
	ScreenToClient(&rcMemory);
	rcMemory.top+=15;
	rcMemory.bottom-=5;
	rcMemory.right-=5;
	rcMemory.left+=5;

	if( !m_wndMemoryCtrl.Create(WS_CHILD | WS_VISIBLE,rcMemory, this, 10001) )
	{
		TRACE0("Create Memory Usage Graph Failed!");
	}

	//m_wndMemoryCtrl.SetGridCount(100);
	m_wndMemoryCtrl.SetStringExtend(_T("MB"));

	// 设置私有字节图表
	m_wndMemoryGraph.CreateFromStatic(IDC_STATIC_MEMORY_GRAPH, this);	
	m_wndMemoryGraph.ModifyStyle(0, WS_THICKFRAME);	
	
	m_wndMemoryGraph.AddLine(m_nMemoryUsage,  RGB(255, 0, 0));
	m_wndMemoryGraph.ShowGrid(TRUE);
	m_wndMemoryGraph.ShowLabels(FALSE);
	m_wndMemoryGraph.SetInterval(5);
	m_wndMemoryGraph.SetPeekRange(0, m_nMemoryMaxRange);
	m_wndMemoryGraph.RedrawWindow();


	// 设置IO Graph
	CRect rcIO;
	GetDlgItem(IDC_STATIC_IO)->GetWindowRect(&rcIO);
	ScreenToClient(&rcIO);
	rcIO.top+=15;
	rcIO.bottom-=5;
	rcIO.right-=5;
	rcIO.left+=5;

	if( !m_wndIOCtrl.Create(WS_CHILD | WS_VISIBLE,rcIO, this, 10002) )
	{
		TRACE0("Create IO Usage Graph Failed!");
	}

	m_wndIOCtrl.SetStringExtend(_T("KB"));

	// 设置IO读写字节图表
	m_wndIOGraph.CreateFromStatic(IDC_STATIC_IO_GRAPH, this);	
	m_wndIOGraph.ModifyStyle(0, WS_THICKFRAME);	
	
	m_wndIOGraph.AddLine(m_nIOReadUsage,   RGB(255, 255, 0));
	m_wndIOGraph.AddLine(m_nIOWriteUsage,  RGB(0, 255, 255));
	m_wndIOGraph.AddLine(m_nIOOtherUsage,  RGB(255, 0, 255));

	m_wndIOGraph.ShowGrid(TRUE);
	m_wndIOGraph.ShowLabels(TRUE);
	m_wndIOGraph.SetLabelForMax(_T("100KB"));
	m_wndIOGraph.SetLabelForMin(_T(""));
	m_wndIOGraph.SetInterval(5);
	m_wndIOGraph.SetPeekRange(0, m_nIOMaxRange);
	m_wndIOGraph.RedrawWindow();
}

void CDlgProcessInfo::OnDestroy()
{
	KillTimer(1);

	m_PerfMon.Uninitialize();

	m_wndMemoryCtrl.DestroyWindow();
	m_wndCPUCtrl.DestroyWindow();													
	m_wndIOCtrl.DestroyWindow();

	m_wndMemoryGraph.DestroyWindow();
	m_wndIOGraph.DestroyWindow();


	CDialog::OnDestroy();

	
}

void CDlgProcessInfo::OnTimer(UINT_PTR nIDEvent)
{
	UpdateCPUUsage();
	UpdateMemoryUsage();
	UpdateIOUsage();


	UpdateData(FALSE);

	CDialog::OnTimer(nIDEvent);
}


void CDlgProcessInfo::UpdateCPUUsage()
{
	// collect the data
	if( !m_PerfMon.CollectQueryData() )
	{
		//AfxMessageBox(_T("访问性能计数器出错!"));
		KillTimer(1);
		
		//OnCancel();
	}

	// update counters
	long lcpu = m_PerfMon.GetCounterValue(m_nCPUUsage);

	// update dialog
	if( lcpu != -999 )
	{
		m_strCPUVal.Format(_T("%d%%"), lcpu);
		m_wndCPUUsage.SetPos(lcpu);
		long lMin, lMax, lMean;
		if( m_PerfMon.GetStatistics(&lMin, &lMax, &lMean, m_nCPUUsage) )
		{
			m_strCPUMin.Format(_T("%d"), lMin);
			m_strCPUMax.Format(_T("%d"), lMax);
			m_strCPUAvg.Format(_T("%d"), lMean);
		}
		else
		{
			m_strCPUMin = _T("N/A");
			m_strCPUMax = _T("N/A");
			m_strCPUAvg = _T("N/A");
		}

		m_wndCPUCtrl.SetGridHeight(lcpu);
		m_wndCPUCtrl.Refresh();
	}
	else
	{
		m_wndCPUUsage.SetPos(0);
		m_strCPUVal = _T("FAIL");
	}

	// update counters
	long lcpuKernel = m_PerfMon.GetCounterValue(m_nCPUUsageKernel);

	// update dialog
	if( lcpuKernel != -999 )
	{
		m_strCPUValKernel.Format(_T("%d%%"), lcpuKernel);
		m_wndCPUUsageKernel.SetPos(lcpuKernel);
		long lMinKernel, lMaxKernel, lMeanKernel;
		if( m_PerfMon.GetStatistics(&lMinKernel, &lMaxKernel, &lMeanKernel, m_nCPUUsageKernel) )
		{
			m_strCPUMinKernel.Format(_T("%d"), lMinKernel);
			m_strCPUMaxKernel.Format(_T("%d"), lMaxKernel);
			m_strCPUAvgKernel.Format(_T("%d"), lMeanKernel);
		}
		else
		{
			m_strCPUMinKernel = _T("N/A");
			m_strCPUMaxKernel = _T("N/A");
			m_strCPUAvgKernel = _T("N/A");
		}
	}
	else
	{
		m_wndCPUUsageKernel.SetPos(0);
		m_strCPUValKernel = _T("FAIL");
	}
}

void CDlgProcessInfo::UpdateMemoryUsage()
{
	// collect the data
	if( !m_PerfMon.CollectQueryData() )
	{
		//AfxMessageBox(_T("访问性能计数器出错!"));
		KillTimer(1);
		
		//OnCancel();
	}

	// update counters
	long lMemory = m_PerfMon.GetCounterValue(m_nMemoryUsage);

	// update dialog
	if( lMemory != -999 )
	{
		m_wndMemoryCtrl.SetGridCount((double)((lMemory>>20)*4)/3);
		m_wndMemoryCtrl.SetGridHeight((double)lMemory/1024/1024);
		m_wndMemoryCtrl.Refresh();

		m_wndMemoryGraph.Push(lMemory>>20,  m_nMemoryUsage);
		if( m_nMemoryMaxRange<lMemory>>20 )
		{
			m_nMemoryMaxRange+=100;
			m_wndMemoryGraph.SetMaxPeek(m_nMemoryMaxRange);
			m_wndMemoryGraph.RedrawWindow();
			return ;
		}

		m_wndMemoryGraph.Update();
	}
	else
	{
		
	}
}

void CDlgProcessInfo::UpdateIOUsage()
{
	// collect the data
	if( !m_PerfMon.CollectQueryData() )
	{
		//AfxMessageBox(_T("访问性能计数器出错!"));
		KillTimer(1);
		
		//OnCancel();
	}

	// update counters
	long lIO = m_PerfMon.GetCounterValue(m_nIOUsage);
	/*long lIORead = m_PerfMon.GetCounterValue(m_nIOReadUsage);
	long lIOWrite = m_PerfMon.GetCounterValue(m_nIOWriteUsage);
	long lIOOther = m_PerfMon.GetCounterValue(m_nIOOtherUsage);*/

	// update dialog
	if( lIO != -999 /*&& lIORead != -999 && lIOWrite != -999 && lIOOther != -999*/ )
	{
		m_wndIOCtrl.SetGridCount((double)((lIO)*4)/3);
		m_wndIOCtrl.SetGridHeight((double)lIO);
		m_wndIOCtrl.Refresh();

		if( m_hProcess!=NULL )
		{
			IO_COUNTERS ioInfo={0};
			::GetProcessIoCounters(m_hProcess,&ioInfo);
			
			int nReadByte	=ioInfo.ReadTransferCount>>10;
			int nWriteByte	=ioInfo.WriteTransferCount>>10;
			int nOtherByte	=ioInfo.OtherTransferCount>>10;

			m_wndIOGraph.Push(nReadByte,   m_nIOReadUsage);
			m_wndIOGraph.Push(nWriteByte,  m_nIOWriteUsage);
			m_wndIOGraph.Push(nOtherByte,  m_nIOOtherUsage);

			if( m_nIOMaxRange<max(nReadByte,max(nWriteByte,nOtherByte)) )
			{
				m_nIOMaxRange+=100;
				m_wndIOGraph.SetMaxPeek(m_nIOMaxRange);

				CString strMaxLable;
				strMaxLable.Format(_T("%d KB"),m_nIOMaxRange);
				m_wndIOGraph.SetLabelForMax(strMaxLable);
				m_wndIOGraph.RedrawWindow();
				return ;
			}

			m_wndIOGraph.Update();
		}
	}
	else
	{
		
	}
}


void CDlgProcessInfo::OnOK()
{
	//CDialog::OnOK();
	DestroyWindow();
}

void CDlgProcessInfo::OnCancel()
{
	//CDialog::OnCancel();
	DestroyWindow();
}

void CDlgProcessInfo::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	delete this;
}
