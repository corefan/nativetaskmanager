#ifndef __WTS_H_
#define __WTS_H_

#include <WtsApi32.h>
#include <windows.h>
#include <vector>
#include <string>

using namespace std;

#pragma comment(lib,"Wtsapi32")



namespace WTS
{
	#ifdef _UNICODE
	#define string wstring
	#endif


	typedef struct tagWTSProcessInfo
	{
		DWORD m_dwPID;					// Process ID
		DWORD m_dwSID;					// Session ID
		PSID  m_pUID;					// User ID

		string m_strPID;				
		string m_strSessionID;
		string m_strProccessName;		// Process Name
		string m_strUserName;			// User Name
	}WTSProcessInfo;

	class EnumWTS
	{
	public:
		EnumWTS(void);
		virtual ~EnumWTS(void);

	public:
		void WTSOpenServer(LPTSTR szServer=NULL);
		VOID WTSCloseServer();
		BOOL ProcessIdToSessionId(DWORD dwProcessId, DWORD *pSessionId);

		BOOL WTSEnumerateProcesses();
		BOOL WTSEnumerateSessions(HANDLE hServer, DWORD Reserved, DWORD Version, 
			PWTS_SESSION_INFO* ppSessionInfo, DWORD* pCount);
		BOOL WTSQuerySessionInformation(HANDLE hServer, DWORD SessionId, 
			WTS_INFO_CLASS WTSInfoClass, LPTSTR *ppBuffer, DWORD *pBytesReturned);

		string GetProcessUserName(DWORD dwPID);

		static LPCTSTR WTSGetConnectState(WTS_CONNECTSTATE_CLASS eClass);

	private:
		BOOL GetUserNameFromSid(PSID pUserSid, LPTSTR pszUserName);
		VOID WTSFreeMemory(PVOID pMemory);

	private:
		HANDLE	m_hServer;
		TCHAR	m_szComputerName[32];
		vector<WTSProcessInfo> m_vecProcInfo;

	};// end of class

}// end of namespace

#endif