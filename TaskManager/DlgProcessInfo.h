#pragma once
#include "perfmon.h"
#include "UsagesCtl.h"
#include "2DPushGraph.h"
#include "afxcmn.h"

// CDlgProcessInfo 对话框
using namespace PDH;

class CDlgProcessInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessInfo)

public:
	CDlgProcessInfo(CString strName,int nPID,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcessInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG_PROCESS_INFORMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();

	
private:
	void	InitCounter();								// 初始化性能计数器
	void	InitCtrl();									// 初始化每个控件

	void	UpdateCPUUsage();							// 更新CPU使用率
	void	UpdateMemoryUsage();						// 更新私有字节使用率
	void	UpdateIOUsage();							// 更新IO使用率

private:
	CString		m_strProcessName;						// 进程名称
	int			m_nPID;									// 进程ID
	HANDLE		m_hProcess;								// 进程句柄

	CPerfMon	m_PerfMon;								// 性能计数器

	CUsageCtrl	m_wndCPUCtrl;							// CPU使用率图
	CUsageCtrl	m_wndMemoryCtrl;						// 私有字节使用率图
	CUsageCtrl	m_wndIOCtrl;							// IO使用率图

	C2DPushGraph m_wndMemoryGraph;						// 私有字节使用率图表
	C2DPushGraph m_wndIOGraph;							// IO字节使用率图表


	int			m_nCPUUsage;							// CPU使用率计数器ID
	int			m_nCPUUsageKernel;						// CPU Kernel使用率计数器
	int			m_nMemoryUsage;							// 私有字节使用率计数器ID
	int			m_nIOUsage;								// IO字节使用率计数器ID
	int			m_nIOReadUsage;
	int			m_nIOWriteUsage;
	int			m_nIOOtherUsage;

	int			m_nMemoryMaxRange;						// 私有字节最大值
	int			m_nIOMaxRange;							// IO字节最大值
	
private:
	CProgressCtrl		m_wndCPUUsage;			
	CProgressCtrl		m_wndCPUUsageKernel;

	CString				m_strCPUMin;
	CString				m_strCPUAvg;
	CString				m_strCPUMax;
	CString				m_strCPUVal;

	CString				m_strCPUMinKernel;
	CString				m_strCPUAvgKernel;
	CString				m_strCPUMaxKernel;
	CString				m_strCPUValKernel;


public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

};
