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
		HICON	m_hIcon;					// 进程图标
		DWORD	m_dwID;						// 进程ID
		DWORD	m_dwParentID;				// 父进程
	
		BOOL	m_bSuspend;					// 挂起进程状态

		string	m_strProcessName;			// 进程名字
		string	m_strProcessID;				// 进程ID
		
		string	m_strCPUUsage;				// 进程CPU使用率
		string	m_strCPUTime;				// 进程CPU使用时间

		string  m_strMemUsage;				// 内存使用率
		string	m_strMemDelta;				// 内存增量
		string	m_strMemPeakUsage;			// 内存使用峰值

		string	m_strPageFaults;			// 页面错误
		string	m_strPageSize;				// 虚拟内存使用率
		string	m_strPagePoolUsage;			// 页面池使用率
		string	m_strPageNonPollUsage;		// 非页面池使用率

		string	m_strUserObject;			// 用户对象
		string	m_strGDIObject;				// GDI对象

		string	m_strSessionID;				// Session ID
		string	m_strHandleCount;			// 句柄总数

		string	m_strIORead;				// IO执行读取数
		string	m_strIOReadByte;			// IO读取字节数
		string	m_strIOWrite;				// IO执行写入数
		string	m_strIOWriteByte;			// IO写入字节数
		string	m_strIOOther;				// IO执行其他数
		string	m_strIOOtherByte;			// IO其他字节数

		string	m_strPriority;				// 进程优先级
		DWORD	m_dwPriority;				
		string	m_strUserName;				// 进程关联用户名
		string  m_strThreadCount;			// 进程包含线程数
		string  m_strProcessPath;			// 进程执行路径

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
		void Clear();		// 清空
		void Init();		// 初始化
		void Enum();		// 遍历进程时间

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
		void	TranslateFilename(LPCTSTR szFilename, LPTSTR szWin32Name);	// 转换路径帮助函数
		void	FormatNumber(DWORD dwNumber, TCHAR *szNumber, TCHAR cSeparator, DWORD GroupLength);// 转换数据格式帮助函数

	public:
		TvecProInfo				m_vecProInfo;
		multimap<DWORD,DWORD>	m_mapProcessTree;	// 进程的子进程容器
};	// end of class

};	// end of namespace

#endif