#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "EnumModule.h"
#include <map>

using namespace std;
using enumModule::ModuleInfo;


// CDlgProcessModule 对话框
typedef struct tagModuleListColInfo
{
	CString		strColName;		// Colnum名称
	UINT		uWidth;			// Colnum宽度
}ModuleListColInfo;


// CDlgModule 对话框

class CDlgModule : public CDialog
{
	DECLARE_DYNAMIC(CDlgModule)

public:
	CDlgModule(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgModule();

// 对话框数据
	enum { IDD = IDD_DIALOG_MODULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
private:
	CSortListCtrl	m_wndListModule;
	CImageList		m_iImageSmall;
	
	BOOL			m_bExit;							// 退出线程标识
	BOOL			m_bPause;							// 暂停线程标识

	CWinThread		*m_pModuleThread;					// 获取模块信息线程

	map<string,ModuleInfo>	m_mapModuleInfo;			// 模块信息

	static const ModuleListColInfo m_ModuleListColInfo[];	// 模块List显示信息

public:
	BOOL	SetPause(BOOL bPause);						// 设置暂停

private:
	void	InitList();									// 初始化ListCtrl
	void	InsertItem();								// 插入数据
	
	static DWORD WINAPI ScanModuleThread(LPVOID pVoid);

public:
	DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnUpdateModuleList(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetPause(WPARAM wParam,LPARAM lParam);

	afx_msg void OnNMDblclkListModule(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListModule(NMHDR *pNMHDR, LRESULT *pResult);
};
