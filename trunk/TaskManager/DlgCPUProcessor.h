#pragma once


// CDlgCPUProcessor �Ի���

class CDlgCPUProcessor : public CDialog
{
	DECLARE_DYNAMIC(CDlgCPUProcessor)

public:
	CDlgCPUProcessor(HANDLE hProcess,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgCPUProcessor();

// �Ի�������
	enum { IDD = IDD_DIALOG_CPU };

private:
	HANDLE	m_hProcess;							// ָ�����̾��

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();


public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonCpuOk();
	afx_msg void OnBnClickedButtonCpuCancel();
};
