#pragma once

#include <rendering/buffers/monitor_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API Monitor : public MonitorData
        {
            static void Init();
            static void Free();
            static Monitor& Get();

            inline MonitorBuffer* GetBuffer() { return &m_Buffer; }

            void Flush();

        private:
            static Monitor* s_Instance;
            MonitorBuffer m_Buffer;
        };

    }

}