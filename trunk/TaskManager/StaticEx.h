#pragma once
#include "TransparentHelper.h"

// CStaticEx

class CStaticEx : public CStatic
{
	DECLARE_DYNAMIC(CStaticEx)

public:
	CStaticEx();
	virtual ~CStaticEx();

protected:
	DECLARE_MESSAGE_MAP()

	CTransparentHelper m_objHelper;
	virtual void PreSubclassWindow();

public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


