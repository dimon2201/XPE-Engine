#pragma once

namespace xpe
{
    namespace core
    {
        typedef long long (*fProc)();

        class ENGINE_API MProc final
        {

        public:
            template<typename T>
            static T* Load(const char* dllpath, const char* objectFactorySignature);

        private:
            static bool Init(const char* dllpath);
            static void Free();
            static fProc LoadProc(const char* signature);

        };

        template<typename T>
        T* MProc::Load(const char *dllpath, const char* objectFactorySignature)
        {
            T* object = nullptr;

            // init dll
            bool initialized = Init(dllpath);
            if (!initialized)
            {
                return nullptr;
            }

            // load DLL proc
            fProc dllProc = LoadProc(objectFactorySignature);
            if (dllProc == nullptr)
            {
                Free();
                LogError("Failed to load DLL process function {} from {}", objectFactorySignature, dllpath);
                return nullptr;
            }

            // call DLL proc and receive object
            typedef T* (*CreateObjectFn)();
            object = ((CreateObjectFn) dllProc)();

            // check object
            if (object == nullptr)
            {
                Free();
                LogError("Failed to load object from DLL process function {} from {}", objectFactorySignature, dllpath);
                return nullptr;
            }

            return object;
        }
    }
}