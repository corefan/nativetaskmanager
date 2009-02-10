#pragma once
#include "Gdiplus.h"

#pragma comment(lib,"Gdiplus")
using namespace Gdiplus;

// CGDIPlusDialog �Ի���

class CGDIPlusDialog : public CDialog
{
	DECLARE_DYNAMIC(CGDIPlusDialog)

public:
	CGDIPlusDialog(UINT nID,CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGDIPlusDialog();

private:
	ULONG_PTR				m_pToken;
	GdiplusStartupInput		m_GdiInput;

protected:
	DECLARE_MESSAGE_MAP()
};
