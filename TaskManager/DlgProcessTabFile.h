#pragma once
#include "afxcmn.h"
#include "ModulVer.h"
#include "FileVersion.h"
#include "EditListCtrl.h"

#include <vector>

using namespace std;

// CDlgProcessTabFile 对话框

class CDlgProcessTabFile : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessTabFile)

public:
	CDlgProcessTabFile(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProcessTabFile();

// 对话框数据
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_FILE };

private:
	CEditListCtrl	m_wndListProcessTabFile;
	CFont		m_font;

	CString		m_strFilePath;					// 加载文件路径
	CWinThread	*m_pFileThread;					// 加载文件信息线程句柄

	static		CString m_strItemName[];		// 信息栏目
	vector<CString> m_vecItemInfo;				// 文件信息

private:
	void	InitList();							// 初始化List
	void	InsertItem();						// 插入数据

private:
	static DWORD WINAPI FileInfoThread(LPVOID pVoid);	// 加载文件信息线程

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()


	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnLoadFileInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushFileInfo(WPARAM wParam,LPARAM lParam);

	afx_msg void OnNMClickListProcessTabFile(NMHDR *pNMHDR, LRESULT *pResult);
};
