#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sMonitorData final
        {
            float Exposure = 1.0f;
            float Gamma = 2.2f;
        };

        struct ENGINE_API sMonitorBuffer : public sItemBuffer<sMonitorData>
        {
            sMonitorBuffer() : sItemBuffer<sMonitorData>(K_SLOT_MONITOR) {}
        };

    }

}