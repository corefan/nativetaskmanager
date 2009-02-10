#include "StdAfx.h"
#include "CriticalSection.h"

CriticalSection::CriticalSection()
{
	::InitializeCriticalSection(&m_CriticalSection);
}


CriticalSection::~CriticalSection()
{
	::DeleteCriticalSection(&m_CriticalSection);
}

 

