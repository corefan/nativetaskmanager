#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "CriticalSection.h"
#include "EnumPort.h"

using namespace enumPort;
// CDlgProcessPort 对话框
typedef struct tagPortColInfo
{
	CString		strColName;		// Colnum名称
	UINT		uWidth;			// Colnum宽度
}PortColInfo;

class CDlgProcessPort : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessPort)

public:
	CDlgProcessPort(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcessPort();

// 对话框数据
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_PORT };

private:
	CSortListCtrl	m_wndListPort;						// List Ctrl	
		
	CFont			m_font;								// the Font of List 
	CriticalSection	m_cs;								// 关键段

	int				m_nProcessID;						// 当前选中进程ID
	CWinThread		*m_pPortThread;						// 获取端口信息线程

	vector<PortInfo> m_vecPortInfo;					// 端口信息

	static const PortColInfo m_PortColInfo[];			// 模块List显示信息

private:
	void	InitList();									// 初始化ListCtrl
	void	InsertItem();								// 插入数据
	
	static DWORD WINAPI PortInfoThread(LPVOID pVoid);	// 端口工作线程

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual void PreSubclassWindow();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();

	afx_msg LRESULT OnLoadPortInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushPortInfo(WPARAM wParam,LPARAM lParam);
};
