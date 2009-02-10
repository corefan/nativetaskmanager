// PieChartCtrl.cpp : implementation file
//
// Written by Yuheng Zhao (yuheng@ministars.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then a simple email would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage whatsoever.
// It's free - so you get what you pay for.

#include "stdafx.h"
#include "PieChartCtrl.h"
#include "CMemDC.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable: 4244)

const double CPieChartCtrl::pi= 3.1415926535;
/////////////////////////////////////////////////////////////////////////////
// CPieChartCtrl

CPieChartCtrl::CPieChartCtrl()
{
	::GdiplusStartup(&m_pToken,&m_GdiInput,NULL);

	CPieChartCtrl::RegisterWndClass(AfxGetInstanceHandle());

	m_nStartAngle	= 0;

	ARGB argb = Color::MakeARGB(255, 0, 0, 0);
	m_ColorLine.SetValue(argb);
	argb = Color::MakeARGB(200, 0, 0, 255);
	m_ColorDefault.SetValue(argb);

	
	m_rectChart.SetRect(0,0,0,0);
	m_bCaptured		= FALSE;

	m_strTitle		= _T("");

	m_fontTitle.CreateFont(15, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));
	
	m_fontInfo.CreateFont(13, 0,0,0,FW_NORMAL, 0,0,0,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));
}

CPieChartCtrl::~CPieChartCtrl()
{
	::GdiplusShutdown(m_pToken);

	Reset();
}


BEGIN_MESSAGE_MAP(CPieChartCtrl, CWnd)
	//{{AFX_MSG_MAP(CPieChartCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPieChartCtrl message handlers

BOOL CPieChartCtrl::RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	wc.lpszClassName	= _T("PieChartControl"); // matches class name in client
	wc.hInstance		= hInstance;
	wc.lpfnWndProc		= ::DefWindowProc;
	wc.hCursor			= ::LoadCursor(NULL, IDC_HAND);
	wc.hIcon			= 0;
	wc.lpszMenuName		= NULL;
	wc.hbrBackground	= (HBRUSH)NULL;
	wc.style			= CS_GLOBALCLASS | CS_OWNDC; // To be modified
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;

	return (::RegisterClass(&wc) != 0);
}

void CPieChartCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CMemDC memDC(&dc);
	Graphics graphics(memDC.GetSafeHdc());

	// 画面优化
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

	// 设置字体信息
	FontFamily  fontFamily(L"Times New Roman");
	Font        font(&fontFamily, 13, FontStyleRegular, UnitPixel);
	SolidBrush  solidBrush(Color(255, 0, 0, 0));
	StringFormat format;
	format.SetAlignment(StringAlignmentFar);


	CRect clientRect;
	GetClientRect(&clientRect);

	// 填充背景和描绘边框
	memDC.FillSolidRect(clientRect, dc.GetBkColor());
	memDC.DrawEdge(clientRect, EDGE_SUNKEN, BF_RECT);

	// 绘制整个Pie图
	Pen pen(m_ColorLine);
	SolidBrush brush(m_ColorDefault);
	graphics.DrawEllipse(&pen,m_rectChart.left,m_rectChart.top,m_rectChart.Width(),m_rectChart.Height());
	graphics.FillEllipse(&brush,m_rectChart.left,m_rectChart.top,m_rectChart.Width(),m_rectChart.Height());

	
	// 绘制每个PieSlice
	int nCount=(int)m_vecChart.size();
	CPieChartPiece* pItem=NULL;

	int nCurrectAngle = m_nStartAngle;
	int nInfo=0;

	for (int i=0; i<nCount; i++)
	{
		pItem=m_vecChart.at(i);
		// 计算Pie Slice所占角度
		nCurrectAngle = pItem->m_nAngle;
		
		graphics.DrawPie(&pen,m_rectChart.left,m_rectChart.top,
			m_rectChart.Width(),m_rectChart.Height(),m_nStartAngle,nCurrectAngle);

		graphics.FillPie(pItem->m_pBrushBack,m_rectChart.left,m_rectChart.top,
			m_rectChart.Width(),m_rectChart.Height(),m_nStartAngle,nCurrectAngle);
		m_nStartAngle+=nCurrectAngle;
		

		//Draw info
		if (!(pItem->m_strInfo).IsEmpty())
		{	
			CSize sz = memDC.GetTextExtent(pItem->m_strInfo);

			CRect rcColor(clientRect);
			rcColor.right	-= 5;
			rcColor.left	= rcColor.right - sz.cy;
			rcColor.top		+= 5;
			rcColor.bottom	= rcColor.top + sz.cy;
			rcColor.OffsetRect(0, nInfo*rcColor.Height());

			graphics.FillRectangle(pItem->m_pBrushBack,rcColor.left,rcColor.top,rcColor.Width(),rcColor.Height());

			CRect rcEdge(rcColor);
			rcEdge.right	+=1;
			rcEdge.bottom	+=1;
			memDC.DrawEdge(rcEdge, EDGE_SUNKEN, BF_RECT);
			
			rcColor.right	= rcColor.left - 3;
			rcColor.left	= clientRect.left;

			graphics.DrawString(pItem->m_strInfo,-1,&font,RectF(rcColor.left,rcColor.top,rcColor.right,rcColor.bottom),
				&format,&solidBrush);

			nInfo++;
		}

		// Draw percent
		/*if (pItem->m_nAngle>25)
		{
			int n = nCurrectAngle - (pItem->m_nAngle)/2;
			CPoint p;
			CountPoint(n, p, TRUE);
			CString str;
			str.Format(_T("%.0f%%"), (double)(pItem->m_nAngle)*100.0/360.0);
			CSize sz = memDC.GetTextExtent(str);
			memDC.SetTextColor(pItem->m_colorText);
			memDC.TextOut(p.x-sz.cx/2, p.y-sz.cy/2, str);
		}*/
	}
	
	// Draw Line for the out circle
	graphics.DrawEllipse(&pen,m_rectChart.left,m_rectChart.right,m_rectChart.Width(),m_rectChart.Height());
	

	//Draw Title
	if (!m_strTitle.IsEmpty())
	{
		graphics.DrawString(m_strTitle,-1,&font,PointF(clientRect.left+10,clientRect.top+10),&solidBrush);
	}

	CWnd::OnPaint();
}

