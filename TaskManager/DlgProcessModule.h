#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "EnumModule.h"
#include <vector>

using namespace std;
using enumModule::TvecModuleInfo;

// CDlgProcessModule �Ի���
typedef struct tagModuleColInfo
{
	CString		strColName;		// Colnum����
	UINT		uWidth;			// Colnum���
}ModuleColInfo;

class CDlgProcessModule : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessModule)

public:
	CDlgProcessModule(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcessModule();

// �Ի�������
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_MODULE };

private:
	CSortListCtrl	m_wndListProcessTabModule;
	CImageList		m_iImageSmall;
	CFont			m_font;

	int				m_nProcessID;				// ��ǰѡ�н���ID
	CWinThread		*m_pModuleThread;			// ��ȡģ����Ϣ�߳�

	TvecModuleInfo	m_vecModuleInfo;			// ģ����Ϣ

	static const ModuleColInfo m_ModuleColInfo[];// ģ��List��ʾ��Ϣ

private:
	void	InitList();							// ��ʼ��ListCtrl
	void	InsertItem();						// ��������
	
	static DWORD WINAPI ModuleInfoThread(LPVOID pVoid);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
