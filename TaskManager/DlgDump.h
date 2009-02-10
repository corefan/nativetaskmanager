#pragma once
#include "afxwin.h"


// CDlgDump 对话框

class CDlgDump : public CDialog
{
	DECLARE_DYNAMIC(CDlgDump)

public:
	CDlgDump(CString strModulePath,CString strModuleName,CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDump();

// 对话框数据
	enum { IDD = IDD_DIALOG_DUMP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	
private:
	void PrintText(CString &strCommandLine);			// 重定向输出

private:
	CString	m_strDumpText;								// 信息显示
	
	CString	m_strModulePath;							// 模块路径
	CString	m_strModuleName;							// 模块名称

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonDumpOk();
	afx_msg void OnBnClickedButtonDumpCancel();
	virtual BOOL OnInitDialog();
};
