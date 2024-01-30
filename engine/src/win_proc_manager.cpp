#ifdef WINDOWS

#include <proc_manager.hpp>

#include <Windows.h>

namespace xpe
{
    namespace core
    {
        static HINSTANCE s_DLL = nullptr;

        bool MProc::Init(const char *dllpath)
        {
            if (s_DLL != nullptr)
            {
                FreeLibrary(s_DLL);
                s_DLL = nullptr;
            }

            s_DLL = static_cast<HINSTANCE>(LoadLibrary(dllpath));

            if (s_DLL == nullptr)
            {
                LogError("Failed to load DLL library from {}", dllpath);
                return false;
            }

            return true;
        }

        void MProc::Free()
        {
            FreeLibrary(s_DLL);
            s_DLL = nullptr;
        }

        fProc MProc::LoadProc(const char* signature)
        {
            return GetProcAddress(s_DLL, signature);
        }
    }
}

#endif // WINDOWS