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

        struct ENGINE_API RenderInstance2d final
        {
            u32 TransformIndex = 0;
            u32 CameraIndex = 0;
        };

        struct ENGINE_API InstanceBuffer : public StructureBuffer<RenderInstance>
        {

            InstanceBuffer(usize count = 0) : StructureBuffer<RenderInstance>(count, K_SLOT_INSTANCES) {}

        };

        struct ENGINE_API InstanceBuffer2d : public StructureBuffer<RenderInstance2d>
        {

            InstanceBuffer2d(usize count = 0) : StructureBuffer<RenderInstance2d>(count, K_SLOT_INSTANCES2D) {}

        };

    }

}