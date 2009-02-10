#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "EnumThread.h"

//using namespace enumThread;
using enumThread::ThreadInfo;


// CDlgProcessThread �Ի���
typedef struct tagThreadColInfo
{
	CString	strColName;
	UINT	uMFT;			// ���뷽ʽ
	UINT	uWidth;			// ���
}ThreadColInfo;

class CDlgProcessThread : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessThread)

public:
	CDlgProcessThread(CWnd* pParent = NULL);			// ��׼���캯��
	virtual ~CDlgProcessThread();

// �Ի�������
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_THREAD };

private:
	CSortListCtrl		m_wndListThread;
	CFont				m_font;

	static const		ThreadColInfo m_ThreadCol[];	// �߳�����Ϣ
	vector<ThreadInfo>	m_vecThreadInfo;				// �߳���Ϣ

	int					m_nProcessID;					// ��ǰѡ�н���ID
	CWinThread			*m_pThreadThread;				// ��ȡ�߳���Ϣ�߳�

	int					m_nCurItem;						// ��ǰѡ����

private:
	void	InitList();									// ��ʼ��List
	void	InsertItem();								// ��������
	void	PopupMenu();								// �����˵�
	void	SetMenuState(CMenu *pTrackMenu);			// ���ò˵�״̬

	static DWORD WINAPI ThreadInfoThread(LPVOID pVoid); 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);

	afx_msg void OnThreadSuspend();
	afx_msg void OnThreadResume();
	afx_msg void OnThreadTerminate();

	afx_msg void OnMenu(UINT uID);


	afx_msg LRESULT OnLoadThreadInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushThreadInfo(WPARAM wParam,LPARAM lParam);
	
	afx_msg void OnNMRclickListProcessTabThread(NMHDR *pNMHDR, LRESULT *pResult);
};
