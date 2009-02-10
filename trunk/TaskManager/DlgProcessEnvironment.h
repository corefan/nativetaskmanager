#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "ProcessEnvReader.h"
// CDlgProcessEnvironment �Ի���

using namespace enumEnvironment;

typedef struct tagEnvColInfo
{
	CString		strColName;		// Colnum����
	UINT		uWidth;			// Colnum���
}EnvColInfo;


class CDlgProcessEnvironment : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessEnvironment)

public:
	CDlgProcessEnvironment(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcessEnvironment();

// �Ի�������
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_ENVIRONMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

private:
	CSortListCtrl	m_wndListEnv;
	CFont			m_font;

	int				m_nProcessID;						// ��ǰѡ�н���ID
	CWinThread		*m_pEnvThread;						// ��ȡ�ڴ���Ϣ�߳�

	EnvVarValArray	m_vecEnv;							// ����������Ϣ
	static const EnvColInfo m_EnvColInfo[];				// ����List��ʾ��Ϣ

private:
	void	InitList();									// ��ʼ��ListCtrl
	void	InsertItem();								// ��������

	
	static DWORD WINAPI EnvInfoThread(LPVOID pVoid);	// �������������߳�
public:
	DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnLoadEnvInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushEnvInfo(WPARAM wParam,LPARAM lParam);
};
