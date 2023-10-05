#pragma once

namespace xpe {

    namespace core {

        class ENGINE_API Thread final {

        public:
            enum ePriority {
                LOWEST, NORMAL, HIGHEST
            };

            struct ENGINE_API Format final {
                ePriority Priority;
                string Name;
            };

            Thread(u32 id, std::thread& thread);

            static void CurrentSleep(u32 millis);
            static u32 CurrentID();

            void SetFormat(const Format& format);

        private:
            u32 m_ID;
            std::thread& m_Thread;

        };

    }

}