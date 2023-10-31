#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API sRenderInstance final
        {
            u32 TransformIndex = 0;
            u32 MaterialIndex = 0;
        };

        struct ENGINE_API sInstanceBuffer : public sListBuffer<sRenderInstance>
        {
            sInstanceBuffer(usize count = 0) : sListBuffer<sRenderInstance>(count, K_SLOT_INSTANCES) {}
        };

    }

}