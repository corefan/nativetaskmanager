#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "EnumThread.h"

//using namespace enumThread;
using enumThread::ThreadInfo;


// CDlgProcessThread 对话框
typedef struct tagThreadColInfo
{
	CString	strColName;
	UINT	uMFT;			// 对齐方式
	UINT	uWidth;			// 宽度
}ThreadColInfo;

class CDlgProcessThread : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessThread)

public:
	CDlgProcessThread(CWnd* pParent = NULL);			// 标准构造函数
	virtual ~CDlgProcessThread();

// 对话框数据
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_THREAD };

private:
	CSortListCtrl		m_wndListThread;
	CFont				m_font;

	static const		ThreadColInfo m_ThreadCol[];	// 线程栏信息
	vector<ThreadInfo>	m_vecThreadInfo;				// 线程信息

	int					m_nProcessID;					// 当前选中进程ID
	CWinThread			*m_pThreadThread;				// 获取线程信息线程

	int					m_nCurItem;						// 当前选中项

private:
	void	InitList();									// 初始化List
	void	InsertItem();								// 插入数据
	void	PopupMenu();								// 弹出菜单
	void	SetMenuState(CMenu *pTrackMenu);			// 设置菜单状态

	static DWORD WINAPI ThreadInfoThread(LPVOID pVoid); 
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
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
