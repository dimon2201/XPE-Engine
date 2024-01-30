#pragma once

namespace xpe
{
    namespace core
    {
        class ENGINE_API cThread final
        {

        public:
            enum ePriority
            {
                LOWEST,
                NORMAL,
                HIGHEST
            };

            cThread(u32 id, std::thread& thread);

            static void CurrentSleep(u32 millis);
            static u32 GetCurrentID();

            void SetFormat(const char* name, ePriority priority);

        private:
            u32 m_ID;
            std::thread& m_Thread;

        };
    }
}