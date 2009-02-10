#ifndef __PRIVILEGE__H
#define __PRIVILEGE__H

//
//	Enables privileges for this process
//	first time CDirectoryChangeWatcher::WatchDirectory() is called.
//
//	It's a singleton.
//
class CPrivilegeEnabler
{
private:
	CPrivilegeEnabler();//ctor
public:
	~CPrivilegeEnabler(){};
	
	static CPrivilegeEnabler & Instance();
	static BOOL EnablePrivilege(LPCTSTR pszPrivName, BOOL fEnable=TRUE);
	//friend static CPrivilegeEnabler & Instance();
};

#endif