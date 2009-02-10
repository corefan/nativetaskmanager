#ifndef __PIE_CHART_H
#define __PIE_CHART_H

#include <vector>
#include "Gdiplus.h"

#pragma comment(lib,"Gdiplus")
using namespace Gdiplus;

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CPieChartCtrl window
class CPieChartPiece : public CObject
{
public:
	CPieChartPiece()
		: m_pBrushBack(NULL)
	{}
	~CPieChartPiece() 
	{
		if( m_pBrushBack!=NULL )
		{
			delete m_pBrushBack;
			m_pBrushBack=NULL;
		}
	}

	Color		m_ColorBack;		// 背景线条色
	Color		m_ColorText;		// 文字颜色
	SolidBrush	*m_pBrushBack;		// 背景画刷色

	int			m_nAngle;			// 偏移角度
	CString		m_strInfo;			// 提示信息
};


class CPieChartCtrl : public CWnd
{
	// Construction
public:
	CPieChartCtrl();
	virtual ~CPieChartCtrl();

	// Attributes
public:

	// Operations
public:
	BOOL AddPiece(COLORREF colorBack, COLORREF colorText,int nAngle, const CString& str = _T(""));	// 增加
	void SetTitle(const CString& str);									// 设置控件标题信息
	void CountPoint(int nAngle, CPoint& pt, BOOL bPercent = FALSE);		
	void GetItemColor(int i, COLORREF& color);
	void Reset();														// 重置
	

private:
	void RecalcRect();
	static BOOL RegisterWndClass(HINSTANCE hInstance);					// 注册控件
	// Implementation
private:
	int			m_nStartAngle;											// 开始角度

	Color		m_ColorDefault;											// 默认背景色
	Color		m_ColorLine;											// 线条色

	vector<CPieChartPiece *> m_vecChart;

	int		m_nTitleHeight;
	CFont	m_fontTitle;
	CFont	m_fontInfo;
	CString m_strTitle;

	CPoint	m_ptOldPt;
	BOOL	m_bCaptured;
	
	CRect	m_rectChart;

	const static double pi;	
	
private:
	ULONG_PTR				m_pToken;
	GdiplusStartupInput		m_GdiInput;
	// Generated message map functions
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


#endif 
