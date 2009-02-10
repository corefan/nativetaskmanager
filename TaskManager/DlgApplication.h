#pragma once
// CDlgApplication �Ի���
#include "EnumTaskWindow.h"
#include "CriticalSection.h"


class CDlgApplication : public CDialog
{
	DECLARE_DYNAMIC(CDlgApplication)

public:
	CDlgApplication(CWnd* pParent = NULL);				// ��׼���캯��
	virtual ~CDlgApplication();

// �Ի�������
	enum { IDD = IDD_DIALOG_APPLICATION };
	enum { IDT_APP_TIMER=1 };							// ��ʱ��ID

private:
	CListCtrl			m_wndListApp;
	CImageList			m_iImageLarge;
	CImageList			m_iImageSmall;

	int					m_nMenuIndex;					// û��ѡ�е�Menuѡ��
	int					m_nCurSelItem;					// ��ǰ��ѡ����

	CEnumTaskWindow		m_enumTask;						// ö������������
	vector<TaskWndInfo> m_vecTask;						// ����������
	
	HWND				*m_pSelectedItem;				// ѡ������������ھ��
	int					m_nSelectedItemCount;			// ѡ���������������

	CriticalSection		m_cs;							// �߳�ͬ���ؼ���

	CWinThread			*m_pScanTaskThread;				// ɨ��Task Window�߳�
	BOOL				m_bExit;						// �˳��̱߳��
	BOOL				m_bPause;						// ��ͣ��־

public:
	BOOL				SetPause(BOOL bPause);			// �����Ƿ���ͣ

private:
	void				InitListApp();					// ��ʼ��List
	void				PopupMenu(LPNMLISTVIEW pListView,UINT uID);			// ��ʾ�˵�
	void				InsertAppItem();				// ���Task Window��
	void				DeleteSelItem();				// ɾ��ѡ����

	BOOL				TestStates(HWND hWnd);			// ����Ƿ�Active

	static				DWORD WINAPI ScanTaskThread(LPVOID pVoid);	// ɨ��Task Window�̺߳���

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();

public:
	DECLARE_EASYSIZE 

	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnApplicationNewTask();
	afx_msg void OnApplicationLarge();
	afx_msg void OnApplicationSmall();
	afx_msg void OnApplicationDetail();
	afx_msg void OnApplicationChange();
	afx_msg void OnApplicationTop();
	afx_msg void OnApplicationMin();
	afx_msg void OnApplicationMax();
	afx_msg void OnApplicationCascade();
	afx_msg void OnApplicationLandscape();
	afx_msg void OnApplicationPortrait();
	afx_msg void OnApplicationTerminate();
	afx_msg void OnApplication();

	afx_msg void OnUpdateApplicationLarge(CCmdUI *pCmdUI);
	afx_msg void OnUpdateApplicationSmall(CCmdUI *pCmdUI);
	afx_msg void OnUpdateApplicationDetail(CCmdUI *pCmdUI);

	afx_msg void OnNMRclickListApplication(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListApplication(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnBnClickedButtonChangeTask();
	afx_msg void OnBnClickedButtonTerminateTask();
	afx_msg void OnBnClickedButtonNewTask();
	

	afx_msg LRESULT DeleteTask(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT AddTask(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT UpdateList(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnSetPause(WPARAM wParam,LPARAM lParam);
};
