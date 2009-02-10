#ifndef __ENUM_PROCESS_H__
#define __ENUM_PROCESS_H__


#include <string>
#include <vector>
#include <map>


using namespace std;


#ifdef _UNICODE
#define string wstring
#endif



namespace enumProcess
{

class ProcessInfo
{
	public:
		HICON	m_hIcon;					// ����ͼ��
		DWORD	m_dwID;						// ����ID
		DWORD	m_dwParentID;				// ������
	
		BOOL	m_bSuspend;					// �������״̬

		string	m_strProcessName;			// ��������
		string	m_strProcessID;				// ����ID
		
		string	m_strCPUUsage;				// ����CPUʹ����
		string	m_strCPUTime;				// ����CPUʹ��ʱ��

		string  m_strMemUsage;				// �ڴ�ʹ����
		string	m_strMemDelta;				// �ڴ�����
		string	m_strMemPeakUsage;			// �ڴ�ʹ�÷�ֵ

		string	m_strPageFaults;			// ҳ�����
		string	m_strPageSize;				// �����ڴ�ʹ����
		string	m_strPagePoolUsage;			// ҳ���ʹ����
		string	m_strPageNonPollUsage;		// ��ҳ���ʹ����

		string	m_strUserObject;			// �û�����
		string	m_strGDIObject;				// GDI����

		string	m_strSessionID;				// Session ID
		string	m_strHandleCount;			// �������

		string	m_strIORead;				// IOִ�ж�ȡ��
		string	m_strIOReadByte;			// IO��ȡ�ֽ���
		string	m_strIOWrite;				// IOִ��д����
		string	m_strIOWriteByte;			// IOд���ֽ���
		string	m_strIOOther;				// IOִ��������
		string	m_strIOOtherByte;			// IO�����ֽ���

		string	m_strPriority;				// �������ȼ�
		DWORD	m_dwPriority;				
		string	m_strUserName;				// ���̹����û���
		string  m_strThreadCount;			// ���̰����߳���
		string  m_strProcessPath;			// ����ִ��·��

};


typedef vector<ProcessInfo> TvecProInfo;


class EnumProcess
{
	public:
		EnumProcess();
		EnumProcess(const EnumProcess &src);
		virtual ~EnumProcess();

		TvecProInfo &GetProcessInfo()
		{
			return m_vecProInfo;
		}
		multimap<DWORD,DWORD> &GetProcessTree()
		{
			return m_mapProcessTree;
		}
		void Clear();		// ���
		void Init();		// ��ʼ��
		void Enum();		// ��������ʱ��

	private:
		void	GetProcessName(ProcessInfo &proInfo);
		void	GetProcessID(ProcessInfo &proInfo,DWORD dwID);

		void	GetCPUUsage(ProcessInfo &proInfo,DWORD dwID);
		void	GetCPUTime(ProcessInfo &proInfo,DWORD dwID);

		void	GetMemInfo(ProcessInfo &proInfo,DWORD dwID);

		void	GetResourceObject(ProcessInfo &proInfo,DWORD dwID);

		void	GetProcessSessionID(ProcessInfo &proInfo,DWORD dwID);
		void	GetProcessHandleCount(ProcessInfo &proInfo,DWORD dwID);

		void	GetProcessIOInfo(ProcessInfo &proInfo,DWORD dwID);

		void	GetProcessPriority(ProcessInfo &proInfo,DWORD dwID);

		void	GetProcessUserName(ProcessInfo &proInfo,DWORD dwID);
		void	GetThreadCount(ProcessInfo &proInfo,DWORD dwCount);
		void	GetProcessPath(ProcessInfo &proInfo,DWORD dwID);
		void	GetProcessIcon(ProcessInfo &proInfo,LPCTSTR lpszPath);

	private:
		void	TranslateFilename(LPCTSTR szFilename, LPTSTR szWin32Name);	// ת��·����������
		void	FormatNumber(DWORD dwNumber, TCHAR *szNumber, TCHAR cSeparator, DWORD GroupLength);// ת�����ݸ�ʽ��������

	public:
		TvecProInfo				m_vecProInfo;
		multimap<DWORD,DWORD>	m_mapProcessTree;	// ���̵��ӽ�������
};	// end of class

};	// end of namespace

#endif