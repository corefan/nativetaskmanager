/*
Module : CPDH.CPP
Purpose: Defines the implementation for a wrapper class for the PDH API
Created: PJN / 06-02-1999
History: PJN / 13-09-1999 1. Fixed tab problem in the source code
2. Fixed a problem which was causing an unhandled 
exception being thrown when the PDH.DLL is not present on client machines..
PJN / 25-01-2000 1. Minor update to improve the trace statements when exceptions
are thrown

Copyright (c) 1999 - 2000 by PJ Naughter.  
All rights reserved.

*/


/////////////////////////////////  Includes  //////////////////////////////////
#include "stdafx.h"
#include "cpdh.h"
//#include "cpdhimpl.h"
#include <pdhmsg.h>



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif



////////////////////////////////// Implementation /////////////////////////////

IMPLEMENT_DYNAMIC(CPdhException, CException)

BOOL CPdhException::GetErrorMessage(LPTSTR pstrError, UINT nMaxError, PUINT pnHelpContext)
{
	ASSERT(pstrError != NULL && AfxIsValidString(pstrError, nMaxError));

	//We don't support help context at the moment
	if (pnHelpContext != NULL)
		*pnHelpContext = 0;

	//First try getting the error text from the PDH module
	LPTSTR lpBuffer;
	BOOL bRet = TRUE;
	HINSTANCE	hPDHLibrary = ::LoadLibraryA("PDH.DLL");
	if (hPDHLibrary == NULL || FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE,
		hPDHLibrary, m_Status, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
		(LPTSTR) &lpBuffer, 0, NULL) == 0)
	{
		// it failed! try Windows...
		bRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,	NULL, m_Status,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT), (LPTSTR) &lpBuffer, 0, NULL);
	}

	//If we couldn't get the error text, then set it to a empty string
	if (bRet == FALSE)
		*pstrError = '\0';
	else
	{
		lstrcpyn(pstrError, lpBuffer, nMaxError);
		bRet = TRUE;

		LocalFree(lpBuffer);
	}

	::FreeLibrary(hPDHLibrary);

	return bRet;
}

BOOL CPdhException::GetErrorMessage(CString& sError, PUINT pnHelpContext)
{
	LPTSTR pszBuffer = sError.GetBuffer(4096);
	BOOL bSuccess = GetErrorMessage(pszBuffer, 4096, pnHelpContext);
	sError.ReleaseBuffer();
	return bSuccess;
}


CPdhException::CPdhException(PDH_STATUS Status)
{
	m_Status = Status;
}

CPdhException::~CPdhException()
{
}

#ifdef _DEBUG
void CPdhException::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "m_Status = " << m_Status;
}
#endif




CPdhCounter::CPdhCounter(LPCTSTR pszFullCounterPath, DWORD dwUserData)
{
	//Validate our parameters
	ASSERT(_tcslen(pszFullCounterPath) != 0);

	//Store away the parameters
	m_dwUserData = dwUserData;
	m_sCounterPath = pszFullCounterPath;
	m_hCounter = NULL;
}

CPdhCounter::CPdhCounter(PDH_COUNTER_PATH_ELEMENTS& CounterPathElements, DWORD dwUserData)
{
	//First determine the size of the string to allocate
	DWORD dwBufferSize = 0;
	PDH_STATUS Status = PdhMakeCounterPath(&CounterPathElements, NULL, &dwBufferSize, 0);
	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);

	//Call again with the newly allocated buffer
	LPTSTR pszData = (LPTSTR) new BYTE[dwBufferSize];
	Status = PdhMakeCounterPath(&CounterPathElements, pszData, &dwBufferSize, 0);
	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);

	//Call the other version of the contructor
	CPdhCounter newValue(pszData, dwUserData);
	m_dwUserData = newValue.m_dwUserData;
	m_sCounterPath = newValue.m_sCounterPath;
	m_hCounter = newValue.m_hCounter;

	//Don't forget to free up our memory
	delete [] pszData;
}

CPdhCounter::~CPdhCounter()
{
}

DWORD CPdhCounter::GetRawValue(PDH_RAW_COUNTER& value)
{
	ASSERT(m_hCounter != NULL); //Did you forget to call CPdhQuery::Add

	DWORD dwType;
	PDH_STATUS Status = PdhGetRawCounterValue(m_hCounter, &dwType, &value);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);

	return dwType;
}

