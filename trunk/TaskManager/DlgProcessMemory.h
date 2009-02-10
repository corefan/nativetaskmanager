#pragma once
#include "SortListCtrl.h"
#include "VMInfo.h"
#include <vector>
#include "afxcmn.h"
#include "CriticalSection.h"

using namespace std;
using namespace VM;


typedef struct tagMemoryColInfo
{
	CString		strColName;		// Colnum����
	UINT		uWidth;			// Colnum���
}MemoryColInfo;


class CDlgProcessMemory : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessMemory)

public:
	CDlgProcessMemory(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgProcessMemory();

// �Ի�������
	enum { IDD = IDD_FORMVIEW_PROCESS_TAB_MEMORY };

private:
	CSortListCtrl	m_wndListMemory;
	CFont			m_font;

	CCriticalSection m_cs;

	int				m_nProcessID;				// ��ǰѡ�н���ID
	CWinThread		*m_pMemoryThread;			// ��ȡ�ڴ���Ϣ�߳�

	vector<VMQUERY>	m_vecMemoryInfo;			// �ڴ���Ϣ

	static const MemoryColInfo m_MemoryColInfo[];		// �ڴ�List��ʾ��Ϣ

private:
	void	InitList();							// ��ʼ��ListCtrl
	void	InsertItem();						// ��������
	
	LPCTSTR GetVMBaseAddr(PVOID pBaseAddr);
	LPCTSTR GetVMProtection(DWORD dwProtection);
	LPCTSTR GetVMSize(SIZE_T szSize);
	LPCTSTR GetVMState(DWORD dwState);
	LPCTSTR GetVMBlocks(DWORD dwBlock);
	LPCTSTR GetVMMap(PVOID pBaseAddr,BOOL bRgnIsAStack);

	static DWORD WINAPI MemoryInfoThread(LPVOID pVoid);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

public:
	DECLARE_EASYSIZE 
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnLoadMemoryInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnFlushMemoryInfo(WPARAM wParam,LPARAM lParam);

};
