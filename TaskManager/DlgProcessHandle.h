#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "NtSystemInfo.h"

using namespace NT;
// CDlgProcessHandle �Ի���

typedef struct tagHandleColInfo
{
	CString		strColName;		// Colnum����
	UINT		uWidth;			// Colnum���
}HandleColInfo;

class CDlgProcessHandle : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessHandle)

public:
	CDlgProcessHandle(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcessHandle();

// �Ի�������
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_HANDLE };

private:
	CSortListCtrl	m_wndListHandle;
	CFont			m_font;

	int				m_nProcessID;						// ��ǰѡ�н���ID
	CWinThread		*m_pHandleThread;						// ��ȡ�ڴ���Ϣ�߳�

	static const HandleColInfo m_HandleColInfo[];		// ���List��ʾ��Ϣ

private:
	void	InitList();									// ��ʼ��ListCtrl
	void	InsertItem();								// ��������

	vector<SystemHandleInformation::HANDLE_INFORMATION> m_vecHandleInfo;				// �����Ϣ
	static DWORD WINAPI HandleInfoThread(LPVOID pVoid);	// ��������߳�

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnLoadHandleInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushHandleInfo(WPARAM wParam,LPARAM lParam);
};