void CPdhCounter::SplitPath(CString& sMachineName, CString& sObjectName, CString& sInstanceName,
							CString& sParentInstance, DWORD& dwInstanceIndex, CString& sCounterName)
{
	ASSERT(!m_sCounterPath.IsEmpty());

	//First call the function to determine the size of buffer required
	DWORD dwBufferSize = 0;
	PDH_STATUS Status = PdhParseCounterPath(m_sCounterPath, NULL, &dwBufferSize, 0);
	if (TRUE)
	{
		//Call again with the newly allocated buffer
		PDH_COUNTER_PATH_ELEMENTS* pszBuffer = (PDH_COUNTER_PATH_ELEMENTS*) new BYTE[dwBufferSize];
		Status = PdhParseCounterPath(m_sCounterPath, pszBuffer, &dwBufferSize, 0);
		if (Status == ERROR_SUCCESS)
		{
			//Assign the data into the strings sent into us
			sMachineName    = pszBuffer->szMachineName;
			sObjectName     = pszBuffer->szObjectName;
			sInstanceName   = pszBuffer->szInstanceName;
			sParentInstance = pszBuffer->szParentInstance;
			dwInstanceIndex = pszBuffer->dwInstanceIndex;
			sCounterName    = pszBuffer->szCounterName;
		}

		//Free up the memory we used
		delete [] pszBuffer;
	}

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}

void CPdhCounter::GetInfo(CPdhCounterInfo& info)
{
	ASSERT(m_hCounter != NULL); //Did you forget to call CPdhQuery::Add

	//First call the function to determine the size of buffer required
	DWORD dwBufferSize = 0;
	PDH_STATUS Status = PdhGetCounterInfo(m_hCounter, TRUE, &dwBufferSize, NULL);
	if (TRUE)
	{
		//Call again with the newly allocated buffer
		PPDH_COUNTER_INFO pszBuffer = (PPDH_COUNTER_INFO) new BYTE[dwBufferSize];
		Status = PdhGetCounterInfo(m_hCounter, TRUE, &dwBufferSize, pszBuffer);
		if (Status == ERROR_SUCCESS)
		{
			//Assign the data into the class sent into us
			info.m_dwType          = pszBuffer->dwType;
			info.m_CVersion        = pszBuffer->CVersion;
			info.m_CStatus         = pszBuffer->CStatus;
			info.m_lScale          = pszBuffer->lScale;
			info.m_lDefaultScale   = pszBuffer->lDefaultScale;
			info.m_dwUserData      = pszBuffer->dwUserData;
			info.m_dwQueryUserData = pszBuffer->dwQueryUserData;
			info.m_sFullPath       = pszBuffer->szFullPath;
			info.m_sMachineName    = pszBuffer->szMachineName;
			info.m_sInstanceName   = pszBuffer->szObjectName;
			info.m_sParentInstance = pszBuffer->szParentInstance;
			info.m_dwInstanceIndex = pszBuffer->dwInstanceIndex;
			info.m_sCounterName    = pszBuffer->szCounterName;
			info.m_sExplainText    = pszBuffer->szExplainText;
		}

		//Free up the memory we used
		delete [] pszBuffer;
	}

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}

void CPdhCounter::CalculateFromRawValue(DWORD dwFormat, PDH_RAW_COUNTER& rawValue1, 
										PDH_RAW_COUNTER& rawValue2, PDH_FMT_COUNTERVALUE& fmtValue)
{
	ASSERT(m_hCounter != NULL); //Did you forget to call CPdhQuery::Add

	PDH_STATUS Status = PdhCalculateCounterFromRawValue(m_hCounter, dwFormat, &rawValue1, &rawValue2, &fmtValue);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}

DWORD CPdhCounter::GetFormattedValue(DWORD dwFormat, PDH_FMT_COUNTERVALUE& value)
{
	ASSERT(m_hCounter != NULL); //Did you forget to call CPdhQuery::Add

	DWORD dwType;
	PDH_STATUS Status = PdhGetFormattedCounterValue(m_hCounter, dwFormat, &dwType, &value);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);

	return dwType;
}

void CPdhCounter::ComputeStatistics(DWORD dwFormat, DWORD dwFirstEntry, DWORD dwNumEntries, 
									PPDH_RAW_COUNTER lpRawValueArray, PPDH_STATISTICS data)
{
	ASSERT(m_hCounter != NULL); //Did you forget to call CPdhQuery::Add

	PDH_STATUS Status = PdhComputeCounterStatistics(m_hCounter, dwFormat, dwFirstEntry, dwNumEntries, lpRawValueArray, data);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}

