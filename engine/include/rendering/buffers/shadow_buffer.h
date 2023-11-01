#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sShadowData final
        {
            int FilterSize = 9;
            float _pad0 = 0;
            float _pad1 = 0;
            float _pad2 = 0;
        };

        struct ENGINE_API sShadowBuffer : public sItemBuffer<sShadowData>
        {
            sShadowBuffer() : sItemBuffer<sShadowData>(K_SLOT_SHADOW) {}
        };

    }

}