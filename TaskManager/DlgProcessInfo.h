#pragma once
#include "perfmon.h"
#include "UsagesCtl.h"
#include "2DPushGraph.h"
#include "afxcmn.h"

// CDlgProcessInfo �Ի���
using namespace PDH;

class CDlgProcessInfo : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessInfo)

public:
	CDlgProcessInfo(CString strName,int nPID,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcessInfo();

// �Ի�������
	enum { IDD = IDD_DIALOG_PROCESS_INFORMATION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual void PostNcDestroy();

	
private:
	void	InitCounter();								// ��ʼ�����ܼ�����
	void	InitCtrl();									// ��ʼ��ÿ���ؼ�

	void	UpdateCPUUsage();							// ����CPUʹ����
	void	UpdateMemoryUsage();						// ����˽���ֽ�ʹ����
	void	UpdateIOUsage();							// ����IOʹ����

private:
	CString		m_strProcessName;						// ��������
	int			m_nPID;									// ����ID
	HANDLE		m_hProcess;								// ���̾��

	CPerfMon	m_PerfMon;								// ���ܼ�����

	CUsageCtrl	m_wndCPUCtrl;							// CPUʹ����ͼ
	CUsageCtrl	m_wndMemoryCtrl;						// ˽���ֽ�ʹ����ͼ
	CUsageCtrl	m_wndIOCtrl;							// IOʹ����ͼ

	C2DPushGraph m_wndMemoryGraph;						// ˽���ֽ�ʹ����ͼ��
	C2DPushGraph m_wndIOGraph;							// IO�ֽ�ʹ����ͼ��


	int			m_nCPUUsage;							// CPUʹ���ʼ�����ID
	int			m_nCPUUsageKernel;						// CPU Kernelʹ���ʼ�����
	int			m_nMemoryUsage;							// ˽���ֽ�ʹ���ʼ�����ID
	int			m_nIOUsage;								// IO�ֽ�ʹ���ʼ�����ID
	int			m_nIOReadUsage;
	int			m_nIOWriteUsage;
	int			m_nIOOtherUsage;

	int			m_nMemoryMaxRange;						// ˽���ֽ����ֵ
	int			m_nIOMaxRange;							// IO�ֽ����ֵ
	
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
