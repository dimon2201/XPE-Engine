#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sSsaoData final
        {
            int DirectionCount = 4;
            int SampleCount = 4;
            float Intensity = 1.0;
        };

        struct ENGINE_API sSsaoBuffer : public sItemBuffer<sSsaoData>
        {
            sSsaoBuffer() : sItemBuffer<sSsaoData>(K_SLOT_SSAO) {}
        };

    }

}