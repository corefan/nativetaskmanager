#pragma once
#include "afxcmn.h"
#include "EnumNtSrv.h"
#include "SortListCtrl.h"
#include "afxwin.h"

#include <map>

using std::map;
// CDlgService 对话框

using namespace enumService;

class CDlgService : public CDialog
{
	DECLARE_DYNAMIC(CDlgService)

public:
	CDlgService(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgService();

// 对话框数据
	enum { IDD = IDD_DIALOG_SERVER };

private:
	CSortListCtrl		m_wndListServer;
	CImageList			m_iImageSmall;

	CComboBox			m_wndComboServiceType;			// 服务类型选择框
	CComboBox			m_wndComboServiceStatus;		// 服务状态选择框
	
	EnumServiceInfo		m_enumService;	
	deque<ServiceInfo>	m_vecServiceInfo;				// 服务详细信息

	int					m_nCurSelItem;					// 当前选择项
	int					m_nServiceType;					// 当前选择服务类型
	int					m_nServiceStatus;				// 当前选择服务状态

	CWinThread			*m_pScanServiceThread;			// 扫描Task Window线程
	BOOL				m_bExit;						// 退出线程标记
	BOOL				m_bPause;						// 暂停标志

	map<UINT,CString> m_mapServiceType;					// 服务类型	
	map<UINT,CString> m_mapServiceStatus;				// 服务状态

public:
	BOOL	SetPause(BOOL bPause);			

private:
	void	InitList();									// 初始化ListCtrl
	void	InitCombo();								// 初始化combo选项
	void	InsertItem();								// 插入项到ListCtrl
	void	SetItemInfo(int nItem,int nSubItem,LPCTSTR lpszText);// 更改选择项的信息

	void	PopupMenu(LPNMLISTVIEW pListView,UINT uID);	// 弹出菜单
	void	SetMenuState(CMenu *pTrackMenu);

	static	DWORD WINAPI ScanServiceThread(LPVOID pVoid);	// 扫描Server线程函数

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()
	
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT Update(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetPause(WPARAM wParam,LPARAM lParam);

	afx_msg void OnServiceOpen();
	afx_msg void OnServiceSatrt();
	afx_msg void OnServiceStop();
	afx_msg void OnServicePause();
	afx_msg void OnServiceResume();
	afx_msg void OnServiceDelete();
	afx_msg void OnServiceAutoStart();
	afx_msg void OnServiceManualStart();
	afx_msg void OnServiceDisabled();

	afx_msg void OnNMClickListServer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);
	
	afx_msg void OnBnClickedButtonServer();
	afx_msg void OnDestroy();

	afx_msg void OnCbnSelchangeComboServiceType();
	afx_msg void OnCbnSelchangeComboServiceStatus();
};
