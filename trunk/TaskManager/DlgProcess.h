#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "EnumProcess.h"
#include "XTabCtrl.h"
#include "xySplitterWnd.h"
#include "CriticalSection.h"

#include "DlgProcessTabFile.h"
#include "DlgProcessModule.h"
#include "DlgProcessThread.h"
#include "DlgProcessMemory.h"
#include "DlgProcessPort.h"
#include "DlgProcessEnvironment.h"
#include "DlgProcessHandle.h"



using namespace enumProcess;
// CDlgProcess 对话框
#define MAX_PROCESS_COLUM	27		// Number of Process Colum 

enum COLUMNID
{
    COL_IMAGENAME           = 0,
    COL_PID                 = 1,
	COL_CPU                 = 2,
    COL_CPUTIME             = 3,

	COL_MEMUSAGE            = 4,
    COL_MEMDELTA            = 5,
	COL_MEMPEAKSAGE			= 6,

    COL_PAGEFAULTS			= 7,
	COL_PAGEFAULTSDELTA		= 8,
    COL_PAGEPOOL           = 9,
	COL_NONPAGEPOOL        = 10,

	COL_USEROBJECTS         = 11,
	COL_GDIOBJECTS          = 12,

	COL_SESSIONID           = 13,
    COL_USERNAME            = 14,
	COL_BASEPRIORITY        = 15,

	COL_HANDLECOUNT         = 16,
    COL_THREADCOUNT         = 17,
    
    COL_IOREADS				= 18,
	COL_IOREADBYTES			= 19,
	COL_IOWRITES			= 20,
	COL_IOWRITEBYTES		= 21,
	COL_IOOTHER				= 22,
	COL_IOOTHERBYTES		= 23,
    
	COL_PEEKMEMORYUSAGE		= 24,
	COL_VIRTUALMEMORYSIZE	= 25,
	COL_IMAGEPATH			= 26
};


// 进程浏览ListCtrl信息
typedef struct tagProcessCol
{
	UINT	uID;			// 
	UINT	uWidth;			// 宽度
	UINT	uMFT;			// 对齐方式
	BOOL	bSelected;		// 是否选择
	CString strColName;		// column名
}ProcessCol;


class CDlgProcess : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcess)

public:
	CDlgProcess(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcess();

// 对话框数据
	enum { IDD = IDD_DIALOG_PROCESS };

private:
	CListCtrl			m_wndListProcess;
	CXTabCtrl			m_wndTabProcess;
	CImageList			m_iImageSmall;
	CySplitterWnd		m_wndSplitterY;

	CWinThread			*m_pThreadProcess;			// 遍历进程线程句柄
	CEvent				*m_pUpdateEvent;			// 开始更新UI事件
	BOOL				m_bExit;					// 进程线程退出标志
	BOOL				m_bPause;					// 线程暂停标志

	int					m_nCurItem;					// 当前选中项
	int					m_nCurSelCol;				// 当前选中排序的项

	CDlgProcessTabFile	m_wndDlgProcessFile;		// 文件信息Dlg
	CDlgProcessModule	m_wndDlgProcessModule;		// 模块信息Dlg
	CDlgProcessThread	m_wndDlgProcessThread;		// 线程信息Dlg
	CDlgProcessMemory	m_wndDlgProcessMemory;		// 内存信息Dlg
	CDlgProcessPort		m_wndDlgProcessPort;		// 端口信息Dlg
	CDlgProcessEnvironment m_wndDlgProcessEnv;		// 环境变量信息Dlg
	CDlgProcessHandle	m_wndDlgProcessHandle;		// 端口信息Dlg
	
	TvecProInfo			m_vecProcessInfo;			// 进程容器
	EnumProcess			m_enumProcess;				// 进程枚举

	CriticalSection		m_cs;						// 关键段加锁
	ProcessCol			m_proCol[MAX_PROCESS_COLUM];// 进程Column信息

	vector<TabInfo>		m_vecTabInfo;				// 所包含子Dlg容器
public:
	BOOL	SetPause(BOOL bPause);					// 设置是否暂停

private:
	void	InitListProcess();						// 初始化ListCtrl
	void	InitTab();								// 初始化TabCtrl
	void	InsertItem();							// 插入数据

	void	UpdateImageList();						// 更新ImageList

	void	SuspendProcess(DWORD dwProcessID, BOOL fSuspend);//挂起进程
	void	PopupMenu();							// 弹出菜单
	void	SetMenuState(CMenu *pTrackMenu);		// 设置菜单状态
	
	static bool	Compare(const ProcessInfo &lv,const ProcessInfo &rv);// 排序
	static DWORD ScanProcessThread(LPVOID pVoid);	// 遍历进程线程

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);

	afx_msg LRESULT OnUpdateProcessList(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetPause(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetSize(WPARAM wParam,LPARAM lParam);

	afx_msg void OnNMClickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdfinditemListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	

	afx_msg void OnProcessEnd();
	afx_msg void OnProcessEndTree();
	afx_msg void OnProcessDebug();
	afx_msg void OnProcessCpu();
	afx_msg void OnProcessProperty();
	afx_msg void OnProcessOpen();
	afx_msg void OnMenu(UINT uID);
	afx_msg void OnProcessSuspend();
	afx_msg void OnProcessView();
};
