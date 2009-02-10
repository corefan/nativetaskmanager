// DlgProcessMemory.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgProcessMemory.h"
#include "ToolHelp.h"

// DlgProcessMemory �Ի���
BEGIN_EASYSIZE_MAP(CDlgProcessMemory)
	EASYSIZE(IDC_LIST_PROCESS_TAB_MEMORY,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP

IMPLEMENT_DYNAMIC(CDlgProcessMemory, CDialog)


const MemoryColInfo CDlgProcessMemory::m_MemoryColInfo[]=
{
	_T("����ַ"),		100,
	_T("��������"),		100,
	_T("��ַ�ռ��С"),	100,
	_T("�����ڴ�����"),	100,
	_T("��ַ������"),	100,
	_T("ӳ��"),			200
};


// ����ģ����Ϣ�߳�
DWORD WINAPI CDlgProcessMemory::MemoryInfoThread(LPVOID pVoid)
{
	CDlgProcessMemory *pThis=(CDlgProcessMemory *)pVoid;

	// ������Ϣ
	HANDLE hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,pThis->m_nProcessID);
	
	
	if( hProcess!=NULL )
	{
		// �����������ڴ���Ϣ
		VMInfo vmInfo(hProcess);
		vmInfo.Enum();
		
		pThis->m_cs.Lock();
		pThis->m_vecMemoryInfo=vmInfo.GetVMInfo();
		pThis->m_cs.Unlock();
	}

	::CloseHandle(hProcess);
	
	// ������Ϣ����UI
	pThis->PostMessage(WM_UPDATE_MEMORY_INFO,0,0);

#ifdef _DEBUG
	::OutputDebugString(_T("Exsit MemroyInfo Thread\n"));
#endif

	return 0;
}


CDlgProcessMemory::CDlgProcessMemory(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessMemory::IDD, pParent)
	, m_pMemoryThread(NULL)
	, m_nProcessID(0)
{

}

CDlgProcessMemory::~CDlgProcessMemory()
{
}

void CDlgProcessMemory::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TAB_MEMORY, m_wndListMemory);
}


