#include "stdafx.h"
#include "AdjustPrivilege.h"


CPrivilegeEnabler::CPrivilegeEnabler()
{
	LPCTSTR arPrivelegeNames[]	=	{
										SE_BACKUP_NAME, //	these two are required for the FILE_FLAG_BACKUP_SEMANTICS flag used in the call to 
										SE_RESTORE_NAME,//  CreateFile() to open the directory handle for ReadDirectoryChangesW

										SE_CHANGE_NOTIFY_NAME, //just to make sure...it's on by default for all users.
										SE_DEBUG_NAME

										//<others here as needed>
									};
	for(int i = 0; i < sizeof(arPrivelegeNames) / sizeof(arPrivelegeNames[0]); ++i)
	{
		if( !EnablePrivilege(arPrivelegeNames[i], TRUE) )
		{
			TRACE("Unable to enable privilege: %s	--	GetLastError(): %d\n", arPrivelegeNames[i], GetLastError());
			TRACE("CDirectoryChangeWatcher notifications may not work as intended due to insufficient access rights/process privileges.\n");
			TRACE("File: %s Line: %d\n", __FILE__, __LINE__);
		}
	}
}

CPrivilegeEnabler & CPrivilegeEnabler::Instance()
{
	static CPrivilegeEnabler theInstance;//constructs this first time it's called.
	return theInstance;
}


//
//	I think this code is from a Jeffrey Richter book...
//
//	Enables user priviledges to be set for this process.
//	
BOOL CPrivilegeEnabler::EnablePrivilege(LPCTSTR pszPrivName, BOOL fEnable /*= TRUE*/) 
{    
	BOOL fOk = FALSE;    
	// Assume function fails    
	HANDLE hToken;    
	// Try to open this process's access token    
	if (OpenProcessToken(GetCurrentProcess(), 		
					TOKEN_ADJUST_PRIVILEGES, &hToken)) 	
	{        
		// privilege        
		TOKEN_PRIVILEGES tp = { 1 };        

		if( LookupPrivilegeValue(NULL, pszPrivName,  &tp.Privileges[0].Luid) )
		{
			tp.Privileges[0].Attributes = fEnable ?  SE_PRIVILEGE_ENABLED : 0;

			AdjustTokenPrivileges(hToken, FALSE, &tp, 			      
									sizeof(tp), NULL, NULL);

			fOk = (GetLastError() == ERROR_SUCCESS);		
		}
		CloseHandle(hToken);	
	}	

	return(fOk);
}