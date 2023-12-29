#ifdef WINDOWS

#define NOMINMAX
#include <Windows.h>

namespace xpe {

    namespace core {

        void cThread::SetFormat(const char* name, ePriority priority)
        {
            HANDLE handle = (HANDLE) m_Thread.native_handle();

            // set thread into dedicated core
            DWORD_PTR affinityMask = 1ull << m_ID;
            DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
            assert(affinity_result > 0);

            // set priority
            int winPriority = THREAD_PRIORITY_NORMAL;
            switch (priority) {
                case ePriority::LOWEST:
                    winPriority = THREAD_PRIORITY_LOWEST;
                    break;
                case ePriority::NORMAL:
                    winPriority = THREAD_PRIORITY_NORMAL;
                    break;
                case ePriority::HIGHEST:
                    winPriority = THREAD_PRIORITY_HIGHEST;
                    break;
            }

            BOOL priority_result = SetThreadPriority(handle, priority);
            assert(priority_result != 0);

            // set thread name
            std::wstringstream wss;
            wss << name << "-" << m_ID;
            HRESULT hr = SetThreadDescription(handle, wss.str().c_str());
            assert(SUCCEEDED(hr));
        }

    }

}

#endif // WINDOWS