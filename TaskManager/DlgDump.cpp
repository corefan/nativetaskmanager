// DlgDump.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TaskManager.h"
#include "DlgDump.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;
// CDlgDump �Ի���

#ifdef _UNICODE
#define ifstream wifstream
#define string	wstring
#endif


IMPLEMENT_DYNAMIC(CDlgDump, CDialog)

CDlgDump::CDlgDump(CString strModulePath,CString strModuleName,CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDump::IDD, pParent)
	, m_strModulePath(strModulePath)
	, m_strModuleName(strModuleName)
{

}

CDlgDump::~CDlgDump()
{
}

void CDlgDump::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DUMP_TEXT, m_strDumpText);
}


BEGIN_MESSAGE_MAP(CDlgDump, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DUMP_OK, &CDlgDump::OnBnClickedButtonDumpOk)
	ON_BN_CLICKED(IDC_BUTTON_DUMP_CANCEL, &CDlgDump::OnBnClickedButtonDumpCancel)
END_MESSAGE_MAP()


// CDlgDump ��Ϣ�������
BOOL CDlgDump::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô��ڱ���
	CString strWindowName;
	GetWindowText(strWindowName);

	strWindowName+=_T(": ")+m_strModuleName;
	SetWindowText(strWindowName);


	GetDlgItem(IDC_CHECK_DUMP1)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CDlgDump::OnBnClickedButtonDumpOk()
{
	CString strCommandLine;
	CString strTemp;

	// ��ȡ�����в���
	for(UINT i=IDC_CHECK_DUMP1; i<IDC_CHECK_DUMP1+24; ++i)
	{
		if( ((CButton *)GetDlgItem(i))->GetCheck()==BST_CHECKED )
		{
			if( i==IDC_CHECK_DUMP1 )
			{
				strCommandLine=_T("ALL");
				break;
			}
			else
			{
				GetDlgItem(i)->GetWindowText(strTemp);
				strCommandLine+=+strTemp;
			}
		}

	}

	/*TCHAR szBuf[256]={0},szTemp[256]={0};
	wsprintf(szBuf,_T("dumpbin.exe -%s /OUT:dllinfo.txt"),strCommandLine);
	wsprintf(szTemp,_T("%s \"%s\""),szBuf,m_strModulePath);
	strCommandLine=szTemp;*/

	strCommandLine=_T("Usefull Tools\\DUMPBIN.EXE -")+strCommandLine+_T(" /OUT:dllinfo.txt ")+_T("\"")+m_strModulePath+_T("\"");

	// �ض������
	PrintText(strCommandLine);
}

void CDlgDump::OnBnClickedButtonDumpCancel()
{
	OnCancel();
}


void CDlgDump::PrintText(CString &strCommandLine)
{
	// ���
	m_strDumpText=_T("");


	// ���ش�ִ��
	STARTUPINFO si={0}; 
	PROCESS_INFORMATION pi={0};
	
    si.cb				= sizeof(STARTUPINFO); 
    GetStartupInfo(&si); 
    si.wShowWindow		= SW_HIDE; 
    si.dwFlags			= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 

	if( !::CreateProcess(NULL,(LPTSTR)(LPCTSTR)strCommandLine,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi) )
	{
		int nLastError=::GetLastError();
		CString strError;
		strError.Format(_T("����: %d"),nLastError);

		MessageBox(strError,_T("����"),MB_ICONERROR);
		return;
	}

	::WaitForSingleObject(pi.hProcess,INFINITE);
	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);

	// ��ȡ�ļ�����ʾ
	string strLine;
	ifstream infile(_T("dllinfo.txt"));

	if( !infile )
	{
		return;
	}

	while(getline(infile,strLine))
	{
		
		strLine+=_T("\r\n");

		m_strDumpText+=strLine.c_str();
	}
	infile.close();
	
	UpdateData(FALSE);

	


	//HANDLE hWrite=NULL,hRead=NULL;

	//SECURITY_ATTRIBUTES sa;
	//sa.bInheritHandle		=TRUE;
	//sa.lpSecurityDescriptor	=NULL;
	//sa.nLength				=sizeof(sa);

	//if( !::CreatePipe(&hRead,&hWrite,&sa,0) )
	//{
	//	int nLastError=::GetLastError();
	//	CString strError;
	//	strError.Format(_T("����: %d"),nLastError);

	//	MessageBox(strError,_T("����"),MB_ICONERROR);
	//	return;
	//}
	//
	//STARTUPINFO si={0}; 
	//PROCESS_INFORMATION pi={0};
	//
 //   si.cb				= sizeof(STARTUPINFO); 
 //   GetStartupInfo(&si); 
 //   si.hStdError		= hWrite; 
 //   si.hStdOutput		= hWrite; 
 //   si.wShowWindow		= SW_HIDE; 
 //   si.dwFlags			= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; 

	////ShellExecute(this->m_hWnd,NULL,(LPTSTR)(LPCTSTR)strCommandLine,NULL,NULL,SW_HIDE);

	//if( !::CreateProcess(NULL,(LPTSTR)(LPCTSTR)strCommandLine,NULL,NULL,TRUE,0,NULL,NULL,&si,&pi) )
	//{
	//	int nLastError=::GetLastError();
	//	CString strError;
	//	strError.Format(_T("����: %d"),nLastError);

	//	MessageBox(strError,_T("����"),MB_ICONERROR);

	//	::CloseHandle(hRead);
	//	::CloseHandle(hWrite);
	//	return;
	//}

	//::WaitForSingleObject(pi.hProcess,INFINITE);

	//TCHAR szBuf[2*4096]={0};
	//DWORD dwReadByte=0;


	//int nIndex=0;

	//
	//if( ::ReadFile(hRead,szBuf,2*4095,&dwReadByte,NULL)==NULL )
	//{
	//	return;
	//}
	//
	//nIndex+=dwReadByte;
	//m_strDumpText+=szBuf;
	//UpdateData(FALSE);
	//

	//::CloseHandle(pi.hProcess);
	//::CloseHandle(pi.hThread);
	//::CloseHandle(hRead);
	//::CloseHandle(hWrite);
}

