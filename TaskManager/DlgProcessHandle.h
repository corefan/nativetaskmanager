#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "NtSystemInfo.h"

using namespace NT;
// CDlgProcessHandle 对话框

typedef struct tagHandleColInfo
{
	CString		strColName;		// Colnum名称
	UINT		uWidth;			// Colnum宽度
}HandleColInfo;

class CDlgProcessHandle : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessHandle)

public:
	CDlgProcessHandle(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcessHandle();

// 对话框数据
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_HANDLE };

private:
	CSortListCtrl	m_wndListHandle;
	CFont			m_font;

	int				m_nProcessID;						// 当前选中进程ID
	CWinThread		*m_pHandleThread;						// 获取内存信息线程

	static const HandleColInfo m_HandleColInfo[];		// 句柄List显示信息

private:
	void	InitList();									// 初始化ListCtrl
	void	InsertItem();								// 插入数据

	vector<SystemHandleInformation::HANDLE_INFORMATION> m_vecHandleInfo;				// 句柄信息
	static DWORD WINAPI HandleInfoThread(LPVOID pVoid);	// 搜索句柄线程

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnLoadHandleInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushHandleInfo(WPARAM wParam,LPARAM lParam);
};
