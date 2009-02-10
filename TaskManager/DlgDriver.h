#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "GDIPlusDialog.h"
#include "DiskInfo.h"
#include "PieChartCtrl.h"
#include "TransparentStatic.h" 
#include "CheckComboBox.h"
#include "BarChart.h"
#include "perfmon.h"


using namespace PDH;
using namespace	Volume;
// CDlgDriver �Ի���

class CDlgDriver : public CGDIPlusDialog
{
	DECLARE_DYNAMIC(CDlgDriver)

public:
	CDlgDriver(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgDriver();

// �Ի�������
	enum { IDD = IDD_DIALOG_DISK_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	
private:
	void			InitDiskInfo();						// ��ʼ��Ӳ����Ϣ
	void			InitTree();							// ��ʼ���ڵ���
	void			InitBarChart();						// ��ʼ������ͼ
	void			InitChart();						// ��ʼ��Pie Chart
	void			InitPerformaceDisk();				// ��ʼ��CheckListBox
	void			InitCounter();						// ��ʼ�����ܼ�����

	void			DisplayDiskChart(int nItem);		// ����Pie Chart
	void			AddItem();							// ����ڵ�
	void			LoadBitmap();						// ����ͼ��
	void			DestroyBitmap();					// ж��ͼ��


	static			DWORD WINAPI ScanDiskThread(LPVOID pVoid);	// ɨ��Disk�̺߳���
	
public:
	BOOL			SetPause(BOOL bPause);				// �����Ƿ���ͣ

private:
	CTransparentStatic m_wndStatic1;
	CTransparentStatic m_wndStatic2;
	CTransparentStatic m_wndStatic3;
	CTransparentStatic m_wndStatic4;
	CTransparentStatic m_wndStatic5;
	CTransparentStatic m_wndStatic6;
	CTransparentStatic m_wndStatic7;
	CTransparentStatic m_wndStatic8;
	CTransparentStatic m_wndStatic9;
	CTransparentStatic m_wndStatic10;
	CTransparentStatic m_wndStatic11;
	CTransparentStatic m_wndStatic12;
	CTransparentStatic m_wndStatic13;
	CTransparentStatic m_wndStatic14;
	CTransparentStatic m_wndStatic15;
	CTransparentStatic m_wndStatic16;
	CTransparentStatic m_wndStatic17;
	//CTransparentStatic m_wndStatic18;
	//CTransparentStatic m_wndStatic19;

	CString			m_strDriveModel;
	CString			m_strDriveSerial;
	CString			m_strRevision;
	CString			m_strDriveSize;
	CString			m_strDriveContraller;
	CString			m_strDriveBufferSize;
	CString			m_strDriveType;
	CString			m_strDriveVolumeType;
	CString			m_strDriveUsage;
	CString			m_strDriveFree;
	
	CBarChart		m_wndBarChart;						// ����ͼ
	CPieChartCtrl	m_wndChart;							// Pie Chart Control
	CTreeCtrl		m_wndTree;							// Ӳ���߼������ڵ���
	CImageList		m_wndImage;							// �ڵ���ͼ��
	CCheckComboBox	m_wndComboDisk;						// ����ѡ���

	CPerfMon		m_PerfMon;							// ���ܼ�����

	CWinThread		*m_pScanDiskThread;					// ɨ��Task Window�߳�
	BOOL			m_bExit;							// �˳��̱߳��
	BOOL			m_bPause;							// ��ͣ��־
	int				n_nPerfCountItem;					// ���ܼ���������

	Bitmap			*m_pImageBack;						// ����ͼƬ
	Bitmap			*m_pImageDiskMain;
	Bitmap			*m_pImageDiskRemove;
	Bitmap			*m_pImageDiskNetwork;
	Bitmap			*m_pImageDiskDVD;

	DiskVolume		m_DiskVolume;
	vector<DiskInfo> m_vecVolume;						// Disk Info
	vector<CString> m_vecDiskPerf;						// Performance counter of Disk		

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT Update(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetPause(WPARAM wParam,LPARAM lParam);

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTvnSelchangedTreeDrive(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	
	afx_msg void OnBnClickedButtonDiskView();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