void CPdhCounter::SetScaleFactor(LONG lFactor)
{
	ASSERT(m_hCounter != NULL); //Did you forget to call CPdhQuery::Add

	PDH_STATUS Status = PdhSetCounterScaleFactor(m_hCounter, lFactor);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}




CPdhQuery::CPdhQuery()
{
	m_hQuery = NULL;
}

CPdhQuery::~CPdhQuery()
{
	//Ensure we don't leak a query handle
	Close();
}

void CPdhQuery::Open(DWORD dwUserData)
{
	PDH_STATUS Status = PdhOpenQuery(NULL, dwUserData, &m_hQuery);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}

void CPdhQuery::Close()
{
	if (m_hQuery)
		PdhCloseQuery(m_hQuery);
	m_hQuery = NULL;
}

void CPdhQuery::Add(CPdhCounter& counter)
{
	ASSERT(m_hQuery != NULL); //Did you forget to call Open

	PDH_STATUS Status = PdhAddCounter(m_hQuery, counter.m_sCounterPath, 
		counter.m_dwUserData, &counter.m_hCounter);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}

void CPdhQuery::Remove(CPdhCounter& counter)
{
	ASSERT(m_hQuery != NULL); //Did you forget to call Open

	PDH_STATUS Status = PdhRemoveCounter(counter.m_hCounter);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}

void CPdhQuery::Collect()
{
	ASSERT(m_hQuery != NULL); //Did you forget to call Open

	PDH_STATUS Status = PdhCollectQueryData(m_hQuery);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		CPdh::ThrowPdhException(Status);
}




