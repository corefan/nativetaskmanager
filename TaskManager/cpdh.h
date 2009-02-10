/*
Module : CPDH.H
Purpose: Defines the interface for a wrapper class for the PDH API

Copyright (c) 1999 - 2000 by PJ Naughter.  
All rights reserved.

*/

#ifndef __CPDH_H__
#define __CPDH_H__


///////////////////////////////// Includes //////////////////////////////
#include <pdh.h>
#include <vector>

using namespace std;



///////////////////////////////// Classes ///////////////////////////////

//Represents an exception condition relating to a PDH classes as below
class CPdhException : public CException
{
public:
	//Constructor / Destructors
	CPdhException(PDH_STATUS Status);
	~CPdhException();

	//Data
	PDH_STATUS m_Status;

	//Methods
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual BOOL GetErrorMessage(LPTSTR lpstrError, UINT nMaxError,	PUINT pnHelpContext = NULL);
	virtual BOOL GetErrorMessage(CString& sError,	PUINT pnHelpContext = NULL);

protected:
	DECLARE_DYNAMIC(CPdhException)
};


//Info relating to a PDH counter as returned in CPdhCounter::GetInfo
class CPdhCounterInfo
{
public:
	DWORD   m_dwType;
	DWORD   m_CVersion;
	DWORD   m_CStatus;
	LONG    m_lScale;
	LONG    m_lDefaultScale;
	DWORD   m_dwUserData;
	DWORD   m_dwQueryUserData;
	CString m_sFullPath;
	CString m_sMachineName;
	CString m_sInstanceName;
	CString m_sParentInstance;
	DWORD   m_dwInstanceIndex;
	CString m_sCounterName;
	CString m_sExplainText;
};


//Class which encapsulates a PDH counter
class CPdhCounter
{
public:
	//Constructors / Destructors
	CPdhCounter(LPCTSTR pszFullCounterPath, DWORD dwUserData = 0);
	CPdhCounter(PDH_COUNTER_PATH_ELEMENTS& CounterPathElements, DWORD dwUserData = 0);
	~CPdhCounter();  

	//Methods
	DWORD GetRawValue(PDH_RAW_COUNTER& value);
	void  SplitPath(CString& sMachineName, CString& sObjectName, CString& sInstanceName,
		CString& sParentInstance, DWORD& dwInstanceIndex, CString& sCounterName);
	void  GetInfo(CPdhCounterInfo& info);
	void  CalculateFromRawValue(DWORD dwFormat, PDH_RAW_COUNTER& rawValue1, 
		PDH_RAW_COUNTER& rawValue2, PDH_FMT_COUNTERVALUE& fmtValue);
	DWORD GetFormattedValue(DWORD dwFormat, PDH_FMT_COUNTERVALUE& value);
	void  ComputeStatistics(DWORD dwFormat, DWORD dwFirstEntry, DWORD dwNumEntries, 
		PPDH_RAW_COUNTER lpRawValueArray, PPDH_STATISTICS data);
	void  SetScaleFactor(LONG lFactor);

protected:
	//Data
	HCOUNTER m_hCounter;
	CString  m_sCounterPath;
	DWORD    m_dwUserData;

	friend class CPdhQuery;
};


//Class which encapsulates a PDH query
class CPdhQuery
{
public:
	//Constructors / Destructors
	CPdhQuery();
	~CPdhQuery();

	//Methods
	void Open(DWORD dwUserData = 0);
	void Close();
	void Add(CPdhCounter& counter);
	void Remove(CPdhCounter& counter);
	void Collect();

protected:
	//Data
	HQUERY m_hQuery;
};


//Other PDH functions which don't belong anywhere else
class CPdh
{
public:
	//Static Methods
	static void ValidatePath(LPCTSTR pszFullCounterPath);
	static void ExpandPath(LPCTSTR pszWildCardPath, CStringArray& expandedPaths);
	static void ConnectMachine(LPCTSTR pszMachineName);
	static void EnumMachines(CStringArray& machineNames);
	static void EnumObjects(LPCTSTR pszMachineName, CStringArray& objects, 
		DWORD dwDetailLevel, BOOL bRefresh);
	static void EnumObjectItems(LPCTSTR pszMachineName, LPCTSTR pszObjectName, 
		CStringArray& counters, CStringArray& instances, 
		DWORD dwDetailLevel);
	static void EnumObjectItems(LPCTSTR pszMachineName, LPCTSTR pszObjectName, 
		vector<CString>& counters, vector<CString>& instances, 
		DWORD dwDetailLevel);
	static void GetDefaultCounter(LPCTSTR pszMachineName, LPCTSTR pszObjectName, 
		CString& sDefaultCounterName);
	static void GetDefaultObject(LPCTSTR pszMachineName, CString& sDefaultObjectName);
	static void BrowseCounters(PDH_BROWSE_DLG_CONFIG& BrowseDlgData);
	static void ParseInstanceName(LPCTSTR pszInstanceString, CString& sInstanceName, 
		CString& sParentName, DWORD& Index);
	static void ThrowPdhException(PDH_STATUS Status);
};



#endif //__CPDH_H__