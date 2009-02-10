#pragma once


// CDlgCPUProcessor 对话框

class CDlgCPUProcessor : public CDialog
{
	DECLARE_DYNAMIC(CDlgCPUProcessor)

public:
	CDlgCPUProcessor(HANDLE hProcess,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgCPUProcessor();

// 对话框数据
	enum { IDD = IDD_DIALOG_CPU };

private:
	HANDLE	m_hProcess;							// 指定进程句柄

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();


public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonCpuOk();
	afx_msg void OnBnClickedButtonCpuCancel();
};
