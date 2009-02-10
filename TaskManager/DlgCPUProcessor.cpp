// DlgCPUProcessor.cpp : 实现文件
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgCPUProcessor.h"


// CDlgCPUProcessor 对话框

IMPLEMENT_DYNAMIC(CDlgCPUProcessor, CDialog)

CDlgCPUProcessor::CDlgCPUProcessor(HANDLE hProcess,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCPUProcessor::IDD, pParent)
	, m_hProcess(hProcess)
{

}

CDlgCPUProcessor::~CDlgCPUProcessor()
{
}

void CDlgCPUProcessor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgCPUProcessor, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CPU_OK, &CDlgCPUProcessor::OnBnClickedButtonCpuOk)
	ON_BN_CLICKED(IDC_BUTTON_CPU_CANCEL, &CDlgCPUProcessor::OnBnClickedButtonCpuCancel)
END_MESSAGE_MAP()


// CDlgCPUProcessor 消息处理程序

BOOL CDlgCPUProcessor::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 获取系统信息
	SYSTEM_INFO sysInfo={0};
	::GetSystemInfo(&sysInfo);

	// 获取CPU个数
	DWORD dwProcesserCount=sysInfo.dwNumberOfProcessors;

	DWORD dwProcessMask=0,dwSystemMask=0;

	::GetProcessAffinityMask(m_hProcess,&dwProcessMask,&dwSystemMask);

	// 初始化CheckBox
	for(DWORD i=0; i<8; ++i)
	{
		if( i<dwProcesserCount )
		{
			GetDlgItem(IDC_CHECK_CPU1+i)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_CHECK_CPU1+i)->EnableWindow(FALSE);
		}
	}

	// 只能工作在双核上
	switch(dwProcessMask)
	{
	case 1:
		CheckDlgButton(IDC_CHECK_CPU1+0,BST_CHECKED);
		break;
	case 2:
		CheckDlgButton(IDC_CHECK_CPU1+1,BST_CHECKED);
		break;

	case 3:
		CheckDlgButton(IDC_CHECK_CPU1+0,BST_CHECKED);
		CheckDlgButton(IDC_CHECK_CPU1+1,BST_CHECKED);
		break;

	default:
		AfxMessageBox(_T("抱歉,本代码只能在双核上运行!"));
		::DebugBreak();
		break;
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



void CDlgCPUProcessor::OnBnClickedButtonCpuOk()
{
	DWORD dwMask=0;
	int nCount=0;
	for(DWORD i=0; i<8; ++i)
	{
		if( ((CButton *)GetDlgItem(IDC_CHECK_CPU1+i))->GetCheck()==BST_CHECKED )
		{
			if( i==0 )
			{
				dwMask=1;
				nCount++;
			}
			else if( i==1 )
			{
				dwMask=2;
				nCount++;
			}
			
			if( nCount==2 )
				dwMask=3;
		}
	}

	if( dwMask==0)
	{
		MessageBox(_T("必须至少关联一个CPU!"),_T("错误"),MB_ICONERROR);
		return;
	}

	// 设置进程关联CPU
	::SetProcessAffinityMask(m_hProcess,dwMask);
	

	OnOK();
}

void CDlgCPUProcessor::OnBnClickedButtonCpuCancel()
{
	OnCancel();
}
