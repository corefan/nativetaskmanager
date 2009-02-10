#ifndef _CCPUUSGESCTL_H_
#define _CCPUUSGESCTL_H_

#ifdef __cplusplus
	extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
// CUsageCtrl window

class CUsageCtrl : public CWnd
{
// Construction
public:
	CUsageCtrl();
	virtual ~CUsageCtrl();


// Attributes
public:
	BOOL Create(DWORD dwStyle, const RECT& rect,
		        CWnd* pParentWnd, UINT nID); 

// Operations
public:

// Implementation
public:
	void SetStringExtend(CString strExtend);
	void SetGridHeight(double dNewCounts);
	void SetGridCount(double dNewHeight);
	void Refresh();
	
private:
  	void DrawCpuUsges(CDC *pDC);

private:
	CFont			m_font;
	COLORREF        m_LableColor;
	COLORREF		m_ForeFontColor;
	COLORREF		m_ForeColor;
	COLORREF		m_BackColor;
	COLORREF		m_FreeColor;
	COLORREF		m_UsedColor;
	COLORREF		m_clrFrameLeftTop;
	COLORREF		m_clrFrameRightBottom;

	double			m_dGridCount;
	double			m_dNewUsges;
	CString			m_strExtend;

	CRect			m_rectClient;
	CRect			m_rectGraph;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifdef __cplusplus
	}
#endif

#endif