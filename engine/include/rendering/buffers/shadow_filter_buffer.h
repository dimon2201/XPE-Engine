#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API ShadowFilter final
        {
            int FilterSize = 9;
        };

        struct ENGINE_API sShadowFilterBuffer : public sItemBuffer<ShadowFilter>
        {
            sShadowFilterBuffer() : sItemBuffer<ShadowFilter>(K_SLOT_SHADOW_FILTER) {}
        };

    }

}