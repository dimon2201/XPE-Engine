#ifdef WINDOWS

#define NOMINMAX
#include <Windows.h>

namespace xpe {

    namespace core {

        void Thread::SetFormat(const Thread::Format& format)
        {
            HANDLE handle = (HANDLE) m_Thread.native_handle();

            // set thread into dedicated core
            DWORD_PTR affinityMask = 1ull << m_ID;
            DWORD_PTR affinity_result = SetThreadAffinityMask(handle, affinityMask);
            assert(affinity_result > 0);

            // set priority
            int priority = THREAD_PRIORITY_NORMAL;
            switch (format.Priority) {
                case ePriority::LOWEST:
                    priority = THREAD_PRIORITY_LOWEST;
                    break;
                case ePriority::NORMAL:
                    priority = THREAD_PRIORITY_NORMAL;
                    break;
                case ePriority::HIGHEST:
                    priority = THREAD_PRIORITY_HIGHEST;
                    break;
            }

            BOOL priority_result = SetThreadPriority(handle, priority);
            assert(priority_result != 0);

            // set thread name
            std::wstringstream wss;
            wss << format.Name.c_str() << "-" << m_ID;
            HRESULT hr = SetThreadDescription(handle, wss.str().c_str());
            assert(SUCCEEDED(hr));
        }

    }

}

#endif // WINDOWS