#pragma once

#include <rendering/core/viewport.h>
#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API ViewportBuffer : public StructureBuffer<Viewport> {
            ViewportBuffer(usize count = 0) : StructureBuffer<Viewport>(count, K_SLOT_VIEWPORT) {}
        };

    }

}