#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "EnumModule.h"
#include <vector>

using namespace std;
using enumModule::TvecModuleInfo;

// CDlgProcessModule 对话框
typedef struct tagModuleColInfo
{
	CString		strColName;		// Colnum名称
	UINT		uWidth;			// Colnum宽度
}ModuleColInfo;

class CDlgProcessModule : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessModule)

public:
	CDlgProcessModule(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcessModule();

// 对话框数据
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_MODULE };

private:
	CSortListCtrl	m_wndListProcessTabModule;
	CImageList		m_iImageSmall;
	CFont			m_font;

	int				m_nProcessID;				// 当前选中进程ID
	CWinThread		*m_pModuleThread;			// 获取模块信息线程

	TvecModuleInfo	m_vecModuleInfo;			// 模块信息

	static const ModuleColInfo m_ModuleColInfo[];// 模块List显示信息

private:
	void	InitList();							// 初始化ListCtrl
	void	InsertItem();						// 插入数据
	
	static DWORD WINAPI ModuleInfoThread(LPVOID pVoid);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);

	afx_msg LRESULT OnLoadModuleInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushModuleInfo(WPARAM wParam,LPARAM lParam);

	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnNMRclickListProcessTabModule(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListProcessTabModule(NMHDR *pNMHDR, LRESULT *pResult);
};
