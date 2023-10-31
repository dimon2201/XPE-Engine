#pragma once

#include <rendering/core/viewport.h>
#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sViewportBuffer : public sListBuffer<sViewport> {
            sViewportBuffer(usize count = 0) : sListBuffer<sViewport>(count, K_SLOT_VIEWPORT) {}
        };

    }

}