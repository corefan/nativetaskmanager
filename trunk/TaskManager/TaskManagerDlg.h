// TaskManagerDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "DlgApplication.h"
#include "DlgService.h"
#include "DlgProcess.h"
#include "DlgModule.h"
#include "DlgDriver.h"
#include "XTabCtrl.h"

#include <vector>

using namespace std;





// CTaskManagerDlg 对话框
class CTaskManagerDlg : public CDialog
{
// 构造
public:
	CTaskManagerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TASKMANAGER_DIALOG ,MAX_DLG=6};

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL OnInitDialog();

	
private:
	CXTabCtrl			m_wndTabMain;
	CDlgApplication		m_wndDlgApp;					// Application
	CDlgService			m_wndDlgService;				// Service
	CDlgProcess			m_wndDlgProcess;				// Process
	CDlgModule			m_wndDlgModule;					// Module
	CDlgDriver			m_wndDlgDrive;					// Drive

	vector<TabInfo>		m_vecTabInfo;					// Tab Infomation
	

private:
	void				InitTab();						// 初始化Tab
// 实现
protected:
	HICON m_hIcon;


public:
	DECLARE_EASYSIZE 

	// 生成的消息映射函数
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg LRESULT OnTabSelChange(WPARAM wParam,LPARAM lParam);
};