void CPdh::ValidatePath(LPCTSTR pszFullCounterPath)
{
	PDH_STATUS Status = PdhValidatePath(pszFullCounterPath);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::ExpandPath(LPCTSTR pszWildCardPath, CStringArray& expandedPaths)
{
	//First call the function to determine the size of buffer required
	DWORD dwBufferSize = 1024;
	TCHAR pszData[1024];
	PDH_STATUS Status = PdhExpandCounterPath(pszWildCardPath, pszData, &dwBufferSize);
	if(TRUE) //(Status == PDH_MORE_DATA)
	{
		//Call again with the newly allocated buffer
		dwBufferSize++;
		LPTSTR mszExpandedPathList = new TCHAR[dwBufferSize];
		Status = PdhExpandCounterPath(pszWildCardPath, mszExpandedPathList, &dwBufferSize);

		//Parse out the MULTI_SZ strings and add each item to the string array  
		if (Status == PDH_CSTATUS_VALID_DATA)
		{
			expandedPaths.RemoveAll();
			LPTSTR ptr = mszExpandedPathList;
			while (*ptr)
			{
				expandedPaths.Add(ptr);
				ptr += _tcslen(ptr);
				ptr++;
			}
		}

		//Don't forget to free up the memory used
		delete [] mszExpandedPathList;
	}

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::ConnectMachine(LPCTSTR pszMachineName)
{
	PDH_STATUS Status = PdhConnectMachine(pszMachineName);

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::EnumMachines(CStringArray& machineNames)
{
	//First call the function to determine the size of buffer required
	DWORD dwBufferSize = 0;
	PDH_STATUS Status = PdhEnumMachines(NULL, NULL, &dwBufferSize);
	if(TRUE) //(Status == ERROR_SUCCESS)
	{
		//Call again with the newly allocated buffer
		LPTSTR mszMachineNameList = (LPTSTR) new BYTE[dwBufferSize];
		Status = PdhEnumMachines(NULL, mszMachineNameList, &dwBufferSize);

		//Parse out the MULTI_SZ strings and add each item to the string array
		if (Status == ERROR_SUCCESS)
		{
			machineNames.RemoveAll();
			LPTSTR ptr = mszMachineNameList;
			while (*ptr)
			{
				machineNames.Add(ptr);
				ptr += _tcslen(ptr);
				ptr++;
			}
		}

		//don't forget to release the memory
		delete [] mszMachineNameList;
	}

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::EnumObjects(LPCTSTR pszMachineName, CStringArray& objects, DWORD dwDetailLevel, BOOL bRefresh)
{
	//First call the function to determine the size of buffer required
	DWORD dwBufferSize = 0;
	PDH_STATUS Status = PdhEnumObjects(NULL, pszMachineName, NULL, &dwBufferSize, dwDetailLevel, bRefresh);
	if(TRUE) //(Status == ERROR_SUCCESS)
	{
		//Call again with the newly allocated buffer
		LPTSTR mszObjectList = (LPTSTR) new BYTE[dwBufferSize];
		Status = PdhEnumObjects(NULL, pszMachineName, mszObjectList, &dwBufferSize, dwDetailLevel, bRefresh);
		if (Status == ERROR_SUCCESS)
		{
			//Parse out the MULTI_SZ strings and add each item to the string array
			if (Status == ERROR_SUCCESS)
			{
				objects.RemoveAll();
				LPTSTR ptr = mszObjectList;
				while (*ptr)
				{
					objects.Add(ptr);
					ptr += _tcslen(ptr);
					ptr++;
				}
			}
		}

		//don't forget to release the memory
		delete [] mszObjectList;
	}

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::EnumObjectItems(LPCTSTR pszMachineName, LPCTSTR pszObjectName, CStringArray& counters, 
						   CStringArray& instances, DWORD dwDetailLevel)
{
	//First call the function to determine the size of buffer required
	DWORD dwCounterBufferSize = 0;
	DWORD dwInstanceBufferSize = 0;
	PDH_STATUS Status = PdhEnumObjectItems(NULL, pszMachineName, pszObjectName, NULL, &dwCounterBufferSize,
		NULL, &dwInstanceBufferSize, dwDetailLevel, 0);
	if(TRUE)// (Status == ERROR_SUCCESS)
	{
		//Call again with the newly allocated buffer
		LPTSTR pszCounterBuffer = (LPTSTR) new BYTE[dwCounterBufferSize];
		LPTSTR pszInstanceBuffer = (LPTSTR) new BYTE[dwInstanceBufferSize];
		Status = PdhEnumObjectItems(NULL, pszMachineName, pszObjectName, pszCounterBuffer, &dwCounterBufferSize,
			pszInstanceBuffer, &dwInstanceBufferSize, dwDetailLevel, 0);
		if (Status == ERROR_SUCCESS)
		{
			counters.RemoveAll();
			instances.RemoveAll();

			//Parse out the MULTI_SZ strings which form the Counter string array
			LPTSTR ptr = pszCounterBuffer;
			while (*ptr)
			{
				counters.Add(ptr);
				ptr += _tcslen(ptr);
				ptr++;
			}

			//Parse out the MULTI_SZ strings which form the Instances string array
			ptr = pszInstanceBuffer;
			while (*ptr)
			{
				instances.Add(ptr);
				ptr += _tcslen(ptr);
				ptr++;
			}
		}

		//Free up the memory we used
		delete [] pszInstanceBuffer;
		delete [] pszCounterBuffer;
	}

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::EnumObjectItems(LPCTSTR pszMachineName, LPCTSTR pszObjectName, vector<CString>& counters, 
						   vector<CString>& instances, DWORD dwDetailLevel)
{
	//First call the function to determine the size of buffer required
	DWORD dwCounterBufferSize = 0;
	DWORD dwInstanceBufferSize = 0;
	PDH_STATUS Status = PdhEnumObjectItems(NULL, pszMachineName, pszObjectName, NULL, &dwCounterBufferSize,
		NULL, &dwInstanceBufferSize, dwDetailLevel, 0);
	if(TRUE)// (Status == ERROR_SUCCESS)
	{
		//Call again with the newly allocated buffer
		CWin32Heap heapCounter(NULL,0);
		CWin32Heap heapInstance(NULL,0);
		
		LPTSTR pszCounterBuffer=(LPTSTR)heapCounter.Allocate(dwCounterBufferSize);
		LPTSTR pszInstanceBuffer=(LPTSTR)heapInstance.Allocate(dwInstanceBufferSize);

		//LPTSTR pszCounterBuffer = (LPTSTR) new BYTE[dwCounterBufferSize];
		//LPTSTR pszInstanceBuffer = (LPTSTR) new BYTE[dwInstanceBufferSize];
		Status = PdhEnumObjectItems(NULL, pszMachineName, pszObjectName, pszCounterBuffer, &dwCounterBufferSize,
			pszInstanceBuffer, &dwInstanceBufferSize, dwDetailLevel, 0);
		if (Status == ERROR_SUCCESS)
		{
			counters.clear();
			instances.clear();

			//Parse out the MULTI_SZ strings which form the Counter string array
			LPTSTR ptr = pszCounterBuffer;
			while (*ptr)
			{
				counters.push_back(ptr);
				/*ptr += _tcslen(ptr);
				ptr++;*/
				while (*ptr++);
			}

			//Parse out the MULTI_SZ strings which form the Instances string array
			ptr = pszInstanceBuffer;
			while (*ptr)
			{
				instances.push_back(ptr);
				ptr += _tcslen(ptr);
				ptr++;
			}
		}

		//Free up the memory we used
		//delete [] pszInstanceBuffer;
		//delete [] pszCounterBuffer;
		heapCounter.Free(pszInstanceBuffer);
		heapInstance.Free(pszCounterBuffer);
	}

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::GetDefaultCounter(LPCTSTR pszMachineName, LPCTSTR pszObjectName, CString& sDefaultCounterName)
{
	//First call the function to determine the size of buffer required
	DWORD dwBufferSize = 0;
	PDH_STATUS Status = PdhGetDefaultPerfCounter(NULL, pszMachineName, pszObjectName, NULL, &dwBufferSize);
	if(TRUE) //(Status == ERROR_SUCCESS)
	{
		//Call again with the newly allocated buffer
		LPTSTR pszBuffer = (LPTSTR) new BYTE[dwBufferSize];
		Status = PdhGetDefaultPerfCounter(NULL, pszMachineName, pszObjectName, pszBuffer, &dwBufferSize);
		if (Status == ERROR_SUCCESS)
		{
			//Assign the data into the string sent into us
			sDefaultCounterName = pszBuffer;
		}

		//Free up the memory we used
		delete [] pszBuffer;
	}

	//Throw an exception if anything goes wrong    
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::GetDefaultObject(LPCTSTR pszMachineName, CString& sDefaultObjectName)
{
	//First call the function to determine the size of buffer required
	DWORD dwBufferSize = 0;
	PDH_STATUS Status = PdhGetDefaultPerfObject(NULL, pszMachineName, NULL, &dwBufferSize);
	if(TRUE) //(Status == ERROR_SUCCESS)
	{
		//Call again with the newly allocated buffer
		LPTSTR pszBuffer = (LPTSTR) new BYTE[dwBufferSize];
		Status = PdhGetDefaultPerfObject(NULL, pszMachineName, pszBuffer, &dwBufferSize);
		if (Status == ERROR_SUCCESS)
		{
			//Assign the data into the string sent into us
			sDefaultObjectName = pszBuffer;
		}

		//Free up the memory we used
		delete [] pszBuffer;
	}

	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::BrowseCounters(PDH_BROWSE_DLG_CONFIG& BrowseDlgData)
{
	PDH_STATUS Status = PdhBrowseCounters(&BrowseDlgData);
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::ParseInstanceName(LPCTSTR pszInstanceString, CString& sInstanceName, CString& sParentName, DWORD& Index)
{
	//First call the function to determine the size of buffer required
	DWORD dwInstanceNameBufferSize = 0;
	DWORD dwParentNameBufferSize = 0;
	PDH_STATUS Status = PdhParseInstanceName(pszInstanceString, NULL, &dwInstanceNameBufferSize, NULL, &dwParentNameBufferSize, &Index);
	if(TRUE) //(Status == ERROR_SUCCESS)
	{
		//Call again with the newly allocated buffer
		LPTSTR pszInstanceName = (LPTSTR) new BYTE[dwInstanceNameBufferSize];
		LPTSTR pszParentName = (LPTSTR) new BYTE[dwParentNameBufferSize];
		Status = PdhParseInstanceName(pszInstanceString, pszInstanceName, &dwInstanceNameBufferSize, pszParentName, 
			&dwParentNameBufferSize, &Index);
		if (Status == ERROR_SUCCESS)
		{
			//Assign the data into the strings sent into us
			sInstanceName = pszInstanceName;
			sParentName = pszParentName;
		}

		//Free up the memory we used
		delete [] pszParentName;
		delete [] pszInstanceName;
	}

	//Throw an exception if anything goes wrong  
	if (Status != ERROR_SUCCESS)
		ThrowPdhException(Status);
}

void CPdh::ThrowPdhException(PDH_STATUS Status)
{
	CPdhException* pException = new CPdhException(Status);

	TRACE1("Warning: throwing CPdhException for error %x\n", Status);
	THROW(pException);
}