BEGIN_MESSAGE_MAP(CDlgProcessMemory, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_MESSAGE(WM_FLUSH_MEMORY,OnFlushMemoryInfo)
	ON_MESSAGE(WM_UPDATE_MEMORY_INFO,OnLoadMemoryInfo)
END_MESSAGE_MAP()


// CDlgProcessMemory ��Ϣ�������

BOOL CDlgProcessMemory::OnInitDialog()
{
	CDialog::OnInitDialog();
	INIT_EASYSIZE;


	InitList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgProcessMemory::OnDestroy()
{
	// �ȴ������
	if( m_pMemoryThread!=NULL )
	{
		::WaitForSingleObject(m_pMemoryThread->m_hThread,INFINITE);
		delete m_pMemoryThread;
		m_pMemoryThread=NULL;
	}

	m_font.DeleteObject();

	CDialog::OnDestroy();
}

void CDlgProcessMemory::OnSize(UINT nType, int cx, int cy)
{
	UPDATE_EASYSIZE;
	CDialog::OnSize(nType, cx, cy);
}


void CDlgProcessMemory::InitList()
{
	// ��ʼ��Colnum
	CString strCol,strTemp;
	for(int i=0; i<sizeof(m_MemoryColInfo)/sizeof(m_MemoryColInfo[0]); ++i)
	{
		strTemp.Format(_T("%s,%d;"),m_MemoryColInfo[i].strColName,m_MemoryColInfo[i].uWidth);
		strCol+=strTemp;
	}
	m_wndListMemory.SetHeadings(strCol);

	// ���÷��
	m_wndListMemory.ModifyStyle(0,LVS_REPORT);
	m_wndListMemory.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER); 
	
	// ��������
	m_font.CreatePointFont(80,_T("΢���ź�"));
	m_wndListMemory.SetFont(&m_font);

	
	// ��������
	InsertItem();
}

void CDlgProcessMemory::InsertItem()
{
	// �������
	m_wndListMemory.DeleteAllItems();

	// ��������
	int nIndex=0,nItem=0;
	LVITEM lView={0};
	lView.mask	=LVIF_IMAGE;

	m_cs.Lock();
	for(vector<VMQUERY>::iterator iter= m_vecMemoryInfo.begin(); iter!= m_vecMemoryInfo.end();iter++) 
	{
		m_wndListMemory.AddItem(
			GetVMBaseAddr(iter->pvRgnBaseAddress),
			GetVMProtection(iter->dwRgnProtection),
			GetVMSize(iter->RgnSize),
			GetVMState(iter->dwRgnStorage),
			GetVMBlocks(iter->dwRgnBlocks),
			GetVMMap(iter->pvRgnBaseAddress,iter->fRgnIsAStack)
			);
	}
	m_cs.Unlock();
}


// �����̶߳�ȡModule��Ϣ
LRESULT CDlgProcessMemory::OnFlushMemoryInfo(WPARAM wParam,LPARAM lParam)
{
	// �ȴ������
	if( m_pMemoryThread!=NULL )
	{
		::WaitForSingleObject(m_pMemoryThread->m_hThread,INFINITE);
		delete m_pMemoryThread;
		m_pMemoryThread=NULL;
	}

	// ���ý���ID
	if( wParam==0 )
		return -1;
	
	m_nProcessID=(int)wParam;

	// ���
	m_vecMemoryInfo.clear();

	// �����߳̽��м����ļ���Ϣ
	m_pMemoryThread=AfxBeginThread((AFX_THREADPROC)MemoryInfoThread,this,0,0,CREATE_SUSPENDED,0);
	m_pMemoryThread->m_bAutoDelete=FALSE;
	m_pMemoryThread->ResumeThread();

	return 0;
}

// ����UI
LRESULT CDlgProcessMemory::OnLoadMemoryInfo(WPARAM wParam,LPARAM lParam)
{
	// ��ֹ��˸
	m_wndListMemory.SetRedraw(FALSE);

	// �����Ϣ
	InsertItem();

	m_wndListMemory.SetRedraw(TRUE);
	m_wndListMemory.Invalidate();
	m_wndListMemory.UpdateWindow();

	return 0;
}

LPCTSTR CDlgProcessMemory::GetVMBaseAddr(PVOID pBaseAddr)
{
	static TCHAR szTemp[32]={0};

	_stprintf_s(szTemp,32,_T("%p"),pBaseAddr);

	return szTemp;
}

LPCTSTR CDlgProcessMemory::GetVMProtection(DWORD dwProtection)
{
	static LPTSTR p = TEXT("Unknown");
	switch (dwProtection & ~(PAGE_GUARD | PAGE_NOCACHE | PAGE_WRITECOMBINE)) 
	{
		case PAGE_READONLY:          p = TEXT("-R--"); break;
		case PAGE_READWRITE:         p = TEXT("-RW-"); break;
		case PAGE_WRITECOPY:         p = TEXT("-RWC"); break;
		case PAGE_EXECUTE:           p = TEXT("E---"); break;
		case PAGE_EXECUTE_READ:      p = TEXT("ER--"); break;
		case PAGE_EXECUTE_READWRITE: p = TEXT("ERW-"); break;
		case PAGE_EXECUTE_WRITECOPY: p = TEXT("ERWC"); break;
		case PAGE_NOACCESS:          p = TEXT("----"); break;
	}

	/*
	if (fShowFlags) 
	{
		_tcscat(szBuf, TEXT(" "));
		_tcscat(szBuf, (dwProtect & PAGE_GUARD)        ? TEXT("G") : TEXT("-"));
		_tcscat(szBuf, (dwProtect & PAGE_NOCACHE)      ? TEXT("N") : TEXT("-"));
		_tcscat(szBuf, (dwProtect & PAGE_WRITECOMBINE) ? TEXT("W") : TEXT("-"));
	}
	*/


	return(p);
}


LPCTSTR CDlgProcessMemory::GetVMSize(SIZE_T szSize)
{
	static TCHAR szTemp[32]={0};

	_stprintf_s(szTemp,32,_T("%16u KB"),szSize>>10);

	return szTemp;
}

LPCTSTR CDlgProcessMemory::GetVMState(DWORD dwState)
{
	static TCHAR szTemp[32]=_T("δ֪");
	switch(dwState) 
	{
		case MEM_FREE:		lstrcpy(szTemp,_T("����"));	break;
		case MEM_RESERVE:	lstrcpy(szTemp,_T("����"));	break;
		case MEM_IMAGE:		lstrcpy(szTemp,_T("ӳ��"));	break;
		case MEM_MAPPED:	lstrcpy(szTemp,_T("ӳ��"));	break;
		case MEM_PRIVATE:	lstrcpy(szTemp,_T("˽��"));	break;
	}

	return szTemp;
}

LPCTSTR CDlgProcessMemory::GetVMBlocks(DWORD dwBlock)
{
	static TCHAR szTemp[32]={0};

	_stprintf_s(szTemp,32,_T("%d"),dwBlock);

	return szTemp;
}

LPCTSTR CDlgProcessMemory::GetVMMap(PVOID pBaseAddr, BOOL bRgnIsAStack)
{
	ToolHelp toolhelp;
	toolhelp.CreateSnapshot(TH32CS_SNAPALL, m_nProcessID);
	static TCHAR szTemp[MAX_PATH]={0};


	if( pBaseAddr != NULL) 
	{
		MODULEENTRY32 me = { sizeof(me) };

		if( toolhelp.ModuleFind(pBaseAddr, &me) )
		{
			lstrcat(szTemp, me.szExePath);
		} 
		else 
		{
			HANDLE hProcess=::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,m_nProcessID);
			// This is not a module; see if it's a memory-mapped file
#ifdef _UNICODE
#define wmemset memset
#endif

			memset(szTemp,0,MAX_PATH);
			::GetMappedFileName(hProcess, pBaseAddr, szTemp, MAX_PATH);
			
			::CloseHandle(hProcess);
		}
	}

	if( bRgnIsAStack ) 
	{
		_tcscat_s(szTemp,MAX_PATH, _T("Thread Stack"));
	}

	return szTemp;
}