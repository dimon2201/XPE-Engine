#pragma once

#include <rendering/core/viewport.h>
#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API ViewportBuffer : public ItemBuffer<Viewport> {
            ViewportBuffer() : ItemBuffer<Viewport>(K_SLOT_VIEWPORT) {}
        };

    }

}