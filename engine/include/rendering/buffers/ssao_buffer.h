#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        using namespace core;

        struct ENGINE_API SSAOData final
        {
            int DirectionCount = 4;
            int SampleCount = 4;
            float Intensity = 1.0;
        };

        struct ENGINE_API SSAOBuffer : public ItemBuffer<SSAOData>
        {
            SSAOBuffer() : ItemBuffer<SSAOData>(K_SLOT_SSAO) {}
        };

    }

}