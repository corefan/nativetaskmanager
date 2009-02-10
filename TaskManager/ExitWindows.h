/**********************************************************************************
/* File Name		:	ExitWindows.h
/*																				   
/* Author			:	Jais T Mathew (jaistmathew@yahoo.com.com)							   
/*																				   
/* Project			:	NetWork developement Toolkit
/*																				   
/* Module			:	Node Manager
/*																				   
/* Purpose			:	Declaration of the CExitWindows class
/*						Abstract functions for system management. Implements the 
/*						all of mixed enviroument.
/*																				   
/* Copy Right		:	Copyright ?ktssoftwares.Com
/*																				   
/* Notes			:															   
/*																				   
/* History			:															   
/*		Created		:   14 Dec 2002
/*		Version		:	1.0.0.0													   
/*																				   
/*		Updated #1	:	6 May 2002															   
/*																				   
/**********************************************************************************/


#ifndef	__EXITWINDOWS_H__
#define	__EXITWINDOWS_H__


#define WINNT							0x11

#define WIN9X							0x12

#define EXITWINDOWS_FAILED				0x13

#define EXITWINDOWS_SUCESS				0x14

#define ADJUST_TOCKEN_SUCESS			0x15

#define ADJUST_TOCKEN_FAILED			0x15

#define ADJUST_PRIVILEGE_FAILED			0x16

#define OPENING_PROCESS_TOKEN_FAILED	0x100


class CExitWindows  
{
public:
	CExitWindows();
	virtual			~CExitWindows();


public:
	static	BOOL	SetItAsLastShutDownProcess();

	BOOL			LogOff();
	BOOL			Restart();
	BOOL			ShutDown();
	BOOL			ShutDown(LPTSTR lpszMachine=NULL,LPTSTR lpszMessage=NULL,DWORD dwTime=0,
		BOOL bForce=FALSE,BOOL bRestart=TRUE);
	BOOL			LockWindow();
	BOOL			StandBy();
	BOOL			Hibernate();

	BOOL			ExitWindowsExt(UINT nFlag,DWORD dwType);
	BOOL			ForceShutDown();
	BOOL			ForceLogOff();
	BOOL			ForceReStart();
	
	BOOL			AbortShutdown(LPTSTR lpszMachine=NULL);

	OSVERSIONINFO	GetOsVersionInfo()const;
	UINT			GetOsVersion();

protected:
	BOOL			AdjustProcessTokenPrivilege();

private:
	UINT			m_nVersionType;
	OSVERSIONINFO	m_osvi;
};

#endif 
