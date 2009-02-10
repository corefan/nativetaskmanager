#pragma once
#include "afxcmn.h"
#include "ModulVer.h"
#include "FileVersion.h"
#include "EditListCtrl.h"

#include <vector>

using namespace std;

// CDlgProcessTabFile �Ի���

class CDlgProcessTabFile : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessTabFile)

public:
	CDlgProcessTabFile(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcessTabFile();

// �Ի�������
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_FILE };

private:
	CEditListCtrl	m_wndListProcessTabFile;
	CFont		m_font;

	CString		m_strFilePath;					// �����ļ�·��
	CWinThread	*m_pFileThread;					// �����ļ���Ϣ�߳̾��

	static		CString m_strItemName[];		// ��Ϣ��Ŀ
	vector<CString> m_vecItemInfo;				// �ļ���Ϣ

private:
	void	InitList();							// ��ʼ��List
	void	InsertItem();						// ��������

private:
	static DWORD WINAPI FileInfoThread(LPVOID pVoid);	// �����ļ���Ϣ�߳�

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()


	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnLoadFileInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushFileInfo(WPARAM wParam,LPARAM lParam);

	afx_msg void OnNMClickListProcessTabFile(NMHDR *pNMHDR, LRESULT *pResult);
};
