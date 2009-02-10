#pragma once
// CDlgApplication 对话框
#include "EnumTaskWindow.h"
#include "CriticalSection.h"


class CDlgApplication : public CDialog
{
	DECLARE_DYNAMIC(CDlgApplication)

public:
	CDlgApplication(CWnd* pParent = NULL);				// 标准构造函数
	virtual ~CDlgApplication();

// 对话框数据
	enum { IDD = IDD_DIALOG_APPLICATION };
	enum { IDT_APP_TIMER=1 };							// 定时器ID

private:
	CListCtrl			m_wndListApp;
	CImageList			m_iImageLarge;
	CImageList			m_iImageSmall;

	int					m_nMenuIndex;					// 没有选中的Menu选项
	int					m_nCurSelItem;					// 当前所选择项

	CEnumTaskWindow		m_enumTask;						// 枚举任务栏窗口
	vector<TaskWndInfo> m_vecTask;						// 任务栏窗口
	
	HWND				*m_pSelectedItem;				// 选择的任务栏窗口句柄
	int					m_nSelectedItemCount;			// 选择的任务栏窗口数

	CriticalSection		m_cs;							// 线程同步关键段

	CWinThread			*m_pScanTaskThread;				// 扫描Task Window线程
	BOOL				m_bExit;						// 退出线程标记
	BOOL				m_bPause;						// 暂停标志

public:
	BOOL				SetPause(BOOL bPause);			// 设置是否暂停

private:
	void				InitListApp();					// 初始化List
	void				PopupMenu(LPNMLISTVIEW pListView,UINT uID);			// 显示菜单
	void				InsertAppItem();				// 添加Task Window项
	void				DeleteSelItem();				// 删除选择项

	BOOL				TestStates(HWND hWnd);			// 检测是否Active

	static				DWORD WINAPI ScanTaskThread(LPVOID pVoid);	// 扫描Task Window线程函数

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
