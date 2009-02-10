#pragma once
#include "afxcmn.h"
#include "SortListCtrl.h"
#include "EnumProcess.h"
#include "XTabCtrl.h"
#include "xySplitterWnd.h"
#include "CriticalSection.h"

#include "DlgProcessTabFile.h"
#include "DlgProcessModule.h"
#include "DlgProcessThread.h"
#include "DlgProcessMemory.h"
#include "DlgProcessPort.h"
#include "DlgProcessEnvironment.h"
#include "DlgProcessHandle.h"



using namespace enumProcess;
// CDlgProcess �Ի���
#define MAX_PROCESS_COLUM	27		// Number of Process Colum 

enum COLUMNID
{
    COL_IMAGENAME           = 0,
    COL_PID                 = 1,
	COL_CPU                 = 2,
    COL_CPUTIME             = 3,

	COL_MEMUSAGE            = 4,
    COL_MEMDELTA            = 5,
	COL_MEMPEAKSAGE			= 6,

    COL_PAGEFAULTS			= 7,
	COL_PAGEFAULTSDELTA		= 8,
    COL_PAGEPOOL           = 9,
	COL_NONPAGEPOOL        = 10,

	COL_USEROBJECTS         = 11,
	COL_GDIOBJECTS          = 12,

	COL_SESSIONID           = 13,
    COL_USERNAME            = 14,
	COL_BASEPRIORITY        = 15,

	COL_HANDLECOUNT         = 16,
    COL_THREADCOUNT         = 17,
    
    COL_IOREADS				= 18,
	COL_IOREADBYTES			= 19,
	COL_IOWRITES			= 20,
	COL_IOWRITEBYTES		= 21,
	COL_IOOTHER				= 22,
	COL_IOOTHERBYTES		= 23,
    
	COL_PEEKMEMORYUSAGE		= 24,
	COL_VIRTUALMEMORYSIZE	= 25,
	COL_IMAGEPATH			= 26
};


// �������ListCtrl��Ϣ
typedef struct tagProcessCol
{
	UINT	uID;			// 
	UINT	uWidth;			// ���
	UINT	uMFT;			// ���뷽ʽ
	BOOL	bSelected;		// �Ƿ�ѡ��
	CString strColName;		// column��
}ProcessCol;


class CDlgProcess : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcess)

public:
	CDlgProcess(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcess();

// �Ի�������
	enum { IDD = IDD_DIALOG_PROCESS };

private:
	CListCtrl			m_wndListProcess;
	CXTabCtrl			m_wndTabProcess;
	CImageList			m_iImageSmall;
	CySplitterWnd		m_wndSplitterY;

	CWinThread			*m_pThreadProcess;			// ���������߳̾��
	CEvent				*m_pUpdateEvent;			// ��ʼ����UI�¼�
	BOOL				m_bExit;					// �����߳��˳���־
	BOOL				m_bPause;					// �߳���ͣ��־

	int					m_nCurItem;					// ��ǰѡ����
	int					m_nCurSelCol;				// ��ǰѡ���������

	CDlgProcessTabFile	m_wndDlgProcessFile;		// �ļ���ϢDlg
	CDlgProcessModule	m_wndDlgProcessModule;		// ģ����ϢDlg
	CDlgProcessThread	m_wndDlgProcessThread;		// �߳���ϢDlg
	CDlgProcessMemory	m_wndDlgProcessMemory;		// �ڴ���ϢDlg
	CDlgProcessPort		m_wndDlgProcessPort;		// �˿���ϢDlg
	CDlgProcessEnvironment m_wndDlgProcessEnv;		// ����������ϢDlg
	CDlgProcessHandle	m_wndDlgProcessHandle;		// �˿���ϢDlg
	
	TvecProInfo			m_vecProcessInfo;			// ��������
	EnumProcess			m_enumProcess;				// ����ö��

	CriticalSection		m_cs;						// �ؼ��μ���
	ProcessCol			m_proCol[MAX_PROCESS_COLUM];// ����Column��Ϣ

	vector<TabInfo>		m_vecTabInfo;				// ��������Dlg����
public:
	BOOL	SetPause(BOOL bPause);					// �����Ƿ���ͣ

private:
	void	InitListProcess();						// ��ʼ��ListCtrl
	void	InitTab();								// ��ʼ��TabCtrl
	void	InsertItem();							// ��������

	void	UpdateImageList();						// ����ImageList

	void	SuspendProcess(DWORD dwProcessID, BOOL fSuspend);//�������
	void	PopupMenu();							// �����˵�
	void	SetMenuState(CMenu *pTrackMenu);		// ���ò˵�״̬
	
	static bool	Compare(const ProcessInfo &lv,const ProcessInfo &rv);// ����
	static DWORD ScanProcessThread(LPVOID pVoid);	// ���������߳�

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu);

	afx_msg LRESULT OnUpdateProcessList(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetPause(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetSize(WPARAM wParam,LPARAM lParam);

	afx_msg void OnNMClickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnGetdispinfoListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnOdfinditemListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	

	afx_msg void OnProcessEnd();
	afx_msg void OnProcessEndTree();
	afx_msg void OnProcessDebug();
	afx_msg void OnProcessCpu();
	afx_msg void OnProcessProperty();
	afx_msg void OnProcessOpen();
	afx_msg void OnMenu(UINT uID);
	afx_msg void OnProcessSuspend();
	afx_msg void OnProcessView();
};
