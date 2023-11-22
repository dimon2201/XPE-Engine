#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sShadowPcf final
        {
            int FilterSize = 0;
            float MinBias = 0.005f;
            float MaxBias = 0.05f;
            float _pad0 = 0;
        };

        struct ENGINE_API sShadowPcfBuffer : public sItemBuffer<sShadowPcf>
        {
            sShadowPcfBuffer() : sItemBuffer<sShadowPcf>(K_SLOT_SHADOW_PCF) {}
        };

    }

}