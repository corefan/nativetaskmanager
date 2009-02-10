#ifndef __CRITICAL_SECTION_H
#define __CRITICAL_SECTION_H

#include <windows.h>

 
class CriticalSection
{
      public:
            CriticalSection();

            virtual ~CriticalSection();
            void Lock()
			{
				::EnterCriticalSection(&m_CriticalSection);
			}

			void Unlock()
			{
				::LeaveCriticalSection(&m_CriticalSection);
			}

      private:
            CRITICAL_SECTION m_CriticalSection;
};


#endif