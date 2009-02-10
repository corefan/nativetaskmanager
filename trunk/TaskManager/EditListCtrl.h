#if !defined(AFX_EDITLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_)
#define AFX_EDITLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditListCtrl.h : header file
//
#define WM_USER_EDIT_END WM_USER+1001

class CListCtrlEdit : public CEdit
{
// Construction
public:
	CListCtrlEdit();

// Attributes
public:
	void  SetCtrlData(DWORD dwData);
	DWORD GetCtrlData();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	DWORD m_dwData;
	BOOL m_bExchange;
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


class CEditListCtrl : public CListCtrl
{
// Construction
public:
	CEditListCtrl();
// Attributes
public:

	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditListCtrl();

private:
	void ShowEdit(BOOL bShow,int nItem,int nIndex,CRect rc = CRect(0,0,0,0));
	int	HitTestEx(CPoint &point, int *col) const;

	CListCtrlEdit    m_edit;
	int m_nItem;
	int m_nSub;

	BOOL Key_Shift(int& nItem,int& nSub);
	BOOL Key_Ctrl(int& nItem,int &nSub);
	// Generated message map functions
protected:
	//{{AFX_MSG(CEditListCtrl)

	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	//}}AFX_MSG

	afx_msg LRESULT OnEditEnd(WPARAM ,LPARAM lParam = FALSE);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITLISTCTRL_H__B26FE6EC_7377_4DA6_BEC1_E3C656DACFB8__INCLUDED_)
