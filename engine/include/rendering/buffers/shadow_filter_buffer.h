#pragma once

#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API ShadowFilter : public core::Object
        {
            int FilterSize = 9;
        };

        struct ENGINE_API ShadowFilterBuffer : public ItemBuffer<ShadowFilter>
        {
            ShadowFilterBuffer() : ItemBuffer<ShadowFilter>(K_SLOT_SHADOW_FILTER) {}
        };

    }

}