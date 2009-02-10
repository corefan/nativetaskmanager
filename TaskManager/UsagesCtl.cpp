// NetCtl.cpp : implementation file
//

#include "stdafx.h"
#include "UsagesCtl.h"
#include "CMemDC.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CUsageCtrl::CUsageCtrl()
{
	m_font.CreatePointFont(80,_T("arial"));
	m_dNewUsges		   = 0;
	m_ForeFontColor    = RGB(0, 255, 0);
	m_ForeColor        = RGB(0, 150, 0);
	m_BackColor        = RGB(0, 0, 0);	
	m_FreeColor		   = RGB(0, 80, 0);
	m_UsedColor        = RGB(0, 140, 0);
	m_LableColor       = RGB(255, 255, 0);
	m_clrFrameRightBottom=RGB(255,255,255);
	m_clrFrameLeftTop  =RGB(192,192,192);
	m_dGridCount       = 50;
}

CUsageCtrl::~CUsageCtrl()
{
	m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CUsageCtrl, CWnd)
	//{{AFX_MSG_MAP(CUsageCtrl)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUsageCtrl message handlers

void CUsageCtrl::OnPaint() 
{
	CPaintDC dc(this); 
	CMemDC memDC(&dc);

	DrawCpuUsges(&memDC);  
}

void CUsageCtrl::DrawCpuUsges(CDC *pDC)
{
	CString strLable;
	CString strUsges;
	CPen BackFreePen;
	CPen BackUsedPen;
	CPen *pOldPen    = NULL;
	
	// Fill background
	pDC->FillSolidRect(m_rectClient, m_BackColor);

	//draw cpu free usges
	BackFreePen.CreatePen(PS_SOLID, 1, m_FreeColor);
	pOldPen=pDC->SelectObject(&BackFreePen);

	for(int i=0; i<m_rectGraph.Height(); i+=2)
	{
		pDC->MoveTo(m_rectGraph.left,  m_rectGraph.top+i);
		pDC->LineTo(m_rectGraph.right, m_rectGraph.top+i);
	}

	//draw cpu used usges
	BackUsedPen.CreatePen(PS_SOLID, 1, m_UsedColor);
	pDC->SelectObject(&BackUsedPen);

	double nHeight=m_dNewUsges*m_rectGraph.Height()/m_dGridCount;

	for(int j = 0; j < (int)nHeight; j+=2)	   //fill used percent
	{
		if( m_rectGraph.bottom - j < m_rectGraph.top+5 )
			break;

		pDC->MoveTo(m_rectGraph.left,  m_rectGraph.bottom - j);
		pDC->LineTo(m_rectGraph.right, m_rectGraph.bottom - j);
	}

	// draw frame
	CRect rcTemp=m_rectClient;
	rcTemp.right-=1;
	rcTemp.bottom-=1;
	pDC->Draw3dRect(rcTemp,m_clrFrameLeftTop,m_clrFrameRightBottom);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextAlign(TA_LEFT | TA_CENTER);

	CFont *pOldFont=pDC->SelectObject(&m_font);
	strUsges.Format(_T("%4.2f"), m_dNewUsges);
	strUsges+=m_strExtend;
	pDC->SetTextColor(m_ForeFontColor);
	pDC->TextOut(m_rectClient.left + m_rectClient.Width() / 2, m_rectClient.bottom - 15, strUsges);

	pDC->SelectObject(pOldFont);
	pDC->SelectObject(pOldPen);
}

int CUsageCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	GetClientRect(&m_rectClient);

	m_rectGraph=m_rectClient;
	m_rectGraph.top+=10;
	m_rectGraph.bottom-=20;
	m_rectGraph.left+=10;
	m_rectGraph.right-=10;

	return 0;
}

BOOL CUsageCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

BOOL CUsageCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	
	return CWnd::Create(NULL, NULL, dwStyle, rect, pParentWnd, nID, NULL);
}

void CUsageCtrl::OnSize(UINT nType, int cx, int cy) 
{ 	
	CWnd::OnSize(nType, cx, cy);
}

void CUsageCtrl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
}


void CUsageCtrl::Refresh()
{
	Invalidate();
}

void CUsageCtrl::SetGridCount(double dNewCounts)
{
    m_dGridCount	= dNewCounts;
}

void CUsageCtrl::SetGridHeight(double dNewHeight)
{
	m_dNewUsges		= dNewHeight;
}

void CUsageCtrl::SetStringExtend(CString strExtend)
{
	m_strExtend=strExtend;
}






