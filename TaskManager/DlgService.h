#pragma once
#include "afxcmn.h"
#include "EnumNtSrv.h"
#include "SortListCtrl.h"
#include "afxwin.h"

#include <map>

using std::map;
// CDlgService �Ի���

using namespace enumService;

class CDlgService : public CDialog
{
	DECLARE_DYNAMIC(CDlgService)

public:
	CDlgService(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgService();

// �Ի�������
	enum { IDD = IDD_DIALOG_SERVER };

private:
	CSortListCtrl		m_wndListServer;
	CImageList			m_iImageSmall;

	CComboBox			m_wndComboServiceType;			// ��������ѡ���
	CComboBox			m_wndComboServiceStatus;		// ����״̬ѡ���
	
	EnumServiceInfo		m_enumService;	
	deque<ServiceInfo>	m_vecServiceInfo;				// ������ϸ��Ϣ

	int					m_nCurSelItem;					// ��ǰѡ����
	int					m_nServiceType;					// ��ǰѡ���������
	int					m_nServiceStatus;				// ��ǰѡ�����״̬

	CWinThread			*m_pScanServiceThread;			// ɨ��Task Window�߳�
	BOOL				m_bExit;						// �˳��̱߳��
	BOOL				m_bPause;						// ��ͣ��־

	map<UINT,CString> m_mapServiceType;					// ��������	
	map<UINT,CString> m_mapServiceStatus;				// ����״̬

public:
	BOOL	SetPause(BOOL bPause);			

private:
	void	InitList();									// ��ʼ��ListCtrl
	void	InitCombo();								// ��ʼ��comboѡ��
	void	InsertItem();								// �����ListCtrl
	void	SetItemInfo(int nItem,int nSubItem,LPCTSTR lpszText);// ����ѡ�������Ϣ

	void	PopupMenu(LPNMLISTVIEW pListView,UINT uID);	// �����˵�
	void	SetMenuState(CMenu *pTrackMenu);

	static	DWORD WINAPI ScanServiceThread(LPVOID pVoid);	// ɨ��Server�̺߳���

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