void CPieChartCtrl::OnLButtonUp(UINT, CPoint) 
{
	if (m_bCaptured)
	{
		::ReleaseCapture();
		m_bCaptured = FALSE;
	}	
}

void CPieChartCtrl::OnLButtonDown(UINT, CPoint point) 
{
	SetCapture();
	m_bCaptured = TRUE;
	m_ptOldPt = point;
}

void CPieChartCtrl::OnMouseMove(UINT, CPoint point) 
{
	if (m_bCaptured)
	{
		int nOffX = point.x - m_ptOldPt.x;

		m_nStartAngle += nOffX;

		InvalidateRect(m_rectChart, FALSE);

		m_ptOldPt = point;
	}
}

void CPieChartCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	RecalcRect();
}


void CPieChartCtrl::RecalcRect()
{
	CRect rect;
	GetClientRect(&rect);
	if (!m_strTitle.IsEmpty())
	{
		CClientDC dc(this);
		CFont* pOldFont = dc.SelectObject(&m_fontTitle);
		CSize sz = dc.GetTextExtent(m_strTitle);
		m_nTitleHeight = sz.cy;
		rect.top += sz.cy*2;// Leave  lines.
		dc.SelectObject(pOldFont);
	}

	int nSize = (rect.Width()>rect.Height())?rect.Height():rect.Width();;

	m_rectChart = CRect(CPoint(rect.left + (rect.Width()-nSize)/2, 
		rect.top + (rect.Height()-nSize)), 
		CSize(nSize, nSize));
	m_rectChart.bottom-=5;

	Invalidate();
}

BOOL CPieChartCtrl::AddPiece(COLORREF colorBack, COLORREF colorText,
							 int nAngle, const CString& str)
{
	CPieChartPiece* pItem = new CPieChartPiece;

	pItem ->m_ColorBack.SetFromCOLORREF(colorBack);
	pItem ->m_ColorText.SetFromCOLORREF(colorText);
	pItem ->m_pBrushBack=new SolidBrush(Color(255,
		pItem -> m_ColorBack.GetR(),pItem -> m_ColorBack.GetG(),pItem -> m_ColorBack.GetB()));

	pItem -> m_nAngle = nAngle;
	pItem -> m_strInfo = str;

	try 
	{
		//m_chartPieces.Add(pItem);
		m_vecChart.push_back(pItem);

		InvalidateRect(NULL, FALSE);
		return TRUE;
	}
	catch (CMemoryException* e)
	{
		if (pItem !=NULL) 
			delete pItem;
		e->Delete();
		return FALSE;
	}
}

// bPercent is TRUE when counting the position for the percent info
void CPieChartCtrl::CountPoint(int nAngle, CPoint & pt, BOOL bPercent)
{
	while (nAngle <0)
		nAngle += 360;

	while (nAngle>359)
		nAngle -= 360;

	double dAngle = ((double)nAngle)*pi/(double)180;

	double r;
	r = ((double)m_rectChart.Height())/2.0;
	if (bPercent)
		r = r*3.0/5.0;

	double dOffX = (r*sin(dAngle));
	double dOffY = 0.0 - (r*cos(dAngle));

	double dX = ((double)(m_rectChart.Width()))/2.0;
	double dY = ((double)(m_rectChart.Height()))/2.0;
	
	pt.x = (int)(dX + dOffX);
	pt.y = (int)(dY + dOffY);
}


void CPieChartCtrl::Reset()
{
	// 清空
	for(vector<CPieChartPiece *>::iterator iter=m_vecChart.begin(); 
		iter!=m_vecChart.end(); ++iter)
	{
		delete *iter;
	}

	m_vecChart.clear();

	// 刷新界面
	if( IsWindow(m_hWnd) )
		Invalidate();
}

void CPieChartCtrl::GetItemColor(int i, COLORREF& color)
{
	int nCount=(int)m_vecChart.size();
	if (i>=nCount)
		i=nCount-1;
}

void CPieChartCtrl::SetTitle(const CString & str)
{
	m_strTitle = str;
	RecalcRect();
	Invalidate();
}

BOOL CPieChartCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}
