#pragma once

namespace focus {

    namespace core {

        typedef long long (*DLLProc)();

        class ProcessManager final {

        public:
            static void StartExe(const char* path);

            template<typename T>
            static T* LoadObjectFromDLL(const char* dllpath, const char* objectFactorySignature);

        private:
            static bool InitDLL(const char* dllpath);
            static void FreeDLL();
            static DLLProc LoadDLLProc(const char* signature);

        };

        template<typename T>
        T* ProcessManager::LoadObjectFromDLL(const char *dllpath, const char* objectFactorySignature)
        {
            T* object = nullptr;

            // init dll
            bool initialized = InitDLL(dllpath);
            if (!initialized) {
                return nullptr;
            }

            // load DLL proc
            DLLProc dllProc = LoadDLLProc(objectFactorySignature);
            if (dllProc == nullptr) {
                FreeDLL();
                LogError("Failed to load DLL process function {} from {}", objectFactorySignature, dllpath);
                return nullptr;
            }

            // call DLL proc and receive object
            typedef T* (*CreateObjectFn)();
            object = ((CreateObjectFn) dllProc)();

            // check object
            if (object == nullptr) {
                FreeDLL();
                LogError("Failed to load object from DLL process function {} from {}", objectFactorySignature, dllpath);
                return nullptr;
            }

            return object;
        }

    }

}