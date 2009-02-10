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

	Color		m_ColorBack;		// ��������ɫ
	Color		m_ColorText;		// ������ɫ
	SolidBrush	*m_pBrushBack;		// ������ˢɫ

	int			m_nAngle;			// ƫ�ƽǶ�
	CString		m_strInfo;			// ��ʾ��Ϣ
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
	BOOL AddPiece(COLORREF colorBack, COLORREF colorText,int nAngle, const CString& str = _T(""));	// ����
	void SetTitle(const CString& str);									// ���ÿؼ�������Ϣ
	void CountPoint(int nAngle, CPoint& pt, BOOL bPercent = FALSE);		
	void GetItemColor(int i, COLORREF& color);
	void Reset();														// ����
	

private:
	void RecalcRect();
	static BOOL RegisterWndClass(HINSTANCE hInstance);					// ע��ؼ�
	// Implementation
private:
	int			m_nStartAngle;											// ��ʼ�Ƕ�

	Color		m_ColorDefault;											// Ĭ�ϱ���ɫ
	Color		m_ColorLine;											// ����ɫ

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
