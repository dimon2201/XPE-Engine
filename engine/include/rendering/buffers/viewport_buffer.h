#pragma once

#include <rendering/core/viewport.h>
#include <rendering/buffers/item_buffer.h>

namespace xpe {

    namespace render {

        class ENGINE_API ViewportBuffer : public ItemBuffer<Viewport> {

        public:
            ViewportBuffer() : ItemBuffer<Viewport>(K_SLOT_VIEWPORT) {}

        };

    }

}