#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API MonitorData : public core::Object
        {
            float Exposure = 1.0f;
            float Gamma = 2.2f;
        };

        struct ENGINE_API MonitorBuffer : public ItemBuffer<MonitorData>
        {
            MonitorBuffer() : ItemBuffer<MonitorData>(K_SLOT_MONITOR) {}
        };

    }

}