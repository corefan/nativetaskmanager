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
// CDlgDriver 对话框

class CDlgDriver : public CGDIPlusDialog
{
	DECLARE_DYNAMIC(CDlgDriver)

public:
	CDlgDriver(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgDriver();

// 对话框数据
	enum { IDD = IDD_DIALOG_DISK_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	
private:
	void			InitDiskInfo();						// 初始化硬盘信息
	void			InitTree();							// 初始化节点树
	void			InitBarChart();						// 初始化性能图
	void			InitChart();						// 初始化Pie Chart
	void			InitPerformaceDisk();				// 初始化CheckListBox
	void			InitCounter();						// 初始化性能计数器

	void			DisplayDiskChart(int nItem);		// 更新Pie Chart
	void			AddItem();							// 插入节点
	void			LoadBitmap();						// 加载图标
	void			DestroyBitmap();					// 卸载图标


	static			DWORD WINAPI ScanDiskThread(LPVOID pVoid);	// 扫描Disk线程函数
	
public:
	BOOL			SetPause(BOOL bPause);				// 设置是否暂停

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
	
	CBarChart		m_wndBarChart;						// 性能图
	CPieChartCtrl	m_wndChart;							// Pie Chart Control
	CTreeCtrl		m_wndTree;							// 硬盘逻辑分区节点树
	CImageList		m_wndImage;							// 节点树图标
	CCheckComboBox	m_wndComboDisk;						// 性能选择框

	CPerfMon		m_PerfMon;							// 性能计数器

	CWinThread		*m_pScanDiskThread;					// 扫描Task Window线程
	BOOL			m_bExit;							// 退出线程标记
	BOOL			m_bPause;							// 暂停标志
	int				n_nPerfCountItem;					// 性能计数器个数

	Bitmap			*m_pImageBack;						// 背景图片
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
