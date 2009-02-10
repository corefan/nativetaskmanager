// StaticEx.cpp : implementation file
//

#include "stdafx.h"
#include "StaticEx.h"


// CStaticEx

IMPLEMENT_DYNAMIC(CStaticEx, CStatic)
CStaticEx::CStaticEx()
{
}

CStaticEx::~CStaticEx()
{
}


BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CStaticEx message handlers


void CStaticEx::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CStatic::PreSubclassWindow();
	m_objHelper.Install( GetSafeHwnd());
}

void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	m_objHelper.TransparentBk( dc.GetSafeHdc(), GetSafeHwnd());
	CString strText;
	GetWindowText( strText);

	CRect rtText;
	GetClientRect( &rtText);
	dc.SetBkMode( TRANSPARENT);
	
	dc.SelectObject( GetFont());
	dc.DrawText( strText, &rtText, DT_LEFT|DT_SINGLELINE);
}


BOOL CStaticEx::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
    return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}
