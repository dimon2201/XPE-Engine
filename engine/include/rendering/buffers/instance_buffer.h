#pragma once

#include <rendering/buffers/structure_buffer.h>

namespace xpe {

    namespace render {

        struct ENGINE_API RenderInstance final
        {
            u32 TransformIndex = 0;
            u32 CameraIndex = 0;
            u32 MaterialIndex = 0;
        };

        struct ENGINE_API InstanceBuffer : public StructureBuffer<RenderInstance>
        {
            InstanceBuffer(usize count = 0) : StructureBuffer<RenderInstance>(count, K_SLOT_INSTANCES) {}
        };

    }

}