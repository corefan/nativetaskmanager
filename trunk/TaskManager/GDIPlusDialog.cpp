// GDIPlusDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GDIPlusDialog.h"


// CGDIPlusDialog �Ի���

IMPLEMENT_DYNAMIC(CGDIPlusDialog, CDialog)

CGDIPlusDialog::CGDIPlusDialog(UINT nID,CWnd* pParent /*=NULL*/)
	: CDialog(nID, pParent)
{
	::GdiplusStartup(&m_pToken,&m_GdiInput,NULL);
}

CGDIPlusDialog::~CGDIPlusDialog()
{
	::GdiplusShutdown(m_pToken);
}


BEGIN_MESSAGE_MAP(CGDIPlusDialog, CDialog)
END_MESSAGE_MAP()


// CGDIPlusDialog ��Ϣ�������
