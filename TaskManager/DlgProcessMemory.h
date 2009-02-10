#pragma once
#include "SortListCtrl.h"
#include "VMInfo.h"
#include <vector>
#include "afxcmn.h"
#include "CriticalSection.h"

using namespace std;
using namespace VM;


typedef struct tagMemoryColInfo
{
	CString		strColName;		// Colnum名称
	UINT		uWidth;			// Colnum宽度
}MemoryColInfo;


class CDlgProcessMemory : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessMemory)

public:
	CDlgProcessMemory(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcessMemory();

// 对话框数据
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_MEMORY };

private:
	CSortListCtrl	m_wndListMemory;
	CFont			m_font;

	CCriticalSection m_cs;

	int				m_nProcessID;				// 当前选中进程ID
	CWinThread		*m_pMemoryThread;			// 获取内存信息线程

	vector<VMQUERY>	m_vecMemoryInfo;			// 内存信息

	static const MemoryColInfo m_MemoryColInfo[];		// 内存List显示信息

private:
	void	InitList();							// 初始化ListCtrl
	void	InsertItem();						// 插入数据
	
	LPCTSTR GetVMBaseAddr(PVOID pBaseAddr);
	LPCTSTR GetVMProtection(DWORD dwProtection);
	LPCTSTR GetVMSize(SIZE_T szSize);
	LPCTSTR GetVMState(DWORD dwState);
	LPCTSTR GetVMBlocks(DWORD dwBlock);
	LPCTSTR GetVMMap(PVOID pBaseAddr,BOOL bRgnIsAStack);

	static DWORD WINAPI MemoryInfoThread(LPVOID pVoid);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnLoadMemoryInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushMemoryInfo(WPARAM wParam,LPARAM lParam);

};
