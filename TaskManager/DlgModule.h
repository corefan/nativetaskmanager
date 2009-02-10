#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "EnumModule.h"
#include <map>

using namespace std;
using enumModule::ModuleInfo;


// CDlgProcessModule �Ի���
typedef struct tagModuleListColInfo
{
	CString		strColName;		// Colnum����
	UINT		uWidth;			// Colnum���
}ModuleListColInfo;


// CDlgModule �Ի���

class CDlgModule : public CDialog
{
	DECLARE_DYNAMIC(CDlgModule)

public:
	CDlgModule(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgModule();

// �Ի�������
	enum { IDD = IDD_DIALOG_MODULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	
private:
	CSortListCtrl	m_wndListModule;
	CImageList		m_iImageSmall;
	
	BOOL			m_bExit;							// �˳��̱߳�ʶ
	BOOL			m_bPause;							// ��ͣ�̱߳�ʶ

	CWinThread		*m_pModuleThread;					// ��ȡģ����Ϣ�߳�

	map<string,ModuleInfo>	m_mapModuleInfo;			// ģ����Ϣ

	static const ModuleListColInfo m_ModuleListColInfo[];	// ģ��List��ʾ��Ϣ

public:
	BOOL	SetPause(BOOL bPause);						// ������ͣ

private:
	void	InitList();									// ��ʼ��ListCtrl
	void	InsertItem();								// ��������
	
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
