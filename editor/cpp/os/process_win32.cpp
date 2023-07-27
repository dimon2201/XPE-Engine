#ifdef WINDOWS

#include <os/process.h>

#include <Windows.h>

namespace focus {

    namespace os {

        VOID startup(LPCTSTR lpApplicationName)
        {
//            // additional information
//            STARTUPINFO si;
//            PROCESS_INFORMATION pi;
//
//            // set the size of the structures
//            ZeroMemory(&si, sizeof(si));
//            si.cb = sizeof(si);
//            ZeroMemory(&pi, sizeof(pi));
//
//            // start the program up
//            CreateProcess(lpApplicationName,   // the path
//                   argv[1],        // Command line
//                   NULL,           // Process handle not inheritable
//                   NULL,           // Thread handle not inheritable
//                   FALSE,          // Set handle inheritance to FALSE
//                   0,              // No creation flags
//                   NULL,           // Use parent's environment block
//                   NULL,           // Use parent's starting directory
//                   &si,            // Pointer to STARTUPINFO structure
//                   &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
//            );
//
//            // Close process and thread handles.
//            CloseHandle(pi.hProcess);
//            CloseHandle(pi.hThread);
        }

        void Process::StartExe(const char* path)
        {
            ShellExecute(NULL, "open", path, NULL, NULL, SW_SHOWDEFAULT);
        }

        static HINSTANCE s_DLL = nullptr;

        bool Process::InitDLL(const char *dllpath)
        {
            if (s_DLL != nullptr) {
                FreeLibrary(s_DLL);
                s_DLL = nullptr;
            }

            // load DLL
            s_DLL = static_cast<HINSTANCE>(LoadLibrary(dllpath));
            if (s_DLL == nullptr) {
                LogError("Failed to load DLL library from {}", dllpath);
                return false;
            }

            return true;
        }

        void Process::FreeDLL() {
            FreeLibrary(s_DLL);
            s_DLL = nullptr;
        }

        DLLProc Process::LoadDLLProc(const char* signature)
        {
            return GetProcAddress(s_DLL, signature);
        }

    }

}

#endif // WINDOWS