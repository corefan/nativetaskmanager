#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "ProcessEnvReader.h"
// CDlgProcessEnvironment 对话框

using namespace enumEnvironment;

typedef struct tagEnvColInfo
{
	CString		strColName;		// Colnum名称
	UINT		uWidth;			// Colnum宽度
}EnvColInfo;


class CDlgProcessEnvironment : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessEnvironment)

public:
	CDlgProcessEnvironment(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcessEnvironment();

// 对话框数据
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_ENVIRONMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

private:
	CSortListCtrl	m_wndListEnv;
	CFont			m_font;

	int				m_nProcessID;						// 当前选中进程ID
	CWinThread		*m_pEnvThread;						// 获取内存信息线程

	EnvVarValArray	m_vecEnv;							// 环境变量信息
	static const EnvColInfo m_EnvColInfo[];				// 环境List显示信息

private:
	void	InitList();									// 初始化ListCtrl
	void	InsertItem();								// 插入数据

	
	static DWORD WINAPI EnvInfoThread(LPVOID pVoid);	// 搜索环境变量线程
public:
	DECLARE_EASYSIZE
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnLoadEnvInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushEnvInfo(WPARAM wParam,LPARAM lParam);
};
