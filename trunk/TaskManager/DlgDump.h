#pragma once
#include "afxwin.h"


// CDlgDump �Ի���

class CDlgDump : public CDialog
{
	DECLARE_DYNAMIC(CDlgDump)

public:
	CDlgDump(CString strModulePath,CString strModuleName,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDump();

// �Ի�������
	enum { IDD = IDD_DIALOG_DUMP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	
private:
	void PrintText(CString &strCommandLine);			// �ض������

private:
	CString	m_strDumpText;								// ��Ϣ��ʾ
	
	CString	m_strModulePath;							// ģ��·��
	CString	m_strModuleName;							// ģ������

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonDumpOk();
	afx_msg void OnBnClickedButtonDumpCancel();
	virtual BOOL OnInitDialog();
};
