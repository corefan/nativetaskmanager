#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "CriticalSection.h"
#include "EnumPort.h"

using namespace enumPort;
// CDlgProcessPort �Ի���
typedef struct tagPortColInfo
{
	CString		strColName;		// Colnum����
	UINT		uWidth;			// Colnum���
}PortColInfo;

class CDlgProcessPort : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessPort)

public:
	CDlgProcessPort(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcessPort();

// �Ի�������
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_PORT };

private:
	CSortListCtrl	m_wndListPort;						// List Ctrl	
		
	CFont			m_font;								// the Font of List 
	CriticalSection	m_cs;								// �ؼ���

	int				m_nProcessID;						// ��ǰѡ�н���ID
	CWinThread		*m_pPortThread;						// ��ȡ�˿���Ϣ�߳�

	vector<PortInfo> m_vecPortInfo;					// �˿���Ϣ

	static const PortColInfo m_PortColInfo[];			// ģ��List��ʾ��Ϣ

private:
	void	InitList();									// ��ʼ��ListCtrl
	void	InsertItem();								// ��������
	
	static DWORD WINAPI PortInfoThread(LPVOID pVoid);	// �˿ڹ����߳�

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void PreSubclassWindow();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	afx_msg LRESULT OnLoadPortInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushPortInfo(WPARAM wParam,LPARAM lParam);
};
